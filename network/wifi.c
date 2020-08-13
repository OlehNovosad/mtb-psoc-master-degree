#include "network.h"
#include "cy_retarget_io.h"
#include "cyhal.h"
#include "cybsp_wifi.h"
#include "cy_lwip.h"
#include "cy_secure_sockets.h"
#include "cy_tls.h"

#include "lwip/tcpip.h"
#include "lwip/api.h"
#include "FreeRTOS.h"
#include "task.h"

#include "display.h"

#define WIFI_SSID "KV80_2"
#define WIFI_KEY "KV80Oleh0410T5R4"

#define MAX_WIFI_RETRY_COUNT (3)

static whd_interface_t iface;

void donothing()
{
}

void connectWifi()
{
    cy_rslt_t res;

    const char *ssid = WIFI_SSID;
    const char *key = WIFI_KEY;
    int retry_count = 0;

    whd_ssid_t ssiddata;
    tcpip_init(donothing, NULL);
    printf("LWiP TCP/IP stack initialized\n");
    /*
     *   Initialize wifi driver
     */
    cybsp_wifi_init_primary(&iface);
    printf("WIFI driver initialized \n");

    while (1)
    {
        /*
         * Join to WIFI AP
         */
        ssiddata.length = strlen(ssid);
        memcpy(ssiddata.value, ssid, ssiddata.length);
        res = whd_wifi_join(iface, &ssiddata, WHD_SECURITY_WPA2_AES_PSK, (const uint8_t *)key, strlen(key));
        vTaskDelay(500);
        printf("CY_RSLT TYPE = %lx MODULE = %lx CODE= %lx\n", CY_RSLT_GET_TYPE(res), CY_RSLT_GET_MODULE(res),
               CY_RSLT_GET_CODE(res));
        if (res != CY_RSLT_SUCCESS)
        {
            retry_count++;
            if (retry_count >= MAX_WIFI_RETRY_COUNT)
            {
                printf("Exceeded max WiFi connection attempts\n");
            }
            printf("Connection to WiFi network failed. Retrying...\n");
            continue;
        }
        else
        {
            printf("Successfully joined wifi network '%s , result = %ld'\n", ssid, res);
            break;
        }
    }

    /* add interface to lwip*/
    cy_lwip_add_interface(iface, NULL);

    /* brings up the network*/
    cy_lwip_network_up();

    struct netif *net = cy_lwip_get_interface();

    while (true)
    {
        if (net->ip_addr.u_addr.ip4.addr != 0)
        {
            printf("IP Address %s assigned\n", ip4addr_ntoa(&net->ip_addr.u_addr.ip4));
            success_connection(ip4addr_ntoa(&net->ip_addr.u_addr.ip4));
            break;
        }
        vTaskDelay(100);
    }
}