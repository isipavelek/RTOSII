
/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal.h"
#include "driver.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (1000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern UART_t Uart3;

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

void task_tick(void *args) {
	while (true) {
		vTaskDelay((TickType_t) (TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
		driver_uart_tx(&Uart3, (uint8_t*) "tick\r\n", sizeof("tick\r\n"));

	}
}

/********************** end of file ******************************************/