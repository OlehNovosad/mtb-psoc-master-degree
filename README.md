# PSoC 6 MCU: Measuring temperature by using thermistor and send information to the server by using sockets

This is my master degree work.

For instructions on modifying and using this application template to create applications, follow steps from the section "My First PSoC 6 MCU Design Using ModusToolbox IDE" in **[AN228571](https://www.cypress.com/AN228571) – Getting Started with PSoC 6 MCU on ModusToolbox.**

## Quick Start
1. A set of pre-defined configuration files have been bundled with this library for FreeRTOS, lwIP, and mbed TLS. These files are located in the *configs* folder.

   You should do the following:

   - Copy *FreeRTOSConfig.h*, *lwipopts.h*, and *mbedtls_user_config.h* files from the *configs* directory to the top-level code example directory in the project.

   - Configure the `MBEDTLS_USER_CONFIG_FILE` C macro to *mbedtls_user_config.h* in the Makefile to provide the user configuration to the mbed TLS library. The Makefile entry would look like as follows:

       ```
       DEFINES+=MBEDTLS_USER_CONFIG_FILE='"mbedtls_user_config.h"'
       ```

   - Add the `CYBSP_WIFI_CAPABLE` build configuration to enable Wi-Fi functionality. The Makefile entry would look like as follows:

       ```
       DEFINES+=CYBSP_WIFI_CAPABLE
       ```

2. Wi-Fi Middleware Core Library requires FreeRTOS config *configENABLE_BACKWARD_COMPATIBILITY* to be enabled. By default, FreeRTOS enables this config. You must not disable this config in the *FreeRTOSConfig.h* file.

3. Secure Sockets, lwIP, and mbed TLS libraries contain reference and test applications. To ensure that these applications do not conflict with the code examples, a *.cyignore* file is also included with this library.

4. Add the following to COMPONENTS in the code example project's Makefile - `FREERTOS`, `PSOC6HAL`, `LWIP`, `MBEDTLS`, and either `4343W` or `43012` depending on the platform.

   For example, if your target is CY8CKIT-062-WIFI-BT, the Makefile entry would look like as follows:

   ```
   COMPONENTS=FREERTOS PSOC6HAL LWIP MBEDTLS 4343W
   ```
5. wifi-mw-core library disables all the debug log messages by default. To enable log messages, the application must perform the following:
 - Add `ENABLE_WIFI_MIDDLEWARE_LOGS` macro to the *DEFINES* in the code example's Makefile. The Makefile entry would look like as follows:
  ```
  DEFINES+=ENABLE_WIFI_MIDDLEWARE_LOGS
  ```
 - Call the `cy_log_init()` function provided by the *cy-log* module. cy-log is part of the *connectivity-utilities* library. See [connectivity-utilities library API documentation](https://cypresssemiconductorco.github.io/connectivity-utilities/api_reference_manual/html/group__logging__utils.html) for cy-log details.

## Requirements

- [ModusToolbox™ software](https://www.cypress.com/products/modustoolbox-software-environment) v2.1
- Programming Language: C
- Associated Parts: All [PSoC 6 MCU](http://www.cypress.com/PSoC6) parts

## Supported Kits

- [PSoC 6 Wi-Fi BT Prototyping Kit](https://www.cypress.com/CY8CPROTO-062-4343W) (CY8CPROTO-062-4343W) - Default target
- [PSoC 6 WiFi-BT Pioneer Kit](https://www.cypress.com/CY8CKIT-062-WiFi-BT) (CY8CKIT-062-WiFi-BT)

## Document History

Document Title: CE228742 - PSoC 6 MCU: Empty Application Template

| Version | Description of Change |
| ------- | --------------------- |
| 1.0.0   | first implementation  |

------