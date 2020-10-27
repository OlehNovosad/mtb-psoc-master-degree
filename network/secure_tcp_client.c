/* Header file includes. */
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

/* FreeRTOS header file. */
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

/* Standard C header file. */
#include <string.h>

/* Cypress secure socket header file. */
#include "cy_secure_sockets.h"
#include "cy_tls.h"

/* Secure TCP client task header file. */
#include "secure_tcp_client.h"

/* lwIP related header files. */
#include "cy_lwip.h"

/* Wi-Fi credentials and TCP port settings header file. */
#include "credentials.h"

/******************************************************************************
* Macros
******************************************************************************/
#define LED_ON_CMD '1'
#define LED_OFF_CMD '0'

/******************************************************************************
* Function Prototypes
******************************************************************************/
cy_rslt_t connect_to_secure_tcp_server(cy_socket_sockaddr_t address);
cy_rslt_t create_secure_tcp_client_socket();
cy_rslt_t tcp_client_recv_handler(cy_socket_t socket_handle, void *arg);
cy_rslt_t tcp_disconnection_handler(cy_socket_t socket_handle, void *arg);

/******************************************************************************
* Global Variables
******************************************************************************/

/* TLS credentials of the TCP client. */
static const char tcp_client_cert[] = keyCLIENT_CERTIFICATE_PEM;
static const char client_private_key[] = keyCLIENT_PRIVATE_KEY_PEM;

/* Root CA certificate for TCP server identity verification. */
static const char tcp_server_ca_cert[] = keySERVER_ROOTCA_PEM;

/* Variable to store the TLS identity (certificate and private key).*/
void *tls_identity;

/* TCP client socket handle */
cy_socket_t client_handle;

/* Binary semaphore handle to keep track of secure TCP server connection. */
SemaphoreHandle_t connect_to_server;

/*******************************************************************************
 * Function Name: tcp_secure_client_task
 *******************************************************************************
 * Summary:
 *  Task used to establish a secure connection to a remote TCP server and
 *  control the LED state (ON/OFF) based on the command received from TCP server.
 *
 * Parameters:
 *  void *args : Task parameter defined during task creation (unused)
 *
 * Return:
 *  void
 *
 *******************************************************************************/
cy_rslt_t tcp_secure_client_task()
{
    cy_rslt_t result;

    /* IP address and TCP port number of the TCP server to which the TCP client
     * connects to. */
    cy_socket_sockaddr_t tcp_server_address = {

        .ip_address.ip.v4 = TCP_SERVER_IP_ADDRESS,
        .ip_address.version = CY_SOCKET_IP_VER_V4,
        .port = TCP_SERVER_PORT};

    /* Create a binary semaphore to keep track of secure TCP server connection. */
    connect_to_server = xSemaphoreCreateBinary();

    /* Give the semaphore so as to connect to TCP server.  */
    xSemaphoreGive(connect_to_server);

    /* TCP client certificate length and private key length. */
    const size_t tcp_client_cert_len = strlen(tcp_client_cert);
    const size_t pkey_len = strlen(client_private_key);

    /* Initialize secure socket library. */
    result = cy_socket_init();
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Secure Socket initialization failed!\n");
        CY_ASSERT(0);
    }
    printf("Secure Socket initialized\n");

    /* Initializes the global trusted RootCA certificate. This examples uses a self signed
     * certificate which implies that the RootCA certificate is same as the certificate of
     * TCP secure server to which client is connecting to.
     */
    result = cy_tls_load_global_root_ca_certificates(tcp_server_ca_cert, strlen(tcp_server_ca_cert));
    if (result != CY_RSLT_SUCCESS)
    {
        printf("cy_tls_load_global_root_ca_certificates failed! Error code: %lu\n", result);
    }
    else
    {
        printf("Global trusted RootCA certificate loaded\n");
    }

    /* Create TCP client identity using the SSL certificate and private key. */
    result = cy_tls_create_identity(tcp_client_cert, tcp_client_cert_len,
                                    client_private_key, pkey_len, &tls_identity);
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Failed cy_tls_create_identity! Error code: %lu\n", result);
        CY_ASSERT(0);
    }

    /* Wait till semaphore is acquired so as to connect to a secure TCP server. */
    xSemaphoreTake(connect_to_server, portMAX_DELAY);

    /* Connect to the secure TCP server. If the connection fails, retry
         * to connect to the server for MAX_TCP_SERVER_CONN_RETRIES times. */
    printf("Connecting to TCP server...\n");
    result = connect_to_secure_tcp_server(tcp_server_address);

    if (result != CY_RSLT_SUCCESS)
    {
        printf("Failed to connect to TCP server. Error code: %lu\n", result);
        CY_ASSERT(0);
    }

    return result;
}

/*******************************************************************************
 * Function Name: create_secure_tcp_client_socket
 *******************************************************************************
 * Summary:
 *  Function to create a secure socket and set the socket options to use TLS
 *  identity, set call back function for handling incoming messages, call back
 *  function to handle disconnection.
 *
 * Return:
 *  cy_result result: Result of the operation.
 *
 *******************************************************************************/
cy_rslt_t create_secure_tcp_client_socket()
{
    cy_rslt_t result;

    /* Variables used to set socket options. */
    cy_socket_opt_callback_t tcp_recv_option;
    cy_socket_opt_callback_t tcp_disconnection_option;

    /* TLS authentication mode.*/
    cy_socket_tls_auth_mode_t tls_auth_mode = CY_SOCKET_TLS_VERIFY_REQUIRED;

    /* Create a new secure TCP socket. */
    result = cy_socket_create(CY_SOCKET_DOMAIN_AF_INET, CY_SOCKET_TYPE_STREAM,
                              CY_SOCKET_IPPROTO_TLS, &client_handle);

    if (result != CY_RSLT_SUCCESS)
    {
        printf("Failed to create socket! Error Code: %lu\n", result);
        return result;
    }

    /* Register the callback function to handle messages received from TCP server. */
    tcp_recv_option.callback = tcp_client_recv_handler;
    tcp_recv_option.arg = NULL;
    result = cy_socket_setsockopt(client_handle, CY_SOCKET_SOL_SOCKET,
                                  CY_SOCKET_SO_RECEIVE_CALLBACK,
                                  &tcp_recv_option, sizeof(cy_socket_opt_callback_t));
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Set socket option: CY_SOCKET_SO_RECEIVE_CALLBACK failed! "
               "Error Code: %lu\n",
               result);
        return result;
    }

    /* Register the callback function to handle disconnection. */
    tcp_disconnection_option.callback = tcp_disconnection_handler;
    tcp_disconnection_option.arg = NULL;

    result = cy_socket_setsockopt(client_handle, CY_SOCKET_SOL_SOCKET,
                                  CY_SOCKET_SO_DISCONNECT_CALLBACK,
                                  &tcp_disconnection_option, sizeof(cy_socket_opt_callback_t));
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Set socket option: CY_SOCKET_SO_DISCONNECT_CALLBACK failed! "
               "Error Code: %lu\n",
               result);
        return result;
    }

    /* Set the TCP socket to use the TLS identity. */
    result = cy_socket_setsockopt(client_handle, CY_SOCKET_SOL_TLS, CY_SOCKET_SO_TLS_IDENTITY,
                                  tls_identity, sizeof(tls_identity));
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Set socket option: CY_SOCKET_SO_TLS_IDENTITY failed! "
               "Error Code: %lu\n",
               result);
    }

    /* Set the TLS authentication mode. */
    result = cy_socket_setsockopt(client_handle, CY_SOCKET_SOL_TLS, CY_SOCKET_SO_TLS_AUTH_MODE,
                                  &tls_auth_mode, sizeof(cy_socket_tls_auth_mode_t));
    if (result != CY_RSLT_SUCCESS)
    {
        printf("Set socket option: CY_SOCKET_SO_TLS_AUTH_MODE failed! "
               "Error Code: %lu\n",
               result);
    }

    return result;
}

/*******************************************************************************
 * Function Name: connect_to_secure_tcp_server
 *******************************************************************************
 * Summary:
 *  Function to connect to secure TCP server.
 *
 * Parameters:
 *  cy_socket_sockaddr_t address: Address of TCP server socket
 *
 * Return:
 *  cy_result result: Result of the operation
 *
 *******************************************************************************/
cy_rslt_t connect_to_secure_tcp_server(cy_socket_sockaddr_t address)
{
    cy_rslt_t result = CY_RSLT_MODULE_SECURE_SOCKETS_TIMEOUT;
    cy_rslt_t conn_result;

    for (uint32_t conn_retries = 0; conn_retries < MAX_TCP_SERVER_CONN_RETRIES; conn_retries++)
    {
        /* Create a secure TCP socket */
        conn_result = create_secure_tcp_client_socket();
        if (conn_result != CY_RSLT_SUCCESS)
        {
            printf("Failed to create secure socket! Error Code: %lu\n", result);
            CY_ASSERT(0);
        }

        conn_result = cy_socket_connect(client_handle, &address, sizeof(cy_socket_sockaddr_t));
        if (conn_result == CY_RSLT_SUCCESS)
        {
            printf("============================================================\n");
            printf("TLS Handshake successful and connected to TCP server\n");
            return conn_result;
        }

        printf("Could not connect to TCP server.\n");
        printf("Trying to reconnect to TCP server...Please check if server is listening\n");

        /* The resources allocated during the socket creation (cy_socket_create)
         * should be deleted.
         */
        cy_socket_delete(client_handle);
    }

    /* Stop retrying after maximum retry attempts. */
    printf("Exceeded maximum connection attempts to the TCP server\n");

    return result;
}

/*******************************************************************************
 * Function Name: tcp_client_recv_handler
 *******************************************************************************
 * Summary:
 *  Callback function to handle incoming TCP server messages.
 *
 * Parameters:
 *  cy_socket_t socket_handle: Connection handle for the TCP client socket
 *  void *args : Parameter passed on to the function (unused)
 *
 * Return:
 *  cy_result result: Result of the operation
 *
 *******************************************************************************/
cy_rslt_t tcp_client_recv_handler(cy_socket_t socket_handle, void *arg)
{
    /* Variable to store number of bytes received. */
    uint32_t bytes_received = 0;

    char message_buffer[MAX_TCP_DATA_PACKET_LENGTH];
    cy_rslt_t result;

    printf("============================================================\n");
    result = cy_socket_recv(socket_handle, message_buffer, TCP_LED_CMD_LEN,
                            CY_SOCKET_FLAGS_NONE, &bytes_received);

    printf("MESSAGE FROM SERVER: %s\n", message_buffer);

    if (message_buffer[0] == LED_ON_CMD)
    {
        /* Turn the LED ON. */
        cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
        printf("LED turned ON\n");
    }
    else if (message_buffer[0] == LED_OFF_CMD)
    {
        /* Turn the LED OFF. */
        cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
        printf("LED turned OFF\n");
    }

    return result;
}

/*******************************************************************************
 * Function Name: tcp_disconnection_handler
 *******************************************************************************
 * Summary:
 *  Callback function to handle TCP client disconnection event.
 *
 * Parameters:
 * cy_socket_t socket_handle: Connection handle for the TCP client socket
 *  void *args : Parameter passed on to the function (unused)
 *
 * Return:
 *  cy_result result: Result of the operation
 *
 *******************************************************************************/
cy_rslt_t tcp_disconnection_handler(cy_socket_t socket_handle, void *arg)
{
    cy_rslt_t result;

    /* Disconnect the TCP client. */
    result = cy_socket_disconnect(socket_handle, 0);

    /* Free the resources allocated to the socket. */
    cy_socket_delete(socket_handle);

    printf("Disconnected from the TCP server! \n");

    /* Give the semaphore so as to connect to TCP server.  */
    xSemaphoreGive(connect_to_server);

    return result;
}

/* [] END OF FILE */
