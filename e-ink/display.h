#ifndef DISPLAY_H
#define DISPLAY_H

void init_eink();
void deinit_eink();

void eink_greeting();
void success_connection(char *device_ip);
void print_eink(char *message);

#endif