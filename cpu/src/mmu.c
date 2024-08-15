#include "mmu.h"

int mmu(int logical_address, int page_size)
{
    uint32_t page_number = (uint32_t)floor(logical_address / page_size);
    uint32_t physical_displacement = logical_address - page_number * page_size;

    return physical_displacement;
}

