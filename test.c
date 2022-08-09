#include<stdio.h>
#include<stdlib.h>
#include "list.h"

int main(){
    struct node *head = NULL;
    Process* p1 = malloc(sizeof(Process));
    p1->pid = 10;
    insert(&head, p1);
    printList(head);
    delete(&head, p1);
    printf("%d\n", head == NULL);
    printList(head);
}

void delete(struct node **head, Process *process) {
  struct node *tmp;

  /*Linked list does not exist or the list is empty*/
  if(head == NULL || *head == NULL) return;
  
  /*Storing the head to a temporary variable*/
  tmp = *head;
  
  /*Moving head to the next node*/
  *head = (*head)->next;
  
  /*Deleting the first node*/
  free(tmp);
}


void insert(struct node** head, Process* process)
{
    // /* 1. allocate node */
    // struct node* new_node = (struct node*) malloc(sizeof(struct node));
  
    // /* 2. put in the data  */
    // new_node->process  = process;
  
    // /* 3. Make next of new node as head */
    // new_node->next = (*head);
  
    // /* 4. move the head to point to the new node */
    // (*head)    = new_node;

	



	struct node* new_node = (struct node*) malloc(sizeof(struct node));
 
    struct node *last = *head;  /* used in step 5*/
  
    /* 2. put in the data  */
    new_node->process  = process;
 
    /* 3. This new node is going to be the last node, so make next
          of it as NULL*/
    new_node->next = NULL;
 
    /* 4. If the Linked List is empty, then make the new node as head */
    if (*head == NULL)
    {
       *head = new_node;
       return;
    } 
      
    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;
  
    /* 6. Change the next of last node */
    last->next = new_node;
    return;   
}

void printList(struct node* head)
{
    while (head != NULL) {
        printf(" %d ", head->process->pid);
        head = head->next;
    }
	printf("\n");
}