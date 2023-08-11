
/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "driver.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (10)
#define BUFFER_RX_SIZE			(100)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern UART_t Uart3;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void task_echo(void *args) {
	size_t char_quantity = 0;
	uint8_t buffer[BUFFER_RX_SIZE];
	while (true) {
		while (0 < (char_quantity = driver_uart_rx(&Uart3, buffer, BUFFER_RX_SIZE))) {
			driver_uart_tx(&Uart3, buffer, char_quantity);
		}

		vTaskDelay((TickType_t) (TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
	}
}

/********************** end of file ******************************************/
