## Process this file with automake to produce Makefile.in
datadir = @datadir@/@PACKAGE@

INCLUDES = \
	$(PACKAGE_CFLAGS) \
	-DGTK_ENABLE_BROKEN \
	-DTERRAFORM_DATA_DIR=\"$(datadir)\" \
	-DGNOME_DATA_DIR=\"$(GNOME_DATADIR)\"

bin_PROGRAMS = terraform

terraform_SOURCES = \
	main.c \
	base64.c base64.h \
	callbacks.c callbacks.h \
	clocale.c clocale.h \
	colormaps.c colormaps.h \
	contour.c contour.h \
	exportview.c exportview.h \
	fftn.c fftn.h \
	filelist.c filelist.h \
	filenameops.c filenameops.h \
	filters.c filters.h \
	filterwindow.c filterwindow.h \
	genfault.c genfault.h \
	genperlin.c genperlin.h \
	genrandom.c genrandom.h \
	genspectral.c genspectral.h \
	gensubdiv.c gensubdiv.h \
	installer.c installer.h \
	interface.c interface.h \
	mainwindow.c mainwindow.h \
	native3d.c native3d.h \
	place.c place.h \
	optionwindow.c optionwindow.h \
	reader.c reader.h \
	selection.c selection.h \
	support_modified.c support.h \
	support2.c support2.h \
	smartexec.c smartexec.h \
	statistics.c statistics.h \
	taspectframe.c taspectframe.h \
	tcanvas.c tcanvas.h \
	terrainwindow.c terrainwindow.h \
	tmatrix.c tmatrix.h \
	tperlin.c tperlin.h \
	tperlin3d.c tperlin3d.h \
	tpixbuf.c tpixbuf.h \
	trandom.c trandom.h \
	tterrain.c tterrain.h \
	tterrainview.c tterrainview.h \
	tundo.c tundo.h \
	tworld.c tworld.h \
	writer.c writer.h \
	xmlsupport.c xmlsupport.h 

if HAVE_LIB_TRIO
TF_EXTRA_LIBS = -ltrio
endif

terraform_LDADD = $(PACKAGE_LIBS) $(PNG_LIBS) $(INTLLIBS) $(TF_EXTRA_LIBS)

install_home: 
	if [ ! -d "$(HOME)/bin" ]; then \
	  mkdir "$(HOME)/bin" ; \
	fi;
	cp -f terraform "$(HOME)/bin"; 


