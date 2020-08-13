#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

#include "FreeRTOS.h"
#include "task.h"

#include "thermistor.h"
#include "display.h"
#include "network.h"

void Executor();

int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    /* Initialize UART */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    xTaskCreate(Executor, "Executor", 1024, NULL, 5, NULL);
    
    vTaskStartScheduler();

    for (;;)
    {
    }
}

void Executor()
{
    eink_greeting();
    vTaskDelay(5000);
    connectWifi();
    vTaskDelay(5000);
}

/* [] END OF FILE */
