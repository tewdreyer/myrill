#  myrill - Makefile
#
#  Copyright (C) 2011 Tobias Dreyer <dreyer@molgen.mpg.de>
#  Copyright (C) 2011 Max Planck Institut for Molecular Genetics
#
#  myrill is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  myrill is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC       = gcc
CFLAGS   = -Wall -g -pedantic
LDFLAGS  = -lcurses -lm

DESTDIR=

PREFIX=/usr
BINDIR=${PREFIX}/bin

MYRILL_BIN = myrill

MYRILL_OBJ = bsp.o save_load.o backtracking.o creatures.o mechanics.o output.o myrill.o

quiet-command = $(if ${V},${1},$(if ${2},@echo ${2} && ${1}, @${1}))
quiet-install = $(call quiet-command,install -m ${1} ${2} ${3},"INSTALL	${3}")
quiet-installdir = $(call quiet-command,install -m ${1} -d ${2},"MKDIR	${2}")

all: build

build: ${MYRILL_BIN}

myrill: ${MYRILL_OBJ}
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(call quiet-command, rm -f *.o, "RM	*.o")
	$(call quiet-command, rm -f ${MYRILL_BIN}, "RM	${MYRILL_BIN}")

install: build install-bin

install-bin: $(addprefix ${DESTDIR}${BINDIR}/, ${MYRILL_BIN})

${DESTDIR}${BINDIR}/%: % install-dir-bindir
	$(call quiet-install, 755, $<, $@)

install-dir-bindir:
	$(call quiet-installdir, 755, ${DESTDIR}${BINDIR})

# version
MAJOR = 0
MINOR = 2

# package
PKG_NAME = myrill-$(MAJOR).$(MINOR)
TARFILE  = $(PKG_NAME).tar

# just for releases
package:
	@rm -rf $(PKG_NAME) $(TARFILE)  $(TARFILE).gz
	@mkdir $(PKG_NAME)
	@cp Makefile *.[ch] LICENSE $(PKG_NAME)
	@tar -cf $(TARFILE) $(PKG_NAME)
	@rm -rf $(PKG_NAME)
	@gzip --best $(TARFILE)
