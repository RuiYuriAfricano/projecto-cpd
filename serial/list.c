#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "list.h"


List* create_list() {
    List* new_list = (List*)malloc(sizeof(List));
    new_list->size = 0;
    new_list->head = NULL;
    return new_list;
}
 
void add_to_list(List* list, void* data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = data;
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
    int *p = (int *) new_node->data;
}

void print_list(List* list) {
    Node * aux = list->head;
    
    while(aux != NULL){
    	int* int_ptr = (int*) aux->data;
    	printf("%d\n", *int_ptr);
    	aux = aux->next;
	}
}

int search_in_list(List* list, int d) {
    Node * aux = list->head;
    int pos = 0;
   	
    while(aux != NULL){
    	int* int_ptr = (int*) aux->data;
    	if(*int_ptr == d)
    		return pos;
    	aux = aux->next;
    	pos++;
	}	
	return -1;
}

 
void* remove_head(List* list) {
    if (list->size == 0) {
        return NULL;
    }
    Node* node_to_remove = list->head;
    void* data = node_to_remove->data;
    list->head = node_to_remove->next;
    free(node_to_remove);
    list->size--;
    return data;
}

List* remove_in_list(List* list, int element) {
    if (list->size == 0) {
        return NULL;
    }
    
    Node *prev = NULL;
    Node *current = list->head;
    while(current != NULL){
        int* int_ptr = (int*) current->data;
        if(*int_ptr == element){
            if(prev == NULL){
                list->head = current->next;
            }
            else{
                prev->next = current->next;
            }
            free(current);
            list->size--;
            return list;
        }
        prev = current;
        current = current->next;
    }
    return list;
}

 
void free_list(List* list) {
    Node* current_node = list->head;
    while (current_node != NULL) {
        Node* next_node = current_node->next;
        free(current_node);
        current_node = next_node;
    }
    free(list);
}





