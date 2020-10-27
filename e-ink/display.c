#include "GUI.h"
#include "mtb_e2271cs021.h"
#include "LCDConf.h"
#include "cy8ckit_028_epd_pins.h"
#include "stdio.h"

cyhal_spi_t spi;

const mtb_e2271cs021_pins_t pins =
    {
        .spi_mosi = CY8CKIT_028_EPD_PIN_DISPLAY_SPI_MOSI,
        .spi_miso = CY8CKIT_028_EPD_PIN_DISPLAY_SPI_MISO,
        .spi_sclk = CY8CKIT_028_EPD_PIN_DISPLAY_SPI_SCLK,
        .spi_cs = CY8CKIT_028_EPD_PIN_DISPLAY_CS,
        .reset = CY8CKIT_028_EPD_PIN_DISPLAY_RST,
        .busy = CY8CKIT_028_EPD_PIN_DISPLAY_BUSY,
        .discharge = CY8CKIT_028_EPD_PIN_DISPLAY_DISCHARGE,
        .enable = CY8CKIT_028_EPD_PIN_DISPLAY_EN,
        .border = CY8CKIT_028_EPD_PIN_DISPLAY_BORDER,
        .io_enable = CY8CKIT_028_EPD_PIN_DISPLAY_IOEN,
};

uint8_t previous_frame[PV_EINK_IMAGE_SIZE] = {0};
uint8_t *current_frame;

void init_eink()
{
    /* Initialize SPI and EINK display */
    cyhal_spi_init(&spi, CY8CKIT_028_EPD_PIN_DISPLAY_SPI_MOSI,
                   CY8CKIT_028_EPD_PIN_DISPLAY_SPI_MISO,
                   CY8CKIT_028_EPD_PIN_DISPLAY_SPI_SCLK, NC, NULL, 8,
                   CYHAL_SPI_MODE_00_MSB, false);

    cyhal_spi_set_frequency(&spi, 20000000);

    mtb_e2271cs021_init(&pins, &spi);
}

void deinit_eink()
{
    cyhal_spi_free(&spi);

    mtb_e2271cs021_free();
}

void eink_greeting()
{
    init_eink();

    current_frame = (uint8_t *)LCD_GetDisplayBuffer();

    GUI_Init();
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetFont(GUI_FONT_24B_ASCII);
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_Clear();

    GUI_DispStringAt("Hello World", LCD_GetXSize() / 2, 15);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_DispStringAt("I'm Oleh Novosad", LCD_GetXSize() / 2, 50);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_DispStringAt("Student of KNIT - 21", LCD_GetXSize() / 2, 85);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_DispStringAt("This is master degree", LCD_GetXSize() / 2, 120);

    /* update the display */
    mtb_e2271cs021_show_frame(previous_frame, current_frame, MTB_E2271CS021_FULL_4STAGE, true);

    deinit_eink();
}

void success_connection(char *device_ip)
{
    init_eink();
    char str[30];

    current_frame = (uint8_t *)LCD_GetDisplayBuffer();

    GUI_Init();
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetFont(GUI_FONT_24B_ASCII);
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_Clear();

    GUI_DispStringAt("Connected to Wi-Fi", LCD_GetXSize() / 2, 50);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    sprintf(str, "IP: %s", device_ip);
    GUI_DispStringAt(str, LCD_GetXSize() / 2, 85);
    /* update the display */
    mtb_e2271cs021_show_frame(previous_frame, current_frame, MTB_E2271CS021_FULL_4STAGE, true);

    deinit_eink();
}

void print_eink(char *message)
{
    init_eink();
    char str[30];

    current_frame = (uint8_t *)LCD_GetDisplayBuffer();

    GUI_Init();
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetFont(GUI_FONT_24B_ASCII);
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_Clear();

    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    sprintf(str, "%s", message);
    GUI_DispStringAt(str, LCD_GetXSize() / 2, 85);
    /* update the display */
    mtb_e2271cs021_show_frame(previous_frame, current_frame, MTB_E2271CS021_FULL_4STAGE, true);

    deinit_eink();
}