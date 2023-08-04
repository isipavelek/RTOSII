/* @file:	bank.c en tp_2_3_3
*/

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "client.h"
#include "bank.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_           (10)
#define QUEUE_ITEM_SIZE_		(sizeof(client_t*))
#define MAX_CLIENTE_CARE		(10)

/********************** internal data declaration ****************************/

static struct {
  QueueHandle_t hclient_queue;
  uint8_t clientCare;
  uint16_t client;
}bank_;

TaskHandle_t clientCareTask[MAX_CLIENTE_CARE];

/********************** internal functions declaration ***********************/
static void client_callback_(client_t* pnew_client);
static void task_(void *args);

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/


static void client_callback_(client_t* pnew_client) {
	client_t* p_cliente_md;
	p_cliente_md = (client_t*) pvPortMalloc( sizeof(client_t) );
	bank_.client++;
	if ( p_cliente_md == NULL ) {
		ELOG("ERROR: No hay memoria para un nuevo cliente en espera");
	} else {
		*p_cliente_md=*pnew_client;
		if ( pdPASS == xQueueSend(bank_.hclient_queue, (void*) &p_cliente_md, 0) ) {
			ELOG("Cliente %d en espera", bank_.client);
		} else {
			BaseType_t status;
			bank_.clientCare++;
			if(pdPASS!=(status = xTaskCreate(task_, "task_bank", 128, NULL, tskIDLE_PRIORITY, &clientCareTask[bank_.clientCare]))){
				ELOG("ERROR: No pude agregar cajero aun");
			}
			if ( pdPASS != xQueueSend(bank_.hclient_queue, (void*) &p_cliente_md, portMAX_DELAY) ) {
				ELOG("ERROR: Sin lugar");
				vPortFree( (void*) p_cliente_md );
				vTaskDelete(clientCareTask[bank_.clientCare]);
			} else {
				ELOG("Cliente %d nuevo en espera", bank_.client);

			}
		}
	}

}


static void task_(void *args) {
	client_t* pclient;

	client_set_callback(client_callback_);

	while (true){
		if (pdPASS == xQueueReceive(bank_.hclient_queue, &pclient,portMAX_DELAY)){
		  ELOG("Llamada al cliente");
	      client_process(pclient);
	      vPortFree( (void*) pclient );
	      ELOG("Fin de la atencion al cliente");
	    } else if (uxQueueMessagesWaiting( bank_.hclient_queue ) == 0 && bank_.clientCare>0) {
	    	bank_.clientCare--;
			vTaskDelete(NULL);
		}
	}
}

/********************** external functions definition ************************/

void bank_init(void)
{

	bank_.clientCare=0;
	bank_.client=0;
	//queue create

	bank_.hclient_queue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while (NULL == bank_.hclient_queue) {
	      ELOG("Error al crear la queue");
	}

	// Task create
	BaseType_t status;
	status = xTaskCreate(task_, "task_bank", 128, NULL, tskIDLE_PRIORITY, &clientCareTask[bank_.clientCare]);
	while (pdPASS != status) {
	      ELOG("Error al crear la tarea");
	}

}

/********************** end of file ******************************************/
