/*
 * Copyright (c) YEAR NOMBRE <MAIL>.
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
 * @file   : task_tick.c
 * @date   : Apr 19, 2023
 * @author : NOMBRE <MAIL>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hal.h"
#include "driver.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_         (100)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static TimerHandle_t htimer_;
static uint32_t cnt_;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void timer_callback_(TimerHandle_t xTimer)
{
  eboard_uart_swrite_line("tick");
  eboard_led_blue(true);
  cnt_ = 0;
}

/********************** external functions definition ************************/

void task_tick(void* args)
{
  eboard_led_red(false);
  eboard_led_green(false);
  eboard_led_blue(false);

  {
    htimer_ = xTimerCreate("timer", 1000, pdFALSE, (void*)0, timer_callback_);
    if (htimer_ == NULL)
    {
      while(1)
      {
        // Error
      }
    }
    else
    {
      if (xTimerStart(htimer_, 0) != pdPASS)
      {
        // Error
      }
    }
  }

  uint8_t byte;
  char msg[64];
  while(true)
  {
    eboard_led_red(false);
    eboard_led_green(false);
    eboard_led_blue(false);

    while(0 < eboard_uart_read(&byte, 1))
    {
      switch (byte) {
        case 'p':
          eboard_led_red(true);
          if(pdPASS != xTimerStop(htimer_, 0))
          {
            eboard_uart_swrite_line("Error");
          }
          cnt_ = 0;
          break;
        case 't':
          eboard_led_green(true);
          if(pdPASS != xTimerStart(htimer_, 0))
          {
            eboard_uart_swrite_line("Error");
          }
          cnt_ = 1;
          break;
        case 'q':
          eboard_led_green(true);
          eboard_led_red(true);
          for(int i = 0; i < 100; ++i)
          {
            if(pdPASS != xTimerStart(htimer_, 0))
            {
              sprintf(msg, "Error %d", (int)i);
              eboard_uart_swrite_line(msg);
              break;
            }
          }
          break;
        default:
          break;
      }
    }

    if(0 < cnt_)
    {
      sprintf(msg, "%d", (int)cnt_);
      eboard_uart_swrite_line(msg);
      cnt_++;
    }

    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}


/********************** end of file ******************************************/
