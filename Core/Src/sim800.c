//
// Created by Hanawarou on 10.10.2025.
//
#include "../Inc/sim800.h"

#include <stdio.h>
#include <string.h>

/**
 *
 * @brief Function of waiting for the arrival of the command_ready status
 */
uint8_t wait_for_command_ready(uint32_t timeout) {
    uint32_t start = HAL_GetTick();
    while (!get_command_ready_flag()) {
        if (HAL_GetTick() - start > timeout) {
            return 0;
        }
        HAL_Delay(10);
    }
    return 1;
}

/**
 *
 * @brief A separate function for displaying different types of errors
 */
void error_indicate(uint8_t error_code) {
    switch (error_code) {
        case 1:
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
            HAL_Delay(100);
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
            HAL_Delay(100);
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
            HAL_Delay(100);
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
            break;
        case 2:
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
            HAL_Delay(500);
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
            break;
        default:
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
            HAL_Delay(100);
            HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
    }
}

/**
 *
 * @brief Sim module initialization function
 */
uint8_t sim_module_init(void) {
    char message_container[64];
    const uint8_t* rx_buffer = get_rx_buffer();

    send_response_to_module("AT\r\n");
    wait_for_command_ready(1000);
    if (!(get_command_ready_flag() && strcmp((char*)rx_buffer, "OK") == 0)) {
        error_indicate(1);
        clear_command_ready_flag();
        return 1;
    }
    clear_command_ready_flag();

    send_response_to_module("AT+CPIN?\r\n");
    wait_for_command_ready(1000);
    if (get_command_ready_flag()) {
        if (strcmp((char*) rx_buffer, "ERROR") == 0) {
            error_indicate(1);
            clear_command_ready_flag();
            return 1;
        }
        if (strcmp((char*) rx_buffer, "+CPIN: SIM PIN") == 0) {
            sniprintf(message_container, sizeof(message_container), "AT+CPIN=%d\r\n", PIN);
            send_response_to_module(message_container);
            wait_for_command_ready(1000);
            if (!(get_command_ready_flag() && strcmp((char*)rx_buffer, "OK") == 0)) {
                error_indicate(2);
                clear_command_ready_flag();
                return 1;
            }
            clear_command_ready_flag();
        }
        if (strcmp((char*) rx_buffer, "+CPIN: READY") == 0) {
            set_sim_ready_flag();
            clear_command_ready_flag();
        }
    }
    clear_command_ready_flag();

    send_response_to_module("AT+CNMI=2,2,0,0,0\r\n");
    wait_for_command_ready(1000);
    if (!(get_command_ready_flag() && strcmp((char*)rx_buffer, "OK") == 0)) {
        error_indicate(1);
        clear_command_ready_flag();
        return 1;
    }
    clear_command_ready_flag();

    return 0;
}

// TODO: Rework the file to work with the module and create a full-fledged library for processing all AT commands