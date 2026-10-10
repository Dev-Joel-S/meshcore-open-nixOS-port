# MeshCore Open — portable NixOS x86_64

Download this branch as a ZIP or tar.gz, extract it, enter `meshcore-portable`, and run:

```sh
./meshcore-open
```

No Nix shell, sudo, installation, Flutter SDK or system rebuild is needed.
On first launch the included static helper extracts two large runtime files
and verifies their SHA-256 hashes. Keep the extracted directory writable.

The app uses a coherent bundled Debian 13 glibc/GTK runtime and matching
ONNX 1.22.0. All original Flutter assets and native plugins are retained.
Details and executed/unexecuted checks are in `meshcore-portable/README.txt`.
An actual NixOS VM and real radio hardware were not tested.

Existing installation: [download the runtime correction ZIP](https://github.com/Dev-Joel-S/meshcore-open-nixOS-port/raw/refs/heads/codex/portable-download/download/nixos-runtime-fix.zip),
extract its contents into your existing `meshcore-portable` directory, replacing
the starter, then run `./meshcore-open`. Includes XKB, UTF-8 locales and MIME data.
