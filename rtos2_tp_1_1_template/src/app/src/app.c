/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : app.c
 * @date   : Feb 17, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "test.h"
#include "test_mock.h"

/********************** macros and definitions *******************************/

#define LEDS_APAGADOS 0x00
#define TIME_BUTTON 10
#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(uint8_t))


/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
static uint8_t command_ = LEDS_APAGADOS;
QueueHandle_t queue;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void ao_button_process_(button_t button, bool button_state) {

	 command_ =button_state ? (command_ | (1 << button)) : (command_ & (~(1 << button)));
	 xQueueSend(queue, (void*)&command_, 0);

}

static void task_button_base_(void *argument) {
	button_t button = (button_t) argument;
	while (true) {
		bool button_state;
		button_state = button_read(button);
		ao_button_process_(button, button_state);
		vTaskDelay((TickType_t) (TIME_BUTTON / portTICK_PERIOD_MS));
	}
}

static void task_ao_led_(void *argument) {
	while (true) {
		uint8_t command_local_;
		if(pdPASS == xQueueReceive(queue, &command_local_, 100)){
			 led_command_send(command_local_);
		}
		vTaskDelay((TickType_t) (TIME_BUTTON / portTICK_PERIOD_MS));
	}
}

/********************** external functions definition ************************/

void app_init(void) {
	// drivers
	{
		driver_init();
		ELOG("drivers init");
	}

	// test
	{
		test_init();
		ELOG("test init");
	}

	BaseType_t status;

	queue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while(NULL == queue)ELOG("Error al crear la Queue");
	vQueueAddToRegistry(queue,"Queue");
	status = xTaskCreate(task_ao_led_, "task_led", 128,NULL,tskIDLE_PRIORITY, NULL);
	// while (pdPASS != status)ELOG("Error al crear el objeto activo");
	ELOG("ao init");


	// tasks

	for (button_t task = 0; task < BUTTON__CNT; task++) {
		status = xTaskCreate(task_button_base_, "task_button", 128,(void*) task,tskIDLE_PRIORITY, NULL);
		//while (pdPASS != status)ELOG("Error al crear una tarea");
	}

	ELOG("app init");
}

/********************** end of file ******************************************/
