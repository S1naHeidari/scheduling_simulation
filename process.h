/**
 *  process.h
 *
 *  Full Name:
 *  Course section:
 *  Representation of a process in the system.
 *  
 */


#ifndef PROCESS_H
#define PROCESS_H

// representation of a a process
typedef struct process {
    int  pid;
    int  cpu_time;
    int  io_time;  
    int  arrival_time;
} Process;

#endif
