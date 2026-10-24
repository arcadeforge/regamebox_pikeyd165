# Arcade input scan optimization (Pi 4 / BCM2835)

This branch starts from **master**, not the separate Pi 5 port. It retains libbcm2835.

## Why 32 clocks rather than 24?
The original driver shifts 32 bits into `pi2jamma_input[0]`, inverts the result, then shifts right by eight:
`newGpio = (pi2jamma_input[0] ^ 0xffffffff) >> 8;`
Thus input indices 0..23 correspond to the **last 24 positions of that 32-clock word**, not its first 24. Reading only 24 clocks would shift the mapping. 32 clocks is the minimal safe drop-in replacement without reworking the serial bit alignment or validating an alternate wiring map.

## Modes
```sh
cd src
make clean && make                  # standard arcade: 32 clocks / 24 mapped inputs
make clean && make SCAN_BITS=96     # extended: 96 clocks (trackball / optional data)
```
Default 32-clock mode deliberately disables mouse event processing even without `-smi`, because the original trackball fields in `pi2jamma_input[1]` are not updated in this mode. For arcade deployment continue using `-smi -ndb`.

Only one 32-bit word is read in the default mode, so original 24 input mapping and combo definitions are unchanged. For additional 96-bit hardware functions use `SCAN_BITS=96`.

## Expected effect
Scanning 32 rather than 96 clocks reduces the serial-loop execution time by approximately two thirds, but the existing `usleep(4000)` after each scan still dominates the overall response interval. This change does **not** promise a 3x faster end-to-end input response, and does not alter polling time or debouncing.

## Testing
Before installing over a working setup:
1. Back up the original binary, compile the experimental branch separately.
2. Verify all 24 switches against `pikeyd165.conf` and the combinations, including simultaneous opposing controls.
3. Verify no incorrect or stuck inputs, and compare latency against the original on the same Pi 4 hardware.
4. For trackball/extended inputs use the 96-clock build.

Status: source change only, **not yet hardware-tested**. No changes to video/audio.
