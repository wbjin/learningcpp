# Kernel Tracing using ftrace

A guide on tracing the kernel using `ftrace`

Compile the target program to trace. This contains a program that sends and
receives UDP packets.
```bash
gcc target.c -o target
```

Mount the `tracefs` for interfacing with `ftrace`
```bash
sudo mount -t tracefs nodev /sys/kernel/tracing
ln -s /sys/kernel/tracing /tracing
```

To trace the send path
```bash
sudo bash trace.sh send ./target client
```

To trace the IRQ related operations
```bash
sudo bash trace.sh irq ./target server
```

If you want to trace specific device driver functions, find the function and
add it to `set_graph_function`
```bash
 grep -iE 'enp1s0f0|poll|irq|msix|napi' /proc/kallsyms | grep igb
```

To find the defintions and usages of a specific function
```bash
rg 'ip_send_skb\s*\('
```
From the root of the kernel source.
