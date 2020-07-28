#include <stddef.h>

typedef struct HashMap * HashMap;

HashMap initializeHashMap(size_t keySize, size_t valueSize, int (*hash)(void* key), int (*equals)(void* v1, void* v2));

void freeHashMap(HashMap map);

void put(HashMap map, void * key, void * value);

void * get(HashMap map, void * key);

void delete(HashMap map, void * key);

int hasKey(HashMap map, void * key);

int hasValue(HashMap map, void * value, int (*equals)(void * value1, void * value2));

int isEmpty(HashMap map);

size_t size(HashMap map);

// Debug only purpouses. Define macros in .c file.
void dumpHashMap_debug(HashMap map);