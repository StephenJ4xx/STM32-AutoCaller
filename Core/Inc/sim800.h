//
// Created by Hanawarou on 10.10.2025.
//

#ifndef AUTOCALLER_SIM800C_H
#define AUTOCALLER_SIM800C_H

#include "main.h"

#define PIN 0000 // Your sim card pin code

uint8_t sim_module_init(void);
uint8_t wait_for_command_ready(uint32_t timeout);
void error_indicate(uint8_t);

#endif //AUTOCALLER_SIM800C_H