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

void master_degree();
cy_stc_rtc_config_t const *RTC_config;
cy_stc_rtc_config_t dateTime;

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

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen */
    printf("\x1b[2J\x1b[;H");
    printf("===============================================================\n");
    printf("Oleh Novosad MKNM - 21, Master degree\n");
    printf("===============================================================\n\n");

    xTaskCreate(master_degree, "master_degree", 1024, NULL, 5, NULL);

    vTaskStartScheduler();

    for (;;)
    {
    }
}

void master_degree()
{
    bool _wifi = false;

    /* Enable E-Ink display */
    eink_greeting();
    vTaskDelay(5000);

    /* Enable Termistor */
    init_thermistor();

    /* Initialize the User LED. */
    cyhal_gpio_init(RED_LED, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    cyhal_gpio_init(GREEN_LED, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);
    cyhal_gpio_init(BLUE_LED, CYHAL_GPIO_DIR_OUTPUT,
                    CYHAL_GPIO_DRIVE_STRONG, CYBSP_LED_STATE_OFF);

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

    tcp_secure_client_task(_wifi);
}

/* [] END OF FILE */