#include "network.h"
#include "cy_retarget_io.h"
#include "cyhal.h"
#include "cybsp_wifi.h"
#include "cy_lwip.h"

#include "lwip/tcpip.h"
#include "lwip/api.h"
#include "FreeRTOS.h"
#include "task.h"

#include "display.h"

#define WIFI_SSID "KV80"
#define WIFI_KEY "KV80Oleh0410T5R4"

#define MAX_WIFI_RETRY_COUNT (3)

static whd_interface_t iface;

void donothing()
{
}

cy_rslt_t connect_to_wifi_ap()
{
    cy_rslt_t res;

    const char *ssid = WIFI_SSID;
    const char *key = WIFI_KEY;
    int retry_count = 0;

    whd_ssid_t ssiddata;
    tcpip_init(donothing, NULL);

    cy_lwip_nw_interface_t nw_interface;
    printf("LWiP TCP/IP stack initialized\r\n");
    /*
     *   Initialize wifi driver
     */
    cybsp_wifi_init_primary(&iface);
    printf("WIFI driver initialized \r\n");

    while (1)
    {
        /*
         * Join to WIFI AP
         */
        ssiddata.length = strlen(ssid);
        memcpy(ssiddata.value, ssid, ssiddata.length);
        res = whd_wifi_join(iface, &ssiddata, WHD_SECURITY_WPA2_AES_PSK, (const uint8_t *)key, strlen(key));
        vTaskDelay(500);
        printf("CY_RSLT TYPE = %lx MODULE = %lx CODE= %lx\r\n", CY_RSLT_GET_TYPE(res), CY_RSLT_GET_MODULE(res),
               CY_RSLT_GET_CODE(res));
        if (res != CY_RSLT_SUCCESS)
        {
            retry_count++;
            if (retry_count >= MAX_WIFI_RETRY_COUNT)
            {
                printf("Exceeded max WiFi connection attempts\r\n");
                return res;
            }
            printf("Connection to WiFi network failed. Retrying...\r\n");
            continue;
        }
        else
        {
            printf("Successfully joined wifi network '%s , result = %ld'\r\n", ssid, res);
            break;
        }
    }

    nw_interface.role = CY_LWIP_STA_NW_INTERFACE;
    nw_interface.whd_iface = iface;

    /* add interface to lwip*/
    cy_lwip_add_interface(&nw_interface, NULL);

    /* brings up the network*/
    cy_lwip_network_up(&nw_interface);

    struct netif *net = cy_lwip_get_interface(CY_LWIP_STA_NW_INTERFACE);

    while (true)
    {
        if (net->ip_addr.u_addr.ip4.addr != 0)
        {
            printf("IP Address %s assigned\r\n", ip4addr_ntoa(&net->ip_addr.u_addr.ip4));
            success_connection(ip4addr_ntoa(&net->ip_addr.u_addr.ip4));
            break;
        }
        vTaskDelay(100);
    }
    return res;
}