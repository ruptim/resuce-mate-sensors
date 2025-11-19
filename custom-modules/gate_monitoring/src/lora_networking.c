/*
    Code provided by @leandrolanzieri at 
        https://github.com/leandrolanzieri/exercises/blob/add_lorawan_exercise/11-lorawan/main.c
*/

#include "lora_networking.h"

#include "net/netdev.h"
#include "net/netif.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/netif/hdr.h"

#include "od.h"
#include "msg.h"
#include "thread.h"
#include "mutex.h"



/* pointer of the LoRaWan network interface */
static netif_t * lorwan_netif;

/* Size of reception message queue */
#define QUEUE_SIZE 8

/* Stack for reception thread */
static char _rx_thread_stack[THREAD_STACKSIZE_DEFAULT];

/* Message queue for reception thread] */
static msg_t _rx_msg_queue[QUEUE_SIZE];


static mutex_t _lorawan_tx_mutex = MUTEX_INIT;


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
void *_rx_thread(void *arg);

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

void *_rx_thread(void *arg)
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


    lorwan_netif = _find_lorawan_network_interface();

    kernel_pid_t rx_pid = thread_create(_rx_thread_stack, sizeof(_rx_thread_stack),
                                    THREAD_PRIORITY_MAIN - 1,
                                    THREAD_CREATE_STACKTEST, _rx_thread, NULL,
                                    "lorawan_rx");

    if (-EINVAL == rx_pid) {
        puts("Failed to create reception thread");
        return -1;
    }

    /* register thread to receive LoRaWAN packets */
    gnrc_netreg_entry_t entry = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                    rx_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &entry);



                                    
    _join_lorawan_network(lorwan_netif);

    return 0;
}

int send_lorawan_packet(uint8_t *cbor_buf, size_t buf_size)
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
    

    packet = gnrc_pktbuf_add(NULL, cbor_buf, buf_size, GNRC_NETTYPE_UNDEF);
    if (packet == NULL) {
        puts("Failed to create packet");
        return -1;
    }

    if (gnrc_neterr_reg(packet) != 0) {
        puts("Failed to register for error reporting");
        gnrc_pktbuf_release(packet);
        return -1;
    }

    header = gnrc_netif_hdr_build(NULL, 0, &address, sizeof(address));
    if (header == NULL) {
        puts("Failed to create header");
        gnrc_pktbuf_release(packet);
        return -1;
    }

    packet = gnrc_pkt_prepend(packet, header);
    netif_header = (gnrc_netif_hdr_t *)header->data;
    netif_header->flags = 0x00;

    mutex_lock(&_lorawan_tx_mutex);

    result = gnrc_netif_send(container_of(lorwan_netif, gnrc_netif_t, netif), packet);
    if (result < 1) {
        printf("error: unable to send\n");
        gnrc_pktbuf_release(packet);
        mutex_unlock(&_lorawan_tx_mutex);
        return -1;
    }

    /* wait for transmission confirmation */
    msg_receive(&msg);
    if (msg.type != GNRC_NETERR_MSG_TYPE) {
        printf("error: unexpected message type %" PRIu16 "\n", msg.type);
        mutex_unlock(&_lorawan_tx_mutex);
        return -1;
    }
    if (msg.content.value != GNRC_NETERR_SUCCESS) {
        printf("error: unable to send, error: (%" PRIu32 ")\n", msg.content.value);
        mutex_unlock(&_lorawan_tx_mutex);
        return -1;
    }

    

    return 0;
}
