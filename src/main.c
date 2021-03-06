#include <stdint.h>
#include <terminal.h>
#include <gdt.h>
#include <asm.h>
#include <kheap.h>
#include <paging.h>
#include <bitset.h>
#include <kstring.h>
#include <kutil.h>
#include <vec.h>
#include <task.h>
#include <kqueue.h>
#include <fs.h>
#include <syscall.h>
#include <lock.h>
#include <kb.h>
#include <trace.h>

terminal_t glb_term;
gdtrec glb_gdt_records[6];
extern uint32_t end;
tss_rec tss_entry;

uint32_t kernel_memory_end;
int32_t user_write(uint32_t fd, void *buffer, uint32_t length);

extern uint32_t inldr_end;
extern uint32_t inldr_start;

void timer_init(uint32_t frequency)
{
    uint16_t divisor = 1193180 / frequency;
    asm_outb(0x43, 0x36);
    uint8_t l = (uint8_t)(divisor % 256);
    uint8_t h = (uint8_t)(divisor / 256);
    asm_outb(0x40, l);
    asm_outb(0x40, h);
}

void stack_init()
{
    kernel_stack_ptr = 0xC0000000;
    user_stack_ptr = kernel_stack_ptr + KERNEL_STACK_SIZE;
    for (uint32_t i = 0; i < KERNEL_STACK_SIZE; i += 0x1000)
    {
        alloc_frame(get_page(kernel_stack_ptr + i, 0, current_page_directory), 1, 0);
    }
    for (uint32_t i = 0; i < USER_STACK_SIZE; i += 0x1000)
    {
        alloc_frame(get_page(user_stack_ptr + i, 0, current_page_directory), 1, 0);
    }
}

void *load_indlr()
{
    alloc_frame(get_page(kernel_memory_end, 0, current_page_directory), 1, 0);
    memcpy((void *)kernel_memory_end, &inldr_start, (uint32_t)&inldr_end - (uint32_t)&inldr_start);
    return (void *)kernel_memory_end;
}

void GPF_handler(registers *regs)
{
    kpanic("general protection fault ( code = %x )", regs->err_code);
}

void common_int_handler(registers *regs)
{
    if (regs->int_no != INTCODE_PIC)
    {
        kprintf("interrupt %u\n", regs->int_no);
    }
}

void kinit()
{
    // the order of these calls should'nt be randomly changed
    term_init(&glb_term);
    term_fg(&glb_term);
    load_gdt_recs(glb_gdt_records, &tss_entry);
    load_idt_recs(common_int_handler);
    uint32_t heap_effective_size = 0x1000000;
    uint32_t heap_index_size = 0x4000;
    uint32_t heap_total_size = heap_effective_size + heap_index_size;
    heap_init(&kernel_heap, &end, heap_total_size, heap_index_size, 1, 1);
    kernel_memory_end = (uint32_t)kernel_heap.start + heap_total_size;
    uint32_t table_space = 0x400000;
    if (kernel_memory_end % table_space != 0)
    {
        kernel_memory_end /= table_space;
        kernel_memory_end *= table_space;
        kernel_memory_end += table_space;
    }
    paging_init();
    stack_init();
}

void kmain()
{
    // the order of these calls should'nt be randomly changed
    kprintf("Kernel initialized successfully\n");
    timer_init(100);
    keyboard_init();
    syscalls_init();
    multitasking_init();
    load_int_handler(INTCODE_GPF, GPF_handler);

    uint32_t pid = task_fork();
    if (pid == 1)
    {
        return;
    }

    fs_init();
    trace_init();
    asm_usermode(load_indlr());
}