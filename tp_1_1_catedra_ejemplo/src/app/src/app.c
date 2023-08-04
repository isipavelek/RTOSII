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

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static uint8_t command_ = 0x00;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void button_process_(button_t button, bool button_state)
{
  if(button_state)
  {
    command_ |= 0x01 << button;
  }
  else
  {
    command_ &= ~(0x01 << button);
  }
}

static void task_button_base_(void* argument)
{
  button_t button = (button_t)argument;
  while(true)
  {
    bool button_state;

    button_state = button_read(button);

    button_process_(button, button_state);

    vTaskDelay((TickType_t)(100 / portTICK_PERIOD_MS));
  }
}

static void task_led_(void* argument)
{
  while(true)
  {
    led_command_send(command_);

    vTaskDelay((TickType_t)(100 / portTICK_PERIOD_MS));
  }
}

/********************** external functions definition ************************/

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
    ELOG("ao init");
  }

  // tasks
  {
    BaseType_t status;
    status = xTaskCreate(task_button_base_, "task_button_red", 128, (void*)BUTTON_RED, tskIDLE_PRIORITY, NULL);
    while (pdPASS != status)
    {
      // error
    }

    status = xTaskCreate(task_button_base_, "task_button_green", 128, (void*)BUTTON_GREEN, tskIDLE_PRIORITY, NULL);
    while (pdPASS != status)
    {
      // error
    }

    status = xTaskCreate(task_button_base_, "task_button_blue", 128, (void*)BUTTON_BLUE, tskIDLE_PRIORITY, NULL);
    while (pdPASS != status)
    {
      // error
    }

    status = xTaskCreate(task_led_, "task_led_", 128, NULL, tskIDLE_PRIORITY, NULL);
    while (pdPASS != status)
    {
      // error
    }

    ELOG("tasks init");
  }

  ELOG("app init");
}

/********************** end of file ******************************************/
