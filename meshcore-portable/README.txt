MeshCore Open PRE-BETA-9.5.2 — portable Linux x86_64 / NixOS

Start after extracting this complete directory:
  ./meshcore-open

No installation, sudo, nix-shell, nix develop, Flutter SDK, or system rebuild.
Keep the hidden .ld-linux-x86-64.so.2 file with the rest of this directory.
The static starter locates its own directory, starts the bundled glibc 2.41
loader, and clears inherited LD_LIBRARY_PATH, LD_PRELOAD and LD_AUDIT.
All native dependencies use the same authenticated Debian 13 runtime base;
no libraries are taken from /nix/store. ONNX Runtime 1.22.0 matches the
plugin's required VERS_1.22.0. Mesa software rendering is the default to
avoid loading the host graphics driver's incompatible glibc dependencies.
Flutter data and assets are unchanged. Identical library aliases use
relative symlinks. Kernel, display server, desktop/session D-Bus, system
D-Bus and hardware device access are provided by the host, as usual.
BLE needs an existing accessible BlueZ service and Bluetooth adapter.
USB needs existing read/write permissions on the serial device.
TCP needs network access to the companion device. No host services are
installed or reconfigured by this package.

Executed checks:
- All 197 ELF objects: recursive DT_NEEDED and required symbol
  versions resolved entirely inside the bundle; no missing dependencies.
- Every original data/ asset (26 files) hash-identical to official release.
- Native shared libraries loaded with RTLD_NOW, including Flutter,
  serial, ONNX, llama and LiteRT libraries.
- GUI rendered under Xvfb with bundled software OpenGL.
- GUI rendered inside a PRoot root with no /usr or /lib host libraries.
- BLE discovery via mocked BlueZ D-Bus: correct NUS UUID filter and
  StartDiscovery observed. This is not a test of physical Bluetooth.
- Native serial plugin: bidirectional I/O via pseudoterminal.
- ONNX session creation and real inference returned [1,4,9,16,25,36].
- Relocation to a directory containing spaces: GUI and native tests pass.

Not executed: actual NixOS VM, real Bluetooth/USB radio, full MeshCore TCP
protocol synchronization, AI model inference, physical host Wayland display.
An optional reproducible NixOS VM test is included under validation/.
It is for maintainers; Nix is not required to start this application.

Original Linux ZIP SHA256:
fe8cc963efedade6d49a81ae6245c6b4e6b94f169f6663689dbff3cd073fbdd9
ONNX 1.22.0 tarball SHA256 (pinned HTTPS download from microsoft/onnxruntime):
8344d55f93d5bc5021ce342db50f62079daf39aaafb5d311a451846228be49b3
Package origins, versions and artifact hashes: provenance.json.
ELF dependency and version report: elf-audit.json.
Licenses and upstream Flutter notices are retained.

GitHub source-archive edition: two files exceeding GitHub's 100 MB file
limit are losslessly compressed. On the first ./meshcore-open launch, the
static included helper extracts them and verifies their SHA-256 hashes.
The extracted directory must be writable for this first launch. No host
XZ tool or package installation is required. All original assets are restored
bit-identically. Subsequent starts use the already prepared files.

Runtime data correction: bundled XKB rules/keymaps, C.UTF-8, de_DE.UTF-8,
en_US.UTF-8 locales and compiled shared MIME database. Unavailable locales
fall back to C.UTF-8. No host /usr/share/X11/xkb or locale archive needed.
Executed: German/US keymap symbols and locales; GUI rendered with a Weston
Wayland compositor in a PRoot root without host /usr, /lib or XKB data.
No actual NixOS VM was executed.
