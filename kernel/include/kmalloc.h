#ifndef KMALLOC_H
#define KMALLOC_H

void kmalloc_init(void);
void * kmalloc(unsigned size);
void kfree(void * d);
#endif // KMALLOC_H
