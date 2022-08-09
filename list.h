/**
 *  list.h
 *
 *  Full Name:
 *  Course section:
 *  Description of the program: list data structure containing the tasks in the system
 *  
 */
 
#include "process.h"

struct node {
    Process *process;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, Process *process);
void delete(struct node **head, Process *process);
void traverse(struct node *head);
void printList(struct node* head);
