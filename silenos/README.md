Silenos
=======

Describe here what the application is about.

Usage
=====

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
