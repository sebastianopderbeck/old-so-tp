#include "kernel.h"

sem_t *mutex_cpu_interrupt_connection;
sem_t *mutex_cpu_dispatch_connection;
sem_t *mutex_memory_connection;
sem_t *sem_process_waiting;
sem_t *sem_process_new;
sem_t *sem_process_ready;
sem_t *mutex_process_exec;
sem_t *sem_process_blocked;
sem_t *sem_process_exit;
sem_t *sem_cpu_available;

t_queue *queue_new;
t_queue *queue_ready;
t_queue *queue_exec;

t_list *all_resources;
t_list *list_of_queue_blocked;
t_list *all_resources_instances;

sem_t *mutex_queue_new;
sem_t *mutex_queue_ready;
sem_t *mutex_queue_blocked;
sem_t *mutex_queue_exec;

pthread_t thread_short_term;
pthread_t thread_long_term;

sem_t *sem_multiprogrammming_degree;
sem_t *sem_short_term_planner;
sem_t *sem_long_term_planner;
sem_t *mutex_stopping_planner;

int memory_connection;
int filesystem_connection;
int cpu_dispatch_connection;
int cpu_interrupt_connection;
int console_connection;

pthread_t thread_cpu_dispatch;
pthread_t thread_console;
pthread_t thread_cpu_interrupt;

t_kernel_config *kernel_config;
t_log *logger;

uint32_t id_to_assign;

int main()
{
	kernel_config = create_kernel_config("configuration/kernel.config");

	logger = log_create("logs/kernel.log", "KERNEL", 1, LOG_LEVEL_TRACE);
	log_info(logger, "Kernel started");

	init_semaphores_queues();
	init_queues();
	init_resources();

	init_semaphores_process();

	init_semaphores_planners();
	init_planners();

	// TODO: connection dispatch e interrupt for memory
	memory_connection = create_connection(logger, "MEMORY", kernel_config->memory_ip, kernel_config->memory_port, TYPE_SOCKET_CLIENT);
	send_header(memory_connection, KERNEL);

	cpu_dispatch_connection = create_connection(logger, "CPU DISPATCH", kernel_config->cpu_ip, kernel_config->cpu_dispatch_port, TYPE_SOCKET_CLIENT);
	send_header(cpu_dispatch_connection, KERNEL);

	cpu_interrupt_connection = create_connection(logger, "CPU INTERRUPT", kernel_config->cpu_ip, kernel_config->cpu_interrupt_port, TYPE_SOCKET_CLIENT);
	send_header(cpu_interrupt_connection, KERNEL);

	init_connections();

	destroy_queues();
	destroy_resources();
	destroy_semaphores();
	destroy_semaphores_queues();
	destroy_semaphores_planners();
	destroy_socket(memory_connection);
	destroy_socket(console_connection);
	destroy_socket(filesystem_connection);
	destroy_socket(cpu_dispatch_connection);
	destroy_socket(cpu_interrupt_connection);
	destroy_kernel_config(kernel_config);

	close_program(logger);
	return EXIT_SUCCESS;
}

void *handle_console_connection()
{
	console_connection = create_connection(logger, "KERNEL_CONSOLE", kernel_config->console_ip, kernel_config->console_port, TYPE_SOCKET_SERVER);
	while (server_listen(logger, "KERNEL_CONSOLE", console_connection, (void *)(*handler_connection)))
		;
}

void init_connections()
{
	// TODO: Add connections with memory and i/o
	pthread_create(&thread_cpu_dispatch, 0, (void *)handle_dispatch_connection, NULL);
	pthread_detach(thread_cpu_dispatch);

	pthread_create(&thread_cpu_interrupt, 0, (void *)handle_interrupt_connection, NULL);
	pthread_detach(thread_cpu_interrupt);

	pthread_create(&thread_console, NULL, (void *)handle_console_connection, NULL);
	pthread_join(thread_console, NULL);
}
