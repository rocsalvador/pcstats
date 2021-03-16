cd src
make
mv pcstats ../pcstatsDeb/usr/bin/
cd ..
dpkg --build pcstatsDeb/
mv pcstatsDeb.deb pcstats.deb
echo -n "Do you want to install pcstats.deb? [Y/n] "
read c
if [ "$c" = "Y" ] || [ "$c" = "y" ] || [ "$c" = "" ]; then
    sudo dpkg -i pcstats.deb
    rm pcstats.deb
fi
