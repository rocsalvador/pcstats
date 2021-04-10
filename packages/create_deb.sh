#!/bin/bash

mkdir -p pcstats_deb/DEBIAN
mkdir -p pcstats_deb/usr/bin
mkdir -p pcstats_deb/usr/share/applications

cp control pcstats_deb/DEBIAN
cp pcstats.desktop pcstats_deb/usr/share/applications
make -C ../src
cp ../src/pcstats pcstats_deb/usr/bin/

dpkg --build pcstats_deb/
mv pcstats_deb.deb pcstats.deb

echo -n "Do you want to install pcstats.deb? [Y/n] "
read -r c
if [ "$c" = "Y" ] || [ "$c" = "y" ] || [ "$c" = "" ]; then
    sudo dpkg -i pcstats.deb
fi
