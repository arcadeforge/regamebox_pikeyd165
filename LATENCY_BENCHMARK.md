# Pi2Jamma latency benchmark (Pi 4)

Experimental branch based on original BCM2835 driver. Do not overwrite a working installation before testing. `make clean` is important because the repository contains committed object files.

## Build and smoke test
```sh
cd src
make clean && make
sudo modprobe uinput
# Stop the existing pikeyd165 service/process before testing; verify no GPIO conflict.
sudo ./pikeyd165 -smi -ndb --poll-us 4000 --benchmark 2000 > /tmp/pikeyd-4000.csv
sudo ./pikeyd165 -smi -ndb --poll-us 2000 --benchmark 2000 > /tmp/pikeyd-2000.csv
sudo ./pikeyd165 -smi -ndb --poll-us 1000 --benchmark 2000 > /tmp/pikeyd-1000.csv
python3 ../tools/compare_poll_benchmarks.py /tmp/pikeyd-{4000,2000,1000}.csv
```

The benchmark prints min/mean/max **scan** (HC165 read plus event processing) and **cycle** (start-to-start) in microseconds. A Python helper estimates phase-dependent polling wait assuming random button arrival phase. This is **not measured switch-to-uinput latency or screen latency**. Default poll is 4000 us; `--poll-us` accepts 500–20000 us; `--benchmark` accepts 10–1000000 scans. Keep `-ndb` for fair comparison.

Test twice, with normal CPU load and with high background load. Make sure all 24 buttons map correctly; the default optimized build clocks 32 bits to preserve the existing 24-input mapping. For extended hardware rebuild with `make clean && make SCAN_BITS=96`.

## True end-to-end hardware latency
For a physical measurement trigger a switch and instrument either the switch signal and an output toggled when the input event is emitted, or measure a display response with high-speed camera. `CLOCK_MONOTONIC` benchmark timestamps alone cannot establish a physical press-to-photon delay.

This prototype has been committed but has not been built or run on physical Pi 4 hardware in this change.
