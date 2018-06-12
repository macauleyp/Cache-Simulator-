#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct block {
    short nruBit;
    unsigned long long int tagDec;
    struct block *next;
};

typedef struct set {

    int items;
    struct block *head;
    struct block *last;
} set;


int sets = 0;
int blockSize = 0;
int cacheSize = 0;
int blocksInSet = 1;

int memRead = 0;
int memWrite = 0;
int cacheHit = 0;
int cacheMiss = 0;

/*void printCache(set *cache){
    int i;
    for(i = 0; i<sets;i++){
        struct block *ptr = cache[i].head;
        while(ptr != NULL){
            printf("%llu -> ",ptr->tagDec);
            ptr = ptr->next;
        }
        printf("\n");
    }
    printf("END \n");
}*/

void directMap(set *cache, unsigned long long int tagBitsValue, int setIndex, int rewrite) {
    //printf("Index: %d | Comparing: %llu and %llu \n", setIndex, cache[setIndex].tagDec, tagBitsValue);
    if (cache[setIndex].head != NULL && cache[setIndex].head->tagDec == tagBitsValue) {
        if (rewrite == 0) {
            cacheHit = cacheHit + 1;
        } else {
            cacheHit = cacheHit + 1;
            memWrite = memWrite + 1;
        }


    } else {
        if (rewrite == 0) {
            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
        } else {

            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
            memWrite = memWrite + 1;
        }

        cache[setIndex].head = (struct block *) (malloc(sizeof(struct block)));
        cache[setIndex].head->tagDec = tagBitsValue;

    }

}

void enqueue(set *cache, int index, unsigned long long int d, int rewrite) {
    struct block *temp = cache[index].last;
    cache[index].last = (struct block *) (malloc(sizeof(struct block)));
    cache[index].last->tagDec = d;
    cache[index].last->next = NULL;
    if (cache[index].head == NULL) {
        cache[index].head = cache[index].last;
        if (rewrite == 0) {
            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
        } else {
            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
            memWrite = memWrite + 1;
        }

    } else {
        if (rewrite == 0) {
            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
        } else {

            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
            memWrite = memWrite + 1;
        }
        temp->next = cache[index].last;

    }
    if (cache[index].items < blocksInSet) {
        cache[index].items = cache[index].items + 1;
    }
}

void dequeue(set *cache, int index, unsigned long long int d, int rewrite) {
    struct block *temp = cache[index].head;
    if (cache[index].head != NULL) {

        cache[index].head = cache[index].head->next;
        free(temp);

}
enqueue(cache, index, d, rewrite);
}

void fifo(set *cache, unsigned long long int tagBitsValue, int setIndex, int rewrite) {

    struct block *ptr = cache[setIndex].head;
    int found = 0;
    while (ptr != NULL) {
        if (ptr->tagDec == tagBitsValue) {
            if (rewrite == 0) {
                cacheHit = cacheHit + 1;
            } else {
                cacheHit = cacheHit + 1;
                memWrite = memWrite + 1;
            }
            found = 1;
            break;
        }
        ptr = ptr->next;

    }
    if (found == 0) {

        if (cache[setIndex].items == blocksInSet) {

            dequeue(cache, setIndex, tagBitsValue, rewrite);

        } else {
            enqueue(cache, setIndex, tagBitsValue, rewrite);
        }
    }

}

void addToHead(set *cache, int setIndex, unsigned long long int d) {
    struct block *newNode = (struct block *) (malloc(sizeof(struct block)));
    newNode->tagDec = d;
    newNode->next = NULL;

    if (cache[setIndex].head == NULL) {
        cache[setIndex].head = newNode;

    } else {

        struct block *temp = cache[setIndex].head;
        cache[setIndex].head = newNode;
        newNode->next = temp;

    }

    cache[setIndex].items = cache[setIndex].items + 1;

}

void updateLRU(set *cache, int setIndex, unsigned long long int d, int found) {
    struct block *ptr = cache[setIndex].head;
    struct block *prev = cache[setIndex].head;
    if (found == 1) {

        if(cache[setIndex].head->tagDec == d){
            return;
        }else{
            while(ptr != NULL){
                if(ptr->tagDec == d){
                    struct block *replace = (struct block *) (malloc(sizeof(struct block)));
                    replace->tagDec = d;

                    if(ptr->next!=NULL){
                        prev->next= ptr->next;

                        replace->next = cache[setIndex].head;
                        cache[setIndex].head = replace;

                    }else{
                        prev->next = NULL;
                        replace->next = cache[setIndex].head;
                        cache[setIndex].head = replace;
                    }
                    free(ptr);
                    break;
                }else{
                    prev = ptr;
                    ptr = ptr->next;
                }
            }

        }

    } else {
        while (ptr != NULL) {
            if (ptr->next == NULL) {
                free(ptr);
                prev->next = NULL;
                struct block *replace = (struct block *) (malloc(sizeof(struct block)));
                replace->tagDec = d;
                replace->next = cache[setIndex].head;
                cache[setIndex].head = replace;
                break;
            }else{
                prev = ptr;
                ptr = ptr->next;
            }

        }
    }

}
void addToEnd(set *cache, int setIndex, unsigned long long int d) {
    struct block *newNode = (struct block *) (malloc(sizeof(struct block)));
    newNode->tagDec = d;
    newNode->next = NULL;
    newNode->nruBit = 0;

    if (cache[setIndex].head == NULL) {
        cache[setIndex].head = newNode;

    } else {
        struct block *ptr = cache[setIndex].head;
        while(ptr!= NULL){
            if(ptr->next == NULL){
                ptr->next =  newNode;
                break;
            }else{
                ptr = ptr->next;
            }

        }

    }

    cache[setIndex].items = cache[setIndex].items + 1;

}
void lru(set *cache, unsigned long long int tagBitsValue, int setIndex, int rewrite) {
    struct block *ptr = cache[setIndex].head;
    int found = 0;

    while (ptr != NULL) {
        if (ptr->tagDec == tagBitsValue) {
            if (rewrite == 0) {
                cacheHit = cacheHit + 1;
            } else {
                cacheHit = cacheHit + 1;
                memWrite = memWrite + 1;
            }

            found = 1;
            break;
        }
        ptr = ptr->next;
    }

    if (found == 0) {
        if (rewrite == 0) {
            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
        } else {
            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
            memWrite = memWrite + 1;
        }
        if (cache[setIndex].items < blocksInSet) {
            addToHead(cache, setIndex, tagBitsValue);
        } else {
            updateLRU(cache, setIndex, tagBitsValue, 0);
        }
    }else{

        updateLRU(cache, setIndex, tagBitsValue, 1);

    }
   // printCache(cache);
}


void updateNRU(set *cache, unsigned long long int tagBitsValue, int setIndex) {
    struct block *ptr = cache[setIndex].head;
    int found = 0;

    while (ptr != NULL) {
        if (ptr->nruBit == 1) {
            ptr->nruBit = 0;
            ptr->tagDec = tagBitsValue;
            found = 1;
            break;
        } else {
            ptr = ptr->next;
        }
    }

    struct block *fill = cache[setIndex].head;
    if (found == 0) {
        while (fill != NULL && fill->nruBit == 0) {
            fill->nruBit = 1;
            fill = fill->next;
        }
        updateNRU(cache,tagBitsValue,setIndex);
    }

}

void nru(set *cache, unsigned long long int tagBitsValue, int setIndex, int rewrite) {
    struct block *ptr = cache[setIndex].head;
    int found = 0;

    while (ptr != NULL) {
        if (ptr->tagDec == tagBitsValue) {
            ptr->nruBit = 0;
            if (rewrite == 0) {
                cacheHit = cacheHit + 1;
            } else {
                cacheHit = cacheHit + 1;
                memWrite = memWrite + 1;
            }
            found = 1;
            break;
        } else {
            ptr = ptr->next;
        }
    }

    if (found == 0) {
        if (rewrite == 0) {
            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
        } else {

            cacheMiss = cacheMiss + 1;
            memRead = memRead + 1;
            memWrite = memWrite + 1;
        }
        if (cache[setIndex].items < blocksInSet) {
            addToEnd(cache, setIndex, tagBitsValue);
        } else {
            updateNRU(cache, tagBitsValue, setIndex);

        }
    }
}
int main(int argc, char **argv){
	cacheSize = atoi(argv[1]);
    blockSize = atoi(argv[4]);

    //printf("%d %s %s %d %s\n", cacheSize, argv[2],argv[3], blockSize, argv[5]);
    FILE *f = fopen(argv[5], "r");
    if (!f || argc!=6) {
        printf("error\n");
        return 0;
    }

    double checkCacheSize = log((double) cacheSize) / log(2);
    double checkBlockSize = log((double) blockSize) / log(2);

    if (checkCacheSize != (int) checkCacheSize || checkBlockSize != (int) checkBlockSize) {
        printf("error\n");
        return 0;
    } else {
        cacheSize = (int) checkCacheSize;
        blockSize = (int) checkBlockSize;
    }

    int i;

    char *assocD = "direct";
    char *assocF = "assoc";

    int policy = 0;
    if ((strcmp(assocD, argv[2])) == 0) {

        policy = 1;
        sets = (int) (pow(2, cacheSize) / pow(2, blockSize));

    } else if ((strcmp(assocF, argv[2])) == 0) {

        sets = 1;
        blocksInSet = (int) (pow(2, cacheSize) / pow(2, blockSize));

    } else if(strlen(argv[2])>6) {
	char assocName[(strlen(argv[2]) - 6)];
        memcpy(assocName, &argv[2][6], (size_t) strlen(argv[2]) - 6);
        sets = (int) (pow(2, cacheSize) / ((pow(2, blockSize)) * atoi(assocName)));

        blocksInSet = atoi(assocName);
	if((log((double)blocksInSet)/log(2)) != (int)(log((double)blocksInSet)/log(2))){
		printf("error\n");
		return 0;
	}
    }else{
	printf("error\n");
        return  0;
	}

    char fifoRP[] = "fifo";
    char lruRP[] = "lru";
    char nruRP[] = "nru";

    if(policy == 1 && ((strcmp(fifoRP, argv[3]) == 0) || (strcmp(lruRP, argv[3]) == 0) || (strcmp(nruRP, argv[3]) == 0))){
	policy =1;
    }else if (strcmp(fifoRP, argv[3]) == 0) {
        policy = 2;

    } else if (strcmp(lruRP, argv[3]) == 0) {
        policy = 3;

    } else if (strcmp(nruRP, argv[3]) == 0) {
        policy = 4;

    }else{
        printf("error\n");
        return  0;
    }
	set cache[sets];
    //printf("Cache: %d | Block: %d | Set: %d\n", cacheSize, blockSize, sets);

    char num[19], memory[19];
    char rw;

    for (i = 0; i < sets; i++) {

        cache[i].head = NULL;
        cache[i].last = NULL;
        cache[i].items = 0;
    }

    unsigned long long int value;
    int bitNum = (int) (log(sets) / log(2));

    while (!feof(f)) {

        fscanf(f, "%s %c %s", memory, &rw, num);
        if (strcmp(memory, "#eof") == 0) {
            rw = 'C';
            break;
        }

        value = strtoull(num, NULL, 0);

        unsigned long long int tagBitsValue = (value >> (blockSize + bitNum));

        int indexBinNum = (int) ((value >> blockSize) & ((int) (pow(2, bitNum) - 1)));

        if (rw == 'R' || rw == 'r') {
            if (policy == 1) {
                directMap(cache, tagBitsValue, indexBinNum, 0);
            } else if (policy == 2) {
                fifo(cache, tagBitsValue, indexBinNum, 0);
            } else if (policy == 3) {
                lru(cache, tagBitsValue, indexBinNum, 0);
            } else if (policy == 4) {
                nru(cache, tagBitsValue, indexBinNum, 0);
            }else{
		            printf("error\n");
		            return 0;
		            }
        } else if (rw == 'W' || rw == 'w') {
            if (policy == 1) {
                directMap(cache, tagBitsValue, indexBinNum, 1);
            } else if (policy == 2) {
                fifo(cache, tagBitsValue, indexBinNum, 1);
            } else if (policy == 3) {
                lru(cache, tagBitsValue, indexBinNum, 1);
            } else if (policy == 4) {
                nru(cache, tagBitsValue, indexBinNum, 1);
            }else{
		            printf("error\n");
		            return 0;
		            }
        }
    }
    //printf("\n");

    printf("Memory reads: %d\nMemory writes: %d\nCache hits: %d\nCache misses: %d\n",memRead,
           memWrite, cacheHit, cacheMiss);
    return 0;
}
