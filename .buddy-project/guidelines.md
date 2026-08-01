# Guidelines

- Keep canonical project state in `.buddy-project/` and update it as the migration progresses. Learn from SoftStep's mistake: adding `.buddy-project/` to `.gitignore` after files are already tracked does **not** untrack them — they still show as `modified` in `git status` on every edit. Check `git status` before staging/committing anything else so unrelated commits don't sweep in `.buddy-project` changes.
- Treat `../SOP-WMS-and-Chunked-Firmware-Update-Migration.md` as the authoritative migration procedure. Treat SoftStep's `Documentation/FIRMWARE_CHUNKING.md` as the design baseline for the packetized updater itself.
- Before deleting `KMI_KMDM`/`KMI_Ports`/`KMI_Updates` call sites, grep the whole tree for `KMI_Ports::`/`KMI_Updates::`/`KMI_KMDM::` usages (SOP §4.1) — don't assume the `.pro` file's `SOURCES`/`HEADERS` lists are the only references.
- Fix `.gitmodules` URL/commit mismatches deliberately and note them in decisions.md when done — don't silently correct without recording why (this repo's `rtmidi` entry is a known example: listed as `thestk/rtmidi.git`, actually pinned to a Muse-Kinetics fork commit).
- Don't remove the stale `qt-qunexus/inc/*` entries from `.gitmodules` without confirming they're truly unused elsewhere first — they're currently orphaned (no matching working-tree directory) but flag/confirm before deleting rather than assuming.
- Keep editor-specific UI changes separate from shared transport changes in the consolidated submodule whenever possible, matching SoftStep's separation of concerns.
- Use the prebuilt `-cs512` firmware payload once generated; do not build ad hoc PID headers or chunking logic inside the editor.
- When debugging updater failures, collect both firmware console output and raw SysEx capture so packet/phase context is preserved.
- Do not expand scope into unrelated cleanup (e.g. the large set of pre-existing modified build/deploy files already in this working tree) just because the repo is already dirty — treat those as separate, pre-existing state, not part of this migration's diff.
- MIDI backend selection (WMS probe / WinMM fallback / `KMI_MIDI_BACKEND` override) is intentionally duplicated across `KMI_MDM`-equivalents, `rtmidi`, and `sendsysex`-style tools rather than shared — when changing the pattern here, check consistency against SoftStep's three implementations.
