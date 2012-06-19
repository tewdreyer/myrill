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
PROGRAMS = myrill
OBJ      = bsp.o save_load.o backtracking.o creatures.o mechanics.o output.o myrill.o

# version
MAJOR = 0
MINOR = 2

# package
PKG_NAME = myrill-$(MAJOR).$(MINOR)
TARFILE  = $(PKG_NAME).tar

all: $(PROGRAMS)

clean:
	@rm -fv $(OBJ) $(PROGRAMS)

install:
	@cp -v myrill $(HOME)/bin

uninstall:
	@rm -v $(HOME)/bin/myrill

myrill: $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o myrill

.c.o:
	$(CC) $(CFLAGS) -c $<

# just for releases
package:
	@rm -rf $(PKG_NAME) $(TARFILE)  $(TARFILE).gz
	@mkdir $(PKG_NAME)
	@cp Makefile *.[ch] LICENSE $(PKG_NAME)
	@tar -cf $(TARFILE) $(PKG_NAME)
	@rm -rf $(PKG_NAME)
	@gzip --best $(TARFILE)
