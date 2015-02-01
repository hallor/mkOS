#ifndef PAGE_H
#define PAGE_H

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define PAGE_MASK 0xFFF

int page_init(void);
void * page_alloc(int num_pages);

#define PAGE_NO(addr) (addr & PAGE_MASK)

#define ALIGN_PGUP(X) ( (X + PAGE_MASK) & ~PAGE_MASK )

#endif // PAGE_H
