# Optional maintainer validation, not needed to run the portable application.
# nix-build nixos-test.nix --arg archive /absolute/path/to/release.tar.xz
{ archive, nixpkgs ? builtins.fetchTarball {
  url = "https://github.com/NixOS/nixpkgs/archive/d6c71932130818840fc8fe9509cf50be8c64634f.tar.gz";
  sha256 = "sha256-ub1gpAONMFsT/GU2hV6ZWJjur8rJ6kKxdm9IlCT0j84=";
} }:
let pkgs = import nixpkgs { system = "x86_64-linux"; }; in
pkgs.testers.runNixOSTest {
  name = "meshcore-portable";
  nodes.machine = { ... }: {
    virtualisation.memorySize = 4096;
    virtualisation.cores = 2;
    services.xserver.enable = true;
    services.xserver.displayManager.lightdm.enable = true;
    services.xserver.desktopManager.xfce.enable = true;
    services.displayManager.autoLogin = { enable = true; user = "alice"; };
    users.users.alice = { isNormalUser = true; uid = 1000; };
    hardware.bluetooth.enable = true;
    environment.systemPackages = [ pkgs.xdotool pkgs.binutils pkgs.xz ];
  };
  testScript = ''
    machine.start()
    machine.wait_for_x()
    machine.succeed("mkdir -p /home/alice/portable; tar -xJf ${archive} -C /home/alice/portable; chown -R alice:users /home/alice/portable")
    machine.succeed("su - alice -c '/home/alice/portable/meshcore-portable/meshcore-open --portable-info'")
    machine.succeed("su - alice -c 'DISPLAY=:0 /home/alice/portable/meshcore-portable/meshcore-open >/home/alice/meshcore.log 2>&1 &' ")
    machine.wait_until_succeeds("DISPLAY=:0 xdotool search --onlyvisible --name meshcore_open")
    machine.sleep(5)
    machine.succeed("! grep -Ei 'not found|symbol lookup error|Couldn.t open lib|Failed to load AOT|Unable to get application documents directory' /home/alice/meshcore.log")
    machine.screenshot("meshcore-portable-nixos")
    # Fail if a host ELF library was mapped, even if NixOS happens to supply it.
    machine.succeed("pid=$(pgrep -u alice -f '^/home/alice/portable/meshcore-portable/.ld-linux'); test -n \"$pid\"; awk '$NF ~ /\\.so/ && $NF !~ /^\\/home\\/alice\\/portable\\/meshcore-portable\\// { bad=1; print } END { exit bad }' /proc/$pid/maps")
  '';
}
