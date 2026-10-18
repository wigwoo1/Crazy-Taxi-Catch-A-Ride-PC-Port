# Crazy Taxi: Catch a Ride — native PC port

Experimental native Windows port in progress. Not a completed faithful port.

## Source

Current native code is in `outputs/crazy-taxi-native/src`; tests and research/extraction tools are included alongside it. The viewer build uses379 C modules. Research tools include offline original-instruction diagnostics; those are not the native game runtime.

## Builds

Download the Windows x64 asset-free prototype from Releases. It requires the original game resources extracted locally from your own USA ROM. ROMs, extracted assets, saves, and toolchains are intentionally excluded. A clean one-click extraction/install flow has not yet been verified. See the release instructions before downloading.

## Build from source

Install Python3 and an LLVM-MinGW x86_64 Windows compiler. Run `python build-public.py --compiler PATH/TO/x86_64-w64-mingw32-clang.exe` from the repository root. The build helper uses the canonical module list and generic x86-64 flags. The existing development scripts and offline research tools may require local path/dependency configuration; this public checkout has not yet had an independent clean-machine build test.

## Verification — 2026-10-18

The current379-module native viewer compiled with strict warnings. Rank rendering passed65 original bitmap comparisons,80 combined digit comparisons, and3 actual3840x2160 comparisons. Rendering preserved retained game state. The exact release executable passed native startup, driving, pause-menu Retry and resumed driving on the development PC. These are scoped checks, not proof of whole-game fidelity, sustained60fps, physicalXbox support, or compatibility with every PC.

## Plan

Faithful gameplay/timing/settlement; native4K widescreen; smooth60fps; Xbox controls; display settings; optional1.5x/2x/3x/4x/5x speed and acceleration settings.

Keep original raw text and character face artwork unchanged. Only approved non-text, non-face menu/HUD artwork is eligible for enhancement.

Audio: identify original tracks, accept user-supplied MP3 replacements, add Original/Remastered music with original fallback, and a Sound Test for music and sound effects.

Daily publication checks run in the development environment. No empty commits or unverified release claims are intended. No game assets or license to the original game are supplied.
