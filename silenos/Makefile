# name of the application
APPLICATION = Silenos
APPDIR                          ?= $(CURDIR)/src
BINDIRBASE                      ?= $(CURDIR)/bin


SENSOR_CONFIG = default_sensor_config


# No spaces between the quotes
CFLAGS += -DCONFIG_LORAMAC_APP_EUI_DEFAULT=\"0000000000000000\"
CFLAGS += -DCONFIG_LORAMAC_DEV_EUI_DEFAULT=\"XXX\"
CFLAGS += -DCONFIG_LORAMAC_APP_KEY_DEFAULT=\"XXX\"


# If no BOARD is found in the environment, use this default:
BOARD ?= feather-nrf52840-sense


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

USEMODULE += sx1261  # for a SX1261 radio device
# USEMODULE += sx1262  # for a SX1262 radio device
# USEMODULE += sx1268  # for a SX1268 radio device
# USEMODULE += sx1272  # for a SX1272 radio device
# USEMODULE += sx1276  # for a SX1276 radio device


FEATURES_REQUIRED += periph_adc

USEPKG += tinycbor
USEPKG += semtech-loramac


GEN_OUTPUT := $(shell python code_gen/gen_sensor_config_code.py -c sensor_configs/$(SENSOR_CONFIG).json)
$(info    $(GEN_OUTPUT))

include $(RIOTBASE)/Makefile.include
