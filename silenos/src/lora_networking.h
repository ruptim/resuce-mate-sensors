#include "net/loramac.h"     /* core loramac definitions */
#include "semtech_loramac.h" /* package API */
#include <stdio.h>


#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"

#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/pktdump.h"
#include "net/loramac.h"


semtech_loramac_t loramac;  /* The loramac stack device descriptor */
/* define the required keys for OTAA, e.g over-the-air activation (the
   null arrays need to be updated with valid LoRa values) */
static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, \
                                                    0x00, 0x00, 0x00, 0x00 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = { 0x00, 0x00, 0x00, 0x00, \
                                                    0x00, 0x00, 0x00, 0x00 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0x00, 0x00, 0x00, 0x00, \
                                                    0x00, 0x00, 0x00, 0x00, \
                                                    0x00, 0x00, 0x00, 0x00, \
                                                    0x00, 0x00, 0x00, 0x00 };


int init_lora_stack(void){

    // gnrc_netreg_entry_t dump = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
    //                                                       gnrc_pktdump_pid);

    // gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);


    /* 1. initialize the LoRaMAC MAC layer */
    semtech_loramac_init(&loramac);
 
    /* 2. set the keys identifying the device */
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appeui(&loramac, appeui);
    semtech_loramac_set_appkey(&loramac, appkey);
 
    /* 3. join the network */
    if (semtech_loramac_join(&loramac, LORAMAC_JOIN_OTAA) != SEMTECH_LORAMAC_JOIN_SUCCEEDED) {
        puts("Join procedure failed");
        return 1;
    }
    puts("Join procedure succeeded");
 
    return 0;
}

int lora_send_data(uint8_t* data, size_t data_len){
    /* 4. send some data */
    if (semtech_loramac_send(&loramac,
                             data, data_len) != SEMTECH_LORAMAC_TX_DONE) {        
        return 1;
    }
    return 0;
}
