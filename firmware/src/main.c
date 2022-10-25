/************************************************************************
* 5 semestre - Eng. da Computao - Insper
*
* 2021 - Exemplo com HC05 com RTOS
*
*/

#include <asf.h>
#include "conf_board.h"
#include <string.h>

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// LEDs
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// Botão
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX      11
#define BUT_IDX_MASK (1 << BUT_IDX)

#define BUT_AZUL              PIOC
#define BUT_AZUL_PIO_ID       ID_PIOC
#define BUT_AZUL_IDX          19
#define BUT_AZUL_IDX_MASK     (1 << BUT_AZUL_IDX)

#define BUT_VERMELHO          PIOA
#define BUT_VERMELHO_PIO_ID   ID_PIOA
#define BUT_VERMELHO_IDX      2
#define BUT_VERMELHO_IDX_MASK (1 << BUT_VERMELHO_IDX)

#define BUT_VERDE             PIOA
#define BUT_VERDE_PIO_ID      ID_PIOA
#define BUT_VERDE_IDX         6
#define BUT_VERDE_IDX_MASK    (1 << BUT_VERDE_IDX)

// usart (bluetooth ou serial)
// Descomente para enviar dados
// pela serial debug

//#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
#define USART_COM USART1
#define USART_COM_ID ID_USART1
#else
#define USART_COM USART0
#define USART_COM_ID ID_USART0
#endif

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_BLUETOOTH_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_BLUETOOTH_STACK_PRIORITY        (tskIDLE_PRIORITY)

QueueHandle_t xQueueButton;


/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* RTOS application HOOK                                                */
/************************************************************************/

/* Called if stack overflow during execution */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	* identify which task has overflowed its stack.
	*/
	for (;;) {
	}
}

/* This function is called by FreeRTOS idle task */
extern void vApplicationIdleHook(void) {
	pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
}

/* This function is called by FreeRTOS each tick */
extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/
void butAzul_callback(void) {
	printf("botão azul\n");
	char x = '1';
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueButton, &x, &xHigherPriorityTaskWoken);
}

void butVermelho_callback(void) {
	if (!pio_get(BUT_VERMELHO, PIO_INPUT, BUT_VERMELHO_IDX_MASK)) {
		printf("botão vermelho descida\n");
		char x = '2';
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xQueueSendFromISR(xQueueButton, &x, &xHigherPriorityTaskWoken);
		} else {
		printf("botão vermelho subida\n");
		char x = '3';
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xQueueSendFromISR(xQueueButton, &x, &xHigherPriorityTaskWoken);
	}
}

void butVerde_callback(void) {
	printf("botão verde\n");
	char x = '4';
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueButton, &x, &xHigherPriorityTaskWoken);
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void io_init(void) {
	pmc_enable_periph_clk(BUT_AZUL);
	pmc_enable_periph_clk(BUT_VERMELHO);
	pmc_enable_periph_clk(BUT_VERDE);

	pio_configure(BUT_AZUL, PIO_INPUT, BUT_AZUL_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);
	pio_configure(BUT_VERMELHO, PIO_INPUT, BUT_VERMELHO_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);
	pio_configure(BUT_VERDE, PIO_INPUT, BUT_VERDE_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);
	
	pio_set_debounce_filter(BUT_AZUL, BUT_AZUL_IDX_MASK, 60);
	pio_set_debounce_filter(BUT_VERMELHO, BUT_VERMELHO_IDX_MASK, 60);
	pio_set_debounce_filter(BUT_VERDE, BUT_VERDE_IDX_MASK, 60);
	
	pio_handler_set(BUT_AZUL, BUT_AZUL_PIO_ID, BUT_AZUL_IDX_MASK, PIO_IT_FALL_EDGE,
	butAzul_callback);
	pio_handler_set(BUT_VERMELHO, BUT_VERMELHO_PIO_ID, BUT_VERMELHO_IDX_MASK, PIO_IT_EDGE,
	butVermelho_callback);
	pio_handler_set(BUT_VERDE, BUT_VERDE_PIO_ID, BUT_VERDE_IDX_MASK, PIO_IT_FALL_EDGE,
	butVerde_callback);

	pio_enable_interrupt(BUT_AZUL, BUT_AZUL_IDX_MASK);
	pio_enable_interrupt(BUT_VERMELHO, BUT_VERMELHO_IDX_MASK);
	pio_enable_interrupt(BUT_VERDE, BUT_VERDE_IDX_MASK);

	pio_get_interrupt_status(BUT_AZUL);
	pio_get_interrupt_status(BUT_VERMELHO);
	pio_get_interrupt_status(BUT_VERDE);

	NVIC_EnableIRQ(BUT_AZUL_PIO_ID);
	NVIC_SetPriority(BUT_AZUL_PIO_ID, 4);

	NVIC_EnableIRQ(BUT_VERDE_PIO_ID);
	NVIC_SetPriority(BUT_VERDE_PIO_ID, 4);

	NVIC_EnableIRQ(BUT_VERMELHO_PIO_ID);
	NVIC_SetPriority(BUT_VERMELHO_PIO_ID, 4);
}

//void io_init(void) {

	// Ativa PIOs
//	pmc_enable_periph_clk(LED_PIO_ID);
//	pmc_enable_periph_clk(BUT_PIO_ID);

	// Configura Pinos
//	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT | PIO_DEBOUNCE);
//	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);
//}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		#if (defined CONF_UART_CHAR_LENGTH)
		.charlength = CONF_UART_CHAR_LENGTH,
		#endif
		.paritytype = CONF_UART_PARITY,
		#if (defined CONF_UART_STOP_BITS)
		.stopbits = CONF_UART_STOP_BITS,
		#endif
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	#if defined(__GNUC__)
	setbuf(stdout, NULL);
	#else
	/* Already the case in IAR's Normal DLIB default configuration: printf()
	* emits one character at a time.
	*/
	#endif
}

uint32_t usart_puts(uint8_t *pstring) {
	uint32_t i ;

	while(*(pstring + i))
	if(uart_is_tx_empty(USART_COM))
	usart_serial_putchar(USART_COM, *(pstring+i++));
}

void usart_put_string(Usart *usart, char str[]) {
	usart_serial_write_packet(usart, str, strlen(str));
}

int usart_get_string(Usart *usart, char buffer[], int bufferlen, uint timeout_ms) {
	uint timecounter = timeout_ms;
	uint32_t rx;
	uint32_t counter = 0;

	while( (timecounter > 0) && (counter < bufferlen - 1)) {
		if(usart_read(usart, &rx) == 0) {
			buffer[counter++] = rx;
		}
		else{
			timecounter--;
			vTaskDelay(1);
		}
	}
	buffer[counter] = 0x00;
	return counter;
}

void usart_send_command(Usart *usart, char buffer_rx[], int bufferlen,
char buffer_tx[], int timeout) {
	usart_put_string(usart, buffer_tx);
	usart_get_string(usart, buffer_rx, bufferlen, timeout);
}

void config_usart0(void) {
	sysclk_enable_peripheral_clock(ID_USART0);
	usart_serial_options_t config;
	config.baudrate = 9600;
	config.charlength = US_MR_CHRL_8_BIT;
	config.paritytype = US_MR_PAR_NO;
	config.stopbits = false;
	usart_serial_init(USART0, &config);
	usart_enable_tx(USART0);
	usart_enable_rx(USART0);

	// RX - PB0  TX - PB1
	pio_configure(PIOB, PIO_PERIPH_C, (1 << 0), PIO_DEFAULT);
	pio_configure(PIOB, PIO_PERIPH_C, (1 << 1), PIO_DEFAULT);
}

int hc05_init(void) {
	char buffer_rx[128];
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+NAMEAdney", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT", 100);
	vTaskDelay( 500 / portTICK_PERIOD_MS);
	usart_send_command(USART_COM, buffer_rx, 1000, "AT+PIN0000", 100);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

void task_bluetooth(void) {
	printf("Task Bluetooth started \n");
	
	printf("Inicializando HC05 \n");
	config_usart0();
	hc05_init();

	// configura LEDs e Botões
	io_init();
	
	char button1 = '0';
	char button = '0';
	char eof = 'X';

	// Task não deve retornar.
	while(1) {

		if (xQueueReceive(xQueueButton, &button, 0)) {
			printf("botão apertado e enviado na task\n");
					//envia status botão
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, button);
			
			// envia fim de pacote
			while(!usart_is_tx_ready(USART_COM)) {
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			usart_write(USART_COM, eof);
		}
		
		;

			//dorme por 500 ms
		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main(void) {
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	configure_console();
	
	xQueueButton = xQueueCreate(32, sizeof(uint32_t));
	if (xQueueButton == NULL)
	printf("falha em criar a queue xQueueButton \n");

	/* Create task to make led blink */
	xTaskCreate(task_bluetooth, "BLT", TASK_BLUETOOTH_STACK_SIZE, NULL,	TASK_BLUETOOTH_STACK_PRIORITY, NULL);

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
