# regamebox_pikeyd165

GPIO input daemon for Pi2Jamma and ReGamebox.

## Lower input scanning latency

The optimized build reads the 24 arcade inputs in **32 shift clocks instead
of 96**, while preserving the existing input mapping. On a Raspberry Pi 3B+,
this reduced the measured mean scan time by **65%**, from **399 µs to 140 µs**.

The recommended polling pause is **2 ms**. In the same test it delivered
**454 polls per second** at **4.0% CPU** and a modeled average wait of
**1.10 ms** for a button press to reach the next polling cycle. At the former
4 ms pause, changing to the 32-clock scan reduced measured mean cycle time
from **4.462 ms to 4.202 ms** and CPU use from **6.8% to 2.6%**.

The 1 ms option raises polling to 833 Hz and lowers modeled average polling
wait to 0.60 ms, with CPU use of 5.4%. The 2 ms setting is the recommended
balance between responsiveness and CPU use.

These figures are software scan and polling measurements, not physical
switch-to-screen latency. See [the full benchmark results](docs/LATENCY_RESULTS.md)
for the test method, all four configurations, and measurement limits.

## Install and start

The checked-in `src/pikeyd165` executable is a 32-bit ARM EABI5 binary for
Raspberry Pi systems running 32-bit Linux. It scans 32 shift clocks and uses
a 2 ms polling pause by default. The GPIO mapping and key combinations come
from the configuration file.

On the Raspberry Pi, install the binary and enable the kernel uinput device:

```sh
sudo install -m 755 src/pikeyd165 /usr/bin/pikeyd165
sudo modprobe uinput
```

When upgrading an existing installation, back up its current binary and
`/etc/pikeyd165.conf` before replacing them.

The daemon looks for `$HOME/.pikeyd165.conf` first, then
`/etc/pikeyd165.conf`. It needs root access for GPIO and uinput. To start it
in the background using the recommended settings:

```sh
sudo /usr/bin/pikeyd165 -smi -ndb -d
```

Omit `-d` to run in the foreground. Stop a daemon started with `-d` using:

```sh
sudo /usr/bin/pikeyd165 -k
```

`-k` sends a termination signal to the PID recorded in `/tmp/pikeyd.pid`.

### Start automatically at boot with systemd

Create `/etc/systemd/system/pikeyd165.service` with:

```ini
[Unit]
Description=Pi2Jamma GPIO input daemon
After=systemd-modules-load.service

[Service]
Type=simple
User=root
Environment=HOME=/root
ExecStartPre=/sbin/modprobe uinput
ExecStart=/usr/bin/pikeyd165 -smi -ndb --poll-us 2000
Restart=on-failure

[Install]
WantedBy=multi-user.target
```

Then enable and start it:

```sh
sudo systemctl daemon-reload
sudo systemctl enable --now pikeyd165.service
```

Check it with `systemctl status pikeyd165.service`. The service runs as root
in the foreground for GPIO and uinput access and explicitly uses the
recommended 2 ms pause.

## Command-line options

| Option | Description |
|---|---|
| `-d` | Run as a background daemon. |
| `-k` | Stop the daemon recorded in `/tmp/pikeyd.pid`. |
| `-smi` | Skip mouse initialization; no mouse or trackball devices are created. |
| `-ndb` | Disable key debounce. |
| `--poll-us N` | Set the delay after each scan to 500–20,000 µs. Default: 2,000 µs. |
| `--benchmark N` | Run 10–1,000,000 scans, print scan and cycle timing statistics, then exit. |
| `-v` | Print version and build information. |
| `-h` | Print help. |

Run the benchmark in the foreground and ensure no other `pikeyd165` process is
using the GPIO pins:

```sh
sudo /usr/bin/pikeyd165 -smi -ndb --poll-us 2000 --benchmark 2000
```

For a source build, use a C compiler, GNU make, and the bcm2835 development
library, then run `make SCAN_BITS=32` from `src/`. Use `SCAN_BITS=96` only for
extended hardware that requires the additional shift-register data.
