# AI use log

The project handout allows AI on this project as long as we disclose meaningful help. Add a row whenever AI wrote or substantially changed something. This log feeds the AI disclosure in the final report.

| Date | Who | Tool | What it helped with | How we checked it |
|---|---|---|---|---|
| 2026-10-20 | William | Claude | Brainstorming and drafting the proposal | The team reviewed it |
| 2026-11-04 | William | Claude | Data prep script, loader, simulator, benchmark, unit tests, Structure A (sorted vector), README | Unit tests pass. All 107,500 landed flights come out in the real arrival order |
| 2026-11-12 | Kyle | Claude | Debugging LazyHeapQueue.cpp & LazyHeapQueue.h, helped writing comments on LazyHeap.h  | Successfully running code and cross referencing code comments to be consistent with logic presented across documents. |
| 2026-11-19 | Heidtmann | Claude | Wrote scripts/plot_results.py (results table and charts), ran the final benchmark, drafted the Experiments section and title page of the report | Script reproduces the Nov 4 README table exactly from the first-run CSV. make test and ./atc check pass for all three structures. Table and ratios checked against the raw rows in results/results.csv |
| 2026-11-19 | Pau | Gemini | Debugging IndexedHeapQueue.h and IndexedHeapQueue.cpp, with a pos table and siftUp / siftDown helpers,Cleaned up syntax typos, method names, and compilation errors while keeping the user's structure, and Updated comparisons to break arrival-time ties by flight id, then resolved compile errors from missing private declarations in IndexedHeapQueue.h and stray text.
