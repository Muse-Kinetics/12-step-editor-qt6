# gh-pages

Hosts the QtIFW online update repository for the 12 Step Editor, served via GitHub Pages.

- `update-repository/` — output of `.vscode/make-update-repo.ps1` (wraps `repogen`), referenced by `win-build/config/config.xml`'s `<RemoteRepositories>` entry.

Regenerate and replace `update-repository/` on each release; do not hand-edit its contents.
