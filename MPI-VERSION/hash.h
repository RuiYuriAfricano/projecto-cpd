#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef LIST_H
#define LIST_H

typedef  struct node {
    int key;
    int value;
    struct node* next;
} Node;

typedef struct hashMap {
    int numOfElements, capacity;
    struct node** arr;
} HashMap;

void setNode(Node *node, int key, int value);

void initializeHashMap(HashMap* mp);

int hashFunction(HashMap* mp, int key);

void insert(HashMap* mp, int key, int value);

void delete_ (HashMap* mp, int key);

int search(HashMap* mp, int key);

#endif // LIST_H
