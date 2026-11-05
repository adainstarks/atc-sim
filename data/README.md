# Data

`atl_arrivals.csv` has every flight scheduled to land at Atlanta Hartsfield-Jackson (ATL) that departed between May 1 and August 31, 2026. There are 109,781 flights, one per row, sorted by scheduled arrival. A flight's id is its row number, starting at 0.

## Source

U.S. Department of Transportation, Bureau of Transportation Statistics. *Airline On-Time Performance Data: Reporting Carrier On-Time Performance (1987–present)*, monthly files for May, June, July and August 2026. https://www.transtats.bts.gov/ (prezipped monthly files at https://transtats.bts.gov/PREZIP/). Downloaded November 4, 2026.

The BTS files cover domestic flights by the larger U.S. airlines, which BTS calls the reporting carriers. International, cargo and private flights aren't included.

## Columns

| Column | Meaning |
|---|---|
| `FlightDate` | Departure date |
| `Reporting_Airline` | Two-letter airline code (DL is Delta) |
| `Flight_Number_Reporting_Airline` | Flight number |
| `Origin` | Airport the flight came from |
| `CRSArrTime` | Scheduled arrival, Atlanta time, as hhmm |
| `ArrTime` | Actual arrival, as hhmm. Blank for cancelled flights. BTS sometimes fills it in for diverted flights; the simulator ignores it |
| `ArrDelay` | Minutes late, negative if early. Blank if the flight never arrived at ATL |
| `Cancelled` | 1 if cancelled |
| `Diverted` | 1 if it landed somewhere else |
| `SchedArrMin` | **Added by us.** The scheduled arrival in minutes since May 1, 2026 00:00, Atlanta time. This is the sort key |

## Counts

| | Flights |
|---|---|
| Total | 109,781 |
| Landed at ATL | 107,500 |
| … early | 70,642 (66% of landed) |
| … late | 35,395 (33%) |
| … exactly on time | 1,463 (1%) |
| Cancelled | 1,781 |
| Diverted | 500 |

The benchmark uses the first N rows:

| Size | Departure dates covered |
|---|---|
| 1,000 | May 1–2 |
| 10,000 | May 1–12 |
| 100,000 | May 1 – August 20 |

## How it was made

`python3 scripts/prepare_data.py` downloads the four monthly zips (about 32 MB each) into `data/raw/`, which git ignores. It then:

1. Keeps the rows where `Dest` is `ATL`.
2. Works out the scheduled arrival date: `FlightDate`, plus one day if `CRSArrTime` is earlier than `CRSDepTime`. That happens for a red-eye that lands after midnight.
3. Sets `SchedArrMin` to the days since May 1 × 1440, plus `CRSArrTime` in minutes.
4. Checks every landed flight: `CRSArrTime` + `ArrDelay` must equal `ArrTime`, modulo 24 hours. All 107,500 pass, so nothing was dropped. This check is why `SchedArrMin + ArrDelay` gives the real order the planes landed in.
5. Sorts by `SchedArrMin`, then airline, then flight number.

## Things to know

- All times are Atlanta local time. The four months have no daylight-saving change, so the minutes are consistent.
- Flights are included by departure date. Red-eyes that left April 30 and landed early on May 1 aren't in the file, so the first arrival is at 05:49 on May 1.
- Two-thirds of the flights landed early, because airlines pad their schedules. That's why "delay" in this project means any change to the arrival time, earlier or later.
