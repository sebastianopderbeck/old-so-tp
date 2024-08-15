#ifndef CONSOLE_CONFIG_H_
#define CONSOLE_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>

typedef struct
{
    t_config *config;
    char *kernel_ip;
    char *kernel_port;
} t_console_config;

t_console_config *create_console_config(char *path_a_config);

void destroy_console_config(t_console_config *console_config);

#endif