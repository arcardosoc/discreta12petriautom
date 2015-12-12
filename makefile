# Autoversion makefile
# (C) 2015, Ruben Carlo Benante
# Data: 2015-10-07
# Versao para Allegro

#Usage:
# * From linux prompt:
#    - Normal C program (ex1.c)
#        $ make ex1.x
#    - Aspipo program (ex1.c)
#        $ make ex1.x obj=libaspipo-ux64.o
# * From vim command:
#    - Normal C program (ex1.c)
#        :make
#    - Aspipo program (ex1.c)
#        :make obj=libaspipo-ux64.o

.PHONY: clean
.PRECIOUS: %.o
SHELL=/bin/bash -o pipefail

MAJOR = 0
MINOR = 1
BUILD = $(shell date +"%g%m%d.%H%M%S")
DEFSYM = $(subst .,_,$(BUILD))
VERSION = "\"$(MAJOR).$(MINOR).$(BUILD)\""
CC = gcc
CFLAGS = "mybib.c" -Wall -g -O0 -std=gnu99
#-ansi -pedantic-errors -c -Ofast -Wextra 
CPPFLAGS = -DVERSION=$(VERSION) -DBUILD="\"$(BUILD)\""
LDLIBS = -Wl,--defsym,BUILD_$(DEFSYM)=0 -lm -lgmp `allegro-config --cflags --libs`

%.x : %.c $(obj)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDLIBS) $^ -o $@ |& tee errors.err

clean:
	rm -f *.x *.o errors.err

	

