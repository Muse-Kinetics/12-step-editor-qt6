# Decisions

## Pointed `KMI_KMDM` directly at `main` tip, no reconciliation branch needed

SOP §3 called for diffing 12 Step's product branch against `main` and porting forward genuinely product-specific fixes before repointing. The actual diff showed the drift was almost entirely license-header/README churn and dead/commented-out code identical on both sides (e.g. a commented-out `12Step` portname-fix block, identical on both branches). `main` already carries full 12 Step product awareness (PID lookup table, firmware version array, port-name handling). Concluded no reconciliation branch was needed — repointed straight at `main`'s tip (`c5c549b`), matching what SoftStep itself uses.

## Relocated the SSL DLLs out of `KMI_Updates` into `QT/ssl/` rather than changing the linking approach

`KMI_Updates/ssl/{libcrypto,libssl}-1_1-x64.dll` were copied at deploy time for the software-update HTTPS check. `main`'s consolidated `KMI_KMDM` doesn't carry an `ssl/` folder (SoftStep instead links against a fixed Qt-installed OpenSSL Tools path at compile time — a different, unrelated approach). Chose to preserve 12 Step's existing working DLL-copy mechanism rather than adopt SoftStep's link-time approach, since changing it is out of scope for this migration and risks breaking the software-update checker. DLLs now live in `QT/ssl/`, tracked via a `.gitignore` exception (`!QT/ssl/*.dll`) since the repo has a blanket `*.dll` ignore rule.

## Removed the old ad hoc `sysExTxChunkSize=48`/`sysExTxChunkDelay=1` override in `mainwindow.cpp`

12 Step's old firmware-detected handler manually overrode the *general* (non-firmware) SysEx chunk size to 48 bytes/1ms delay as a workaround before dedicated firmware-mode chunking existed. `main`'s `KMI_mdm` now has a separate `sysExTxChunkSizeFW` (already defaulted to 48) used automatically during firmware transfer, leaving the general `sysExTxChunkSize` at its platform-appropriate default (512 macOS / 1000 Windows) for normal preset sends. Removed the override to match SoftStep's `mainwindow.cpp`, which has no such override either.

## Used 12 Step's own `UserDialog` convention for the pre-1.0.0 guard, not SoftStep's `slotCreateDialog`

SoftStep's `slotFirmwareDetected` pre-1.0.0 guard uses a `slotCreateDialog` helper that 12 Step doesn't have. 12 Step already has its own `UserDialog` class used elsewhere in `mainwindow.cpp` for similar blocking error dialogs (e.g. missing bootloader/firmware file). Ported the guard's *logic* (version check, support-URL launch, quit) but expressed it with 12 Step's existing dialog convention rather than introducing a new one.

## `mk_firmware_tester` added as a submodule at repo root, not linked into the editor build

Per explicit instruction, added `mk_firmware_tester` (pulling in nested `sendsysex`/`midi_cpp` submodules) to source the prebuilt `-cs512` chunked firmware asset from `sendsysex/syx/12Step/`. It's a dev/hardware-validation tool, not a build dependency — same relationship SoftStep has with its own `shared/sendsysex` (present in the repo, not referenced by the `.pro` file).
