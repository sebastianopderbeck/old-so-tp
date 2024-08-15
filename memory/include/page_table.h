#ifndef MEMORY_PAGE_TABLE_H_
#define MEMORY_PAGE_TABLE_H_

#include "memory.h"

typedef struct
{
  int frame;
  int presence;
  int modified;
  int swap_position;
} t_page;

typedef struct
{
  int frame;
  int modified;
  int page_number;
  int pid;
} t_frame_page;


typedef struct
{
  t_list *pages;
  int pid;
} t_page_table;

void init_page_table_list();

t_page_table *create_page_table(int pid, t_list* swap_blocks);

t_page *create_page(int swap_position);

t_list* delete_page_table(t_page_table* page_table);

t_page_table *get_table_pages_by_pid(int pid);

void choose_victim(int page_number, int page_modified);

bool check_full();

bool check_modified(int page_modified);

void add_page_to_main_memory(t_page page_new);

void delete_page_from_main_memory(t_page page_new);

void update_victim(t_frame_page* frame_page);

void replace_page_fifo(t_page* new_page, int pid, int page_number);

void replace_page_lru_with_presence(int pid, int page_number);


#endif