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
echo $root_dir
# Compile the binary using make
make -C $root_dir clean
make -C $root_dir -j
# Set up rpmbuild directories
rpmbuild_dir="$root_dir/packages/pcstats_rpm"
rm -rf $rpmbuild_dir
mkdir -p $rpmbuild_dir/{BUILD,RPMS,SOURCES,SPECS,SRPMS}

# Copy the binary to SOURCES
cp $root_dir/pcstats "$rpmbuild_dir/SOURCES/"

echo "[Desktop Entry]
Name=pcstats
Version=$version
Type=Application
Comment=Resource usage monitor
Terminal=true
Exec=/usr/bin/pcstats
Icon=utilities-system-monitor
Categories=System;"  > $root_dir/packages/pcstats_rpm/SOURCES/pcstats.desktop


# Create spec file
cat > "$rpmbuild_dir/SPECS/pcstats.spec" << EOF
Name: pcstats
Version: $version
Release: 1%{?dist}
Summary: pcstats - Resource usage monitor
License: MIT
Source0: pcstats
Source1: pcstats.desktop
URL: https://github.com/rocsalvador/pcstats

%description
%{summary}

%prep
# No prep needed

%build
# No build needed, binary is pre-compiled

%install
mkdir -p %{buildroot}/usr/bin
cp %{SOURCE0} %{buildroot}/usr/bin/pcstats
mkdir -p %{buildroot}/usr/share/applications
cp %{SOURCE1} %{buildroot}/usr/share/applications/pcstats.desktop

%post
update-desktop-database

%files
/usr/bin/pcstats
/usr/share/applications/pcstats.desktop

EOF

# Build the RPM
rpmbuild --define "_topdir $rpmbuild_dir" -ba "$rpmbuild_dir/SPECS/pcstats.spec"

# Locate the most recently created RPM under the RPMS directory
rpm_path=$(find "$rpmbuild_dir/RPMS" -type f -name '*.rpm' -printf '%T@ %p\n' | sort -n | tail -n1 | cut -d' ' -f2-)

if $install; then
    echo "Installing pcstats version $version..."
    sudo rpm -i $rpm_path
fi