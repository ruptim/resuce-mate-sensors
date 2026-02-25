MODULE := gate_monitoring
SRC += $(wildcard src/*.c) 

# select sensor configuration for gate_monitoring module
SENSOR_CONFIG ?= sensor_config_only_no
# set gate_monitoring module to use lorawan
CFLAGS+= -DGATE_MONITORING_ENABLE_DEBUG=1

# CFLAGS += -O0

DISABLE_SENSOR_INTERUPTS=1


GEN_OUTPUT := $(shell python code_gen/gen_sensor_config_code.py -c $(CURDIR)/sensor_configs/$(SENSOR_CONFIG).json $(if $(filter 1,$(DISABLE_SENSOR_INTERUPTS)),-d))
$(info    python code_gen/gen_sensor_config_code.py -c $(CURDIR)/sensor_configs/$(SENSOR_CONFIG).json $(if $(filter 1,$(DISABLE_SENSOR_INTERUPTS)),-d))
$(info    $(GEN_OUTPUT))

include $(RIOTBASE)/Makefile.base
