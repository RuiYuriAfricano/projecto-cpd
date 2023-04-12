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

// Defini��o da fun��o create_list
List* create_list() ;

// Defini��o da fun��o add_to_list
void add_to_list(List* list, void* data);

// Defini��o da fun��o print_list
void print_list(List* list);

// Defini��o da fun��o search_in_list
int search_in_list(List* list, int d);

// Defini��o da fun��o remove_head
void* remove_head(List* list);

// Defini��o da fun��o remove_in_list
List* remove_in_list(List* list, int element);

// Defini��o da fun��o free_list
void free_list(List* list);

#endif // LIST_H
