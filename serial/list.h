#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef LIST_H
#define LIST_H

typedef struct node {
    void* data;
    struct node* next;
} Node;
 
typedef struct list {
    int size;
    Node* head;
} List;

// Definição da função create_list
List* create_list() ;

// Definição da função add_to_list
void add_to_list(List* list, void* data);

// Definição da função print_list
void print_list(List* list);

// Definição da função search_in_list
int search_in_list(List* list, int d);

// Definição da função remove_head
void* remove_head(List* list);

// Definição da função remove_in_list
List* remove_in_list(List* list, int element);

// Definição da função free_list
void free_list(List* list);

#endif // LIST_H
