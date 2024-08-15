#include "config.h"

t_memory_config *create_memory_config(char *path_a_config)
{
  t_memory_config *memory_config = malloc(sizeof(t_memory_config));
  memory_config->config = config_create(path_a_config);
  memory_config->listening_ip = config_get_string_value(memory_config->config, "LISTENING_IP");
  memory_config->listening_port = config_get_string_value(memory_config->config, "LISTENING_PORT");
  memory_config->filesystem_ip = config_get_string_value(memory_config->config, "FILESYSTEM_IP");
  memory_config->filesystem_port = config_get_string_value(memory_config->config, "FILESYSTEM_PORT");
  memory_config->memory_size = config_get_int_value(memory_config->config, "MEMORY_SIZE");
  memory_config->pagination_size = config_get_int_value(memory_config->config, "PAGINATION_SIZE");
  memory_config->instruction_path = config_get_string_value(memory_config->config, "INSTRUCTION_PATH");
  memory_config->response_delay = config_get_int_value(memory_config->config, "RESPONSE_DELAY");
  memory_config->replacement_algorithm = config_get_int_value(memory_config->config, "REPLACEMENT_ALGORITHM");
  return memory_config;
}

void destroy_memory_config(t_memory_config *memory_config)
{
  // config_destroy(memory_config -> config); is necessary?
  free((void *)memory_config->listening_ip);
  free((void *)memory_config->listening_port);
  free((void *)memory_config->filesystem_ip);
  free((void *)memory_config->filesystem_port);
  free((void *)memory_config->instruction_path);
  free((void *)memory_config);
}