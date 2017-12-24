/* Author(s): <Your name here>
 * Implementation of the memory manager for the kernel.
 */

#include "common.h"
#include "interrupt.h"
#include "kernel.h"
#include "memory.h"
#include "printf.h"
#include "scheduler.h"
#include "util.h"

#define MEM_START 0xa0908000

/* Static global variables */
// Keep track of all pages: their vaddr, status, and other properties

static uint32_t *pgdir;
// other global variables...
/*static lock_t page_fault_lock;*/

uint32_t get_table_index(uint32_t vaddr){
    return (vaddr & 0xfffff000)>>12;
}
/*
uint32_t get_dir_index(uint32_t vaddr){
    return (vaddr & 0xffc00000)>>22;
}
*/


/* TODO: Returns virtual address (in kernel) of page number i */
uint32_t page_vaddr( int i ) {
    return page_map[i].vaddr_start;
}

/* TODO: Returns physical address of page number i */
uint32_t page_paddr( int i ) {
    return page_map[i].paddr_start;
}

/* get the physical address from virtual address (in kernel) */
uint32_t va2pa( uint32_t va ) {
    return (uint32_t) va - 0xa0000000;
}

/* get the virtual address (in kernel) from physical address */
uint32_t pa2va( uint32_t pa ) {
    return (uint32_t) pa + 0xa0000000;
}


// TODO: insert page table entry to page table
void insert_page_table_entry( uint32_t *table, uint32_t vaddr, uint32_t paddr,
                              uint32_t flag, uint32_t pid ) {
    // insert entry
    uint32_t index = get_table_index(vaddr);
    table[index] = ((paddr & 0xfffff000)>>6)|(flag & 0xfff);
    tlb_flush((vaddr & 0xffffe000) | (pid & 0xfff));
    // tlb flush
}

/*
void insert_page_dir_entry( uint32_t *pgdir, uint32_t *table, uint32_t vaddr, 
                              uint32_t flag, uint32_t pid ) {
    // insert entry
    uint32_t index = get_dir_index(vaddr);
    pgdir[index] = ((uint32_t)table & 0xfffff000)|(flag & 0xfff);
} 
*/

/* TODO: Allocate a page. Return page index in the page_map directory.
 *
 * Marks page as pinned if pinned == TRUE.
 * Swap out a page if no space is available.
 */


int page_alloc( int pinned ) {
 
    // code here
    uint32_t index;
    uint32_t *page_table;
    uint32_t pid;
/*
    for(index=0; index<PAGEABLE_PAGES; index++){
        printf(25+index, 1, "process %d: %d", page_map[index].pid,(uint32_t)page_map[index].pinned);
    }
 */ 
    for(index = 0; index < PAGEABLE_PAGES; index++){
        if(page_map[index].available == TRUE){
            break;
        }
    }
    //IF don't have empty page
    if (index == PAGEABLE_PAGES){
        for(index = 0; index < PAGEABLE_PAGES; index++){
            if(page_map[index].pinned == 0){
                /*clean the former*/
                pid = page_map[index].pid;
                page_table = pcb[pid].page_table;
                insert_page_table_entry(page_table, pcb[pid].entry_point, 0x0, 0x0, pid);
                break;
            }
        }
    }
    page_map[index].pinned = pinned;
    page_map[index].available = FALSE;

    bzero((char *)page_paddr(index), PAGE_SIZE);

    return index;
    //ASSERT( free_index < PAGEABLE_PAGES );

}


/* TODO: 
 * This method is only called once by _start() in kernel.c
 */
uint32_t init_memory( void ) {
    
    // initialize all pageable pages to a default state
    int i;
    for (i = 0; i < PAGEABLE_PAGES; i++){
        page_map[i].available = TRUE;
        page_map[i].pinned = FALSE;
        page_map[i].paddr_start = MEM_START + i * PAGE_SIZE;
    }
    /*
    page_map[0].pinned = TRUE;
    page_map[0].available = FALSE;
    pgdir = page_map[0].paddr_start;
    */


}

/* TODO: 
    // alloc page for page table 
    // initialize PTE and insert several entries into page tables using insert_page_table_entry
 * 
 */
uint32_t setup_page_table( int pid ) {
    uint32_t page_table;
    int pg;
    uint32_t paddr, vaddr;
    int index;
    int j;
    pcb_t *p;
    index = page_alloc(TRUE);
    page_map[index].pid = pid;
   
    p = &pcb[pid];

    pg = page_alloc(FALSE);
    page_map[pg].pid = pid;
    insert_page_table_entry(page_map[index].paddr_start, p->entry_point, page_map[pg].paddr_start, 0x0, pid);
    //bcopy((char *)p_loc, (char *)paddr, PAGE_SIZE);


    page_table = page_map[index].paddr_start;
    return page_table;
}

uint32_t do_tlb_miss(uint32_t vaddr, int pid) {
    return 0;
}

void create_pte(uint32_t vaddr, int pid) {
    return;
}
