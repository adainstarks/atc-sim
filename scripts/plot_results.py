#!/usr/bin/env python3
"""Plot ATC benchmark CSV files; only measured results are displayed.
Usage: python3 scripts/plot_results.py --results results --output results/charts
Requires: matplotlib (python3 -m pip install matplotlib)
"""
import argparse
import csv
from collections import defaultdict
from pathlib import Path
from statistics import mean

METRICS = ('add_ms', 'delay_ms', 'remove_ms', 'land_ms', 'total_ms')

def load_results(directory):
    samples = defaultdict(lambda: defaultdict(list))
    for path in [directory / 'results.csv']:
        if not path.exists():
            continue
        with path.open(newline='', encoding='utf-8-sig') as stream:
            reader = csv.DictReader(stream)
            if not reader.fieldnames or not {'structure', 'flights', *METRICS}.issubset(reader.fieldnames):
                print(f'Skipping {path.name}: not a benchmark CSV')
                continue
            for row in reader:
                try:
                    structure = row['structure'].strip()
                    flights = int(row['flights'])
                    if not structure or flights <= 0:
                        raise ValueError('invalid structure or flight count')
                    values = {key: float(row[key]) for key in METRICS}
                except (ValueError, TypeError) as exc:
                    print(f'Skipping invalid row in {path.name}: {exc}')
                    continue
                samples[(structure, flights)][path.name].append(values)
    # Group by file first so duplicate copies are not silently counted as new runs.
    summary = {}
    for key, sources in samples.items():
        rows = [r for group in sources.values() for r in group]
        summary[key] = {'runs': len(rows), **{m: mean(r[m] for r in rows) for m in METRICS}}
    return summary

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--results', type=Path, default=Path('results'))
    parser.add_argument('--output', type=Path, default=Path('results/charts'))
    args = parser.parse_args()
    data = load_results(args.results)
    if not data:
        parser.error('No valid benchmark CSV files found')
    args.output.mkdir(parents=True, exist_ok=True)
    table = args.output / 'benchmark_summary.csv'
    with table.open('w', newline='', encoding='utf-8') as file:
        writer = csv.writer(file)
        writer.writerow(['structure', 'flights', 'runs', *METRICS])
        for (structure, flights), stats in sorted(data.items()):
            writer.writerow([structure, flights, stats['runs'], *(f'{stats[m]:.6f}' for m in METRICS)])
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    for metric in METRICS:
        fig, ax = plt.subplots(figsize=(9, 5.5))
        for structure in sorted({s for s, _ in data}):
            points = sorted((n, data[(s, n)][metric]) for s, n in data if s == structure)
            ax.plot([p[0] for p in points], [p[1] for p in points], marker='o', linewidth=2, label=structure)
        ax.set_xscale('log')
        ax.set_yscale('log')  # log scale so fast and slow structures are all visible
        ax.set_xticks(sorted({n for _, n in data}))
        ax.set_xticklabels([f'{n:,}' for n in sorted({n for _, n in data})])
        ax.set_xlabel('Number of flights')
        ax.set_ylabel('Average time (milliseconds, log scale)')
        ax.set_title(f'ATC Simulator: {metric.replace("_ms", "").replace("_", " ").title()} Time')
        ax.grid(True, alpha=.25)
        ax.legend(title='Measured structures')
        fig.tight_layout()
        fig.savefig(args.output / f'{metric}.png', dpi=180)
        plt.close(fig)
    print(f'Wrote {table} and {len(METRICS)} charts to {args.output}')
    print('Plots are based only on results/results.csv from the current benchmark.')

if __name__ == '__main__':
    main()
