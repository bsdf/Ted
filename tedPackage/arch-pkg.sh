set -e -u -x

TEDSRCDIR=src/Ted-2.23
TEDPKGDIR=${TEDSRCDIR}/tedPackage

( cd ${TEDSRCDIR} && make tedPackage/arch-PKGBUILD )
cp ${TEDPKGDIR}/arch-PKGBUILD ${TEDPKGDIR}/arch-Ted.install .

echo If this fails.. first try sudo pacman -S libpaper

fakeroot makepkg --asroot --noextract -p arch-PKGBUILD

echo To validate the package call: '(First call pacman -S namcap)'
echo '   ' namcap ted-2.23-1-*.pkg.tar.xz
echo To install call:
echo '   ' sudo pacman -U ted-2.23-1-*.pkg.tar.xz

