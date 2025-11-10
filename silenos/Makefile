ifneq (,$(wildcard ./.env))
    include .env
endif

# name of the application
APPLICATION = Silenos
APPDIR                          ?= $(CURDIR)/src
BINDIRBASE                      ?= $(CURDIR)/bin



SENSOR_CONFIG ?= default_sensor_config



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

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1



USEMODULE += dwax509m183x0
USEMODULE += reed_sensor_driver
USEMODULE += ztimer_sec
USEPKG += tinycbor


# interfaces
USEMODULE += netdev_default
USEMODULE += auto_init_gnrc_netif

# GNRC modules to build and receive packets
USEMODULE += gnrc_pktbuf
USEMODULE += gnrc_netif_hdr
USEMODULE += gnrc_netapi
USEMODULE += gnrc_netreg
USEMODULE += gnrc_neterr

# Include OD to print received packets
USEMODULE += od
# Add support for GNRC LoRaWAN
USEMODULE += gnrc_lorawan

# We want to use OTAA join procedure
CFLAGS += -DCONFIG_LORAMAC_DEFAULT_JOIN_PROCEDURE_OTAA

USEMODULE += sx127x
LORA_REGION = EU868

# ifeq ($(BOARD),adafruit-feather-nrf52840-express)
# Indicate how we connected the LoRa radio to the microcontroller
CFLAGS += -DSX127X_PARAM_SPI_NSS=GPIO_PIN\(0,27\) #B
CFLAGS += -DSX127X_PARAM_RESET=GPIO_PIN\(0,26\) #C
CFLAGS += -DSX127X_PARAM_DIO0=GPIO_PIN\(0,6\) #A
CFLAGS += -DSX127X_PARAM_DIO1=GPIO_PIN\(1,8\) #E
CFLAGS += -DSX127X_PARAM_DIO2=GPIO_PIN\(0,7\) #D
CFLAGS += -DSX127X_PARAM_DIO3=GPIO_UNDEF
CFLAGS += -DSX127X_PARAM_DIO4=GPIO_UNDEF
CFLAGS += -DSX127X_PARAM_DIO5=GPIO_UNDEF
CFLAGS += -DSX127X_PARAM_PASELECT=SX127X_PA_BOOST
# endif

CFLAGS += -DDEBUG_ASSERT_VERBOSE



GEN_OUTPUT := $(shell python code_gen/gen_sensor_config_code.py -c sensor_configs/$(SENSOR_CONFIG).json)
$(info    $(GEN_OUTPUT))

include $(RIOTBASE)/Makefile.include
