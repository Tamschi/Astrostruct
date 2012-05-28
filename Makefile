# Makefile.  Generated from Makefile.in by configure.

# Package-related substitution variables
package			= astrostruct
version			= 1.0
tarname			= astrostruct
distdir			= $(tarname)-$(version)

# Prefix-related substitution variables
prefix			= /usr/local
exec_prefix		= ${prefix}
bindir			= ${exec_prefix}/bin

# VPATH-related substitution variables
srcdir			= .


# Recursive commands
all main:
	$(MAKE) -C src $@

# Package up for distrobution
dist: $(distdir).tar.gz

$(distdir).tar.gz: FORCE $(distdir)
	tar chof - $(distdir) |\
	 gzip -9 -c >$(distdir).tar.gz
	rm -rf $(distdir)

$(distdir):
	mkdir -p $(distdir)/src
	cp $(srcdir)/configure $(distdir)
	cp $(srcdir)/Makefile.in $(distdir)
	cp $(srcdir)/src/Makefile.in $(distdir)/src
	cp $(srcdir)/src/*.cpp $(distdir)/src
	cp $(srcdir)/src/*.hpp $(distdir)/src
	cp $(srcdir)/src/config.h.in $(distdir)/src
	cp $(srcdir)/install-sh $(distdir)
	cp -r $(srcdir)/pkg $(distdir)

distcheck: $(distdir).tar.gz
	gzip -cd $+ | tar xvf -
	cd $(distdir); CXXFLAGS="-I/usr/include/freetype2" ./configure
	$(MAKE) -C $(distdir) all
	$(MAKE) -C $(distdir) \
	DESTDIR=$${PWD}/$(distdir)/_inst \
	install uninstall
	$(MAKE) -C $(distdir) clean
	rm -rf $(distdir)
	@echo "*** Package $(distdir).tar.gz is\
	 ready for distribution."

clean:
	rm -rf doc/html doc/latex
	$(MAKE) -C src $@

FORCE:
	-rm -f $(distdir).tar.gz
	-rm -rf $(distdir)

# Install Uninstall mumbo jumbo
install: all
	@echo "Not sure how this is going to be installed yet!"
	@echo "Pretend some files appeared in $(DESTDIR)/$(prefix)/share/$(package)."
	@echo "As well as a binary in $(DESTDIR)/$(bindir)."

uninstall:
	@echo "Nothing's been installed! Dummy :u"
	@echo "Pretend I removed files from $(DESTDIR)/$(prefix)/share/$(package)."
	@echo "As well as a binary from $(DESTDIR)/$(bindir)."

# Autotools mumbo jumbo
Makefile: Makefile.in config.status
	./config.status $@

config.status: configure
	./config.status --recheck

.PHONY: all clean dist FORCE distcheck install uninstall
