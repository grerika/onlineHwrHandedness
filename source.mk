
#
#	Author: Peter Csaszar <csjpeter@gmail.com>
#	Copyright (C) 2015 Peter Csaszar
#

# We dont want to use built in rules. Those are ineffective and we
# can never know all of them, thus those can be unexpected as well.

.SUFFIXES:

MAKEFLAGS += --no-builtin-rules --no-builtin-variables
EMPTY=
COMMA=,
SPACE=$(EMPTY) $(EMPTY)
SHELL=bash

$(DIST_DIR)/debian/copyright: enduser-license.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh enduser-license.in > $@.in
	cat $@.in | groff -T utf8 > $@

$(DIST_DIR)/debian/rules: debian/rules.native.in debian/rules.cross.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	test "x$(PACKAGING)" = "xdebian" && { \
		./generator.$(PACKAGING).sh debian/rules.native.in > $@ ; \
	} || { \
		./generator.$(PACKAGING).sh debian/rules.cross.in > $@ ; \
	}
	chmod u+x $@

$(DIST_DIR)/debian/compat:
	@test -d $(dir $@) || mkdir -p $(dir $@)
	echo "5" > $(DIST_DIR)/debian/compat

$(DIST_DIR)/debian/source/format:
	@test -d $(dir $@) || mkdir -p $(dir $@)
	echo "1.0" > $@

$(DIST_DIR)/debian/%: debian/%.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh debian/$*.in > $@

$(DIST_DIR)/debian/$(PKGNAME).install.in: debian/pkg.install.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh debian/pkg.install.in > $@

$(DIST_DIR)/debian/$(PKGNAME)-dbg.install.in: debian/dbg.install.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh debian/dbg.install.in > $@

debian: \
		$(DIST_DIR)/debian/copyright \
		$(DIST_DIR)/debian/changelog \
		$(DIST_DIR)/debian/control \
		$(DIST_DIR)/debian/rules \
		$(DIST_DIR)/debian/compat \
		$(DIST_DIR)/debian/source/format \
		$(DIST_DIR)/debian/$(PKGNAME).install.in \
		$(DIST_DIR)/debian/$(PKGNAME)-dbg.install.in \
		$(DIST_DIR)/onlineHwrHandedness.desktop.in
	@true

$(DIST_DIR)/nsis/$(PKGNAME).nsi.in: nsis/install.nsi.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh nsis/install.nsi.in > $@

$(DIST_DIR)/nsis/license.txt.in: enduser-license.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh enduser-license.in > $@

windows: \
		$(DIST_DIR)/nsis/$(PKGNAME).nsi.in \
		$(DIST_DIR)/nsis/license.txt.in
	@true

$(DIST_DIR)/configure: configure.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh configure.in > $@
	chmod u+x $(DIST_DIR)/configure

$(DIST_DIR)/%-license: %-license.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh $*-license.in > $@

$(DIST_DIR)/share/translations/%.qm: share/translations/%.ts
	@test -d $(dir $@) || mkdir -p $(dir $@)
	lrelease share/translations/$*.ts -qm $@

$(DIST_DIR)/Makefile.in: Makefile.in Makefile.$(PACKAGING).in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	@echo "all: build" > $@
	./generator.$(PACKAGING).sh Makefile.$(PACKAGING).in >> $@
	./generator.$(PACKAGING).sh Makefile.in >> $@

$(DIST_DIR)/%.in: %.in
	@test -d $(dir $@) || mkdir -p $(dir $@)
	./generator.$(PACKAGING).sh $*.in > $@

$(DIST_DIR)/src/%.h: \
		src/%.h
	@test -d $(dir $@) || mkdir -p $(dir $@)
	cp src/$*.h $@

$(DIST_DIR)/src/%.cpp: \
		src/%.cpp
	@test -d $(dir $@) || mkdir -p $(dir $@)
	cp src/$*.cpp  $@

$(DIST_DIR)/%: %
	@test -d $(dir $@) || mkdir -p $(dir $@)
	cp -pd $< $@

source: \
	$(DIST_DIR)/config \
	$(DIST_DIR)/configure \
	$(DIST_DIR)/Makefile.in \
	$(DIST_DIR)/source-license \
	$(DIST_DIR)/enduser-license \
	$(DIST_DIR)/share/translations/en.qm \
	$(DIST_DIR)/share/translations/hu.qm \
	\
	$(DIST_DIR)/src/config.h.in \
	$(DIST_DIR)/src/h_macros.h \
	$(DIST_DIR)/src/h_conversions.h \
	\
	$(DIST_DIR)/src/h_texts.h \
	$(DIST_DIR)/src/h_texts.cpp \
	\
	$(DIST_DIR)/src/h_config.h \
	$(DIST_DIR)/src/h_config.cpp \
	\
	$(DIST_DIR)/src/qt/h_mainview.h \
	$(DIST_DIR)/src/qt/h_mainview.cpp \
	\
	$(DIST_DIR)/src/qt/h_application.h \
	$(DIST_DIR)/src/qt/h_application.cpp \
	\
	$(DIST_DIR)/src/qt/main.cpp \
	\
	$(DIST_DIR)/share/icons/handedness.png \
	\
	$(DIST_DIR)/share/icons/copyright
	@true

