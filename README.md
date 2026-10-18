# Crazy Taxi: Catch a Ride — native PC port

Work in progress toward a native Windows port of the GBA game. This is not a completed faithful port.

## Publication status

Initial source and build publication is being prepared. This README does not mean the code or executable has already been uploaded.

The public repository will exclude the commercial ROM, extracted game assets/music, saves, credentials, and local toolchains. Game resources must come from the user's own copy. No standalone asset-free installation workflow has yet been verified.

## Verified development milestone — 2026-10-18

- The current viewer builds from 379 native C modules.
- Rank rendering passed 65 original bitmap comparisons, 80 combined digit comparisons, and three 3840×2160 pixel comparisons in controlled fixtures.
- The pixel-corrected candidate passed native startup, driving, and pause-menu Retry checks on the development PC.
- These checks do not establish full-game fidelity, sustained 60 fps, physical Xbox validation, or compatibility with every PC.

## Remaining goals

Faithful gameplay and settlement timing; native 4K widescreen presentation; smooth 60 fps; Xbox controls; display settings; optional 1.5×, 2×, 3×, 4× and 5× speed/acceleration settings.

Preserve original raw text and character face artwork. Only approved non-text, non-face menu/HUD artwork is eligible for enhancement.

Audio plan: identify the original music tracks, accept user-supplied replacement MP3s, provide Original/Remastered music selection with original fallback, and add a Sound Test for music and sound effects.

Daily publishing checks are scheduled in the development environment. Only changed, reviewed source and appropriately verified builds should be published; no empty daily commits are required.
