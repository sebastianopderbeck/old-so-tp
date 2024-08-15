#ifndef KERNEL_DEADLOCKS_H_
#define KERNEL_DEADLOCKS_H_

#include <commons/collections/queue.h>
#include <commons/collections/list.h>

#include "kernel.h"
#include "queues.h"

void init_resources();

void destroy_resources();

void check_deadlocks(t_pcb *process_to_check);

char *get_required_resource(t_pcb *process_to_check);

void add_resource_instance(char *resource_to_remove);

void remove_resource_instance(char *resource_to_remove);

void _modify_resource_instance(char *resource_to_remove, int quantity_to_add);

char *find_resource(t_list *list_to_find, char *resource_requested);

int quantity_resource_instance_available(char *resource);

int quantity_resource_instance_retained(t_pcb *process, char *resource);

void log_deadlock(t_pcb *process_to_check);

t_list *char_array_to_list(char **resource);

void free_resources_retained(t_pcb *process);

void handle_signal_interruption(t_pcb *process, char *resource_requested);

void handle_wait_interruption(t_pcb *process, char *resource_requested);

bool finish_process_if_is_not_valid(t_pcb *process, char *resource_requested);

#endif