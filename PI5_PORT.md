# Raspberry Pi 5 experimental port

This branch replaces the BCM2835 GPIO access in pikeyd165 with libgpiod **2.x** (Linux GPIO character-device API). Original Pi 4 users should continue using master until a multi-platform backend is added.

## Build
On a Pi 5 running a recent Raspberry Pi OS:
```sh
sudo apt update
sudo apt install -y build-essential libgpiod-dev gpiod
cd src
make clean && make
```

## Hardware lines
The original gpio_joystick.h maps physical pins 16/18/15:
- CLK = BCM GPIO 23 = header pin 16 (output)
- PL = BCM GPIO 24 = header pin 18 (output)
- DIN = BCM GPIO 22 = header pin 15 (input)

The port defaults to /dev/gpiochip0. **Verify** that this is the RP1 pin-controller on the installed kernel using `gpioinfo`; gpiochip numbering can vary. Set `PIKEYD_GPIOCHIP=/dev/gpiochipN` to the correct chip. Run with sufficient permissions to access /dev/gpiochipN and /dev/uinput.

## Smoke test
```sh
sudo modprobe uinput
sudo PIKEYD_GPIOCHIP=/dev/gpiochip0 ./pikeyd165 -smi
```
Run `evtest` against the new input device, press individual switches, confirm mapping and no stuck buttons. Ensure no other daemon owns the GPIO lines. Never connect unknown voltage levels to Pi GPIO.

## Timing and latency
- NRHC165BITS = 96.
- The current serial clock loop calls four 1-us delays per bit, so the **theoretical** minimum is 384 us plus PL delay (~9 us), GPIO-call overhead, and scheduling.
- main() calls `usleep(4000)` **after** a scan. This is not a fixed 4-ms scan period. The ideal upper scan rate is ~1/(4.0+0.393) ms = 228 Hz before further overhead.
- libgpiod does kernel ioctls for each edge, and can significantly lengthen scans compared with direct MMIO, so measure the scan time, overall period, CPU usage, and key-to-event latency on real Pi 5 hardware. The existing software microsecond delays are retained as minimum intervals but **not** precise waveform timings under Linux.
- The debounce mechanism uses *scan counts*, not wall-clock duration. Increasing scan time increases effective debounce and latency. The `-ndb` switch bypasses this filtering.
- Optimize only after a real measurement: a hardware SPI/PIO-style implementation or a faster RP1 GPIO strategy may be appropriate if the 96-bit scan gets too slow.

## Scope / status
Unverified prototype: source adapted, not compiled or tested against a physical Pi 5 in this change. No video, sync, or audio changes; Pi2Jamma overall compatibility is not established. Input mapping and uinput emission are retained.
