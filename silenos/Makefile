# name of the application
APPLICATION = Silenos
APPDIR                          ?= $(CURDIR)/src
BINDIRBASE                      ?= $(CURDIR)/bin
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

USEMODULE += sx1261  # for a SX1261 radio device
# USEMODULE += sx1262  # for a SX1262 radio device
# USEMODULE += sx1268  # for a SX1268 radio device
# USEMODULE += sx1272  # for a SX1272 radio device
# USEMODULE += sx1276  # for a SX1276 radio device


FEATURES_REQUIRED += periph_adc

USEPKG += tinycbor
USEPKG += semtech-loramac


include $(RIOTBASE)/Makefile.include
