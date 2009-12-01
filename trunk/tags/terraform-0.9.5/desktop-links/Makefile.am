## Process this file with automake to produce Makefile.in

EXTRA_DIST = terraform.png Terraform.desktop

install-data-local:
	@$(NORMAL_INSTALL)
	if test -n "@datadir@"; then \
		$(mkinstalldirs) $(GNOME_DATADIR)/gnome/apps/Graphics; \
		$(INSTALL_DATA) Terraform.desktop $(GNOME_DATADIR)/gnome/apps/Graphics; \
		$(mkinstalldirs) $(GNOME_DATADIR)/gnome/ximian/Programs/Graphics; \
		$(INSTALL_DATA) Terraform.desktop $(GNOME_DATADIR)/gnome/ximian/Programs/Graphics; \
		$(mkinstalldirs) $(GNOME_DATADIR)/pixmaps; \
		$(INSTALL_DATA) terraform.png $(GNOME_DATADIR)/pixmaps; \
	fi
