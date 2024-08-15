#ifndef KERNEL_QUEUES_H_
#define KERNEL_QUEUES_H_

#include <stdlib.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <semaphore.h>

#include "utils.h"
#include "sockets.h"
#include "kernel.h"
#include "process.h"
#include "semaphores.h"
#include "deadlocks.h"

extern t_queue *queue_new, *queue_ready, *queue_exec;

extern t_list *all_resources;
extern t_list *all_resources_instances;
extern t_list *list_of_queue_blocked;

extern sem_t *mutex_queue_new, *mutex_queue_ready, *mutex_queue_blocked, *mutex_queue_exec;

void init_semaphores_queues();

void init_queues();

void destroy_queues();

void destroy_semaphores_queues();

t_list *build_all_queues_in_list();

t_list *build_all_mutex_queues_in_list();

t_pcb *find_process_from_queue(t_queue *queue_from, int pid);

void finish_process(uint32_t pid);

void check_process_in_blocked_and_send_to_ready();

void update_queue_semaphores(uint8_t state);

void move_process_to_new(t_pcb *process_to_move);

void move_process_to_ready();

void move_process_to_exec();

void sort_queue();

void move_process_from_execution_to_ready();

void dispatch_process_to_cpu(t_pcb *process_to_move, uint8_t header);

void handle_interruption();

void move_process_from_exec_to_blocked(t_queue *queue_blocked);

void move_process_from_block_to_ready(t_pcb *process_to_move, t_queue *queue_blocked);

t_queue *get_queue_blocked(char *queue_tag);

#endif