#include "semaphores.h"

void init_semaphores_process(void)
{
	sem_process_new = malloc(sizeof(sem_t));
	sem_init(sem_process_new, 0, 0);

	sem_process_ready = malloc(sizeof(sem_t));
	sem_init(sem_process_ready, 0, 0);

	// sem_process_waiting = malloc(sizeof(sem_t));
	// sem_init(sem_process_waiting, 0, 0);

	sem_process_blocked = malloc(sizeof(sem_t));
	sem_init(sem_process_blocked, 0, 0);

	mutex_process_exec = malloc(sizeof(sem_t));
	sem_init(mutex_process_exec, 0, 1);

	sem_cpu_available = malloc(sizeof(sem_t));
	sem_init(sem_cpu_available, 0, 1);

	sem_process_exit = malloc(sizeof(sem_t));
	sem_init(sem_process_exit, 0, 0);

	mutex_cpu_dispatch_connection = malloc(sizeof(sem_t));
	sem_init(mutex_cpu_dispatch_connection, 0, 1);

	mutex_cpu_interrupt_connection = malloc(sizeof(sem_t));
	sem_init(mutex_cpu_interrupt_connection, 0, 1);

	mutex_memory_connection = malloc(sizeof(sem_t));
	sem_init(mutex_memory_connection, 0, 1);
}

void destroy_semaphores()
{
	free(sem_process_new);
	free(sem_process_ready);
	// free(sem_process_waiting);
	free(sem_process_blocked);
	free(mutex_process_exec);
	free(sem_process_exit);
	free(sem_cpu_available);

	free(mutex_cpu_dispatch_connection);
	free(mutex_cpu_interrupt_connection);
}