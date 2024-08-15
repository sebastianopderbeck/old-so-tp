#include "semaphores.h"

void init_semaphores_cpu()
{
	mutex_interrupt = malloc(sizeof(sem_t));
	sem_init(mutex_interrupt, 0, 1);

	mutex_dispatch_connection = malloc(sizeof(sem_t));
	sem_init(mutex_dispatch_connection, 0, 1);
}

void destroy_semaphores()
{
	free(mutex_interrupt);
	free(mutex_dispatch_connection);
}