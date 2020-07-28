#include "hashmap.h"
#include <stdio.h>
#include <string.h>

int hashcode(void* n){
    return *(int*)n;
}

int equals(void* n1, void* n2){
    return *(int*)n1 == *(int*)n2;
}

int equalsString(void* v1, void* v2){
    return !strcmp((char*)v1, (char*)v2);
}

int main(int argc, char const *argv[]){

    int num[] = {5, 70, 40, 23, 44, 6, 555};

    char* strings[] = {"prueba", "tobias", "malena", "gerardo", "me canse", "chau", "jeje"};

    HashMap map = initializeHashMap(sizeof(int), sizeof(char*), hashcode, equals);

    put(map, &num[0], &strings[0]);
    put(map, &num[1], &strings[1]);
    put(map, &num[2], &strings[2]);
    put(map, &num[3], &strings[3]);

    delete(map, &num[1]);

    putchar('0');
    dumpHashMap_debug(map);

    put(map, &num[4], &strings[4]);
    put(map, &num[0], &strings[5]);
    put(map, &num[5], &strings[6]);

    putchar('1');
    dumpHashMap_debug(map);

    delete(map, &num[2]);
    printf("%d", num[5]);
    delete(map, &num[5]);

    putchar('2');
    dumpHashMap_debug(map);

    printf("%d %d %d %d\n", hasKey(map, &num[1]), hasKey(map, &num[2]), hasKey(map, &num[3]), hasValue(map, &strings[3], equalsString));

    freeHashMap(map);

    return 0;
}
