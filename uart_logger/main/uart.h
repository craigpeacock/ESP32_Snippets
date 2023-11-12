
#define UART_PORT				UART_NUM_1
#define UART_BAUD_RATE			115200
#define GPIO_UART_TXD			17
#define GPIO_UART_RXD			18
//#define GPIO_UART_TXD			43
//#define GPIO_UART_RXD			44

#define UART_BUF_SIZE			512

void uart_init(void);
void uart_event_task(void *pvParameters);