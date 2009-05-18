## Process this file with automake to produce Makefile.in

SUBDIRS = po src data desktop-links docs help

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

HOME_INSTALL_DIRS = data src 
install_home: 
	list='$(HOME_INSTALL_DIRS)'; \
	for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) install_home); \
	done;


