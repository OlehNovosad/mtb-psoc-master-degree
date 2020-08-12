#include "GUI.h"
#include "mtb_e2271cs021.h"
#include "LCDConf.h"
#include "cy8ckit_028_epd_pins.h"

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
    uint32_t result;
    result = cyhal_spi_init(&spi, CY8CKIT_028_EPD_PIN_DISPLAY_SPI_MOSI,
                            CY8CKIT_028_EPD_PIN_DISPLAY_SPI_MISO,
                            CY8CKIT_028_EPD_PIN_DISPLAY_SPI_SCLK, NC, NULL, 8,
                            CYHAL_SPI_MODE_00_MSB, false);
    if (CY_RSLT_SUCCESS == result)
    {
        result = cyhal_spi_set_frequency(&spi, 20000000);
    }

    mtb_e2271cs021_init(&pins, &spi);
}

void eink_greeting()
{
    GUI_Init();
    GUI_SetTextMode(GUI_TM_NORMAL);
    GUI_SetFont(GUI_FONT_24B_ASCII);
    GUI_SetBkColor(GUI_WHITE);
    GUI_SetColor(GUI_BLACK);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_Clear();

    GUI_DispStringAt("Hello World", LCD_GetXSize() / 2, 15);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_DispStringAt("I'm Oleh Novosad", LCD_GetXSize() / 2, 15);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_DispStringAt("Student of KNIT - 21", LCD_GetXSize() / 2, 15);
    GUI_SetTextAlign(GUI_ALIGN_HCENTER | GUI_ALIGN_HCENTER);
    GUI_DispStringAt("This is master degree", LCD_GetXSize() / 2, 15);

    /* update the display */
    mtb_e2271cs021_show_frame(previous_frame, current_frame, MTB_E2271CS021_FULL_4STAGE, true);
}