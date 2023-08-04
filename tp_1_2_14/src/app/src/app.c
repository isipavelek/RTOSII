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
#define TIME_ACCESS 			 (1000)
#define TIME_SENSOR			     (10)
#define QUEUE_LENGTH_            (100)
#define QUEUE_ITEM_SIZE_         (sizeof(access_state_t))

/********************** internal data declaration ****************************/

static QueueHandle_t queue;

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

typedef enum {
	NO_CAR,
	CAR_PRESENT,
}car_access_state_t;

typedef struct {
	access_t access;
	car_access_state_t car_present;

}access_state_t;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

static void ao_access_process(access_state_t access_state) {
	if(access_state.car_present){
		access_open(access_state.access);
		vTaskDelay((TickType_t) (TIME_ACCESS / portTICK_PERIOD_MS));
		access_close(access_state.access);
	 }
}

static void task_access_(void *argument) {
	while(true){
		access_state_t access_state;
		if(pdPASS == xQueueReceive(queue, &access_state, portMAX_DELAY )){
			ao_access_process(access_state);
		}

	}
}

static void ao_sensor_process(access_t access) {
	access_state_t access_state;
	access_state.access=access;
	access_state.car_present=car_sensor_read(access);
	if(access_state.car_present){
		xQueueSend(queue, (void*)&access_state, 0);
	}
}


static void task_ao_access(void *argument) {


	access_t access= (access_t) argument;
	while(true){
		ao_sensor_process(access);
		vTaskDelay((TickType_t) (TIME_SENSOR / portTICK_PERIOD_MS));
	}
}


static void ao_access_init (void) {

	queue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while (NULL == queue){
		ELOG("Error creating the queue ");
	}
	vQueueAddToRegistry(queue,"Queue");

	BaseType_t status;
	access_t access=ACCESS_EAST;
	status = xTaskCreate(task_ao_access, "task_acc_oa_EAST", 128,(void*)access, tskIDLE_PRIORITY + 1, NULL);
	while (pdPASS != status)
	{
		ELOG("Error creating task_acc_oa_EAST ");
	// error
	}
	access=ACCESS_WEST;
	status = xTaskCreate(task_ao_access, "task_acc_oa_WEST", 128, (void*)access, tskIDLE_PRIORITY + 1, NULL);
	while (pdPASS != status)
	{
		ELOG("Error creating task_acc_oa_WEST ");
	// error
	}
}

void task_create() {
	BaseType_t status;
	status = xTaskCreate(task_access_, "task_access", 128, NULL, tskIDLE_PRIORITY + 1, NULL);
	while (pdPASS != status) {
		ELOG("Error creating task_access_");
	// error
	}

}
void app_init(void)
{
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

  // OA
  {
	ao_access_init();
    ELOG("ao init");
  }

  // tasks
  {
	task_create();
	ELOG("tasks init");

  }

  ELOG("app init");
}

/********************** end of file ******************************************/
