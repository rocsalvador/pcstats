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
Categories=System;"  > pcstats_deb/usr/share/applications/pcstats.desktop

make -j8 -C ../src
cp ../src/pcstats pcstats_deb/usr/bin/

dpkg --build pcstats_deb/
package_name=pcstats_"$version"_amd64.deb
mv pcstats_deb.deb $package_name

echo -n "Do you want to install $package_name [Y/n] "
read -r c
if [ "$c" = "Y" ] || [ "$c" = "y" ] || [ "$c" = "" ]; then
    sudo dpkg -i $package_name
fi
