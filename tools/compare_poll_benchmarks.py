#!/usr/bin/env python3
"""Compare measured poll cycles and model phase-dependent detection latency.
NOT a physical switch-to-screen measurement.
"""
import argparse
import csv
import statistics
from pathlib import Path

def read_cycle(path):
    with Path(path).open(newline="") as f:
        lines = f.readlines()\n        start = next((i for i, line in enumerate(lines) if line.startswith("metric,count,")), None)\n        if start is None:\n            raise ValueError(f"{path}: benchmark CSV header not found")\n        rows = list(csv.DictReader(lines[start:]))
    cycle = next((r for r in rows if r["metric"] == "cycle"), None)
    scan = next((r for r in rows if r["metric"] == "scan"), None)
    if not cycle or not scan:
        raise ValueError(f"{path}: missing scan/cycle rows")
    return float(cycle["mean_us"]), float(scan["mean_us"])

def main():
    p=argparse.ArgumentParser()
    p.add_argument("csv_files",nargs="+",help="stdout of --benchmark runs")
    args=p.parse_args()
    print("file,cycle_ms,scan_ms,estimated_poll_wait_avg_ms,estimated_poll_wait_worst_ms")
    for path in args.csv_files:
        cycle, scan=read_cycle(path)
        # Assumes switch transitions are uniformly distributed across polling phases.
        # Not measured end-to-end input latency; input events also incur scan and scheduling.
        print(f"{Path(path).name},{cycle/1000:.3f},{scan/1000:.3f},{cycle/2000:.3f},{cycle/1000:.3f}")
if __name__ == "__main__":
    main()
