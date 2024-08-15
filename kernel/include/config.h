#ifndef KERNEL_CONFIG_H_
#define KERNEL_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>

typedef struct
{
    t_config *config;
    char *memory_ip;
    char *memory_port;
    char *listenning_port;
    char *cpu_ip;
    char *cpu_dispatch_port;
    char *cpu_interrupt_port;
    char *planning_algorithm;
    int quantum;
    char **resources;
    char **resources_instances;
    int multiprogramming_degree;
    char *console_ip;
    char *console_port;
} t_kernel_config;

t_kernel_config *create_kernel_config(char *path_a_config);

void destroy_kernel_config(t_kernel_config *kernel_config);

#endif