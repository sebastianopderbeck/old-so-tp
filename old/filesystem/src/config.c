#include "config.h"

t_filesystem_config* create_filesystem_config(char* path_a_config) {
    t_filesystem_config* filesystem_config = malloc(sizeof(t_filesystem_config)); 
    filesystem_config -> config = config_create(path_a_config);
    filesystem_config -> memory_ip = config_get_string_value(filesystem_config -> config, "MEMORY_IP");
    filesystem_config -> memory_port = config_get_string_value(filesystem_config -> config, "MEMORY_PORT");
    filesystem_config -> listening_ip = config_get_string_value(filesystem_config -> config, "LISTENING_IP");
    filesystem_config -> listening_port = config_get_string_value(filesystem_config -> config, "LISTENING_PORT");
    filesystem_config -> fat_path = config_get_string_value(filesystem_config -> config, "FAT_PATH");
    filesystem_config -> block_path = config_get_string_value(filesystem_config -> config, "BLOCK_PATH");
    filesystem_config -> fcb_path = config_get_string_value(filesystem_config -> config, "FCB_PATH");
    filesystem_config -> total_quantity_blocks = config_get_int_value(filesystem_config -> config, "TOTAL_QUANTITY_BLOCKS");
    filesystem_config -> swap_quantity_blocks = config_get_int_value(filesystem_config -> config, "SWAP_QUANTITY_BLOCKS");
    filesystem_config -> block_size = config_get_int_value(filesystem_config -> config, "BLOCK_SIZE");
    filesystem_config -> block_access_delay = config_get_int_value(filesystem_config -> config, "BLOCK_ACCESS_DELAY");
    filesystem_config -> fat_access_delay = config_get_int_value(filesystem_config -> config, "FAT_ACCESS_DELAY");
    return filesystem_config;
}

void destroy_filesystem_config(t_filesystem_config* filesystem_config) {
    // config_destroy(filesystem_config -> config); is necessary?
    free((void*) filesystem_config -> memory_ip);
    free((void*) filesystem_config -> memory_port);
    free((void*) filesystem_config -> listening_ip);   
    free((void*) filesystem_config -> listening_port);   
    free((void*) filesystem_config -> fat_path);
    free((void*) filesystem_config -> block_path);  
    free((void*) filesystem_config -> fcb_path);  
    free((void*) filesystem_config);   
}