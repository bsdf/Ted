##
##  Preamble:
##
Summary:        An easy Rich Text Processor
Name:           ted
Version:        2.23
Release:        1
License:        GPLv2+
Group:          Applications/Editors
Source:         ftp://ftp.nluug.nl/pub/editors/ted/ted-2.23.src.tar.gz
URL:            http://www.nllgg.nl/Ted/index.html
Packager:	Mark de Does
BuildRequires:	gcc, make, zlib-devel, pcre-devel, libX11-devel, libXpm-devel, libjpeg-devel, libtiff-devel, libpng-devel, gtk2-devel, libpaper-devel
#requires:	zlib, pcre, libX11, libXpm, libjpeg, libtiff, libpng, gtk2, libpaper
requires:	pcre, gtk2

Summary(cs_CZ):	Editor pro snadné formátovaní textu
Summary(nl_NL):	Een makkelijke 'Rich Text' verwerker

##
##  Description
##
%description
Ted is an easy rich text processor. It can edit RTF files
in a WYSIWYG manner. It supports multiple fonts and can
print to PostScript printers.  Additionally Ted is an rtf2ps,
rtf2pdf, rtf2html and an rtf2epub converter.

Ted consists of a general part: The program, something.afm files
and an American spelling checker. Additional packages with
spell checking dictionaries for different languages exist.

This package is the general part.

%description -l cs_CZ
Ted je jednoduchý textový procesor.
Umožňuje editovat RTF soubory WYSIWYG způsobem. Podporuje použití
více fontů a tisk na PostScriptové tiskárny.

Ted obsahuje v základní části: program, pár .afm souborů a americký
korektor překlepů. Existují i další balíčky obsahující slovníky
pro korekturu překlepů jiných jazyků.

Toto je základní část.

%description -l nl_NL
Ted is een makkelijke 'Rich Text' verwerker. Je kan er RTF bestanden
op een visuele manier mee redigeren. Ted werkt met meerdere lettertypes
en kan op PostScript printers afdrukken. Daarbij is Ted een rtf2ps en 
een rtf2pdf conversieprogramma.

Ted bestaat uit een algemeen gedeelte: Het programma, allerlei.afm
bestanden en een Amerikaanse spellingwoordenlijst. Extra pakketten
met spellingwoordenlijsten voor andere talen zijn beschikbaar.

Dit pakket is het algemene gedeelte.

##
##  Prep section.
##
%prep
rm -rf Ted-2.23
zcat ${RPM_SOURCE_DIR}/ted-2.23.src.tar.gz | tar xvf -
##
##  Build section:
##
%build
( cd Ted-2.23 && make CONFIGURE_OPTIONS=--with-GTK )
( cd Ted-2.23 && make package CONFIGURE_OPTIONS=--with-GTK )
##
##  Install section:
##
%install
mkdir -p ${RPM_BUILD_ROOT}
rm -rf ${RPM_BUILD_ROOT}/usr/share/Ted
cat Ted-2.23/tedPackage/ted-2.23-linux-*.tar.gz | ( cd ${RPM_BUILD_ROOT} && umask 0 && tar xfz - )
##
##  Files section:
##
%files
%dir %attr(755,root,root) /usr/share/Ted
%dir %attr(755,root,root) /usr/share/Ted/afm
%dir %attr(755,root,root) /usr/share/Ted/dfa
%dir %attr(755,root,root) /usr/share/Ted/varia
%dir %attr(755,root,root) /usr/share/Ted/config
%dir %attr(755,root,root) /usr/share/Ted/doc
%dir %attr(755,root,root) /usr/share/Ted/fontsdocs
%dir %attr(755,root,root) /usr/share/Ted/examples
%dir %attr(755,root,root) /usr/share/Ted/ps

%attr(644,root,root) /usr/share/applications/Ted.desktop
%attr(644,root,root) /usr/share/man/man1/Ted.1.gz
%attr(755,root,root) /usr/bin/Ted
%attr(644,root,root) /usr/share/Ted/afm/AvantGarde-Book.afm
%attr(644,root,root) /usr/share/Ted/afm/AvantGarde-BookOblique.afm
%attr(644,root,root) /usr/share/Ted/afm/AvantGarde-Demi.afm
%attr(644,root,root) /usr/share/Ted/afm/AvantGarde-DemiOblique.afm
%attr(644,root,root) /usr/share/Ted/afm/Bookman-Demi.afm
%attr(644,root,root) /usr/share/Ted/afm/Bookman-DemiItalic.afm
%attr(644,root,root) /usr/share/Ted/afm/Bookman-Light.afm
%attr(644,root,root) /usr/share/Ted/afm/Bookman-LightItalic.afm
%attr(644,root,root) /usr/share/Ted/afm/Courier.afm
%attr(644,root,root) /usr/share/Ted/afm/Courier-Bold.afm
%attr(644,root,root) /usr/share/Ted/afm/Courier-BoldOblique.afm
%attr(644,root,root) /usr/share/Ted/afm/Courier-Oblique.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica-Bold.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica-BoldOblique.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica-Narrow.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica-Narrow-Bold.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica-Narrow-BoldOblique.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica-Narrow-Oblique.afm
%attr(644,root,root) /usr/share/Ted/afm/Helvetica-Oblique.afm
%attr(644,root,root) /usr/share/Ted/afm/NewCenturySchlbk-Bold.afm
%attr(644,root,root) /usr/share/Ted/afm/NewCenturySchlbk-BoldItalic.afm
%attr(644,root,root) /usr/share/Ted/afm/NewCenturySchlbk-Italic.afm
%attr(644,root,root) /usr/share/Ted/afm/NewCenturySchlbk-Roman.afm
%attr(644,root,root) /usr/share/Ted/afm/Palatino-Bold.afm
%attr(644,root,root) /usr/share/Ted/afm/Palatino-BoldItalic.afm
%attr(644,root,root) /usr/share/Ted/afm/Palatino-Italic.afm
%attr(644,root,root) /usr/share/Ted/afm/Palatino-Roman.afm
%attr(644,root,root) /usr/share/Ted/afm/Symbol.afm
%attr(644,root,root) /usr/share/Ted/afm/Times-Bold.afm
%attr(644,root,root) /usr/share/Ted/afm/Times-BoldItalic.afm
%attr(644,root,root) /usr/share/Ted/afm/Times-Italic.afm
%attr(644,root,root) /usr/share/Ted/afm/Times-Roman.afm
%attr(644,root,root) /usr/share/Ted/afm/ZapfChancery-MediumItalic.afm
%attr(644,root,root) /usr/share/Ted/afm/ZapfDingbats.afm

%attr(644,root,root) /usr/share/Ted/dfa/en_US.dfa
%attr(644,root,root) /usr/share/Ted/TedDocument-en_US.rtf

%attr(755,root,root) /usr/share/Ted/examples/rtf2ps.sh
%attr(755,root,root) /usr/share/Ted/examples/rtf2pdf.sh
%attr(755,root,root) /usr/share/Ted/examples/rtf2txt.sh
%attr(755,root,root) /usr/share/Ted/examples/rtf2html.sh
%attr(755,root,root) /usr/share/Ted/examples/rtf2epub.sh
%attr(755,root,root) /usr/share/Ted/examples/gsafm.sh
%attr(755,root,root) /usr/share/Ted/examples/fixOpenOfficeRtf.sh

%attr(644,root,root) /usr/share/Ted/doc/Ted.1
%attr(644,root,root) /usr/share/Ted/doc/rtf2ps.1
%attr(644,root,root) /usr/share/Ted/doc/rtf2pdf.1

%attr(644,root,root) /usr/share/Ted/ps/gsfontmetrics.ps
%attr(644,root,root) /usr/share/Ted/ps/gslibpath.ps
%attr(644,root,root) /usr/share/Ted/ps/gslsfonts.ps

%attr(644,root,root) /usr/share/Ted/fontsdocs/font.rtf
%attr(644,root,root) /usr/share/Ted/fontsdocs/font-i.rtf
%attr(644,root,root) /usr/share/Ted/fontsdocs/font-b.rtf
%attr(644,root,root) /usr/share/Ted/fontsdocs/font-bi.rtf
%attr(755,root,root) /usr/share/Ted/fontsdocs/checkfontsdocs.sh

%attr(644,root,root) /usr/share/Ted/Ted.png

%attr(644,root,root) /usr/share/Ted/varia/Ted.mailcap
%attr(644,root,root) /usr/share/Ted/varia/Ted.desktop
%attr(644,root,root) /usr/share/Ted/varia/base35.fonts.conf
%attr(644,root,root) /usr/share/Ted/varia/base35.rtf
%attr(644,root,root) /usr/share/Ted/varia/ted-2.23.lsm
%attr(644,root,root) /usr/share/Ted/varia/README
%attr(644,root,root) /usr/share/Ted/config/ted.gtkrc-2.0
%attr(644,root,root) /usr/share/Ted/config/Ted.properties.sample
##
##  Cleanup section:
##
%clean
( cd Ted-2.23 && make clean )
%post
update-desktop-database -q || true
%postun
update-desktop-database -q || true
%changelog
* Mon Feb 04 2013 Mark de Does <mark@mdedoes.com> 2.23-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Stability fixes.
- Many-many annoying bugs fixed.
- Some steps to support the few missing features such as absolutely 
positioned objects and shapes.
- Preparations for bidirectional text support
- Comply with LSB directory layout.
- Conformant deb and rpm packages
* Wed Apr 04 2012 Mark de Does <mark@mdedoes.com> 2.22-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Thorough brush-up of the internals.
- Undu/Redo/Recovery
- Many-many annoying bugs fixed.
- Some steps to support the few missing features such as absolutely 
positioned objects and shapes.
* Tue Oct 20 2009 Mark de Does <mark@mdedoes.com> 2.21-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Many improvements in the rendering of embedded images.
- Removed functionality that is not directly related to word processing 
and document formatting. (Email, Fax)
- Added GUI functionality that I forgot while moving to GTK.
- Can now embed fonts from a true type collection in the printout
- Moved more ad-hoc dialogs to the format tool as a preparation for a 
better UI
* Sun Jun 07 2009 Mark de Does <mark@mdedoes.com> 2.20-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Vertical alignment of table cells.
- Multi column layout (of sections).
- Text background and text borders
- Table of Contents
- Unicode compliant (So most scripts and fonts are now supported)
- Use fontconfig: Removed restrictions on the collection of fonts that 
Ted can use; Use Freetype/Xft for anti aliassed text rendering
- Can now render nested tabes. (They are not yet supprted in the user 
interface)
- Versions 2.18 and 2.19 were skipped.
* Fri Jan 28 2005 Mark de Does <mark@mdedoes.com> 2.17-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Numbered lists finished.
- Yet more footnote bugs fixed.
- Behaviour of explicit line and page breaks simulates that of MS-Word.
- Made a configurable resources mechanism that works without X11 
for command line calls.
- Improvements in numbered lists functionality: Opened user interface.
- Preliminary support for 'shapes': The newer Word figures mechanism. 
The Word 97 Drawing Objects are mapped to 'shapes'.
* Thu Apr 01 2004 Mark de Does <mark@mdedoes.com> 2.16-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Fonts can be embedded in the printout to print on any printer.
- Version 2.15 was skipped.
* Sun Apr 06 2003 Mark de Does <mark@mdedoes.com> 2.14-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Table Headers
- Tabs in page headers/footers compatible with MS-Word 2000
* Sat Mar 15 2003 Mark de Does <mark@mdedoes.com> 2.13-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Options for making much more compact PostScript when a document 
is printed.
- Support for smallcaps.
- Upgraded the pdfmarks to a version that more recent versions of 
acroread support.
* Sun Dec 01 2002 Mark de Does <mark@mdedoes.com> 2.12-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Solid shading of paragraphs and table cells.
- Colored table cell borders, Text colors.
* Sun Mar 01 2002 Mark de Does <mark@mdedoes.com> 2.11-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Footnotes and endnotes.
- Detailed manipulation of the tabulator settings with a 'Tabs' tool.
* Mon Apr 30 2001 Mark de Does <mark@mdedoes.com> 2.10-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Widow/Orphan control.
- Keep paragraph on one page, Keep paragraph on same page as next supported.
- Better support for sending MIME and HTML mail. Include images in message.
* Mon Jan 31 2001 Mark de Does <mark@mdedoes.com> 2.9-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Full support for page headers and footers including page numbers.
- Command line conversion to html or to plain text.
- The improvements in WMF drawing and support for PAGEREF fields make 
the pdf files from the printed postscript very similar to the RTF 
original.
* Tue Apr 15 2000 Mark de Does <mark@mdedoes.com> 2.8-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Editing behavior closer to that of Word. E.G. support for Control key 
in navigation and selection has been extended.
- The spelling packages have been renamed since Ted 2.6 to comply 
with naming conventions. If rpm complains about conflicts, please 
remove the conflicting old package using the command rpm -e old_package.
* Fri Dec 31 1999 Mark de Does <mark@mdedoes.com> 2.7-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- A major step toward wysiwyg vertical layout: Pagination is visible 
on screen.
* Thu Sep 30 1999 Mark de Does <mark@mdedoes.com> 2.6-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- The HTML produced is now simpler and syntactically correct.
* Sat Jul 31 1999 Mark de Does <mark@mdedoes.com> 2.5-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Right aligned and centered text are supported.
* Fri May 21 1999 Mark de Does <mark@mdedoes.com> 2.4-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Little bugs that prevented Ted from working with other than Latin1 
fonts removed.
- The Ted document has been improved. It is added as an online document.
* Thu Mar 11 1999 Mark de Does <mark@mdedoes.com> 2.3-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Printing of tables.
* Sat Feb 06 1999 Mark de Does <mark@mdedoes.com> 2.2-1
- New upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
- Usability improvements.
- Version 2.1 was skipped.
* Mon Nov 09 1998 Mark de Does <mark@mdedoes.com> 2.0-1
- First upstream release.
See http://ftp.nluug.nl/pub/editors/ted/announce.html
