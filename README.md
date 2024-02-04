                   _______________________________
       ____   _____\______   \______   \_   _____/
     _/ __ \ /  ___/|    |  _/|     ___/|    __)
     \  ___/ \___ \ |    |   \|    |    |     \
      \___  >____  >|______  /|____|    \___  /
          \/     \/        \/               \/

# esBPF
esBPF is a flexible, smallest packet filtering framework for ethernet drivers that
uses cBPF virtual machine compatible with a set of BPF instructions [tcpdump](https://github.com/the-tcpdump-group/tcpdump) providing.
The framework provides simple APIs to implement filtering feature at the driver layer.

# Table of Contents
- [Architecture](#architecture)
- [API Overview](#api-overview)
- [Generate Instructions](#generate-instructions)
- [Who uses esBPF](#who-uses-esbpf)
- [Contributing](#contributing)
- [License](#license)

# Architecture
![diagram](/resources/diagram/esbpf.jpg)

# API Overview
- es-core.c/h: include core data structures, BPF interpreter, and APIs using in bottom-half handler.
- es-proc.c: A procfs class giving APIs to users to attach a filter to drivers.
- es-ctrl.h: A controller structure to keep handling core objects and locks for itself.

# Generate Instructions
It's super easy, use `tcpdump` with an option `-dd`.

For instance, if you want to reject incoming ICMP packets,
```sh
tcpdump -dd -nn icmp
```

Please see `tools/filter_icmp.c` how to use the instructions in detail.

# Who uses esBPF
1. [smsc95xx-esbpf](https://github.com/memnoth/smsc95xx-esbpf) - (Raspberry PI 3 B ethernet driver)

# Contributing
Happy hacking!

# License
Please see `LICENSE` file.
