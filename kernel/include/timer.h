#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>
// System timer driver
void timer_init(void);
void timer_dump(void);
void timer_restart(void);

#endif // TIMER_H
