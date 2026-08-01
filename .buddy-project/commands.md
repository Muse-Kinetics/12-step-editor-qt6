# Commands

Operational commands for the 12 Step Editor repository, post-migration.

## Project State

```powershell
Set-Location "c:\Users\eric\KMI Dropbox\Eric Bateman\00_Editors\12 Step"
git status --short
git submodule status
```

## Build Editor (verified working, 2026-07-30)

```powershell
Set-Location "c:\Users\eric\KMI Dropbox\Eric Bateman\00_Editors\12 Step"

# Debug build
New-Item -ItemType Directory -Force -Path build-12StepEditor-Desktop_Qt_6_3_2_MSVC2019_64bit-Debug
cmd /c '.vscode\run-msvc-qmake-task.cmd "QT\12StepEditor.pro" build "build-12StepEditor-Desktop_Qt_6_3_2_MSVC2019_64bit-Debug"'

# Or via VS Code: "Build 12 Step Editor Debug" / "Build 12 Step Editor Release" tasks
```

Requires locally: Qt 6.3.2 (msvc2019_64 kit), VS2019 Community, Windows SDK 10.0.26100.0, and the Windows MIDI Services Desktop App SDK Runtime (`C:/Program Files/Windows MIDI Services/Desktop App SDK Runtime/Microsoft.Windows.Devices.Midi2.winmd`) — the `.pro` file hard-errors at qmake time if any WMS SDK path is missing.

## Force the WinMM Backend (test the fallback path on a WMS-equipped machine)

```powershell
# VS Code launch config: "Launch 12 Step Editor (Force WinMM)"
# Equivalent manual override:
$env:KMI_MIDI_BACKEND = "winmm"
```

## Firmware Asset Sync

The chunk-safe `-cs512` firmware asset is sourced from the `mk_firmware_tester` submodule's nested `sendsysex` tool:

```powershell
# Source of truth for the prebuilt chunked asset:
# mk_firmware_tester/sendsysex/syx/12Step/12 Step Firmware v1.0.8_cs512.syx

# Destination in the editor (already done for v1.0.8):
Copy-Item -LiteralPath "mk_firmware_tester\sendsysex\syx\12Step\12 Step Firmware v1.0.8_cs512.syx" `
  -Destination "QT\resources\firmware\12Step_Firmware_v1.0.8-cs512.syx" -Force
# Then update QT\resources.qrc and bump _fw_ver_12step in QT\inc\KMI_KMDM\KMI_FwVersions.h to match.
```

## Notes

- `.gitmodules` now points `KMI_KMDM` and `rtmidi` at their GitHub `Muse-Kinetics` forks — do not repoint back to the `bitbucket.org/keith-mcmillen-instruments` mirrors, they carry unrelated/stale history for these two.
- `git fetch` against any remaining `bitbucket.org` remote in this repo can hang on a GCM credential prompt in non-interactive shells — use `timeout` if scripting this.
- `.buddy-project/` changes will show as `modified` in `git status` on every edit once these files are committed — check staging before any commit (see guidelines.md).
