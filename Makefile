VERSION_MAJOR = 0
VERSION_MINOR = 0
VERSION_PATCH = 1
VERSION_FULL = $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)

os = $(shell uname -s)
arch = $(shell uname -m)

# default to gcc

ifeq ($(CC),)
CC = gcc
endif
ifeq ($(CC),cc)
CC = gcc
endif

# use compiler as linker if linker is not set
LINKER ?= $(CC)

make_path := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
project_dir := $(notdir $(patsubst %/,%,$(make_path)))
proj = stdext
PROJ = $(shell echo $(proj) | tr '[:lower:]' '[:upper:]')
libshared = $(proj).so
libstatic = $(proj).a

PREFIX ?= $(HOME)/.local

$(info project ${proj} OS=${os} ARCH=${arch} CC=${CC} LINKER=${LINKER})

OLEVEL ?= 0 # ignored in debug
STD ?= -std=c11

ignored_warnings = -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable
version_flags = -D$(PROJ)_VERSION_MAJOR=$(VERSION_MAJOR) -D$(PROJ)_VERSION_MINOR=$(VERSION_MINOR) -D$(PROJ)_VERSION_PATCH=$(VERSION_PATCH)

ifdef RELEASE
CFLAGS += -DNDEBUG -O$(OLEVEL)
else
CFLAGS += -DDEBUG -g -rdynamic
endif

CFLAGS += -fPIC $(STD) -Wall -Wextra -Ilibs -Iinclude  -I$(PREFIX)/include $(ignored_warnings) $(version_flags)

# libraries that can be found with pkg-config
SYSLIBS =

ifdef SYSLIBS
LDFLAGS += $(shell pkg-config --libs $(SYSLIBS))
CFLAGS += $(shell pkg-config --cflags $(SYSLIBS))
endif

# other libs
libsources =

# force rebuild if flag checksum changes
buildhash = build/$(strip $(shell echo "$(CFLAGS) $(LDFLAGS)" | md5sum | cut -d " " -f1))

sources += $(wildcard src/*.c)
sources += $(libsources)
headers += $(wildcard include/*.h) $(wildcard include/ccpsx/*.h)
objects += $(sources:%.c=$(buildhash)/%.o)
libobjects = $(filter-out $(buildhash)/src/main.o, $(objects))

testsources = $(wildcard test/*.c)
testobjects = $(testsources:%.c=$(buildhash)/%.o)


# use one shell to make exit working in target, only used in uninstall
.ONESHELL:

.PHONY: all

all: $(libshared) $(libstatic) $(executable)

init:
	$(info init)
	git submodule update --init --recursive

update:
	$(info updating dependencies)
	git submodule update --merge --remote

format:
	clang-format -i src/*.c
	clang-format -i src/*.h
	clang-format -i test/*.c


test: $(libobjects) $(testobjects)
	$(LINKER) $(LDFLAGS) $^ -o $@.bin


shared: $(libshared)

$(libshared): $(libobjects)
	@echo "$(LINKER) $^ -shared $(LDFLAGS) -o $@"
	$(LINKER) $^ -shared $(LDFLAGS) -o $@

static: $(libstatic)


$(libstatic): $(libobjects)
	$(AR) -rcs $@ $^

$(objects): $(headers)

$(buildhash)/%.o: %.c
	@mkdir -p $(@D)
	@echo "$(CC) $(CFLAGS) -c $< -o $@"
	$(CC) $(CFLAGS) -c $< -o $@

install: $(libshared) $(libstatic)
	@printf "#pragma once\n#define $(PROJ)_VERSION_MAJOR $(VERSION_MAJOR)\n#define $(PROJ)_VERSION_MINOR $(VERSION_MINOR)\n#define $(PROJ)_VERSION_PATCH $(VERSION_PATCH)\n" > include/$(proj)/generated_version.h
	mkdir -p "$(PREFIX)/include/"
	mkdir -p "$(PREFIX)/lib"
	cp -r include/* "$(PREFIX)/include/"
	cp $(libshared) "$(PREFIX)/lib/$(libshared).$(VERSION_FULL)"
	ln -rsf "$(PREFIX)/lib/$(libshared).$(VERSION_FULL)" "$(PREFIX)/lib/$(libshared)"
	ln -srf "$(PREFIX)/lib/$(libshared).$(VERSION_FULL)" "$(PREFIX)/lib/$(libshared).$(VERSION_MAJOR)"
	@echo "$(PREFIX)" > .installdir

uninstall: targetdir := $(shell [ -f .installdir ] && cat .installdir)
uninstall: uninstall_action

uninstall_action:
	@if [ ! -d "$(targetdir)" ]; then echo "no valid install dir"; exit; fi;
	rm -rf "$(targetdir)/lib/$(proj)*"
	rm -rf "$(targetdir)/include/$(proj)"
	rm -f .installdir


.PHONY: clean

clean:
	rm -f $(libstatic)
	rm -f $(libshared)
	rm -rf build/*
	rm -f test.bin
	rm -f test/*.o
