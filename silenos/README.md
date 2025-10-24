# Silenos

The Silenos application is being developed as part of the [RESCUE-MATE](https://www.rescue-mate.de/) research project to monitor floodgates.

## Usage

### Sensor Config Code Generation

The script `code_gen/gen_sensor_config_code.py` allows to generate all necessary code to initialize and use the supported sensors based on a given json config file.

The list of currently supported sensor types and their type IDS are:

- 1 - Generic reed sensors
- 2 - DW-Ax-509-M18 inductive sensor

### Data Format
Sends data encoded using CBOR in the following format:

```json
{
    "d": [
        1234,
        1,
        0
    ],
    "c": 3,
    "s": 1
}
```

- `d` being the raw data array
- `c` the event counter
- `s` the sequence number.
