#ifndef MONITOR_H
#define MONITOR_H

#include "common.h"


void monitor_putc(char c);
void monitor_clear();
void monitor_puts(char *c);
void monitor_put_dec(u32int n);

#endif
