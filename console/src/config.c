#include "config.h"

t_console_config *create_console_config(char *path_a_config)
{
    t_console_config *console_config = malloc(sizeof(t_console_config));
    console_config->config = config_create(path_a_config);
    console_config->kernel_ip = config_get_string_value(console_config->config, "KERNEL_IP");
    console_config->kernel_port = config_get_string_value(console_config->config, "KERNEL_PORT");
    return console_config;
}

void destroy_console_config(t_console_config *console_config)
{
    // config_destroy(console_config -> config); is necessary?
    free((void *)console_config->kernel_ip);
    free((void *)console_config->kernel_port);
    free((void *)console_config);
}