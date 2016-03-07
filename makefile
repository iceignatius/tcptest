# ----------------------------------------------------------
# ---- TCP connection test tool ----------------------------
# ----------------------------------------------------------

# Detect OS name
ifeq ($(OS),)
	OS := $(shell uname -s)
endif

# Tools setting
CC  := gcc
CXX := g++
LD  := g++
AR  := ar rcs

# Installation path
ifndef INSTDIR
ifeq ($(OS),Windows_NT)
	INSTDIR = C:/Windows/System32
else
	INSTDIR = /usr/local
endif
endif

# Setting
OUTDIR  := bin
OUTNAME := tcptest
ifeq ($(OS),Windows_NT)
	OUTEXT := .exe
else
	OUTEXT :=
endif
OUTPUT  := $(OUTDIR)/$(OUTNAME)$(OUTEXT)
TEMPDIR := temp
INCDIR  :=
INCDIR  += -I../igntoolkit/genutil
ifeq ($(OS),Windows_NT)
	INCDIR += -I../igntoolkit/wingetopt/src
endif
LIBDIR  :=
CFLAGS  :=
CFLAGS  += -Wall
CFLAGS  += -fexceptions
CFLAGS  += -std=gnu++11
CFLAGS  += -O2
LDFLAGS :=
LDFLAGS += -s
SRCS    :=
ifeq ($(OS),Windows_NT)
	SRCS += ../igntoolkit/wingetopt/src/getopt.c
endif
SRCS    += ../igntoolkit/genutil/gen/systime.c
SRCS    += ../igntoolkit/genutil/gen/timeinf.c
SRCS    += ../igntoolkit/genutil/gen/net/iptype.c
SRCS    += ../igntoolkit/genutil/gen/net/sockbase.c
SRCS    += ../igntoolkit/genutil/gen/net/socktcp.c
SRCS    += ../igntoolkit/genutil/gen/net/urlpar.c
SRCS    += ../igntoolkit/genutil/gen/net/winwsa.c
SRCS    += cmdopt.cpp
SRCS    += main.cpp
LIBS    :=
ifeq ($(OS),Windows_NT)
	LIBS += -lws2_32
else
	LIBS += -lrt
endif
OBJS    := $(notdir $(SRCS))
OBJS    := $(addprefix $(TEMPDIR)/,$(OBJS))
OBJS    := $(OBJS:%.c=%.o)
OBJS    := $(OBJS:%.cpp=%.o)
DEPS    := $(OBJS:%.o=%.d)

# Process summary
.PHONY: all clean
.PHONY: pre_step create_dir build_step post_step
.PHONY: install uninstall test
all: pre_step create_dir build_step post_step

# Clean process
clean:
ifeq ($(OS),Windows_NT)
	-del /Q $(subst /,\,$(OBJS))
	-del /Q $(subst /,\,$(DEPS))
	-del /Q $(subst /,\,$(OUTPUT))
	-rmdir /Q $(subst /,\,$(TEMPDIR))
	-rmdir /Q $(subst /,\,$(OUTDIR))
else
	-@rm -f $(OBJS) $(DEPS) $(OUTPUT)
	-@rmdir $(TEMPDIR) $(OUTDIR)
endif

# Build process

pre_step:
create_dir:
ifeq ($(OS),Windows_NT)
	@cmd /c if not exist $(subst /,\,$(TEMPDIR)) mkdir $(subst /,\,$(TEMPDIR))
	@cmd /c if not exist $(subst /,\,$(OUTDIR)) mkdir $(subst /,\,$(OUTDIR))
else
	@test -d $(TEMPDIR) || mkdir $(TEMPDIR)
	@test -d $(OUTDIR)  || mkdir $(OUTDIR)
endif
build_step: $(OUTPUT)
post_step:

$(OUTPUT): $(OBJS)
	$(LD) -o $@ $(LIBDIR) $(LDFLAGS) $^ $(LIBS)

define Compile-C-Unit
$(CC) -MM $(INCDIR) $(CFLAGS) -o $(TEMPDIR)/$*.d $< -MT $@
$(CC) -c  $(INCDIR) $(CFLAGS) -o $@ $<
endef
define Compile-Cpp-Unit
$(CXX) -MM $(INCDIR) $(CFLAGS) -o $(TEMPDIR)/$*.d $< -MT $@
$(CXX) -c  $(INCDIR) $(CFLAGS) -o $@ $<
endef

-include $(DEPS)
$(TEMPDIR)/%.o: ../igntoolkit/genutil/gen/%.c
	$(Compile-C-Unit)
$(TEMPDIR)/%.o: ../igntoolkit/genutil/gen/%.cpp
	$(Compile-Cpp-Unit)
$(TEMPDIR)/%.o: ../igntoolkit/genutil/gen/net/%.c
	$(Compile-C-Unit)
$(TEMPDIR)/%.o: ../igntoolkit/genutil/gen/net/%.cpp
	$(Compile-Cpp-Unit)
$(TEMPDIR)/%.o: %.c
	$(Compile-C-Unit)
$(TEMPDIR)/%.o: %.cpp
	$(Compile-Cpp-Unit)

ifeq ($(OS),Windows_NT)
$(TEMPDIR)/%.o: wingetopt/src/%.c
	$(Compile-C-Unit)
$(TEMPDIR)/%.o: wingetopt/src/%.cpp
	$(Compile-Cpp-Unit)
endif

# User extended process

install:
ifeq ($(OS),Windows_NT)
	copy /Y $(subst /,\,$(OUTPUT)) $(subst /,\,$(INSTDIR))
else
	cp $(OUTPUT) $(INSTDIR)/bin
endif

uninstall:
ifeq ($(OS),Windows_NT)
	-del /Q $(subst /,\,$(INSTDIR)/$(OUTNAME)$(OUTEXT))
else
	-rm $(INSTDIR)/bin/$(OUTNAME)$(OUTEXT)
endif

test: all
	$(OUTPUT) --help
