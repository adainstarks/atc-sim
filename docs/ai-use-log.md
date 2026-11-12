# AI use log

The project handout allows AI on this project as long as we disclose meaningful help. Add a row whenever AI wrote or substantially changed something. This log feeds the AI disclosure in the final report.

| Date | Who | Tool | What it helped with | How we checked it |
|---|---|---|---|---|
| 2026-10-20 | William | Claude | Brainstorming and drafting the proposal | The team reviewed it |
| 2026-11-04 | William | Claude | Data prep script, loader, simulator, benchmark, unit tests, Structure A (sorted vector), README | Unit tests pass. All 107,500 landed flights come out in the real arrival order |
| 2026-11-12 | Heidtmann (draft for review) | OpenAI ChatGPT | Completed lazy heap and indexed heap implementations, graph script, experiment report and experiment slides in a separate working copy | g++ -O2 build; 30/30 unit tests; all three queues passed full dataset landing-order check; 5-run benchmarks for 1k/10k/100k saved in results/results.csv |
| 2026-11-12 | Heidtmann | Claude | Reviewed the completed copy; switched chart to a log y-axis; added a Big-O comparison paragraph and a platform note to the report; updated slides 3 and 5 | Rebuilt, 30/30 tests, ./atc check OK, reran bench (same ranking); report numbers checked against results/results.csv |
| 2026-11-12 | Heidtmann | Claude | Drafted combined report (sections 1-4, 6, 7), combined 9-slide deck, and docs/research-indexed-heap.md from the code and results; updated README status | Numbers checked against results/results.csv and data/README.md; counts of changes/removals computed from the CSV; authors still to review their own sections |
| 2026-11-12 | Heidtmann | Claude | Added speaker notes to the combined slides and docs/REVIEW_CHECKLIST.md | Notes timed to about 4:50; checklist items taken from the code |
| 2026-11-12 | Heidtmann | Claude | Created kid-friendly slides (ATC_Kid_Friendly_Slides.pptx) and speaker outline (ATC_Speaker_Outline.docx) | Numbers match results/results.csv; timing about 4:45 |
| 2026-11-12 | Heidtmann | Claude | Created balanced (technical but plain-English) slides and speaker outline | Numbers match results/results.csv; timing about 4:50 |
| 2026-11-12 | Heidtmann | Claude | Final consistency pass: checked report, slides and outline against the code, data and results.csv; restored PROGRAMMER headers and program descriptions in the two heap files (names per the README assignments); removed draft banners from the report; small wording fixes (3.9 to 4 times; rerun note on slide 9); rewrote docs/REVIEW_CHECKLIST.md as a reviewer checklist; updated the README (three structures, final results) and the research write-up banner | Rebuilt, 30/30 tests, ./atc check OK on all three structures; recomputed every number in the report from data/atl_arrivals.csv and results/results.csv |
| 2026-11-12 | Heidtmann | Claude | Trimmed docs/ for submission: the report is submitted as a separate file, and slides, speaker outline, earlier drafts and review notes are kept outside the repo ZIP (earlier rows above refer to those files) | Rebuilt and re-ran tests after the change; no source code touched |
