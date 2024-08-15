#ifndef KERNEL_SEMAPHORES_H_
#define KERNEL_SEMAPHORES_H_

#include <semaphore.h>
#include "kernel.h"

extern sem_t *sem_process_waiting, *sem_process_new, *sem_process_ready, *mutex_process_exec, *sem_process_blocked, *sem_process_exit, *sem_cpu_available;

extern sem_t *mutex_cpu_dispatch_connection, *mutex_cpu_interrupt_connection, *mutex_memory_connection;

void init_semaphores_process(void);
void destroy_semaphores();
#endif