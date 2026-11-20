# A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
# project allows AI with disclosure; the output was checked against the raw rows in results.csv.
"""
PROGRAMMER:          Heidtmann Oppong
Program Description: Turns results/results.csv (written by ./atc bench) into
                      1. results/results-table.md   a results table for the report
                      2. results/results-table.csv  the same averages, for spreadsheets
                      3. results/total-time.png     total run time vs. number of flights
                      4. results/phase-times.png    add / delay / remove / land time separately

Usage (run from the repo folder, after ./atc bench):
    python3 scripts/plot_results.py
    python3 scripts/plot_results.py results/first-run-2026-11-04-sorted-vector.csv

Needs Python 3 and matplotlib (pip install matplotlib). Structures that are not in the
CSV are skipped, so the script also works on a file that has only the sorted vector.
"""

import csv
import statistics
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib

matplotlib.use("Agg")  # draw to files only; no window needed
import matplotlib.pyplot as plt

PHASES = ["add_ms", "delay_ms", "remove_ms", "land_ms"]
PHASE_TITLES = {"add_ms": "Add", "delay_ms": "Delay", "remove_ms": "Remove", "land_ms": "Land"}

# Order, labels and line styles. A name not listed here is still drawn (see ordered()).
LABELS = {
    "sorted-vector": "A: sorted vector",
    "lazy-heap": "B: heap, lazy deletion",
    "indexed-heap": "C: indexed heap",
}
STYLES = {
    "sorted-vector": dict(color="#c0392b", marker="o", linestyle="-"),
    "lazy-heap": dict(color="#2471a3", marker="s", linestyle="--"),
    "indexed-heap": dict(color="#1e8449", marker="^", linestyle="-."),
}


def read_rows(path):
    """Read every run from the CSV into {(structure, flights): {column: [values...]}}."""
    needed = {"structure", "flights", "run", "total_ms", *PHASES}
    data = defaultdict(lambda: defaultdict(list))
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        missing = needed - set(reader.fieldnames or [])
        if missing:
            sys.exit(f"{path} is missing columns: {', '.join(sorted(missing))}")
        for row in reader:
            key = (row["structure"], int(row["flights"]))
            for col in PHASES + ["total_ms"]:
                data[key][col].append(float(row[col]))
    if not data:
        sys.exit(f"{path} has no rows. Run ./atc bench first.")
    return data


def ordered(structures):
    known = [s for s in LABELS if s in structures]
    return known + sorted(s for s in structures if s not in LABELS)


def fmt(ms):
    """Milliseconds with about 3 significant digits, and thousands separators."""
    if ms < 0.0005:
        return "<0.001"  # below the resolution worth printing
    if ms >= 100:
        return f"{ms:,.0f}"
    if ms >= 10:
        return f"{ms:.1f}"
    if ms >= 1:
        return f"{ms:.2f}"
    return f"{ms:.3f}"


def summarize(data):
    """Mean of every column, plus the min and max of the total, for each (structure, size)."""
    summary = {}
    for key, cols in data.items():
        entry = {c: statistics.mean(cols[c]) for c in PHASES + ["total_ms"]}
        entry["total_min"] = min(cols["total_ms"])
        entry["total_max"] = max(cols["total_ms"])
        entry["runs"] = len(cols["total_ms"])
        summary[key] = entry
    return summary


def write_tables(summary, structures, sizes, outdir):
    runs = {v["runs"] for v in summary.values()}
    runs_text = str(runs.pop()) if len(runs) == 1 else "varying numbers of"

    lines = [
        f"Average time in milliseconds over {runs_text} runs per size. Lower is better.",
        "",
        "| Flights | Structure | Add | Delay | Remove | Land | Total |",
        "|---:|---|---:|---:|---:|---:|---:|",
    ]
    for n in sizes:
        for s in structures:
            e = summary.get((s, n))
            if e is None:
                continue
            lines.append(
                f"| {n:,} | {LABELS.get(s, s)} | {fmt(e['add_ms'])} | {fmt(e['delay_ms'])} | "
                f"{fmt(e['remove_ms'])} | {fmt(e['land_ms'])} | **{fmt(e['total_ms'])}** |"
            )

    # How much faster is each structure than the sorted vector, by total time?
    if "sorted-vector" in structures and len(structures) > 1:
        lines += [
            "",
            "Total time relative to the sorted vector (above 1 means faster than the vector):",
            "",
            "| Flights | " + " | ".join(LABELS.get(s, s) for s in structures if s != "sorted-vector") + " |",
            "|---:|" + "---:|" * (len(structures) - 1),
        ]
        for n in sizes:
            base = summary.get(("sorted-vector", n))
            cells = []
            for s in structures:
                if s == "sorted-vector":
                    continue
                e = summary.get((s, n))
                cells.append(f"{base['total_ms'] / e['total_ms']:.2f}x" if base and e else "n/a")
            lines.append(f"| {n:,} | " + " | ".join(cells) + " |")

    (outdir / "results-table.md").write_text("\n".join(lines) + "\n")

    with open(outdir / "results-table.csv", "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["structure", "flights", "runs", "add_ms", "delay_ms", "remove_ms", "land_ms", "total_ms"])
        for n in sizes:
            for s in structures:
                e = summary.get((s, n))
                if e:
                    w.writerow([s, n, e["runs"]] + [f"{e[c]:.6f}" for c in PHASES + ["total_ms"]])


def style_axes(ax, title, ylabel=True):
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_title(title)
    ax.set_xlabel("Flights in the list (n)")
    if ylabel:
        ax.set_ylabel("Time (ms, log scale)")
    ax.grid(True, which="both", alpha=0.25)


def plot_total(summary, structures, sizes, outdir):
    fig, ax = plt.subplots(figsize=(7, 4.6))
    for s in structures:
        ns = [n for n in sizes if (s, n) in summary]
        mean = [summary[(s, n)]["total_ms"] for n in ns]
        lo = [summary[(s, n)]["total_ms"] - summary[(s, n)]["total_min"] for n in ns]
        hi = [summary[(s, n)]["total_max"] - summary[(s, n)]["total_ms"] for n in ns]
        ax.errorbar(ns, mean, yerr=[lo, hi], capsize=3, label=LABELS.get(s, s), **STYLES.get(s, {}))
    style_axes(ax, "Total time for a whole run (add, delay, remove, land)")
    ax.set_xticks(sizes)
    ax.set_xticklabels([f"{n:,}" for n in sizes])
    ax.minorticks_off()
    ax.legend()
    fig.text(0.01, 0.01, "Points are averages; bars show the fastest and slowest run.", fontsize=8, alpha=0.7)
    fig.tight_layout(rect=(0, 0.03, 1, 1))
    fig.savefig(outdir / "total-time.png", dpi=200)
    plt.close(fig)


def plot_phases(summary, structures, sizes, outdir):
    fig, axes = plt.subplots(2, 2, figsize=(10, 7), sharex=True)
    for ax, phase in zip(axes.flat, PHASES):
        for s in structures:
            ns = [n for n in sizes if (s, n) in summary]
            ax.plot(ns, [summary[(s, n)][phase] for n in ns], label=LABELS.get(s, s), **STYLES.get(s, {}))
        style_axes(ax, PHASE_TITLES[phase])
        ax.set_xticks(sizes)
        ax.set_xticklabels([f"{n:,}" for n in sizes], fontsize=8)
        ax.minorticks_off()
    axes[0, 0].legend(fontsize=8)
    fig.suptitle("Time per phase (average of the runs)")
    fig.tight_layout()
    fig.savefig(outdir / "phase-times.png", dpi=200)
    plt.close(fig)


def main():
    path = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("results/results.csv")
    if not path.exists():
        sys.exit(f"Cannot find {path}. Run ./atc bench from the repo folder first.")
    outdir = Path("results")
    outdir.mkdir(exist_ok=True)

    data = read_rows(path)
    summary = summarize(data)
    structures = ordered({s for s, _ in summary})
    sizes = sorted({n for _, n in summary})

    write_tables(summary, structures, sizes, outdir)
    plot_total(summary, structures, sizes, outdir)
    plot_phases(summary, structures, sizes, outdir)

    print(f"Read {path}: {len(structures)} structure(s) x {len(sizes)} size(s).")
    if len(structures) < 3 or len(sizes) < 3:
        print("Note: the final report needs all 3 structures at all 3 sizes.")
    print("Wrote results/results-table.md, results-table.csv, total-time.png, phase-times.png")


if __name__ == "__main__":
    main()
