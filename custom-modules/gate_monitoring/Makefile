MODULE := gate_monitoring
SRC += $(wildcard src/*.c) 


# SENSOR_CONFIG ?= default_sensor_config
# SENSOR_CONFIG ?= sensor_config_only_no
# CFLAGS += -O0


GEN_OUTPUT := $(shell python code_gen/gen_sensor_config_code.py -c $(CURDIR)/sensor_configs/$(SENSOR_CONFIG).json)
$(info    $(GEN_OUTPUT))

include $(RIOTBASE)/Makefile.base
