/**
 *  scheduler.c
 *
 *  Full Name:
 *  Course section:
 *  Description of the program:
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "list.h"
#include "scheduler.h"
#include "process.h"

#define SIZE    100
#define MAX_OUTPUT 50

void fcfs(char[MAX_OUTPUT], Process*, int);
void round_robin(char[MAX_OUTPUT], Process*, int);
void sjf(char[MAX_OUTPUT], Process*, int);

int main(int argc, char *argv[])
{
	FILE *fp;

	int num_of_processes;
	char* scheduling_algorithm = argv[2];
	Process *queue;
	fp  = fopen(argv[1],"r");
	fscanf(fp, "%d", &num_of_processes);    
	if (num_of_processes > 0){
		queue = malloc(num_of_processes * sizeof(Process));      
		for (int i=0; i<num_of_processes; i++){
			fscanf(fp, "%d %d %d %d",
					&queue[i].pid, 
					&queue[i].cpu_time, 
					&queue[i].io_time, 
					&queue[i].arrival_time); 
		}                                                                               

		for(int i =0; i<num_of_processes; i++) {
			printf("A: %d B: %d  C: %d D: %d \n",
					queue[i].pid, 
					queue[i].cpu_time, 
					queue[i].io_time, 
					queue[i].arrival_time);    
		}
	}

	fclose(fp);

	// output file name
    char output_file_name[50];
	char *token = strtok(argv[1], ".");
	strcat(output_file_name, token);
	strcat(output_file_name,"-");
	strcat(output_file_name, argv[2]);
    strcat(output_file_name , ".text\n");
	output_file_name[0] = ' ';

	if(strcmp(scheduling_algorithm,"0") == 0){
        fcfs(output_file_name, queue, num_of_processes);
    }else if(strcmp(scheduling_algorithm,"1") == 0){
        round_robin(output_file_name, queue, num_of_processes);
    }else if(strcmp(scheduling_algorithm,"2") == 0){
        sjf(output_file_name, queue, num_of_processes);
    }

	// this only frees the first process
	free(queue); 
	return 0;
}

void fcfs(char output[MAX_OUTPUT], Process *jobs, int task_count){
    printf("fcfs\n");
    printf("%s\n", output);
    struct node* ready_running = NULL;
	struct node* temp;
	int states[task_count][2];
	int turn_round[task_count][2], utilization = 0;
	for(int i = 0; i<task_count; i++){
		states[i][0] = 0;
		states[i][1] = ceil(jobs[i].cpu_time/2);
	}
	int continues;
	for(int i = 0; i<SIZE; i++){
		
		for(int j = 0; j<task_count; j++){
			// handle ready
			if(ready_running!=NULL && ready_running->process->pid == j && states[j][0] == 1){
				states[j][0] = 2;
			}
			else if(ready_running!=NULL && ready_running->process->pid == j && states[j][0] == 2){
				ready_running->process->cpu_time -=1;
				if(ready_running->process->cpu_time == states[ready_running->process->pid][1]){
					states[ready_running->process->pid][0] = 3;
					delete(&ready_running, (*(ready_running)).process);
				}else if(ready_running->process->cpu_time == 0){
					states[ready_running->process->pid][0] = 4;
					turn_round[ready_running->process->pid][1] = i;
					delete(&ready_running, (*(ready_running)).process);
				}
			}
			else if(states[j][0] == 3){
				jobs[j].io_time -= 1;
				if(jobs[j].io_time==0){
					insert(&ready_running, &jobs[j]);
				}
			}
		}

		for(int j = 0; j<task_count; j++){
			if(i == jobs[j].arrival_time){
				insert(&ready_running, &jobs[j]);
				turn_round[j][0] = i;
			}
		}
		temp = ready_running;
		while(temp != NULL){
			if(temp == ready_running){
				states[temp->process->pid][0] = 2;
			}else{
				states[temp->process->pid][0] = 1;
			}
			temp = temp->next;
		}

		continues = 0;
		for(int j=0;j<task_count;j++){
			if(states[j][0] != 4){
				continues = 1;
			}
		}
		continues = 0;
		for(int k=0;k<task_count;k++){
			if(states[k][0] != 4){
				continues = 1;
			}
		}
		if(continues == 0){
			FILE *fptr = fopen(output, "a");
			fprintf(fptr,"\nFinished time: %d\n", i-1);
			fprintf(fptr,"CPU utilization: %.2f\n", (utilization/(float)(i)));
			fclose(fptr);
			break;
		}
		if(ready_running!=NULL){
			utilization+=1;
		}
		FILE *fptr = fopen(output, "a");
		fprintf(fptr,"%d ", i);
		for(int j = 0; j<task_count; j++){
			if(states[j][0] == 1){
				fprintf(fptr,"%d: %s ", j, "ready");
			}else if(states[j][0] == 2){
				fprintf(fptr,"%d: %s ", j, "running");
			}else if(states[j][0] == 3){
				fprintf(fptr,"%d: %s ", j, "blocked");
			}
		}
		fprintf(fptr,"\n");
		fclose(fptr);
	}

	FILE *fptr = fopen(output, "a");
	for(int j = 0; j<task_count; j++){
			fprintf(fptr,"Turnaround process %d: %d\n", j, turn_round[j][1] - turn_round[j][0]);
		}
	fclose(fptr);
}


void round_robin(char output[MAX_OUTPUT], Process *jobs, int task_count){
    printf("round-robin\n");
    printf("%s\n", output);
    struct node* ready_running = NULL;
	struct node* temp;
	int states[task_count][2];
	int quantum=2;
	int utilization = 0, turn_round[task_count][2];
	for(int i = 0; i<task_count; i++){
		states[i][0] = 0;
		states[i][1] = ceil(jobs[i].cpu_time/2);
	}
	int continues;
	for(int i = 0; i<SIZE; i++){
		
		for(int j = 0; j<task_count; j++){
			// handle ready
			if(ready_running!=NULL && ready_running->process->pid == j && states[j][0] == 1){
				states[j][0] = 2;
				quantum = 2;
				
			}
			else if(ready_running!=NULL && ready_running->process->pid == j && states[j][0] == 2){
				ready_running->process->cpu_time -=1;
				quantum-=1;
				if(quantum == 0){
					
					if(ready_running->process->cpu_time == states[ready_running->process->pid][1]){
						states[ready_running->process->pid][0] = 3;
					}else if(ready_running->process->cpu_time > states[ready_running->process->pid][1]){
						
						states[ready_running->process->pid][0] = 1;
						
						insert(&ready_running, &jobs[j]);
					}else if(ready_running->process->cpu_time > 0){
						states[ready_running->process->pid][0] = 1;
						
						insert(&ready_running, &jobs[j]);
					}
					else if(ready_running->process->cpu_time == 0){
						
						turn_round[ready_running->process->pid][1] = i;
						states[ready_running->process->pid][0] = 4;
					}
					
					delete(&ready_running, (*(ready_running)).process);
					quantum = 2;
				}
				else if(ready_running->process->cpu_time == states[ready_running->process->pid][1]){
					states[ready_running->process->pid][0] = 3;
					delete(&ready_running, (*(ready_running)).process);
				}else if(ready_running->process->cpu_time == 0){
					
					
					states[ready_running->process->pid][0] = 4;
					turn_round[ready_running->process->pid][1] = i;
					delete(&ready_running, (*(ready_running)).process);
				}
			}
			else if(states[j][0] == 3){
				jobs[j].io_time -= 1;
				if(jobs[j].io_time==0){
					insert(&ready_running, &jobs[j]);
				}
			}
		}

		for(int j = 0; j<task_count; j++){
			if(i == jobs[j].arrival_time){
				insert(&ready_running, &jobs[j]);
				turn_round[j][0] = i;
			}
		}
		temp = ready_running;
		while(temp != NULL){
			if(temp == ready_running){
				states[temp->process->pid][0] = 2;
				if(quantum == 0){
				quantum = 2;}
			}else{
				states[temp->process->pid][0] = 1;
			}
			temp = temp->next;
		}

		continues = 0;
		for(int k=0;k<task_count;k++){
			if(states[k][0] != 4){
				continues = 1;
			}
		}
		continues = 0;
		for(int k=0;k<task_count;k++){
			if(states[k][0] != 4){
				continues = 1;
			}
		}
		if(continues == 0){
			FILE *fptr = fopen(output, "a");
			fprintf(fptr,"\nFinished time: %d\n", i-1);
			fprintf(fptr,"CPU utilization: %.2f\n", (utilization/(float)(i)));
			fclose(fptr);
			break;
		}
		if(ready_running!=NULL){
			utilization+=1;
		}
		FILE *fptr = fopen(output, "a");
		fprintf(fptr,"%d ", i);
		for(int j = 0; j<task_count; j++){
			if(states[j][0] == 1){
				fprintf(fptr,"%d: %s ", j, "ready");
			}else if(states[j][0] == 2){
				fprintf(fptr,"%d: %s ", j, "running");
			}else if(states[j][0] == 3){
				fprintf(fptr,"%d: %s ", j, "blocked");
			}
		}
		fprintf(fptr,"\n");
		fclose(fptr);
	}
	
	FILE *fptr = fopen(output, "a");
	for(int j = 0; j<task_count; j++){
			fprintf(fptr,"Turnaround process %d: %d\n", j, turn_round[j][1] - turn_round[j][0]);
		}
	fclose(fptr);
}
void printList(struct node* head)
{
    while (head != NULL) {
        printf(" %d ", head->process->pid);
        head = head->next;
    }
	printf("\n");
}
void sjf(char output[MAX_OUTPUT], Process *jobs, int task_count){
    printf("sjf\n");
    printf("%s\n", output);
    struct node* ready_running = NULL;
	struct node* temp;
	int states[task_count][2];
	for(int i = 0; i<task_count; i++){
		states[i][0] = 0;
		states[i][1] = ceil(jobs[i].cpu_time/2);
	}
	int shortest_cpu_arrival, chosen, continues, utilization = 0, turn_round[task_count][2];
	for(int i = 0; i<SIZE; i++){
		
		// handle blocked
		for(int j = 0; j<task_count; j++){
			if(states[j][0] == 3){
				jobs[j].io_time -= 1;
				if(jobs[j].io_time == 0){
					states[j][0] = 1;
					insert(&ready_running, &jobs[j]);
				}
			}
		}
		
		if(ready_running != NULL){
			
			if(states[ready_running->process->pid][0] == 2){
				ready_running->process->cpu_time -= 1;
				if(ready_running->process->cpu_time == states[ready_running->process->pid][1]){
					states[ready_running->process->pid][0] = 3;
					delete(&ready_running, &jobs[ready_running->process->pid]);
				}else if(ready_running->process->cpu_time == 0){
					states[ready_running->process->pid][0] = 4;
					turn_round[ready_running->process->pid][1] = i;
					delete(&ready_running, &jobs[ready_running->process->pid]);
				}
			}
		}
		

		if(ready_running != NULL && states[ready_running->process->pid][0] == 1){
			temp = ready_running;
			shortest_cpu_arrival = 100;
			chosen = -1;
			while(temp != NULL){
				if(temp->process->cpu_time < shortest_cpu_arrival){
					shortest_cpu_arrival = temp->process->cpu_time;
					chosen = temp->process->pid;
				}
				temp = temp->next;
			}
			states[chosen][0] = 2;

			for(int j = 0; j<task_count; j++){
				delete(&ready_running, &jobs[j]);
			}


			insert(&ready_running, &jobs[chosen]);
			for(int j=0; j<task_count; j++){
				if(j!=chosen && states[j][0] == 1){
					insert(&ready_running, &jobs[j]);
				}
			}
		}
		shortest_cpu_arrival = 100;
		// if any process arrives, chosen will change
		chosen = -1;
		for(int j = 0; j<task_count; j++){
			if(i == jobs[j].arrival_time){
				turn_round[j][0] = i;
				if(jobs[j].cpu_time < shortest_cpu_arrival){
					shortest_cpu_arrival = jobs[j].cpu_time;
					chosen = jobs[j].pid;
				}
			}
		}
		// add the chosen
		if(chosen != -1 && ready_running == NULL){
			insert(&ready_running, &jobs[chosen]);
			states[chosen][0] = 2;
			// add the rest of arrived processes in order of id
			for(int j = 0; j<task_count; j++){
				if(i == jobs[j].arrival_time){
					if(jobs[j].pid != chosen){
						insert(&ready_running, &jobs[j]);
						states[j][0] = 1;
					}
				}
			}
		}else if(chosen != -1 && ready_running != NULL){

			for(int j = 0; j<task_count; j++){
				if(i == jobs[j].arrival_time){
					insert(&ready_running, &jobs[j]);
					states[j][0] = 1;
				}
			}
		}

		continues = 0;
		for(int k=0;k<task_count;k++){
			if(states[k][0] != 4){
				continues = 1;
			}
		}
		if(continues == 0){
			FILE *fptr = fopen(output, "a");
			fprintf(fptr,"\nFinished time: %d\n", i-1);
			fprintf(fptr,"CPU utilization: %.2f\n", (utilization/(float)(i)));
			fclose(fptr);
			break;
		}
		if(ready_running!=NULL){
			utilization+=1;
		}
		FILE *fptr = fopen(output, "a");
		fprintf(fptr,"%d ", i);
		for(int j = 0; j<task_count; j++){
			if(states[j][0] == 1){
				fprintf(fptr,"%d: %s ", j, "ready");
			}else if(states[j][0] == 2){
				fprintf(fptr,"%d: %s ", j, "running");
			}else if(states[j][0] == 3){
				fprintf(fptr,"%d: %s ", j, "blocked");
			}
		}
		fprintf(fptr,"\n");
		fclose(fptr);
	}
	FILE *fptr = fopen(output, "a");
	for(int j = 0; j<task_count; j++){
			fprintf(fptr,"Turnaround process %d: %d\n", j, turn_round[j][1] - turn_round[j][0]);
		}
	fclose(fptr);
}

void delete(struct node** head, Process *process)
{
	struct node *tmp;

	
	if(head == NULL || *head == NULL) return;

	
	tmp = *head;

	
	*head = (*head)->next;

	
	free(tmp);
}
	

void insert(struct node** head, Process* process)
{
		
	struct node* new_node = (struct node*) malloc(sizeof(struct node));
 
    struct node *last = *head;  
  
    new_node->process  = process;
 
    new_node->next = NULL;
 
    if (*head == NULL)
    {
       *head = new_node;
       return;
    } 

    while (last->next != NULL)
        last = last->next;
	
	
    last->next = new_node;
    return;   
}