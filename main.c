#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

#include "FreeRTOS.h"
#include "task.h"

#include "thermistor.h"
#include "display.h"
#include "network.h"
#include "secure_tcp_client.h"
#include "cy_secure_sockets.h"

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

    /* Initialize the User LED. */
    cyhal_gpio_init(CYBSP_USER_LED, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("===============================================================\n");
    printf("Oleh Novosad MKNM - 21, Master degree\n");
    printf("===============================================================\n\n");

    xTaskCreate(Executor, "Executor", 1024, NULL, 5, NULL);

    vTaskStartScheduler();

    for (;;)
    {
    }
}

void Executor()
{
    bool _wifi = false;
    bool _sockets = false;

    /* Enable E-Ink display */
    eink_greeting();
    vTaskDelay(5000);

    /* Enable Termistor */
    init_thermistor();

    /* Connect to Wi-Fi AP */
    if (connect_to_wifi_ap() != CY_RSLT_SUCCESS)
    {
        printf("\n Failed to connect to Wi-Fi AP.\n");
        print_eink("Failed to connect to Wi-Fi");
        _wifi = false;
    }
    else
    {
        _wifi = true;
        vTaskDelay(5000);
    }

    if (_wifi)
    {
        if (tcp_secure_client_task() != CY_RSLT_SUCCESS)
        {
            printf("\n Failed to connect to Server.\n");
            print_eink("Failed to connect to Server");
            _sockets = false;
        }
        else
        {
            printf("\n Successfully to connect to Server.\n");
            print_eink("connected to Server");
            _sockets = true;
            vTaskDelay(5000);
        }
    }

    for (;;)
    {
    }
}

/* [] END OF FILE */
