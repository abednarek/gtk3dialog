
EXTRA_DIST = gtk3-porting.md gtkdialog.texi version.texi.in mdate-sh texinfo.tex

if BUILD_TEXINFO
gdlg3_infodir = $(infodir)
gdlg3_info_DATA = gdlg3.info

gdlg3.info: $(srcdir)/gtkdialog.texi version.texi
	$(MAKEINFO) -I $(srcdir) -I . -o $@ $(srcdir)/gtkdialog.texi

TEXI2DVI = texi2dvi
.PHONY: pdf
pdf: gtkdialog.pdf

gtkdialog.pdf: $(srcdir)/gtkdialog.texi version.texi
	$(TEXI2DVI) --pdf --batch -I $(srcdir) -I . $(srcdir)/gtkdialog.texi

CLEANFILES = gdlg3.info gtkdialog.pdf
endif
