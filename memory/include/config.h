#ifndef MEMORY_CONFIG_H_
#define MEMORY_CONFIG_H_

#include "utils.h"

typedef struct
{
  t_config *config;
  char *listening_ip;
  char *listening_port;
  char *filesystem_ip;
  char *filesystem_port;
  int memory_size;
  int pagination_size;
  char *instruction_path;
  int response_delay;
  int replacement_algorithm;
} t_memory_config;

t_memory_config *create_memory_config(char *path_a_config);

void destroy_memory_config(t_memory_config *memory_config);

#endif