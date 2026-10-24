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
