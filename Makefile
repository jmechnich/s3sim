TARGET           = S3Sim

CPPFLAGS  = -Wall -pedantic -g -I. -I$(BUILDDIR) -I$(MUSASHIDIR)

MUSASHIDIR = Musashi
BUILDDIR   = .build

MUSASHIGENINFILE = $(MUSASHIDIR)/m68k_in.c
MUSASHIGENERATOR = $(BUILDDIR)/m68kmake

# files created by m68kmake
MUSASHIGENFILES = \
	$(BUILDDIR)/m68kops.h \
	$(BUILDDIR)/m68kops.c \
	$(BUILDDIR)/m68kopac.c \
	$(BUILDDIR)/m68kopdm.c \
	$(BUILDDIR)/m68kopnz.c
MUSASHIGEN_CC  = $(filter %.c,$(MUSASHIGENFILES))
MUSASHIGEN_OBJ = $(MUSASHIGEN_CC:.c=.o)

CCSOURCES  = \
	$(MUSASHIDIR)/m68kcpu.c \
	$(MUSASHIDIR)/m68kdasm.c
CCSOURCES_OBJ = $(addprefix $(BUILDDIR)/,$(notdir $(CCSOURCES:.c=.o)))

CXXSOURCES = $(TARGET).cc
CXXSOURCES_OBJ = $(addprefix $(BUILDDIR)/,$(notdir $(CXXSOURCES:.cc=.o)))
OBJECTS = \
	$(CCSOURCES_OBJ)  \
	$(MUSASHIGEN_OBJ) \
	$(CXXSOURCES_OBJ)

all: $(TARGET)

$(CXXSOURCES_OBJ): $(CCSOURCES_OBJ)
$(CCSOURCES_OBJ):  $(MUSASHIGEN_OBJ)
$(MUSASHIGEN_OBJ): $(MUSASHIGENFILES)

$(MUSASHIGENERATOR): $(MUSASHIDIR)/m68kmake.c | $(BUILDDIR)
	$(CC) -o $@ $<

$(MUSASHIGENFILES): $(MUSASHIGENERATOR) | $(BUILDDIR)
	$< $(BUILDDIR) $(MUSASHIGENINFILE)

$(BUILDDIR):
	mkdir -p $@

clean:
	rm -rf $(BUILDDIR) $(TARGET) *~

allclean: clean
	rm -f memdump*

$(TARGET): $(OBJECTS) Makefile
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

$(CXXSOURCES_OBJ): $(BUILDDIR)/%.o: %.cc $(BUILDDIR)
	$(CXX) -c $(CPPFLAGS) -o $@ $<

$(CCSOURCES_OBJ):  $(BUILDDIR)/%.o: $(MUSASHIDIR)/%.c | $(BUILDDIR)
	$(CC) -c $(CPPFLAGS) -o $@ $<

$(MUSASHIGEN_OBJ): $(BUILDDIR)/%.o: $(BUILDDIR)/%.c  | $(BUILDDIR)
	$(CC) -c $(CPPFLAGS) -o $@ $<

.NOTPARALLEL:
