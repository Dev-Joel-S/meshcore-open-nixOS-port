# MeshCore Open für NixOS

Portables Paket von [MeshCore Open](https://github.com/zjs81/meshcore-open)
**PRE-BETA-9.5.2** für **NixOS x86_64**. Der Start des korrigierten Pakets
wurde inzwischen auf einem NixOS-Rechner vom Nutzer bestätigt.

## Download und Start

**[Vollständiges Paket mit allen bisherigen Korrekturen herunterladen — ZIP, 227 MB](https://github.com/Dev-Joel-S/meshcore-open-nixOS-port/archive/847e1568f7f06c7a1bcc377e3abfc53237a30035.zip)**

Das ZIP in einen **neuen Ordner** entpacken, darin `meshcore-portable` öffnen
und starten:

```sh
./meshcore-open
```

Keine Installation, kein sudo, kein `nix-shell`/`nix develop`, kein
`nix-env -i`, kein `nixos-rebuild` und kein Flutter-SDK erforderlich.
Alle bisherigen Korrekturen sind im Download enthalten; ein separates Patch
ist nicht nötig. Das vollständige Verzeichnis einschließlich versteckter
Dateien zusammenhalten.

Beim ersten Start entpackt der mitgelieferte statische Helper zwei große
Laufzeitdateien und prüft ihre SHA-256. Dafür muss das entpackte Verzeichnis
schreibbar sein. Zusätzliche Programme sind nicht erforderlich.

## Enthaltene Korrekturen

- Gemeinsamer ELF-Loader und Laufzeitbibliotheken aus Debian 13, einschließlich
  glibc 2.41, libstdc++, GTK3, GLib und D-Bus; keine Bibliotheken aus `/nix/store`.
- ONNX Runtime **1.22.0**, passend zur benötigten Symbolversion `VERS_1.22.0`.
- Reparatur von Symlinks, die ZIP-Entpacker als kleine Textdateien ablegen.
- XKB-Tastaturdaten, UTF-8-Locales und GTK-MIME-Datenbank.
- Korrigierte native USB-Bibliothek: Frühe `[BOOT]`-Daten rufen keinen noch
  ungesetzten Callback auf. Mutexes werden vor dem Worker-Start initialisiert.
- Software-OpenGL/Vulkan, Schriftarten, Zertifikate und ein `bluetoothctl`-Helper.

Die bereits kompilierte Flutter-Anwendung und alle ursprünglichen Assets
bleiben unverändert. Nur das native USB-Plugin wurde für die Absturzkorrektur
neu gebaut. Gepinnter Quellcode, Patch und Build-Rezept liegen im Paket unter
`validation/`. Herkunft und Versionen stehen in `provenance.json`, Lizenzen
sind enthalten. Die Paketgröße wird nicht durch Entfernen von Funktionen reduziert.

## Voraussetzungen und Funktionsumfang

Eine laufende grafische Linux-Sitzung und D-Bus. BLE benötigt einen vorhandenen
BlueZ-Dienst und Bluetooth-Adapter; USB benötigt Zugriff auf das Serial-Gerät.
Das Paket installiert keine Systemdienste und ändert keine Systemkonfiguration.

Der Funktionsumfang richtet sich nach MeshCore Open PRE-BETA-9.5.2.
Diese Portierung ergänzt keine fehlenden Funktionen gegenüber der geschlossenen
MeshCore-App. Die vorhandene `flake.nix` definiert eine Entwicklungsumgebung;
sie ist zum Starten dieses Pakets nicht erforderlich.

## Prüfstand

- **NixOS-Start:** vom Nutzer mit dem korrigierten Download bestätigt.
- Alle **197 ELF-Dateien**: rekursive Abhängigkeiten und Symbolversionen geprüft.
- Alle **26 ursprünglichen Assets**: unveränderte Prüfsummen.
- Oberfläche unter Xvfb und isoliertem Wayland gerendert, auch ohne
  Hostbibliotheken unter `/usr` oder `/lib` und ohne Host-XKB-Daten.
- Start nach Verschieben in einen Pfad mit Leerzeichen.
- USB-Fehler mit der offiziellen Bibliothek reproduziert; **30 Wiederholungen**
  mit frühen Daten und verzögerter Callback-Registrierung mit der Korrektur bestanden.
- Schließen vor Callback-Registrierung und bidirektionale Serial-I/O über PTY geprüft.
- ONNX-Inferenz und BLE-Discovery gegen einen BlueZ-D-Bus-Mock geprüft.
- Vollständigen GitHub-Download erneut heruntergeladen, entpackt und geprüft.

**Nicht getestet:** eigene NixOS-VM, reale BLE-/USB-Funkhardware, vollständige
TCP-Protokollsynchronisation und AI-Modellinferenz. Der reproduzierbare
NixOS-VM-Test unter `validation/nixos-test.nix` ist optional für Maintainer.

SHA-256 des verlinkten ZIP:

```text
cdd58c3a8142ba971425533a794f58051fc6b14bd30d3e9e59dfce0488673f93
```
