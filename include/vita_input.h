#ifndef VITA_INPUT_H
#define VITA_INPUT_H

#include "vita_compat.h"

void vita_read_pad(OSContPad *pad);
void vita_read_pad_port(OSContPad *pad, int port);
int vita_is_pstv(void);
int vita_get_external_controller_ports(int *ports, int max_ports);
int vita_get_external_controller_info(int *ports, int *types, int max_ports);

#endif
