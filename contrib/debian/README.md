
Debian
====================
This directory contains files used to package pexad/pexa-qt
for Debian-based Linux systems. If you compile pexad/pexa-qt yourself, there are some useful files here.

## pexa: URI support ##


pexa-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install pexa-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your pexa-qt binary to `/usr/bin`
and the `../../share/pixmaps/pexa128.png` to `/usr/share/pixmaps`

pexa-qt.protocol (KDE)

