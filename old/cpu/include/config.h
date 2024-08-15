#ifndef CPU_CONFIG_H_
#define CPU_CONFIG_H_

#include "utils.h"
#include "config.h"

typedef struct
{
  t_config *config;
  char *memory_ip;
  char *memory_port;
  char *listening_ip;
  char *listen_dispatch_port;
  char *listen_interrupt_port;
} t_cpu_config;

t_cpu_config *create_cpu_config(char *path_a_config);

void destroy_cpu_config(t_cpu_config *cpu_config);

#endif