make
cp pcstats pcstatsDeb/usr/bin/
rm pcstats
dpkg --build pcstatsDeb/
mv pcstatsDeb.deb pcstats.deb
