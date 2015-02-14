#ifndef GIC_H
#define GIC_H

void gic_init(void);
void gic_enable_interrupts(void);
void gic_disable_interrupts(void);
void gic_dump(void);

void gic_handle_irq(void);
#endif // GIC_H
