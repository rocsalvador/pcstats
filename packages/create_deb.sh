#!/bin/bash

mkdir -p pcstats_deb/DEBIAN
mkdir -p pcstats_deb/usr/bin
mkdir -p pcstats_deb/usr/share/applications

echo -n  "Version: "
read version

echo "Package: pcstats
Version: $version
Section: custom
Architecture: amd64
Depends: ncurses-base
Maintainer: rocsalvador
Description: Monitor CPU and RAM stats
Homepage: https://github.com/rocsalvador/pcstats" > pcstats_deb/DEBIAN/control

echo "[Desktop Entry]
Name=pcstats
Version=$version
Type=Application
Comment=CPU and RAM monitor
Terminal=true
Exec=/usr/bin/pcstats
Icon=utilities-system-monitor
Categories=System;"  > pcstats_deb/usr/share/applications

make -C ../src
cp ../src/pcstats pcstats_deb/usr/bin/

dpkg --build pcstats_deb/
mv pcstats_deb.deb pcstats.deb

echo -n "Do you want to install pcstats.deb? [Y/n] "
read -r c
if [ "$c" = "Y" ] || [ "$c" = "y" ] || [ "$c" = "" ]; then
    sudo dpkg -i pcstats.deb
fi
