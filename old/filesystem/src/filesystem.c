#include "filesystem.h"

t_log *logger;

pthread_t thread_filesystem;

t_filesystem_config *filesystem_config;

int filesystem_connection;

int memory_connection;

t_list *free_swap_blocks;
t_list *free_fat_blocks;

int main()
{
    free_swap_blocks = list_create();
    free_fat_blocks = list_create();

    filesystem_config = create_filesystem_config("configuration/filesystem.config");

    open_fat();

    logger = log_create("logs/filesystem.log", "FILESYSTEM", 1, LOG_LEVEL_TRACE);
    log_info(logger, "File system started");

    init_connections();

    // fcb = read_fcb(filesystem_config -> fcb_path, "prueba.fcb");

    // destroy_fcb(fcb);
    /*
        t_list *free_fat_blocks = list_create();
        t_list *free_swap_blocks = list_create();

        open_fat(filesystem_config -> fat_path, filesystem_config -> total_quantity_blocks, filesystem_config -> swap_quantity_blocks, free_fat_blocks, free_swap_blocks);

        FILE* fat_file = fopen("/home/utnso/fs/fat.dat", "wb");

        write_fat(free_fat_blocks, 4, 70, fat_file);

        fclose(fat_file);

        //create_fcb(filesystem_config -> fcb_path, "prueba.fcb");

        truncate_file("/home/utnso/fs/fcbs/prueba.fcb", "/home/utnso/fs/fat.dat", 0, 70, 64, filesystem_config -> block_size, free_fat_blocks);
    */

    destroy_socket(filesystem_connection);
    // destroy_socket(memory_connection);
    destroy_filesystem_config(filesystem_config);
    close_program(logger);

    return EXIT_SUCCESS;
}

void init_connections()
{
    pthread_create(&thread_filesystem, NULL, (void *)handle_filesystem_connection, NULL);
    pthread_join(thread_filesystem, NULL);
}

void *handle_filesystem_connection()
{
    filesystem_connection = create_connection(logger, "FILESYSTEM", filesystem_config->listening_ip, filesystem_config->listening_port, TYPE_SOCKET_SERVER);
    while (server_listen(logger, "FILESYSTEM", filesystem_connection, (void *)(*handler_connection)))
        ;
}
/*
void send_instruction(int client_socket){
    char* response = "INSTRUCTION RECEIVED";
    send(client_socket, &response, sizeof(string_length(response) + 1));
}*/

// FCB
/*
void create_fcb(char* path, char* file_name){

    string_append(&path, "/");
    string_append(&path, file_name);
    FILE *ptr = fopen(path, "w");
    fclose(ptr);

    t_config *fcb_config = malloc(sizeof(t_FCB));

    fcb_config = config_create(path);
    config_set_value(fcb_config, "NOMBRE_ARCHIVO", file_name);
    config_set_value(fcb_config, "TAMANIO_ARCHIVO", "0");
    //config_set_value(fcb_config, "BLOQUE_INICIAL", NULL);

    config_save_in_file(fcb_config, path);

    config_destroy(fcb_config);


}

t_FCB* read_fcb(char* fcb_path, char* fcb_name){

    char *full_path;
    //full_path = (char *)malloc(sizeof(char) * (64 + 1));

    full_path = fcb_path;
    //string_append(&full_path, fcb_path);
    string_append(&full_path, "/");
    string_append(&full_path, fcb_name);

    t_config* fcb_config = config_create("/home/utnso/fs/fcbs/prueba.fcb");

    t_FCB* fcb = malloc(sizeof(t_FCB));

    fcb -> file_name = config_get_string_value(fcb_config, "NOMBRE_ARCHIVO");
    fcb -> file_size = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");
    fcb -> start_block = config_get_int_value(fcb_config, "BLOQUE_INICIAL");

    config_destroy(fcb_config);

    return fcb;
}

void destroy_fcb(t_FCB* fcb) {
    //free(): double free detected in tcache 2
    free((void*) fcb -> file_name);
    free((void*) fcb);
}





//TRUNCATE


void truncate_file(char* fcb_path, char* fat_path, int file_size, int first_block, int truncate_size, int block_size, t_list* free_blocks){
    update_fcb_file_size(fcb_path, truncate_size);

    FILE* fat_file = fopen(fat_path, "rb+");

    int block_quantity = calculate_blocks(truncate_size, block_size);

    t_list* fat_directions = list_create();

    get_file_fat_directions(fat_directions, first_block, fat_file);

    if(truncate_size > file_size){
        int eof = list_remove(fat_directions, list_size(fat_directions) - 1);
        write_fat(free_blocks, block_quantity, eof, fat_file);
    } else if(truncate_size < file_size) {
        clear_blocks(block_quantity, free_blocks, fat_directions, fat_file);
    }

}

//LIBERO BLOQUE A BLOQUE, UNA VEZ LIBERADOS TODOS, SETEO EL NUEVO EOF
void clear_blocks(int block_quantity, t_list* free_blocks, t_list* fat_directions, FILE* fat_file){
    int fat_direction = 0;

    int list_index = list_size(fat_directions) - 1;

    uint32_t value = 0;

    for(int i=0; i < block_quantity; i++){
        fat_direction = list_remove(fat_directions, list_index - i);

        fseek(fat_file, fat_direction * sizeof(uint32_t), SEEK_SET);
        fwrite(&value, sizeof(uint32_t), 1, fat_file);

        list_add(free_blocks, fat_direction);
    }

    fat_direction = list_remove(fat_directions, list_index - block_quantity);
    fseek(fat_file, fat_direction * sizeof(uint32_t), SEEK_SET);

    value = UINT32_MAX;
    fwrite(&value, sizeof(uint32_t), 1, fat_file);

}

    //OBTENGO LOS PUNTEROS DE LA FAT EN FORMA DE LISTA
    void get_file_fat_directions(t_list* fat_directions, int first_block, FILE* fat_file){
        uint32_t fat_direction = 0;

        list_add(fat_directions, first_block);

        fseek(fat_file, first_block * sizeof(uint32_t), SEEK_SET);
        fread(&fat_direction, sizeof(uint32_t), 1, fat_file);

        while(fat_direction != UINT32_MAX){
            list_add(fat_directions, fat_direction);

            fseek(fat_file, fat_direction * sizeof(uint32_t), SEEK_SET);
            fread(&fat_direction, sizeof(uint32_t), 1, fat_file);
        }
    }

    //PRUEBA

    void write_fat(t_list* free_blocks, int blocks_quantity,int start_block, FILE* fat_file)
    {
        uint32_t value = 0;swap_position
            int value = list_remove(free_blocks, 0);
            fwrite(&value, sizeof(uint32_t), 1, fat_file);
            fseek(fat_file, value * sizeof(uint32_t), SEEK_SET);
        }

        value = UINT32_MAX;
        fwrite(&value, sizeof(uint32_t), 1, fat_file);
    }

    int calculate_blocks(int truncate_size, int block_size){
        return (truncate_size + block_size - 1) / block_size;
    }

    void update_fcb_file_size(char* fcb_path, int truncate_size){
    char* config_truncate_size = string_itoa(truncate_size);
    t_config* fcb_config = malloc(sizeof(t_FCB));

    fcb_config = config_create(fcb_path);

    int file_size = config_get_int_value(fcb_config, "TAMANIO_ARCHIVO");

    config_set_value(fcb_config, "TAMANIO_ARCHIVO", config_truncate_size);
    config_save(fcb_config);

    config_destroy(fcb_config);
    }


*/
void read_swap(int pointer, uint32_t value)
{
    FILE *blocks_file = fopen(filesystem_config->block_path, "rb");

    fseek(blocks_file, pointer, SEEK_SET);
    fread(&value, filesystem_config->block_size, 1, blocks_file);

    fclose(blocks_file);
}

void write_swap(int pointer, uint32_t value)
{
    FILE *blocks_file = fopen(filesystem_config->block_path, "wb");

    fseek(blocks_file, pointer, SEEK_SET);
    fwrite(&value, filesystem_config->block_size, 1, blocks_file);

    fclose(blocks_file);
}

// SE ABRE LA TABLA FAT, SI NO EXISTE SE CREA Y SE INICIALIZA EN 0
void open_fat()
{
    FILE *fat_file = fopen(filesystem_config->block_path, "wb");
    // if(fat_file == NULL){
    int fat_quantity = filesystem_config->total_quantity_blocks - filesystem_config->swap_quantity_blocks;
    uint32_t value = 0;
    for (int i = 0; i < fat_quantity; i++)
    {
        fwrite(&value, sizeof(uint32_t), 1, fat_file);
    }

    initialize_free_blocks(0, filesystem_config->swap_quantity_blocks, free_swap_blocks);
    initialize_free_blocks(1, fat_quantity, free_fat_blocks);
    //}
    fclose(fat_file);
}

void initialize_free_blocks(int start, int quantity, t_list *free_blocks)
{

    // EMPIEZA DE 1 PORQUE 0 ES BLOQUE RESERVADO
    for (int i = start; i < quantity; i++)
    {
        list_add(free_blocks, i);
    }
}