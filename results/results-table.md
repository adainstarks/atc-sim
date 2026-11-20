Average time in milliseconds over 5 runs per size. Lower is better.

| Flights | Structure | Add | Delay | Remove | Land | Total |
|---:|---|---:|---:|---:|---:|---:|
| 1,000 | A: sorted vector | 0.080 | 0.156 | 0.006 | 0.002 | **0.245** |
| 1,000 | B: heap, lazy deletion | 0.027 | 0.020 | <0.001 | 0.127 | **0.174** |
| 1,000 | C: indexed heap | 0.071 | 0.018 | 0.003 | 0.095 | **0.187** |
| 10,000 | A: sorted vector | 2.64 | 7.38 | 0.048 | 0.023 | **10.1** |
| 10,000 | B: heap, lazy deletion | 0.280 | 0.190 | 0.001 | 1.57 | **2.04** |
| 10,000 | C: indexed heap | 0.657 | 0.105 | 0.014 | 1.46 | **2.23** |
| 100,000 | A: sorted vector | 415 | 1,462 | 20.4 | 0.345 | **1,898** |
| 100,000 | B: heap, lazy deletion | 2.68 | 1.89 | 0.008 | 16.5 | **21.0** |
| 100,000 | C: indexed heap | 9.43 | 1.66 | 0.241 | 26.0 | **37.3** |

Total time relative to the sorted vector (above 1 means faster than the vector):

| Flights | B: heap, lazy deletion | C: indexed heap |
|---:|---:|---:|
| 1,000 | 1.40x | 1.31x |
| 10,000 | 4.95x | 4.51x |
| 100,000 | 90.23x | 50.88x |
