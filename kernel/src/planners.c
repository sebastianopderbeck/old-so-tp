#include "planners.h"

void init_semaphores_planners()
{
	sem_short_term_planner = malloc(sizeof(sem_t));
	sem_init(sem_short_term_planner, 0, 0);

	sem_long_term_planner = malloc(sizeof(sem_t));
	sem_init(sem_long_term_planner, 0, 0);

	sem_multiprogrammming_degree = malloc(sizeof(sem_t));
	sem_init(sem_multiprogrammming_degree, 0, kernel_config->multiprogramming_degree);

	mutex_stopping_planner = malloc(sizeof(sem_t));
	sem_init(mutex_stopping_planner, 0, 1);
}

void init_planners()
{
	pthread_create(&thread_short_term, 0, (void *)short_term_planner, NULL);
	pthread_detach(thread_short_term);

	pthread_create(&thread_long_term, 0, (void *)long_term_planner, NULL);
	pthread_detach(thread_long_term);
}

void destroy_semaphores_planners()
{
	free(sem_short_term_planner);
	free(sem_long_term_planner);
	free(sem_multiprogrammming_degree);
}

void long_term_planner()
{
	while (1)
	{
		sem_wait(sem_long_term_planner);
		sem_wait(sem_multiprogrammming_degree);

		sem_wait(sem_process_new);
		move_process_to_ready();
		sem_post(sem_process_ready);

		sem_post(sem_long_term_planner);
	}
}

void short_term_planner()
{
	// TODO: remove priorities and do VRR
	if (strcmp(kernel_config->planning_algorithm, "FIFO") == 0)
	{
		planing_with_fifo();
	}
	else if (strcmp(kernel_config->planning_algorithm, "ROUND_ROBIN") == 0)
	{
		planning_with_round_robin();
	}
	else if (strcmp(kernel_config->planning_algorithm, "PRIORITIES") == 0)
	{
		planning_with_priorities();
	}
}

void planing_with_fifo()
{
	while (1)
	{
		sem_wait(sem_short_term_planner);
		sem_wait(sem_process_ready);
		sem_wait(sem_cpu_available);

		log_info(logger, "Sending process to execute...");
		sem_wait(mutex_process_exec);
		move_process_to_exec();
		sem_post(mutex_process_exec);

		sem_post(sem_short_term_planner);
	}
}

void planning_with_round_robin()
{
	while (1)
	{
		sem_wait(sem_short_term_planner);
		sem_wait(sem_process_ready);
		sem_wait(sem_cpu_available);

		log_info(logger, "Sending process to execute...");
		sem_wait(mutex_process_exec);
		move_process_to_exec();
		sem_post(mutex_process_exec);

		log_trace(logger, "APLYING QUANTUM");
		usleep(kernel_config->quantum);
		log_trace(logger, "REPLANING");

		sem_wait(mutex_queue_exec);
		interrupt_process(INTERRUPT_QUANTUM);
		sem_post(mutex_queue_exec);

		// TEST: For interrupt by quantum, uncomment the next line
		// move_process_from_execution_to_ready();
		sem_post(sem_short_term_planner);
	}
}

void planning_with_priorities()
{
	while (1)
	{
		sem_wait(sem_short_term_planner);
		sem_wait(sem_process_ready);
		sem_wait(sem_cpu_available);

		log_trace(logger, "SORTING QUEUE...");
		sort_queue();

		log_info(logger, "Sending process to execute...");
		sem_wait(mutex_process_exec);
		move_process_to_exec();
		sem_post(mutex_process_exec);

		/*
		sem_wait(mutex_queue_exec);
		interrupt_process(INTERRUPT_PRIORITY);
		sem_post(mutex_queue_exec);
		*/
		sem_post(sem_short_term_planner);
	}
}

void interrupt_process(uint8_t operation)
{
	sem_wait(mutex_cpu_interrupt_connection);
	send_header(cpu_interrupt_connection, operation);
	sem_post(mutex_cpu_interrupt_connection);
	log_info(logger, "Sending process due to interruption");

	// sem_wait(mutex_cpu_interrupt_connection);
	// receive_message(cpu_interrupt_connection, &message);
	// sem_post(mutex_cpu_interrupt_connection);
	// log_info(logger, "Proceso recibido con motivo de desalojo: %s", message);
}