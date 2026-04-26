# nix-channel --list
# nix-channel --add https://channels.nixos.org/nixos-unstable nixos
# nix-channel --update
# nano /etc/nixos/configuration.nix
# nixos-rebuild build
# nixos-rebuild switch --upgrade
# nix-collect-garbage -d
# flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
# gsettings set org.gnome.desktop.input-sources xkb-options "['grp:alt_shift_toggle']"


{ config, pkgs, lib, ... }:

{
  imports = [ ./hardware-configuration.nix ];
  console = {
    font = "Lat2-Terminus16";
    earlySetup = true;
    useXkbConfig = true; };
  boot = {
    loader = {
      systemd-boot.enable = true;
      efi.canTouchEfiVariables = true; };
    kernelPackages = pkgs.linuxPackages_latest; };
  hardware = {
    cpu.amd.updateMicrocode = true;
    enableRedistributableFirmware = true;
    graphics = {
      enable = true;
      enable32Bit = true;
      extraPackages = with pkgs; [ libva libva-vdpau-driver libvdpau-va-gl ]; }; };
  system = {
    autoUpgrade = {
      channel = "https://channels.nixos.org/nixos-25.11";
      enable = false;
      allowReboot = false; };
    stateVersion = "25.11"; };
  networking = {
    hostName = "nixos";
    networkmanager.enable = true; };
  time.timeZone = "Asia/Tomsk";
  i18n = {
    defaultLocale = "ru_RU.UTF-8";
    extraLocaleSettings = { LC_TIME = "ru_RU.UTF-8"; }; };
  users.users.alexey = {
    isNormalUser = true;
    description = "Alexey Pozdnyakov";
    extraGroups = [ "networkmanager" "wheel" "input" "audio"];
    packages = with pkgs; [  ]; };
  fonts.fontDir.enable = true;
  fonts = {
    packages = with pkgs; [
      times-newer-roman terminus_font dejavu_fonts
      comic-mono google-fonts fira-code roboto ]; };
  services = {
    desktopManager = { gnome = { enable = true; }; };
    displayManager = { gdm = { enable = true; };
      autoLogin = { enable = true; user = "alexey"; }; };
    xserver = { enable = true;
      xkb = { layout = "us,ru"; variant = "";
        options = "grp:ctrl_shift_toggle"; }; };
    pipewire = { enable = true;
      alsa.enable = true; alsa.support32Bit = true;
      pulse.enable = true; };
    flatpak.enable = true;
    printing.enable = false;
    pulseaudio.enable = false; };
  xdg = {
    portal = {
      enable = true;
      extraPortals = [ pkgs.xdg-desktop-portal-gtk ];
      config.common.default = "gtk"; }; };
  systemd.services = { "getty@tty1".enable = false;
    "autovt@tty1".enable = false; };

  programs = {
    firefox.enable = true;
    gamemode.enable = false;
    mtr.enable = true; };  
  nixpkgs.config.allowUnfree =false;
  environment.gnome.excludePackages = with pkgs; [
    epiphany simple-scan totem yelp geary seahorse
    gnome-characters gnome-music gnome-photos gnome-terminal
    gnome-tour gnome-maps gnome-contacts gnome-connections gnome-disk-utility
    evince loupe baobab folio ];
  environment.systemPackages = with pkgs; [
    gnomeExtensions.transparent-top-bar-adjustable-transparency
    gnomeExtensions.compiz-windows-effect gnomeExtensions.coverflow-alt-tab
    gnomeExtensions.gradient-top-bar gnomeExtensions.burn-my-windows gnomeExtensions.weather-oclock
    gnomeExtensions.dash-to-dock gnomeExtensions.user-themes gnomeExtensions.vitals
    gnome-tweaks gnome-graphs gnome-font-viewer gnome-sound-recorder gnome-extension-manager
    gnome-chess stockfish brave thunderbird speedcrunch vitetris qbittorrent
    hunspell hunspellDicts.ru_RU libreoffice-fresh onlyoffice-desktopeditors koreader
    cavalier museeks clementine soundconverter netease-cloud-music-gtk sacd mpv papers avidemux
    file-roller p7zip gthumb gparted ntfs3g btrfs-progs wget git gnumake clang gcc sakura
    iputils traceroute pkg-config curl orchis-theme graphite-cursors arc-icon-theme
    (pkgs.makeDesktopItem {
      name = "продукты"; desktopName = "Моя программа для жены";
      exec = "sakura -e /home/alexey/Документы/Alexey/Programming/execution/food/food";
      terminal = false; icon = "x-office-spreadsheet"; categories = [ "Utility" ]; }) ];
  nix = {
    gc = { automatic = true;
      dates = "weekly";
      options = "--delete-older-than 7d"; }; }; }
