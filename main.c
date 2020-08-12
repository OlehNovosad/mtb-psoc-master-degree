#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

#include "project.h"

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

    /* Initialize ADC and Thermistor */
    init_thermistor();

    /* Initialize UART */
    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    init_eink();
    eink_greeting();

    for (;;)
    {
        /* Measure the temperature and send the value via UART */
        printf("Temperature = %.3fC\r\n", get_themperature());
        cyhal_system_delay_ms(1000);
    }
}

/* [] END OF FILE */
