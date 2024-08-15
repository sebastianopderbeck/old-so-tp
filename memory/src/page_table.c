#include "page_table.h"

void init_page_table_list()
{
  page_table_list = list_create();
  frames_used_list = list_create();
}

t_page_table *create_page_table(int pid, t_list *swap_blocks)
{
  t_page_table *page_table = malloc(sizeof(t_page_table));
  page_table->pages = list_create();
  page_table->pid = pid;

  int page_quantity = list_size(swap_blocks);

  for (int i = 0; i < page_quantity; i++)
  {
    int swap_pos = list_get(swap_blocks, i);
    swap_pos = swap_pos * memory_config->pagination_size;
    list_add(page_table->pages, create_page(swap_pos));
  }

  return page_table;
}

t_page *create_page(int swap_position)
{
  t_page *page_table = malloc(sizeof(t_page));
  page_table->frame = NULL;
  page_table->presence = 0;
  page_table->modified = 0;
  page_table->swap_position = swap_position;
  return page_table;
}

t_list *delete_page_table(t_page_table *page_table)
{
  t_list *swap_positions = list_create();
  t_page *page = malloc(sizeof(t_page));
  for (int i = 0; i < list_size(page_table->pages); i++)
  {
    page = list_get(page_table->pages, i);

    if (page->presence == 1)
    {
    }

    list_add(swap_positions, page->swap_position);
    free(page);
  }
  free(page_table);
  return swap_positions;
}

t_page_table *get_table_pages_by_pid(int pid)
{
  t_page_table *page_table = malloc(sizeof(t_page_table));
  for (int i = 0; i < list_size(page_table_list); i++)
  {
    page_table = list_get(page_table_list, i);
    if (page_table->pid == pid)
    {
      return page_table;
    }
  }
}

void replace_page_fifo(t_page *new_page, int pid, int page_number)
{
  t_frame_page *page_to_get_replaced = list_get(frames_used_list, 0);

  t_frame_page *new_frame_page = malloc(sizeof(t_frame_page));

  if (check_modified(page_to_get_replaced->modified))
  {
    uint32_t value = 0;
    memcpy(&value, main_memory + (page_to_get_replaced->frame * memory_config->pagination_size), sizeof(uint32_t));
  }
  update_victim(page_to_get_replaced);

  new_page->frame = page_to_get_replaced->frame;
  new_page->presence = 1;

  new_frame_page->frame = page_to_get_replaced->frame;
  new_frame_page->modified = 0;
  new_frame_page->page_number = page_number;
  new_frame_page->pid = pid;

  // desharcodear page out
  // log_info(logger, "Remplazo - Marco %d - Page Out: %d - %d - Page Out: %d - %d", page_to_remplace->frame, 1, 1);

  list_remove(frames_used_list, 0);
  list_add(frames_used_list, new_frame_page);
  free(new_frame_page);
}

void update_victim(t_frame_page *frame_page)
{
  t_page_table *page_table = get_table_pages_by_pid(frame_page->pid);

  t_page *page = list_remove(page_table->pages, frame_page->page_number);
  page->modified = 0;
  page->presence = 0;
  list_add(page, frame_page->page_number);
}

void replace_page_lru_with_presence(int pid, int page_number)
{
  int frame_size = list_size(frames_used_list);
  int i = 0;
  t_frame_page *frame_page = malloc(sizeof(t_frame_page));

  do
  {

    frame_page = list_get(frames_used_list, i);

    if ((pid == frame_page->pid) && (page_number == frame_page->page_number))
    {
      frame_page = list_remove(frames_used_list, i);
    }
    else
    {
      i++;
    }

  } while (list_size(frames_used_list) == frame_size);

  list_add(frames_used_list, frame_page);
  free(frame_page);
}

bool check_modified(int page_modified)
{
  return page_modified == 1;
}

void add_page_to_main_memory(t_page page_new)
{
  // paginasEnMp = list_add(paginasEnMp, page_new);
}
void delete_page_from_main_memory(t_page page_new)
{
  // paginasEnMp = list_add(paginasEnMp, page_new);
}

bool check_full()
{
  int maximum_frame_list_length = memory_config->memory_size / memory_config->pagination_size;
  return list_size(frames_used_list) == maximum_frame_list_length;
}

void delete_from_frame_list(int frame){
  t_frame_page* frame_page = malloc(sizeof(t_frame_page));

  for(int i=0; i<list_size(frames_used_list); i++){
    frame_page = (list_get(frames_used_list, i));
    if(frame_page->frame == frame){
      list_remove(frames_used_list, frame);
    }
  }
}
