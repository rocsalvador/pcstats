#!/bin/bash

usage() {
    echo "Usage: $0 -v pcstats_version [-i]"
    exit 1
}

if [ "$#" -lt 2 ]; then
    usage
fi

install=false
while getopts "v:i" opt; do
    case $opt in
        v)
            version="$OPTARG"
            ;;
        i)
            install=true
            ;;
        *)
            usage
            ;;
    esac
done

root_dir=$(readlink -f $(dirname "$0")/..)

mkdir -p $root_dir/packages/pcstats_deb/DEBIAN
mkdir -p $root_dir/packages/pcstats_deb/usr/bin
mkdir -p $root_dir/packages/pcstats_deb/usr/share/applications

echo "Package: pcstats
Version: $version
Section: custom
Architecture: amd64
Depends: ncurses-base
Maintainer: rocsalvador
Description: Resource usage monitor
Homepage: https://github.com/rocsalvador/pcstats" > $root_dir/packages/pcstats_deb/DEBIAN/control

echo "[Desktop Entry]
Name=pcstats
Version=$version
Type=Application
Comment=Resource usage monitor
Terminal=true
Exec=/usr/bin/pcstats
Icon=utilities-system-monitor
Categories=System;"  > $root_dir/packages/pcstats_deb/usr/share/applications/pcstats.desktop

make -j8 -C $root_dir
cp $root_dir/pcstats $root_dir/packages/pcstats_deb/usr/bin/

dpkg --build $root_dir/packages/pcstats_deb/
package_name=pcstats_"$version"_amd64.deb
mv $root_dir/packages/pcstats_deb.deb $package_name

if $install; then
    sudo dpkg -i $package_name
fi
