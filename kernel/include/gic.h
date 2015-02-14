#ifndef GIC_H
#define GIC_H

typedef void (*irq_handler_t)(void *);

void gic_init(void);
void gic_handle_irq(void);

void gic_enable_interrupts(void);
void gic_disable_interrupts(void);

void gic_enable_irq(unsigned no);
void gic_disable_irq(unsigned no);

void gic_register_irq(unsigned no, irq_handler_t fcn, void * parm);
void gic_unregister_irq(unsigned no);

void gic_dump(void);

#endif // GIC_H
