# Blockers

## No blockers on the code side — hardware validation is the only gap

All SOP §4 migration steps are code-complete and the editor builds clean. What's left is SOP §5 hardware validation, which requires a physical 12 Step device and cannot be done without one.

## `KMI_FwVersions.h` firmware-version bump is uncommitted inside the `KMI_KMDM` submodule

Bumped `_fw_ver_12step` from `1,0,7` to `1,0,8` (to match the new `-cs512` chunked asset) directly in `QT/inc/KMI_KMDM/KMI_FwVersions.h`. This file lives inside the shared `kmi_midi_device_manager` submodule (used by SoftStep, QuNeo, QuNexus, K-Mix too) — the change is currently a local uncommitted edit in that submodule's working tree, not committed or pushed. **Flag to the user before committing/pushing** since it affects a repo shared across products (even though this specific array entry is 12-Step-only and doesn't touch other products' entries).

## `.gitmodules` URL/commit mismatch applied to KMI_KMDM too, not just rtmidi

The SOP's §6.1 only explicitly called out the `rtmidi` URL/commit mismatch, but the same pattern existed for `KMI_KMDM`: `.gitmodules` pointed at `bitbucket.org/keith-mcmillen-instruments/kmi_midi_device_manager.git`, which turned out to be a **completely unrelated/stale history** (`git fetch` returned "no common commits" against the actual GitHub `Muse-Kinetics/kmi_midi_device_manager.git` fork where `main`/`c5c549b` actually lives). Fixed by repointing to the GitHub fork, same as rtmidi. Worth flagging in case other target editors (K-Mix) have the same issue for KMI_KMDM specifically, not just rtmidi.

## Bitbucket credential prompt hangs non-interactively

`git fetch` against the bitbucket.org remotes hangs indefinitely on a GCM (git-credential-manager) prompt in a non-interactive shell (`GIT_TERMINAL_PROMPT=0` does not suppress it — GCM has its own UI). Not an issue once submodules are repointed to the GitHub SSH remotes, but worth knowing if any future fetch against a `bitbucket.org` remote in this repo seems to hang — it's the credential prompt, not a network issue.

## Pre-existing dirty state discovered during migration (not caused by this migration)

- `QT/inc/rtmidi` had uncommitted local state before this session: checked out at a `-dirty` commit (`a3fa1dea78e`, branch `SoftStep`) not matching the superproject's recorded pointer (`5a77aba`), plus deleted `android/` files and file-mode-only changes (755→644) on `gradlew`/`autogen.sh`. Discarded as noise (confirmed via diff these were mode-only / no real content changes) when repointing the submodule to `WMS`/`b79b83e`. The `gradlew`/`autogen.sh` mode churn reappeared after checkout — likely a recurring Windows/Dropbox `core.filemode` artifact, harmless.
- Large number of pre-existing modified/deleted files across `win-build/`, `Signing and Notarization/`, `CHANGELOG.md` predate this work and are unrelated to the WMS/chunking migration — left untouched, not swept into this diff.

## No `.vscode/` tooling existed yet

Now created (see current-task.md). Debug build directory (`build-12StepEditor-Desktop_Qt_6_3_2_MSVC2019_64bit-Debug`) was created fresh during build validation and is gitignored — not tracked, safe to delete/rebuild.
