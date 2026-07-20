#!/usr/bin/env python3

import sys

if len(sys.argv) != 3:
    print("Usage: bin_to_hex.py input.bin output.hex")
    sys.exit(1)

with open(sys.argv[1], "rb") as f:
    data = f.read()

with open(sys.argv[2], "w") as out:
    for i in range(0, len(data), 4):
        word = data[i:i+4]

        if len(word) < 4:
            word = word.ljust(4, b"\x00")

        value = int.from_bytes(word, byteorder="little")

        out.write(f"{value:08x}\n")