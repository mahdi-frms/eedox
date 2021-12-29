#include <prog.h>
#include <util.h>
#include <paging.h>
#include <elf.h>

int32_t prog_load(const char *file, uint32_t laddr, uint32_t *entry)
{
    Elf32_Ehdr elf_header = *(Elf32_Ehdr *)file;
    Elf32_Phdr *prog_arr = (Elf32_Phdr *)(file + elf_header.e_phoff);
    uint32_t prog_arrlen = elf_header.e_phnum;
    for (uint32_t i = 1; i < prog_arrlen; i++)
    {
        uint32_t start = prog_arr[i].p_vaddr;
        uint32_t end = prog_arr[i].p_vaddr + prog_arr[i].p_memsz - 1;
        if (start == 0)
            continue;
        if (start < laddr)
        {
            return -1;
        }
        for (uint32_t i = start; i < end; i += 0x1000)
        {
            page_t *page = get_page(i, 0, current_page_directory);
            if (!page->frame)
            {
                alloc_frame(page, 1, 0);
            }
        }
        const char *data = file + prog_arr[i].p_offset;
        memcpy((void *)start, data, prog_arr[i].p_memsz);
    }
    *entry = elf_header.e_entry;
    return 0;
}