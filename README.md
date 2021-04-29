                   _______________________________
       ____   _____\______   \______   \_   _____/ 
     _/ __ \ /  ___/|    |  _/|     ___/|    __)   
     \  ___/ \___ \ |    |   \|    |    |     \    
      \___  >____  >|______  /|____|    \___  /    
          \/     \/        \/               \/     

# esBPF
esBPF is a flexible, smallest packet filtering framework for ethernet drivers that
uses a cBPF virtual machine compatible with a set of BPF instructions which [tcpdump](https://github.com/the-tcpdump-group/tcpdump) provides.
It provides simple APIs to implement filtering feature at in-driver layer.

# Table of Contents
- [Basic Overview](#basic-overview)
- [Generate Instructions](#generate-instructions)
- [Who uses esBPF](#who-uses-esbpf)
- [Contributing](#contributing)
- [License](#license)

# Basic Overview
- core.c/h: They include core structs, instructions, and functions to manipulate packets by attached filter.
- proc.c/h: A procfs class that gives you a way to attach a filter to drivers.
- helper.h: A helper struct that might help developers to do programming on drivers.

# Generate Instructions
It's super easy, use `tcpdump` with an option `-dd`.

For instance, if you want to reject incoming ICMP packets,
```sh
tcpdump -dd -nn icmp
```

Please see `tools/filter_icmp.c` for more detail.

# Who uses esBPF
1. [smsc95xx-esbpf](https://github.com/memnoth/smsc95xx-esbpf) - (Raspberry PI 3 B ethernet driver)

# Contributing
Always welcome to good ideas but you must be nice with others.  
What you do on the net is saying who you are.

# License
Please see `LICENSE` file.
