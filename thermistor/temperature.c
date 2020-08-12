#include "cy8ckit_028_epd.h"

/* Thermistor variables */
cyhal_adc_t adc;
mtb_thermistor_ntc_gpio_t thermistor;
mtb_thermistor_ntc_gpio_cfg_t thermistor_cfg = {
    .r_ref = CY8CKIT_028_EPD_THERM_R_REF,
    .b_const = CY8CKIT_028_EPD_THERM_B_CONST,
    .r_infinity = CY8CKIT_028_EPD_THERM_R_INFINITY,
};

void Init_Thermistor()
{

    /* Intialize adc */
    cyhal_adc_init(&adc, CY8CKIT_028_EPD_PIN_THERM_OUT1, NULL);
    // CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* Initialize thermistor */
    mtb_thermistor_ntc_gpio_init(&thermistor, &adc, CY8CKIT_028_EPD_PIN_THERM_GND, CY8CKIT_028_EPD_PIN_THERM_VDD, CY8CKIT_028_EPD_PIN_THERM_OUT1, &thermistor_cfg);
    // CY_ASSERT(result == CY_RSLT_SUCCESS);

    // return result;
}

float Get_Themperature()
{
    float temperature;
    temperature = mtb_thermistor_ntc_gpio_get_temp(&thermistor);

    return temperature;
}