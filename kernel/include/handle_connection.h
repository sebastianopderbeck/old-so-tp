#ifndef KERNEL_HANDLE_CONNECTION_H_
#define KERNEL_HANDLE_CONNECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <semaphore.h>
#include <pthread.h>

#include "sockets.h"
#include "serialization.h"
#include "operation_handler.h"
#include "process.h"

#include "kernel.h"
#include "semaphores.h"
#include "queues.h"
#include "planners.h"
typedef struct
{
  t_pcb *process;
  int sleep_time;
} t_thread_args;

extern uint32_t id_to_assign;

void *thread_sleep_process(void *arg);

void block_process(t_pcb *process_to_move, int time_to_sleep);

void handler_connection(void *args);

void handle_dispatch_connection(void *args);

void handle_interrupt_connection(void *args);

void modify_multiprogramming_degree(int socket);

void show_all_processes();

void handle_sleep_interruption(t_pcb *process, int time_to_sleep);

void handle_end_of_quantum_interruption();

void handle_priority_interruption();

void handle_page_fault_interruption();

void *thread_page_fault_process(t_pcb *process_to_move);

void block_process_by_page_fault(t_pcb *process_to_move);

t_pcb *create_pcb(t_process *process, int socket);

t_pcb *receive_pcb_and_create(int socket);

#endif