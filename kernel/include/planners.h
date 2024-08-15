#ifndef KERNEL_PLANNERS_H_
#define KERNEL_PLANNERS_H_

#include <pthread.h>
#include <commons/log.h>

#include "kernel.h"
#include "queues.h"
#include "semaphores.h"
#include "operation_handler.h"

extern pthread_t thread_short_term, thread_long_term;

extern sem_t *sem_multiprogrammming_degree;
extern sem_t *sem_short_term_planner, *sem_long_term_planner;
extern sem_t *mutex_stopping_planner;

void init_semaphores_planners();

void init_planners();

void destroy_semaphores_planners();

void long_term_planner();

void short_term_planner();

void planing_with_fifo();

void planning_with_round_robin();

void planning_with_priorities();

void interrupt_process(uint8_t operation);

#endif