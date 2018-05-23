PKG_PATH=http://ftp.nluug.nl/pub/OpenBSD/5.2/packages/`machine -a`/

export PKG_PATH

pkg_add pcre libiconv png jpeg tiff gtk+2 
