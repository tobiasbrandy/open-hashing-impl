#include "hashmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

// Debug Macros
#define DEBUG_KEY_TYPE int
#define DEBUG_VALUE_TYPE char*
#define DEBUG_KEY_PRINTF_IDENTIFIER "%d"
#define DEBUG_VALUE_PRINTF_IDENTIFIER "%s"

#define INIT_SIZE 8
#define MAX_LOAD_FACTOR 0.5

typedef struct HashMapItem{
    uint8_t isActive;
    void * key;
    void * value;
}HashMapItem;

struct HashMap{
    size_t size;
    size_t allocSize;
    size_t keySize;
    size_t valueSize;
    int (*hash)(void* key);
    int (*equals)(void* key1, void* key2);
    HashMapItem ** items;
};

static size_t hash(HashMap map, void * key);
static size_t probe(size_t initPos, size_t iter, size_t size);
static void assignItem(HashMap map, size_t pos, void * key, void * value);
static size_t getKeyPosition(HashMap map, void * key, int * found);
static void resizeHashMap(HashMap map);


HashMap initializeHashMap(size_t keySize, size_t valueSize, int (*hash)(void* key), int (*equals)(void* v1, void* v2)){
    HashMap map = malloc(sizeof(*map));

    map->size = 0;
    map->allocSize = INIT_SIZE;
    map->keySize = keySize;
    map->valueSize = valueSize;
    map->hash = hash;
    map->equals = equals;
    map->items = calloc(map->allocSize, sizeof(*map->items));

    return map;
}

void freeHashMap(HashMap map){

    for(size_t i = 0; i < map->allocSize; i++){
        if(map->items[i] != NULL)
            free(map->items[i]);
    }

    free(map->items);
    free(map);
}

void put(HashMap map, void * key, void * value){
    int found;

    size_t pos = getKeyPosition(map, key, &found);

    if(found){
        memcpy(map->items[pos]->value, value, map->valueSize);
        return;
    }

    assignItem(map, pos, key, value);

    map->size++;

    // load factor check
    if(((double)map->size / map->allocSize) > MAX_LOAD_FACTOR)
        resizeHashMap(map);
}

void * get(HashMap map, void * key){

    int found;
    size_t pos = getKeyPosition(map, key, &found);

    return (found)? map->items[pos]->value : NULL;
}

void delete(HashMap map, void * key){
    size_t initPos = hash(map, key);
    size_t pos = initPos;
    size_t probingIter = 0;

    while(map->items[pos] != NULL && !(map->items[pos]->isActive && map->equals(map->items[pos]->key, key))){
        probingIter++;
        pos = probe(initPos, probingIter, map->allocSize);
    }

    HashMapItem * itemToDelete = map->items[pos];

    if(itemToDelete != NULL){
        map->size--;

        if(map->items[probe(initPos, probingIter + 1, map->allocSize)] == NULL){ // if next item is null

            free(itemToDelete);
            map->items[pos] = NULL;
        }
        else
            itemToDelete->isActive = 0;
    }
}

int hasKey(HashMap map, void * key){
    int found;
    getKeyPosition(map, key, &found);

    return found;
}

int hasValue(HashMap map, void * value, int (*equals)(void * value1, void * value2)){
    HashMapItem * item;
    size_t hits = 0;

    for(size_t i = 0; hits < map->size && i < map->allocSize; i++){
        item = map->items[i];

        if(item != NULL){
            hits++;
            if(equals(item->value, value))
                return 1;
        }
    }

    return 0;
}

int isEmpty(HashMap map){
    return map->size == 0;
}

size_t size(HashMap map){
    return map->size;
}

void dumpHashMap_debug(HashMap map){

    printf("------ Hash Map Dump -------\n");

    for (size_t i = 0; i < map->allocSize; i++){
        HashMapItem * item = map->items[i];

        printf("Bucket %zu: ", i);
        if(item == NULL)
            printf("NULL\n");
        else
            printf("%s, "DEBUG_KEY_PRINTF_IDENTIFIER" => "DEBUG_VALUE_PRINTF_IDENTIFIER"\n", (item->isActive)? "ACTIVE" : "INACTIVE", *(DEBUG_KEY_TYPE*)item->key, *(DEBUG_VALUE_TYPE*)item->value);
    }

    printf("----- Size: %zu, AllocSize: %zu ------\n", map->size, map->allocSize);
}

static size_t hash(HashMap map, void * key){
    return map->hash(key) & (map->allocSize - 1);
}

// Assuming probe function can't fail (it hits every space)
static size_t probe(size_t initPos, size_t iter, size_t mapSize){
    // Linear Probe
    return (initPos + iter) & (mapSize - 1); // (pos + i) % mapSize. Works because mapSize is a power of 2.

    // Quadratic Probing (can fail, that's why is not implemented yet)
    // return (initPos + iter*iter) & (mapSize - 1);
}

static void assignItem(HashMap map, size_t pos, void * key, void * value){

    HashMapItem * item = map->items[pos];

    if(item == NULL){

        // I assign memory to item, plus key and value. Then I assign their pointers correspondingly, after item.
        // I do this to call malloc once and have continous memory.

        item = malloc(sizeof(HashMapItem) + map->keySize + map->valueSize);
        item->key = (void*)((uintptr_t)item + sizeof(*item));
        item->value = (void*)((uintptr_t)item->key + map->keySize);

        map->items[pos] = item;
    }

    item->isActive = 1;
    memcpy(item->key, key, map->keySize);
    memcpy(item->value, value, map->valueSize);
}

static size_t getKeyPosition(HashMap map, void * key, int * found){
    size_t initPos = hash(map, key);
    size_t pos = initPos;
    int firstEmptyPos = -1;
    size_t probingIter = 0;

    while(map->items[pos] != NULL){

        // Check if found
        if(map->items[pos]->isActive && map->equals(map->items[pos]->key, key)){
            *found = 1;
            return pos;
        }

        if(firstEmptyPos != -1 && !map->items[pos]->isActive)
            firstEmptyPos = pos;

        probingIter++;
        pos = probe(initPos, probingIter, map->allocSize);
    }

    *found = 0;

    // Key is placed in first empty position
    return (firstEmptyPos != -1)? firstEmptyPos : pos;   
}

static void resizeHashMap(HashMap map){
    size_t oldSize = map->allocSize;
    HashMapItem ** oldItems = map->items;

    map->allocSize *= 2;
    map->items = calloc(map->allocSize, sizeof(*map->items));

    int found;
    for(size_t i = 0; i < oldSize; i++){

        HashMapItem * oldItem = oldItems[i];

        if(oldItem != NULL){

            if(oldItem->isActive){

                size_t pos = getKeyPosition(map, oldItem->key, &found); // found should always be 0
                map->items[pos] = oldItem;
            }
            else
                free(oldItem);
        }
    }

    free(oldItems);
}