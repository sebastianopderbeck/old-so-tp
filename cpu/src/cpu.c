#include "cpu.h"

t_log *logger;

t_cpu_config *cpu_config;

pthread_t thread_dispatch, thread_interrupt, thread_cpu_memory;
int dispatch_connection;
int interrupt_connection;
int memory_connection;

sem_t *mutex_interrupt;
sem_t *mutex_dispatch_connection;

bool has_interrupt;

uint32_t page_size;

int main(void)
{
  cpu_config = create_cpu_config("configuration/cpu.config");

  logger = log_create("logs/cpu.log", "CPU", 1, LOG_LEVEL_TRACE);
  log_info(logger, "CPU started");

  memory_connection = create_connection(logger, "MEMORY", cpu_config->memory_ip, cpu_config->memory_port, TYPE_SOCKET_CLIENT);
  send_header(memory_connection, CPU);
  
  init_semaphores_cpu();

  init_connections();

  get_memory_config();

  destroy_socket(memory_connection);
  destroy_socket(dispatch_connection);
  destroy_socket(interrupt_connection);
  destroy_cpu_config(cpu_config);

  destroy_semaphores();

  close_program(logger);
  return EXIT_SUCCESS;
}

void *handle_dispatch_connection()
{
  dispatch_connection = create_connection(logger, "CPU DISPATCH", cpu_config->listening_ip, cpu_config->listen_dispatch_port, TYPE_SOCKET_SERVER);
  while (server_listen(logger, "DISPATCH CONNECTION", dispatch_connection, (void *)(*handler_connection)))
    ;
}

void *handle_interrupt_connection()
{
  interrupt_connection = create_connection(logger, "CPU INTERRUPT", cpu_config->listening_ip, cpu_config->listen_interrupt_port, TYPE_SOCKET_SERVER);
  while (server_listen(logger, "INTERRUPT CONNECTION", interrupt_connection, (void *)(*handler_connection)))
    ;
}

void init_connections()
{
  pthread_create(&thread_interrupt, NULL, (void *)handle_interrupt_connection, NULL);
  pthread_create(&thread_dispatch, NULL, (void *)handle_dispatch_connection, NULL);
  pthread_join(thread_dispatch, NULL);
  pthread_join(thread_interrupt, NULL);
  /*
   pthread_create(&thread_cpu_memory, NULL, (void *)handle_memory_connection, NULL);
   pthread_join(thread_cpu_memory, NULL);
   */
}

void get_memory_config(void)
{
  send_header(memory_connection, CONNECTION_CPU_MEMORY);
  omit_header(memory_connection);

  t_buffer *payload = receive_payload(memory_connection);
  page_size = take_UINT32_from_buffer(payload);
  destroy_buffer(payload);

  log_info(logger, "Configuration received from memory with page size (%d)", page_size);
}
