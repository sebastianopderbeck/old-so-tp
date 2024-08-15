#ifndef CPU_SEMAPHORES_CPU_H_
#define CPU_SEMAPHORES_CPU_H_

#include "config.h"
#include "cpu.h"

extern sem_t *mutex_interrupt, *mutex_dispatch_connection;

void init_semaphores_cpu();

void destroy_semaphores();

#endif