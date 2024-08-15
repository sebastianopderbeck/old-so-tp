#ifndef FILESYSTEM_CONFIG_H_
#define FILESYSTEM_CONFIG_H_

#include "utils.h"
#include "config.h"

typedef struct {
    t_config* config;
    char* memory_ip;
    char* memory_port;
    char* listening_ip;   
    char* listening_port;   
    char* fat_path;   
    char* block_path;
    char* fcb_path;    
    int total_quantity_blocks;   
    int swap_quantity_blocks;   
    int block_size;   
    int block_access_delay;   
    int fat_access_delay;   
} t_filesystem_config;

t_filesystem_config* create_filesystem_config(char* path_a_config);

void destroy_filesystem_config(t_filesystem_config* filesystem_config);

#endif