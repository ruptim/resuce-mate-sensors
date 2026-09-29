/*
    Code provided by @leandrolanzieri at 
        https://github.com/leandrolanzieri/exercises/blob/add_lorawan_exercise/11-lorawan/main.c
*/

#include "lora_networking.h"
#include "cbor_encoding.h"
#include "lw.h"

#include "net/netdev.h"
#include "net/netif.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif/hdr.h"
#include "net/gnrc/pktdump.h"

#include "od.h"
#include "msg.h"
#include "thread.h"
#include "mutex.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LOG_LEVEL   LOG_INFO
#include "log.h"

#define _LOGDBG(...) LOG_DEBUG("[LoRaWAN]: " __VA_ARGS__)
#define _LOGINF(...) LOG_INFO("[LoRaWAN]: " __VA_ARGS__)

#define SILENOS_DISPATCH_MSG 0x1

/* pointer of the LoRaWan network interface */
static netif_t * lorwan_netif;

// static gnrc_netreg_entry_t entry;

static kernel_pid_t rx_pid;

/* Size of reception message queue */
#define QUEUE_SIZE 8

/* Stack for reception thread */
static char _rx_thread_stack[THREAD_STACKSIZE_DEFAULT];

/* Message queue for reception thread] */
static msg_t _rx_msg_queue[QUEUE_SIZE];


static mutex_t _lorawan_tx_mutex = MUTEX_INIT;

/* Stack for sending thread */
static char _tx_thread_stack[THREAD_STACKSIZE_DEFAULT];

/* Message queue for sending thread */
static msg_t _tx_msg_queue[QUEUE_SIZE];

static kernel_pid_t tx_pid;

static uint8_t data_buffer[CBOR_BUFFER_SIZE];

bool lorawan_connected = false;


/**
 * @brief   Find the LoRaWAN network interface in the registry.
 * @return Pointer to the LoRaWAN network interface, or NULL if not found.
 */
static netif_t *_find_lorawan_network_interface(void);

/**
 * @brief   Join the LoRaWAN network using OTAA.
 * @param   netif  Pointer to the LoRaWAN network interface.
 *
 * This function will attempt to join the LoRaWAN network using Over-The-Air
 * Activation (OTAA). It will keep retrying until a successful join is achieved.
 */
static void _join_lorawan_network(const netif_t *netif);

/**
 * @brief   Print to STDOUT the received packet.
 * @param   pkt  Pointer to the received packet.
 */
static void _print_received_packet(gnrc_pktsnip_t *pkt);

/**
 * @brief   Routine for packet reception thread.
 * @param   arg  not used.
 */
static void *_rx_thread(void *arg);

/**
 * @brief   Send a LoRaWAN packet with the cbor data.
 * @param   netif       Pointer to the LoRaWAN network interface.
 * @param   cbor_buf    Pointer to the cbor data to be sent.
 * @param   buf_size    Length of the cbor data to be sent.
 *
 * @retval   0 on success
 * @retval  -1 on failure
 */
int _send_lorawan_packet(uint8_t *cbor_buf, size_t buf_size);


/**
 * @brief   Routine for packet sending thread.
 * @param   arg  not used.
 */
static void *_tx_thread(void *arg);

static netif_t *_find_lorawan_network_interface(void)
{
    netif_t *netif = NULL;
    uint16_t device_type = 0;

    do {
        netif = netif_iter(netif);
        if (netif == NULL) {
            puts("No network interface found");
            break;
        }
        netif_get_opt(netif, NETOPT_DEVICE_TYPE, 0, &device_type, sizeof(device_type));
    } while (device_type != NETDEV_TYPE_LORA);

    return netif;
}

static void _join_lorawan_network(const netif_t *netif)
{
    assert(netif != NULL);
    netopt_enable_t status;
    uint8_t data_rate = 5;

    while (1) {
        status = NETOPT_ENABLE;
        printf("Joining LoRaWAN network...\n");
        ztimer_now_t timeout = ztimer_now(ZTIMER_SEC);
        netif_set_opt(netif, NETOPT_LINK, 0, &status, sizeof(status));

        while (ztimer_now(ZTIMER_SEC) - timeout < 10000) {
            /* Wait for a while to allow the join process to complete */
            ztimer_sleep(ZTIMER_SEC, 1);
            puts("Checking LoRaWAN connection ...");
            netif_get_opt(netif, NETOPT_LINK, 0, &status, sizeof(status));
            if (status == NETOPT_ENABLE) {
                printf("Joined LoRaWAN network successfully\n");

                /* Set the data rate */
                netif_set_opt(netif, NETOPT_LORAWAN_DR, 0, &data_rate, sizeof(data_rate));

                /* Disable uplink confirmation requests */
                status = NETOPT_DISABLE;
                netif_set_opt(netif, NETOPT_ACK_REQ, 0, &status, sizeof(status));
                lorawan_connected = true;
                return;
            }
        }
    }
}

static void _print_received_packet(gnrc_pktsnip_t *pkt)
{
    assert(pkt != NULL);

    gnrc_pktsnip_t *snip = pkt;

    while (snip != NULL) {
        /* LoRaWAN payload will have 'undefined' type */
        if (snip->type == GNRC_NETTYPE_UNDEF) {
            od_hex_dump(((uint8_t *)pkt->data), pkt->size, OD_WIDTH_DEFAULT);
        }
        snip = snip->next;
    }

    gnrc_pktbuf_release(pkt);
}

static void *_rx_thread(void *arg)
{
    (void)arg;
    msg_t msg;

    msg_init_queue(_rx_msg_queue, QUEUE_SIZE);

    while (1) {
        msg_receive(&msg);

        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            puts("Received data");
            gnrc_pktsnip_t *pkt = msg.content.ptr;
            _print_received_packet(pkt);
            // TODO: handle packets 
        }
    }

    /* never reached */
    return NULL;
}


int init_lorawan_stack(void){
    (void ) _rx_thread;
    (void ) _rx_thread_stack;

    lorwan_netif = _find_lorawan_network_interface();

    rx_pid = thread_create(_rx_thread_stack, sizeof(_rx_thread_stack),
                                    THREAD_PRIORITY_MAIN - 1,
                                    THREAD_CREATE_STACKTEST, _rx_thread, NULL,
                                    "lorawan_rx");
    
    if (-EINVAL == rx_pid) {
        puts("Failed to create reception thread");
        return -1;
    }

   
    /* register thread to receive LoRaWAN packets */
    entry =  (gnrc_netreg_entry_t) GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                    rx_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &entry);

    pid_t tx_pid = thread_create(_tx_thread_stack, sizeof(_tx_thread_stack),
                                    THREAD_PRIORITY_MAIN - 1,
                                    THREAD_CREATE_STACKTEST, _tx_thread, NULL,
                                    "lorawan_tx");
    if (-EINVAL == tx_pid) {
        puts("Failed to create sending thread");
        return -1;
    }

    _join_lorawan_network(lorwan_netif);

    return 0;
}


int notify_tx_thread(uint8_t *cbor_buf, size_t buf_size){
     
    mutex_lock(&_lorawan_tx_mutex);

    memcpy(data_buffer, cbor_buf, buf_size);
    msg_t msg;
    msg.type = SILENOS_DISPATCH_MSG;
    msg_send(&msg,tx_pid);

    mutex_unlock(&_lorawan_tx_mutex);
    return 0;
}

int _send_lorawan_packet(uint8_t *cbor_buf, size_t buf_size)
{


    if(!lorawan_connected){
        puts("[INFO] No LoRaWan connection: can't send data!");
        return -1;
    }

    assert(lorwan_netif != NULL);
    assert(cbor_buf != NULL);

     int result;
    gnrc_pktsnip_t *packet;
    gnrc_pktsnip_t *header;
    gnrc_netif_hdr_t *netif_header;
    uint8_t address = 1;
    msg_t msg;

    _LOGDBG("Package size: %d\n", buf_size);
    packet = gnrc_pktbuf_add(NULL, cbor_buf, buf_size, GNRC_NETTYPE_UNDEF);
    if (packet == NULL) {
        _LOGDBG("Failed to create packet.");
        return -1;
    }
    printf("==> Post Packet\n");
    gnrc_pktbuf_stats();

    if (gnrc_neterr_reg(packet) != 0) {
        _LOGDBG("Failed to register for error reporting.");
        gnrc_pktbuf_release(packet);
        return -2;
    }

    header = gnrc_netif_hdr_build(NULL, 0, &address, sizeof(address));
    if (header == NULL) {
        _LOGDBG("Failed to create header.");
        gnrc_pktbuf_release(packet);
        return -3;
    }

    packet = gnrc_pkt_prepend(packet, header);
    netif_header = (gnrc_netif_hdr_t *)header->data;
    netif_header->flags = 0x00;

    result = gnrc_netif_send(container_of(lorwan_netif, gnrc_netif_t, netif), packet);
    if (result < 1) {
        _LOGDBG("Error unable to send.\n");
        gnrc_pktbuf_release(packet);
        return -4;
    }

    /* wait for transmission confirmation */
    msg_receive(&msg);
    if (msg.type != GNRC_NETERR_MSG_TYPE) {
        _LOGDBG("Error unexpected message type %" PRIu16 ".\n", msg.type);
        return -5;
    }
    if (msg.content.value != GNRC_NETERR_SUCCESS) {
        _LOGDBG("Error unable to send, error: (%" PRIu32 ").\n", msg.content.value);
        return -6;
    }

    return 0;
    
}



static void *_tx_thread(void *arg)
{
    (void)arg;
    msg_t msg;
    /* initialize the message queue] */
    msg_init_queue(_tx_msg_queue, QUEUE_SIZE);


    /* registration entry for incoming packets */
    static gnrc_netreg_entry_t netreg_entry;

    /* register for receiving  LoRaWAN packets in our rx thread */
    gnrc_netreg_entry_init_pid(&netreg_entry,
                               GNRC_NETREG_DEMUX_CTX_ALL,
                               thread_getpid());

    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &netreg_entry);

    /* update the pid with valid value once it is ready to receive messages */
    tx_pid = thread_getpid();

    while (1) {
        msg_receive(&msg);
        if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            // _handle_received_packet(pkt);
        } else if(msg.type == SILENOS_DISPATCH_MSG) {
            mutex_lock(&_lorawan_tx_mutex);
            _send_lorawan_packet(data_buffer, sizeof(data_buffer));
            mutex_unlock(&_lorawan_tx_mutex);
        }        

    }
    /* never reached */
    return NULL;
}
