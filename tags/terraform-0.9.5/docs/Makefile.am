datadir = @prefix@/doc/terraform

EXTRA_DIST =	FAQ.sgml \
		i18n.txt \
		README.sgml \
		UsersGuide.sgml

data_DATA =	FAQ.sgml \
		i18n.txt \
		README.sgml \
		UsersGuide.sgml

if HAVE_SGMLTOOLS

FAQ.txt: FAQ.sgml
	sgml2txt FAQ.sgml

FAQ: FAQ.sgml
	sgml2html FAQ.sgml

README.txt: README.sgml
	sgml2txt README.sgml

README: README.sgml
	sgml2html README.sgml

UsersGuide.txt: UsersGuide.sgml
	sgml2txt UsersGuide.sgml

UsersGuide: UsersGuide.sgml
	sgml2html UsersGuide.sgml

buildFAQ: FAQ.txt FAQ 
buildREADME: README.txt README 
buildUsersGuide: UsersGuide.txt UsersGuide 

html: FAQ README UsersGuide
txt: FAQ.txt README.txt UsersGuide.txt

all: buildFAQ buildREADME buildUsersGuide
endif


clean: 
	rm -rf FAQ.txt FAQ
	rm -rf README.txt README
	rm -rf UsersGuide.txt UsersGuide

