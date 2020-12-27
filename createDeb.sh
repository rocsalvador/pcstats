cd src
make
mv pcstats ../pcstatsDeb/usr/bin/
cd ..
dpkg --build pcstatsDeb/
mv pcstatsDeb.deb pcstats.deb
