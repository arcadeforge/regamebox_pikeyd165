# Pi2Jamma polling latency results

## Test conditions

Measurements were taken on a Raspberry Pi 3B+ running Arch Linux ARM
(`armv7l`, kernel `4.19.56-1-ARCH`). The active 24-input configuration was
used. Each variant ran for 2,000 scans with debounce disabled for consistent
timing.

Scan time is measured from the beginning to the end of one polling iteration,
including GPIO reads and event processing. Cycle time is measured from the
start of one iteration to the start of the next, including the configured
polling pause and scheduler delays. Both use `CLOCK_MONOTONIC`. Polling rate is
calculated as the reciprocal of the mean cycle time. CPU percentage is total
user and system CPU time divided by process wall time; it includes startup.

## Measurements

| Variant | Shift clocks / pause | Scan min / mean / max (µs) | Cycle min / mean / max (µs) | Polling rate (Hz) | CPU / wall time | Max cycle minus mean |
|---|---:|---:|---:|---:|---:|---:|
| Original | 96 / 4 ms | 392.969 / 399.388 / 1,786.823 | 4,442.031 / 4,461.552 / 5,848.073 | 224.1 | 6.8% (0.821 / 12.082 s) | 1,386.5 µs |
| Optimized A | 32 / 4 ms | 136.979 / 139.715 / 273.697 | 4,166.458 / 4,201.747 / 4,344.583 | 238.0 | 2.6% (0.302 / 11.587 s) | 142.8 µs |
| Optimized B | 32 / 2 ms | 136.979 / 139.418 / 231.510 | 2,157.865 / 2,200.834 / 2,292.135 | 454.4 | 4.0% (0.302 / 7.597 s) | 91.3 µs |
| Optimized C | 32 / 1 ms | 136.979 / 139.385 / 264.948 | 1,195.625 / 1,200.643 / 1,338.594 | 832.9 | 5.4% (0.302 / 5.557 s) | 138.0 µs |

Each variant contains 2,000 scan samples and 1,999 cycle samples. The
96-clock run had a large scan and cycle outlier, indicating scheduling or
system jitter. Background system load was not separately measured.

## Estimated polling wait

The average phase wait assumes button transitions occur uniformly throughout
the measured polling cycle. The final column is the largest cycle interval
observed during the run; it is a sample maximum, not a guaranteed upper bound.

| Variant | Modeled average phase wait | Largest observed cycle interval |
|---|---:|---:|
| Original, 96 / 4 ms | 2.231 ms | 5.848 ms |
| Optimized A, 32 / 4 ms | 2.101 ms | 4.345 ms |
| Optimized B, 32 / 2 ms | 1.100 ms | 2.292 ms |
| Optimized C, 32 / 1 ms | 0.600 ms | 1.339 ms |

These estimates describe waiting for the next polling cycle. They do not
measure physical switch-to-Linux-event or switch-to-screen latency; input
delivery after scanning, application handling, and display response are not
included.

## Recommendation

The 32-clock scan reduced mean scan time by about 65%, from 399 µs to 140 µs.
With a 4 ms pause, mean cycle time improved by about 260 µs while CPU use fell
from 6.8% to 2.6%. A 2 ms pause achieved a 454 Hz polling rate at 4.0% CPU and
a modeled average phase wait of 1.10 ms. This is the best balance among the
tested settings. A 1 ms pause lowers modeled average phase wait to 0.60 ms and
raises CPU use to 5.4%.

These results are from one 2,000-scan run per variant under ambient system
load; repeat measurements under representative workloads may vary.
