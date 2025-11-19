from enum import Enum
from csnake import CodeWriter, Struct, Variable, Function, VariableValue, Subscript, TextModifier
import numpy as np
from typing import Literal


cw = CodeWriter()
num_sensors=5

cw.add_define("NUM_SENSORS", num_sensors)

registered_sensors = Variable(

    "registered_sensors", primitive="sensor_base_type_t", array=f"NUM_SENSORS"
)


sensor_id = TextModifier(f"SENSOR_{0}_ID")
cw.add_define(sensor_id.text, 0)


i = 1
cw.add_lines(
                    [
                        f"alarm_cb_args[0].pid = thread_getpid();",
                        f"alarm_cb_args[0].msg.type = ENCODE_SENSOR_TYPE_ID({1000},{0});",
                        f"alarm_cb_args[{0}].msg.content.ptr = (void *)&{Subscript(registered_sensors, sensor_id)};\n",
                        "\n",
                    ]
                )




print(cw)
