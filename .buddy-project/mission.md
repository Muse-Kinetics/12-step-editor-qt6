# Mission

The 12 Step Editor repository (`QT/12StepEditor.pro`) is the single Qt editor used to configure 12 Step hardware, manage presets, troubleshoot device connectivity, and deliver firmware updates. Unlike SoftStep there is no Basic/Advanced split — one editor, one `.pro` file.

## Goals

- Migrate to the SoftStep-validated architecture per `../SOP-WMS-and-Chunked-Firmware-Update-Migration.md`: Windows MIDI Services (WMS) as primary MIDI backend with automatic WinMM fallback, plus chunked/packetized firmware SysEx updates.
- Consolidate the three separate device/transport submodules (`KMI_KMDM`, `KMI_Ports`, `KMI_Updates`) into one `KMI_MDM`-style submodule, mirroring SoftStep's `shared/KMI_MDM`.
- Fix the stale/incorrect `.gitmodules` (rtmidi points at `thestk/rtmidi.git` upstream but is actually pinned to a Muse-Kinetics fork commit; leftover unused `qt-qunexus/inc/*` entries reference a directory that doesn't exist in this working tree).
- Stand up `.vscode/` dev tooling (12 Step currently has none) so `KMI_MIDI_BACKEND=winmm` fallback testing is possible during development.
- Keep embedded firmware assets and updater logic aligned with the actual firmware packaging pipeline (`-cs512` chunk-safe `.syx`).

## Current Architecture (starting point — pre-migration)

- Single editor: `QT/12StepEditor.pro`.
- Submodules under `QT/inc/`: `KMI_KMDM` (branch `12Step`), `KMI_Ports` (branch `12Step`), `KMI_Updates` (branch `main`), `rtmidi` (pinned at commit `a3fa1dea78e...`, tag-described `2.1.1-427-ga3fa1de`).
- `rtmidi` submodule's `origin` remote is `git@github.com:Muse-Kinetics/rtmidi.git` (the fork) — but `.gitmodules` lists `https://github.com/thestk/rtmidi.git` (stock upstream). URL/commit mismatch, same issue flagged for K-Mix in the SOP.
- `.gitmodules` also contains four `qt-qunexus/inc/*` submodule entries (`KMI_Ports`, `KMI_MIDI_Device_Manager`, `rtmidi`, `KMI_Updates`) — these paths do not exist in the working tree; stale leftovers from an unrelated tree, per SOP §2.
- No `.vscode/` tooling exists yet.
- No dual-backend (WMS/WinMM) support, no chunked firmware transport, no consolidated `KMI_MDM` — none of the SoftStep-side migration work has started here.

## Target Architecture (per SOP)

- One consolidated `KMI_MDM`-equivalent submodule replacing `KMI_KMDM`/`KMI_Ports`/`KMI_Updates`, reconciled against SoftStep's `shared/KMI_MDM` `main` branch tip (currently `c5c549b` or later) by porting only genuinely 12-Step-specific fixes forward — not a full branch merge.
- `rtmidi` submodule repointed to the correct fork URL (`git@github.com:Muse-Kinetics/rtmidi.git`), `WMS` branch, commit `b79b83e` or later.
- `.pro` file updated with `INCLUDEPATH`/`SOURCES`/`HEADERS` for the consolidated submodule, using `standalone/advanced/SoftStepAdvanced.pro` (SoftStep repo) as the template, plus the Windows `win32{}` block for WMS SDK/WinRT projection headers.
- Dual MIDI backend (WMS primary, WinMM fallback, `KMI_MIDI_BACKEND` override), pre-1.0.0 firmware guard, chunked firmware transport via `KMI_fwupdate`/`KMI_mdm`, shared `diagnosticlogger.cpp/h`.
- `.vscode/` tooling copied from SoftStep (generic scripts unchanged, `tasks.json`/`launch.json` retargeted) — **note the app-data log path uses organization name `KeithMcMillenInstruments`** (no spaces), unlike other products; using a spaced variant will silently point the log-tail task at an empty directory.

## Constraints

- Follow `../SOP-WMS-and-Chunked-Firmware-Update-Migration.md` as the authoritative procedure — it is a plan, not a log of completed work, and 12 Step is listed as **not started**.
- 12 Step is first in the SOP's execution order (12 Step → QuNexus → QuNeo → K-Mix) specifically because it needs full submodule retirement plus `.gitmodules` cleanup — expect this to surface the hardest pattern first.
- Do not attempt a full `KMI_MDM` branch merge between product branches and SoftStep's `main` — diff and port forward selectively (SOP §3).
- Validate changes against real 12 Step hardware when transport/bootloader-timing behavior changes.
- `-cs512` is the standard chunk size for all four SOP target editors; don't second-guess it per product (SOP §6).
- UWP backend, if ever added, must not be introduced as a third option — SoftStep removed UWP entirely (its MIDI port object was destroyed unrecoverably on device reboot).
