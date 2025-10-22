import os
import pathlib
from enum import Enum
import json
from argparse import ArgumentParser
from csnake import CodeWriter,  Variable, Function


parser = ArgumentParser()

parser.add_argument("-c", "--config", type=str, required=True, help="Config file to generate header for.")
parser.add_argument("-o", "--out-dir", type=str, required=False, default= pathlib.Path(__file__).parent.parent.joinpath("src").as_posix(), help="Output directory of the header file. (Default: ../src)")
parser.add_argument("-f", "--format-file", action="store_true",help="Run formatter on generated file." )


class SensorTypeID(Enum):
    DWAX    = "SENSOR_TYPE_ID_DWAX509M183X0"
    REED_NC = "SENSOR_TYPE_ID_REED_SWITCH_NC"
    REED_NO = "SENSOR_TYPE_ID_REED_SWITCH_NO"




def main():

    args = parser.parse_args()


    data = None
    with open(args.config,"r") as f:
        data = json.load(f)



    cw = CodeWriter()


    cw.add_lines([
        "#ifndef SENSOR_CONFIG_H_",
        "#define SENSOR_CONFIG_H_",
        "\n"
    ])
    cw.include("sensors.h")
    cw.include("dwax509m183x0.h")
    cw.include("reed_sensor_driver.h")
    cw.add_line(ignore_indent=True)




    cw.add_define("SENSOR_TYPE_ID_DWAX509M183X0",1)
    cw.add_define("SENSOR_TYPE_ID_REED_SWITCH_NC",2)
    cw.add_define("SENSOR_TYPE_ID_REED_SWITCH_NO",3)
    cw.add_line(ignore_indent=True)


    cw.add_define("REED_SENSOR_DEBOUNCE_MS",60)
    cw.add_line(ignore_indent=True)

    cw.start_comment()
    cw.add_lines([
                "Macro to encode sensor type and global sensor id in a 16-Bit integer.",
                "-----------------------------------------------------------",
                "|   Sensor Type (8 bits)     |   Sensor Number (8 bits)   |",
                "|----------------------------|----------------------------|",
                "|  7  6  5  4  3  2  1  0    |  7  6  5  4  3  2  1  0    |",
                "|---------------------------------------------------------|",
                ])
    cw.end_comment()
    cw.add_line("#define ENCODE_SENSOR_TYPE_ID(type, id) ((type) << 8 | (id))")
    cw.add_line(ignore_indent=True)


    # add sensor base type
    cw.add_lines([
        "typedef union {",
        "\treed_sensor_driver_t reed_sensor;",
        "\tdwax509m183x0_t inductive_sensor;",
        "} sensor_base_type_t;",
        "\n"
    ])

    cw.add_lines([
        "typedef union {",
        "\treed_sensor_driver_params_t reed_sensor_params;",
        "\tdwax509m183x0_params_t inductive_sensor_params;",
        "} sensor_base_params_t;",
        "\n"
    ])


    cw.add_line(comment='-' * 30 + 'Sensor Declaration' + '-' * 30)
    cw.add_line(ignore_indent=True)

    num_sensors = len(data["sensors"])
    cw.add_line(comment='Number of physical sensors connected (some sensors have multiple contacts e.g. reed sensors)')
    cw.add_define("NUM_SENSORS",num_sensors)
    cw.add_line(ignore_indent=True)

    num_unique_sensor_values = num_sensors + sum([1 for s in data["sensors"] if int(s["type"]) == 1 ])
    cw.add_define("NUM_UNIQUE_SENSOR_VALUES",num_unique_sensor_values)


    cw.add_line("static alarm_cb_args_t alarm_cb_args[NUM_UNIQUE_SENSOR_VALUES];")
    cw.add_line(ignore_indent=True)


    init_code_lines = []
    id_counter= 0
    for i,s in enumerate(data["sensors"]):

        name_base= ""

        if int(s["type"]) == 1: # REED
            name_base = "reed"


        elif int(s["type"]) == 2: # INDUCTIVE
            name_base = "dwax"

        else:
            TypeError(f"Type {s['type']} not supported!")


        ## create sensor handle variable

        s_name = f"sensor_{i+1}_{name_base}"



        ## create sensor contact defines and callback arguments
        init_code_lines.append(f"/* {'-'*20} init code for sensor '{s_name}' {'-'*20} */")
        if int(s["type"]) == 1:
            s_id_nc = f"{s_name.upper()}_NC_ID"
            cw.add_define(s_id_nc,id_counter)

            init_code_lines.extend([
                f"alarm_cb_args[{s_id_nc}].pid = thread_getpid();",
                f"alarm_cb_args[{s_id_nc}].msg.type = ENCODE_SENSOR_TYPE_ID({SensorTypeID.REED_NC.value},{id_counter});",
                f"alarm_cb_args[{s_id_nc}].msg.content.ptr = (void *)&registered_sensors[{i}];\n",
                "\n"
            ])

            id_counter += 1

            ## no
            s_id_no = f"{s_name.upper()}_NO_ID"

            cw.add_define(s_id_no,id_counter)

            init_code_lines.extend([
                f"alarm_cb_args[{s_id_no}].pid = thread_getpid();",
                f"alarm_cb_args[{s_id_no}].msg.type = ENCODE_SENSOR_TYPE_ID({SensorTypeID.REED_NO.value},{id_counter});",
                f"alarm_cb_args[{s_id_no}].msg.content.ptr = (void *)&registered_sensors[{i}];",
                "\n"
            ])


            id_counter += 1

            ## driver init
            init_code_lines.extend([
                "// first cast to specific param type and then to base params type for the array.",
                f"registered_sensors_params[{i}] = (sensor_base_params_t) (reed_sensor_driver_params_t) {{",
                f"    .nc_pin = GPIO_PIN({s['port_pin1'][0]},{s['port_pin1'][1]}),",
                f"    .no_pin = GPIO_PIN({s['port_pin2'][0]},{s['port_pin2'][1]}),",
                f"    .nc_int_flank = GPIO_BOTH,",
                f"    .no_int_flank = GPIO_BOTH,",
                f"    .nc_callback = reed_nc_callback,",
                f"    .no_callback = reed_no_callback,",
                f"    .nc_callback_args = (void *)&alarm_cb_args[{s_id_nc}],",
                f"    .no_callback_args = (void *)&alarm_cb_args[{s_id_no}],",
                f"    .use_external_pulldown = false,",
                f"    .debounce_ms = REED_SENSOR_DEBOUNCE_MS }};",
                "\n",
                f"if (reed_sensor_driver_init(&registered_sensors[{i}].reed_sensor, &registered_sensors_params[{i}].reed_sensor_params) != 0){{",
                "\treturn -1;",
                "}",
                "\n\n",
            ])




        elif int(s["type"]) == 2:
            s_id = f"{s_name.upper()}_ID"
            cw.add_define(s_id,id_counter)
            init_code_lines.extend([
                f"alarm_cb_args[{s_id}].pid = thread_getpid();"
                f"alarm_cb_args[{s_id}].msg.type = ENCODE_SENSOR_TYPE_ID({SensorTypeID.DWAX.value},{id_counter});"
                f"alarm_cb_args[{s_id}].msg.content.ptr = (void *)&registered_sensors[{i}];",
                "\n"

            ])
            id_counter += 1

        cw.add_line(ignore_indent=True)



    cw.add_line(ignore_indent=True)


    registered_sensors = Variable(
        "registered_sensors",
        primitive="sensor_base_type_t",
        array=num_sensors
    )

    registered_sensors_params = Variable(
        "registered_sensors_params",
        primitive="sensor_base_params_t",
        array=num_sensors
    )



    cw.add_variable_declaration(registered_sensors)
    cw.add_variable_declaration(registered_sensors_params)


    cw.add_line(ignore_indent=True)
    cw.add_line("int init_sensors(void)")
    cw.open_brace()
    cw.add_lines(init_code_lines + ["return 0;"])
    cw.close_brace()

    

    cw.add_line("#endif // SENSOR_CONFIG_H_")

    file_path = pathlib.Path(args.out_dir).joinpath("sensor_config.h")
    print(f"Writing code to file: {file_path}")
    with open(file_path,"w") as f:
        f.writelines(cw.code)


    if args.format_file:
        os.system(f'clang-format -i {file_path}')



if __name__ == "__main__":
    main()
