"""Build data/atl_arrivals.csv from the BTS Airline On-Time Performance files.

A.I. Disclaimer: written with help from Claude (Anthropic), an AI assistant. The semester
project allows AI with disclosure; the team checks the output (see data/README.md).

Usage, from the repo folder:
    python3 scripts/prepare_data.py               downloads May-Aug 2026 into data/raw/ first
    python3 scripts/prepare_data.py ZIP [ZIP ...] uses monthly BTS zips you already have

It keeps every flight scheduled to land at Atlanta (ATL), sorts them by scheduled arrival,
and writes the columns the simulator needs plus SchedArrMin: the scheduled arrival in
minutes since midnight (Atlanta time) on the first day. Row number = flight id.
"""
import csv
import datetime
import io
import os
import sys
import urllib.request
import zipfile

AIRPORT = "ATL"
MONTHS = [(2026, 5), (2026, 6), (2026, 7), (2026, 8)]
URL = ("https://transtats.bts.gov/PREZIP/"
       "On_Time_Reporting_Carrier_On_Time_Performance_1987_present_{y}_{m}.zip")
DATA = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "data")
OUT = os.path.join(DATA, "atl_arrivals.csv")
COLUMNS = ["FlightDate", "Reporting_Airline", "Flight_Number_Reporting_Airline", "Origin",
           "CRSArrTime", "ArrTime", "ArrDelay", "Cancelled", "Diverted", "SchedArrMin"]
NEEDED = ["FlightDate", "Reporting_Airline", "Flight_Number_Reporting_Airline", "Origin", "Dest",
          "CRSDepTime", "CRSArrTime", "ArrTime", "ArrDelay", "Cancelled", "Diverted"]


def minutes(hhmm):
    """'0530' -> 330. BTS writes midnight as 2400, which becomes 1440."""
    hhmm = hhmm.zfill(4)
    return int(hhmm[:2]) * 60 + int(hhmm[2:])


def download():
    raw = os.path.join(DATA, "raw")
    os.makedirs(raw, exist_ok=True)
    paths = []
    for y, m in MONTHS:
        path = os.path.join(raw, f"bts_{y}_{m:02d}.zip")
        if not os.path.exists(path):
            print("downloading", URL.format(y=y, m=m))
            urllib.request.urlretrieve(URL.format(y=y, m=m), path)
        paths.append(path)
    return paths


def arrivals(path):
    """Yield the rows of one monthly zip whose destination is AIRPORT, as dicts of NEEDED."""
    with zipfile.ZipFile(path) as z:
        name = next(n for n in z.namelist() if n.lower().endswith(".csv"))
        with z.open(name) as fh:
            reader = csv.reader(io.TextIOWrapper(fh, encoding="utf-8", newline=""))
            header = next(reader)
            col = {k: header.index(k) for k in NEEDED}
            for row in reader:
                if row[col["Dest"]] == AIRPORT:
                    yield {k: row[col[k]] for k in NEEDED}


def main():
    paths = sys.argv[1:] or download()
    flights, dropped = [], {"missing arrival data": 0, "delay disagrees with ArrTime": 0}
    for path in paths:
        count = 0
        for r in arrivals(path):
            count += 1
            date = datetime.date.fromisoformat(r["FlightDate"])
            # FlightDate is the departure date. An arrival clock time earlier than the
            # departure clock time means the flight lands the next day (a red-eye).
            arr_date = date + datetime.timedelta(days=1) if int(r["CRSArrTime"]) < int(r["CRSDepTime"]) else date
            cancelled = r["Cancelled"].startswith("1")
            diverted = r["Diverted"].startswith("1")
            delay = ""
            if not cancelled and not diverted:
                if r["ArrDelay"] == "" or r["ArrTime"] == "":
                    dropped["missing arrival data"] += 1
                    continue
                delay = int(float(r["ArrDelay"]))
                # Check: scheduled clock time + delay must equal the actual clock time.
                if (minutes(r["CRSArrTime"]) + delay - minutes(r["ArrTime"])) % 1440 != 0:
                    dropped["delay disagrees with ArrTime"] += 1
                    continue
            flights.append({"FlightDate": r["FlightDate"], "Reporting_Airline": r["Reporting_Airline"],
                            "Flight_Number_Reporting_Airline": int(r["Flight_Number_Reporting_Airline"]),
                            "Origin": r["Origin"], "CRSArrTime": r["CRSArrTime"], "ArrTime": r["ArrTime"],
                            "ArrDelay": delay, "Cancelled": int(cancelled), "Diverted": int(diverted),
                            "arr_date": arr_date})
        print(f"{os.path.basename(path)}: {count} flights to {AIRPORT}")

    start = min(datetime.date.fromisoformat(f["FlightDate"]) for f in flights)
    for f in flights:
        f["SchedArrMin"] = (f.pop("arr_date") - start).days * 1440 + minutes(f["CRSArrTime"])
    flights.sort(key=lambda f: (f["SchedArrMin"], f["Reporting_Airline"], f["Flight_Number_Reporting_Airline"]))

    with open(OUT, "w", newline="") as out:
        w = csv.DictWriter(out, fieldnames=COLUMNS, lineterminator="\n")
        w.writeheader()
        w.writerows(flights)

    landed = [f for f in flights if not f["Cancelled"] and not f["Diverted"]]
    print(f"\nwrote {OUT}: {len(flights)} flights, minute 0 = {start} 00:00 {AIRPORT} time")
    print(f"  scheduled arrivals from {flights[0]['FlightDate']} to {flights[-1]['FlightDate']} (departure dates)")
    print(f"  landed {len(landed)}: late {sum(f['ArrDelay'] > 0 for f in landed)}, "
          f"early {sum(f['ArrDelay'] < 0 for f in landed)}, on the minute {sum(f['ArrDelay'] == 0 for f in landed)}")
    print(f"  cancelled {sum(f['Cancelled'] for f in flights)}, diverted {sum(f['Diverted'] for f in flights)}")
    print("  dropped:", ", ".join(f"{k} {v}" for k, v in dropped.items()))
    for n in (1000, 10000, 100000):
        if n <= len(flights):
            print(f"  first {n} flights end at {flights[n - 1]['FlightDate']}")


if __name__ == "__main__":
    main()
