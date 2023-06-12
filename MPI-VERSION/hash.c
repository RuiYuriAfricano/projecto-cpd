#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

void setNode(Node* node, int key, int value) {
    node->key = key;
    node->value = value;
    node->next = NULL;
}


void initializeHashMap(HashMap* mp) {
    mp->capacity = 1000;
    mp->numOfElements = 0;
    mp->arr = (Node**)malloc(sizeof( Node*) * mp->capacity);
    int i;
    for (i = 0; i < mp->capacity; i++) {
        mp->arr[i] = NULL;
    }
}

int hashFunction(HashMap* mp, int key) {
    return key % mp->capacity;
}

void insert(HashMap* mp, int key, int value) {
    int bucketIndex = hashFunction(mp, key);
    Node* newNode = (Node*)malloc(sizeof(Node));
    setNode(newNode, key, value);
    if (mp->arr[bucketIndex] == NULL) {
        mp->arr[bucketIndex] = newNode;
    }
    else {
        newNode->next = mp->arr[bucketIndex];
        mp->arr[bucketIndex] = newNode;
    }
    mp->numOfElements++; // Atualiza o número de elementos no mapa
}


void delete_ (HashMap* mp, int key) {
    int bucketIndex = hashFunction(mp, key);
     Node* prevNode = NULL;
     Node* currNode = mp->arr[bucketIndex];
    while (currNode != NULL) {
        if (key == currNode->key) {
            if (currNode == mp->arr[bucketIndex]) {
                mp->arr[bucketIndex] = currNode->next;
            }
            else {
                prevNode->next = currNode->next;
            }
            free(currNode);
            mp->numOfElements--;
            break;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
    return;
}

int search(HashMap* mp, int key) {
    int bucketIndex = hashFunction(mp, key);
     Node* bucketHead = mp->arr[bucketIndex];
    while (bucketHead != NULL) {
        if (key == bucketHead->key) {
            return bucketHead->value;
        }
        bucketHead = bucketHead->next;
    }
    return -1; // Valor sentinela indicando que a chave não foi encontrada
}


