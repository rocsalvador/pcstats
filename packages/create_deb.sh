#!/bin/bash

cd ../src || exit
make
mv pcstats ../packages/pcstats_deb/usr/bin/
cd ../packages
dpkg --build pcstats_deb/
mv pcstats_deb.deb pcstats.deb
echo -n "Do you want to install pcstats.deb? [Y/n] "
read -r c
if [ "$c" = "Y" ] || [ "$c" = "y" ] || [ "$c" = "" ]; then
    sudo dpkg -i pcstats.deb
fi
