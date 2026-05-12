ifneq (,$(wildcard ./.env))
    include .env
endif

# name of the application
APPLICATION = Silenos

PROJECT_BASE ?= $(CURDIR)/../
EXTERNAL_MODULE_DIRS += $(PROJECT_BASE)/custom-modules
SRC := src/main.c

BINDIRBASE                      ?= $(CURDIR)/bin



# No spaces between the quotes
CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"0000000000000000\"
CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"${LORAWAN_DEV_EUI}\"
CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"${LORAWAN_APP_KEY}\"

$(info  LoRaWAN DEV_EUI: ${LORAWAN_DEV_EUI})

# If no BOARD is found in the environment, use this default:
# BOARD ?= adafruit-feather-nrf52840-sense
BOARD ?= adafruit-feather-nrf52840-express


# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= ../RIOT

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1
VERBOSE_ASSERT ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

CFLAGS+= -DGATE_MONITORING_USE_LORAWAN=true
CFLAGS+= -DCONFIG_GNRC_PKTBUF_SIZE=512

# [from gnrc/lorwan example]

# For TTN, It's necessary to set the RX2 DR to 3 in EU_868 region
CFLAGS += -DCONFIG_LORAMAC_DEFAULT_RX2_DR_3

# For FIT-IoT Lab usage. Use the highest DR since gateway is nearby.
# If uncommented, the default value (DR0) is used.
# Note this value is also used for the OTAA.
CFLAGS += -DCONFIG_LORAMAC_DEFAULT_DR_3 # max size of cbor buffer is 114 bytes, DR3 is 1760 bits/s

# Set default messages to unconfirmable
CFLAGS += -DCONFIG_LORAMAC_DEFAULT_TX_MODE_UNCNF

# Set region
CFLAGS += -DCONFIG_LORAMAC_REGION_EU_868




USEMODULE += gate_monitoring


USEMODULE += shell_cmds_default
USEMODULE += ps

ENABLE_DEBUG = 1

USEMODULE += ztimer_sec
USEPKG += tinycbor









include $(RIOTBASE)/Makefile.include
