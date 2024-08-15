#include "config.h"

t_io_config *create_io_config(char *path_a_config)
{
  t_io_config *io_config = malloc(sizeof(t_io_config));
  io_config->config = config_create(path_a_config);
  io_config->memory_ip = config_get_string_value(io_config->config, "MEMORY_IP");
  io_config->memory_port = config_get_string_value(io_config->config, "MEMORY_PORT");
  io_config->kernel_ip = config_get_string_value(io_config->config, "KERNEL_IP");
  io_config->kernel_port = config_get_string_value(io_config->config, "KERNEL_PORT");
  // TODO CREATE OTHER CONFIGS
  return io_config;
}

void destroy_io_config(t_io_config *io_config)
{
  // config_destroy(io_config -> config); is necessary?
  free((void *)io_config->memory_ip);
  free((void *)io_config->memory_port);
  free((void *)io_config->kernel_ip);
  free((void *)io_config->kernel_port);
  free((void *)io_config);
}