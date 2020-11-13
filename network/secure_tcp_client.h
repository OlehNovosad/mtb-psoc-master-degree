#ifndef SECURE_TCP_CLIENT_H_
#define SECURE_TCP_CLIENT_H_

#include "cy_secure_sockets.h"

#define MAKE_IPV4_ADDRESS(a, b, c, d) ((((uint32_t)d) << 24) | \
                                       (((uint32_t)c) << 16) | \
                                       (((uint32_t)b) << 8) |  \
                                       ((uint32_t)a))

/* Change the server IP address to match the TCP server address (IP address
 * of the PC).
 */
#define TCP_SERVER_IP_ADDRESS MAKE_IPV4_ADDRESS(10, 0, 0, 6)

#define TCP_SERVER_PORT (50007)

/* Maximum number of connection retries to the TCP server. */
#define MAX_TCP_SERVER_CONN_RETRIES (5)

/* Length of the TCP data packet. */
#define MAX_TCP_DATA_PACKET_LENGTH (20)

/* Length of the LED ON/OFF command issued from the TCP server. */
#define TCP_LED_CMD_LEN (1)

/* TCP client socket handle */
cy_socket_t client_handle;

#define RED_LED (P0_3)
#define GREEN_LED (P1_1)
#define BLUE_LED (P11_1)

/*******************************************************************************
* Function Prototype
********************************************************************************/
cy_rslt_t tcp_secure_client_task(bool _wifi);

#endif /* SECURE_TCP_CLIENT_H_ */
