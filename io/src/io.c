#include "io.h"

t_log *logger;

t_io_config *io_config;

int kernel_connection;
int memory_connection;

int main(void)
{
  io_config = create_io_config("configuration/io.config");

  logger = log_create("logs/io.log", "IO", 1, LOG_LEVEL_TRACE);
  log_info(logger, "IO started");

  memory_connection = create_connection(logger, "MEMORY", io_config->memory_ip, io_config->memory_port, TYPE_SOCKET_CLIENT);
  send_header(memory_connection, IO);
  
  kernel_connection = create_connection(logger, "KERNEL", io_config->kernel_ip, io_config->kernel_port, TYPE_SOCKET_CLIENT);
  send_header(kernel_connection, IO);

  destroy_socket(memory_connection);
  destroy_socket(kernel_connection);
  destroy_io_config(io_config);

  close_program(logger);
  return EXIT_SUCCESS;
}
