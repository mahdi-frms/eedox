#ifndef DGT_H
#define DGT_H

#include <stdint.h>

typedef struct
{
    unsigned short length;
    unsigned short base_l;
    unsigned char base_m;
    unsigned char access;
    unsigned char fllen;
    unsigned char base_h;
} __attribute__((packed)) gdtrec;

typedef struct
{
    unsigned short len;
    gdtrec *ptr;
} __attribute__((packed)) gdtarray;

typedef struct
{
    uint32_t prev_tss; // The previous TSS - if we used hardware task switching this would form a linked list.
    uint32_t esp0;     // The stack pointer to load when we change to kernel mode.
    uint32_t ss0;      // The stack segment to load when we change to kernel mode.
    uint32_t esp1;     // Unused...
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;  // The value to load into ES when we change to kernel mode.
    uint32_t cs;  // The value to load into CS when we change to kernel mode.
    uint32_t ss;  // The value to load into SS when we change to kernel mode.
    uint32_t ds;  // The value to load into DS when we change to kernel mode.
    uint32_t fs;  // The value to load into FS when we change to kernel mode.
    uint32_t gs;  // The value to load into GS when we change to kernel mode.
    uint32_t ldt; // Unused...
    uint32_t trap;
    uint32_t iomap_base;
} __attribute__((packed)) tss_rec;

void load_gdt_recs(gdtrec *gdt_records, tss_rec *tss_entry);

#endif