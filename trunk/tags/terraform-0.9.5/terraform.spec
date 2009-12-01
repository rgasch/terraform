# Note that this is NOT a relocatable package
%define ver		0.9.2
%define RELEASE		1
%define rel		%{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define prefix		/usr/local
%define gnome_datadir	/usr/local/share

Summary: An interactive fractal landscape generator
Name: terraform
Version: %ver
Release: %rel
Copyright: GPL
Group: Graphics
Source: terraform-%{ver}.tar.gz
BuildRoot: /var/tmp/terraform-%{PACKAGE_VERSION}-root
URL: http://terraform.sourceforge.net
Docdir: %{prefix}/doc

%description
Terraform is an interactive height field generation and manipulation
program, giving you the ability to generate random terrain and transform it.

%changelog
* Mon Oct 18 1999 Robert Gasch <Robert_Gasch@peoplesoft.com>
- first spec file 

%prep
%setup

%build
./configure --prefix=%prefix  --disable-sgmltools
make

%install
rm -rf $RPM_BUILD_ROOT
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%{prefix}/bin/terraform
%{gnome_datadir}/terraform/terraform-logo.jpg
%{gnome_datadir}/terraform/include/defaults.inc
%{gnome_datadir}/terraform/include/earth_canyon_landscape.inc
%{gnome_datadir}/terraform/include/earth_green_landscape.inc
%{gnome_datadir}/terraform/include/earth_sky.inc
%{gnome_datadir}/terraform/include/earth_water.inc
%{gnome_datadir}/terraform/include/generic_land.inc
%{gnome_datadir}/terraform/objects/monolith.inc
%{gnome_datadir}/terraform/objects/tree_macro.inc
%{gnome_datadir}/terraform/themes/bare_rock.pov 
%{gnome_datadir}/terraform/themes/earth_canyon.pov
%{gnome_datadir}/terraform/themes/earth_green.pov
%{gnome_datadir}/gnome/help/terraform/C/faq.html
%{gnome_datadir}/gnome/help/terraform/C/hacking.html
%{gnome_datadir}/gnome/help/terraform/C/index.html
%{gnome_datadir}/gnome/help/terraform/C/mainwindow.png
%{gnome_datadir}/gnome/help/terraform/C/povray.png
%{gnome_datadir}/gnome/help/terraform/C/templates.html
%{gnome_datadir}/gnome/help/terraform/C/terrainwindow.png
%{gnome_datadir}/gnome/help/terraform/C/topic.dat
%{gnome_datadir}/gnome/help/terraform/C/tutorial.html
%{gnome_datadir}/gnome/help/terraform/C/wire.png
%{gnome_datadir}/gnome/help/terraform/fr/faq.html
%{gnome_datadir}/gnome/help/terraform/fr/hacking.html
%{gnome_datadir}/gnome/help/terraform/fr/index.html
%{gnome_datadir}/gnome/help/terraform/fr/templates.html
%{gnome_datadir}/gnome/help/terraform/fr/topic.dat
%{gnome_datadir}/gnome/help/terraform/fr/tutorial.html
%{gnome_datadir}/pixmaps/terraform/add.xpm
%{gnome_datadir}/pixmaps/terraform/arrow.xpm
%{gnome_datadir}/pixmaps/terraform/circle.xpm
%{gnome_datadir}/pixmaps/terraform/die.xpm
%{gnome_datadir}/pixmaps/terraform/rectangle.xpm
%{gnome_datadir}/pixmaps/terraform/replace.xpm
%{gnome_datadir}/pixmaps/terraform/subtract.xpm
%{gnome_datadir}/pixmaps/terraform/terraform_logo.xpm
%{gnome_datadir}/pixmaps/terraform.png
%{gnome_datadir}/gnome/apps/Graphics/Terraform.desktop
%{gnome_datadir}/gnome/ximian/Programs/Graphics/Terraform.desktop
%doc AUTHORS COPYING ChangeLog README docs/i18n.txt
