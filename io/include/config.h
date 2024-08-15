#ifndef IO_CONFIG_H_
#define IO_CONFIG_H_

#include "utils.h"
#include "config.h"

typedef struct
{
  t_config *config;
  char *memory_ip;
  char *memory_port;
  char *kernel_ip;
  char *kernel_port;
  //TODO: CREATE OTHERS CONFIGS
} t_io_config;

t_io_config *create_io_config(char *path_a_config);

void destroy_io_config(t_io_config *io_config);

#endif