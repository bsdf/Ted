zypper install \
    gcc \
    make \
    zlib-devel \
    pcre-devel \
    libX11-devel \
    libXpm-devel \
    libtiff-devel \
    libjpeg-devel \
    libpng-devel \
    gtk2-devel \
    libpaper-devel \
    rpm-build

echo if zypper complains about libpaper.. your SuSE version does not 
echo have it. To use the fixup in the Ted build just compile with:
echo rpmbuild --nodeps etc.
