#include "config.h"

t_kernel_config *create_kernel_config(char *path_a_config)
{
  t_kernel_config *kernel_config = malloc(sizeof(t_kernel_config));
  kernel_config->config = config_create(path_a_config);
  kernel_config->memory_ip = config_get_string_value(kernel_config->config, "MEMORY_IP");
  kernel_config->memory_port = config_get_string_value(kernel_config->config, "MEMORY_PORT");
  kernel_config->listenning_port = config_get_string_value(kernel_config->config, "LISTENNING_PORT");
  kernel_config->cpu_ip = config_get_string_value(kernel_config->config, "CPU_IP");
  kernel_config->cpu_dispatch_port = config_get_string_value(kernel_config->config, "CPU_DISPATCH_PORT");
  kernel_config->cpu_interrupt_port = config_get_string_value(kernel_config->config, "CPU_INTERRUPT_PORT");
  kernel_config->planning_algorithm = config_get_string_value(kernel_config->config, "PLANNING_ALGORITHM");
  kernel_config->quantum = config_get_int_value(kernel_config->config, "QUANTUM");
  kernel_config->resources = config_get_array_value(kernel_config->config, "RESOURCES");
  kernel_config->resources_instances = config_get_array_value(kernel_config->config, "RESOURCES_INSTANCES");
  kernel_config->multiprogramming_degree = config_get_int_value(kernel_config->config, "MULTIPROGRAMMING_DEGREE");
  kernel_config->console_ip = config_get_string_value(kernel_config->config, "CONSOLE_IP");
  kernel_config->console_port = config_get_string_value(kernel_config->config, "CONSOLE_PORT");

  return kernel_config;
}
void destroy_kernel_config(t_kernel_config *kernel_config)
{
  // config_destroy(kernel_config->config); // is necessary?
  free((void *)kernel_config->memory_ip);
  free((void *)kernel_config->memory_port);
  free((void *)kernel_config->listenning_port);
  free((void *)kernel_config->cpu_ip);
  free((void *)kernel_config->cpu_dispatch_port);
  free((void *)kernel_config->cpu_interrupt_port);
  free((void *)kernel_config->planning_algorithm);
  free((void *)kernel_config->resources);
  free((void *)kernel_config->resources_instances);
  free((void *)kernel_config->console_ip);
  free((void *)kernel_config->console_port);
  free((void *)kernel_config);
}