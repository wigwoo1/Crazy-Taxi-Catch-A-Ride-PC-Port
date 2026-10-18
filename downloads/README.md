# Experimental native Windows prototype — 2026-10-18

[Download the Windows x64 build](https://github.com/wigwoo1/Crazy-Taxi-Catch-A-Ride-PC-Port/raw/refs/heads/main/downloads/Crazy-Taxi-Native-Windows-x64-2026-10-18-Asset-Free.zip) · [Complete source, tests and tools](https://github.com/wigwoo1/Crazy-Taxi-Catch-A-Ride-PC-Port/raw/refs/heads/main/downloads/Crazy-Taxi-Native-Complete-Source-2026-10-18.tar.xz)

**The Windows build is asset-free, not a standalone playable game.** It needs original resources extracted locally from your own USA ROM. If you already have the development extraction, copy `outputs/crazy-taxi-native/assets` into the same relative location in the extracted build and run `Launch.cmd`. A clean one-click extraction installer is unfinished. ROMs, extracted art/music, saves and credentials are excluded.

The exact executable passed the strict 379-module build, 65 original bitmap comparisons, 80 digit-pair comparisons, three 4K rank comparisons, and native startup/driving/pause Retry/resume checks. Executable SHA256: `e25935b1dde9cafbac84db51d6f617dfcea2f984104305d3f9867c0a099fbac9`.

Full-game fidelity, sustained 60 fps, physical Xbox testing and other-PC compatibility remain unverified. The complete source archive includes historical diagnostic tests and extraction scripts; not every research probe is a supported end-user tool. Native game code is also browsable in the repository. See `SHA256SUMS.txt` for download integrity.
