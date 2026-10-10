# MeshCore Open für NixOS x86_64

[Vollständiges korrigiertes Paket herunterladen](https://github.com/Dev-Joel-S/meshcore-open-nixOS-port/archive/refs/heads/codex/nixos-portable-fixed.zip)

ZIP in einen neuen Ordner entpacken, darin `meshcore-portable` öffnen und starten:

```sh
./meshcore-open
```

Alle Korrekturen sind enthalten: ONNX-Bibliothek und ZIP-Linkreparatur,
XKB-Tastaturdaten, UTF-8-Locales und GTK-MIME-Datenbank. Kein separates Patch nötig.
Keine Installation, sudo, Nix-Shell oder Flutter-SDK nötig.
Beim ersten Start werden zwei enthaltene Dateien automatisch entpackt und
per SHA-256 geprüft. Das Verzeichnis muss dafür schreibbar sein.

Die vollständigen Flutter-Assets bleiben erhalten. Die Bibliotheken stammen aus
einer gemeinsamen Debian-13-Laufzeit. Bluetooth verwendet den vorhandenen
BlueZ-Dienst, USB die vorhandenen Geräteberechtigungen.

Rekursive ELF- und ABI-Prüfung, originale Asset-Hashes und isolierter Wayland-
Start wurden getestet. Eine echte NixOS-VM und physische Funkgeräte wurden
nicht getestet. Weitere Details stehen in `meshcore-portable/README.txt`.

USB boot-data correction: the native serial plugin now waits for callback
registration before notification and initializes its mutexes before starting
the worker. Early `[BOOT]` data previously caused a NULL callback SIGSEGV.
The regression reproduces the crash in the official library and passes
30 cycles with the replacement. All prior runtime corrections are included.
Source, patch, rebuild recipe and test results are included in `validation/`.
