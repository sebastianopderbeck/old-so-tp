#include "config.h"

t_cpu_config *create_cpu_config(char *path_a_config)
{
  t_cpu_config *cpu_config = malloc(sizeof(t_cpu_config));
  cpu_config->config = config_create(path_a_config);
  cpu_config->memory_ip = config_get_string_value(cpu_config->config, "MEMORY_IP");
  cpu_config->memory_port = config_get_string_value(cpu_config->config, "MEMORY_PORT");
  cpu_config->listening_ip = config_get_string_value(cpu_config->config, "LISTENING_IP");
  cpu_config->listen_dispatch_port = config_get_string_value(cpu_config->config, "LISTEN_DISPATCH_PORT");
  cpu_config->listen_interrupt_port = config_get_string_value(cpu_config->config, "LISTEN_INTERRUPT_PORT");

  return cpu_config;
}

void destroy_cpu_config(t_cpu_config *cpu_config)
{
  // config_destroy(cpu_config -> config); is necessary?
  free((void *)cpu_config->memory_ip);
  free((void *)cpu_config->memory_port);
  free((void *)cpu_config->listening_ip);
  free((void *)cpu_config->listen_dispatch_port);
  free((void *)cpu_config->listen_interrupt_port);
  free((void *)cpu_config);
}