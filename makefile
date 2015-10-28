
# find the os
UNAME_S := $(shell uname -s)

# main compiler
CC := gcc
# CC := clang --analyze # and comment out the linker last line for sanity

# define the directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin

# define common variables
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -type f \( -iname "*.$(SRCEXT)" ! -iname "*main-*.$(SRCEXT)" \) )
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g # -Wall
ifneq ($(UNAME_S),Darwin)
	# only add this when compiling in buildroot
	LIB := -l m
endif
#LIB := -pthread -lmongoclient -L lib -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
INC := $(shell find $(INCDIR) -maxdepth 1 -type d -exec echo -I {}  \;)

# define specific binaries to create
APP0 := pwm-exp
SOURCE0 := $(SRCDIR)/main-$(APP0).$(SRCEXT)
OBJECT0 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE0:.$(SRCEXT)=.o))
TARGET0 := $(BINDIR)/$(APP0)

all: $(TARGET0)

$(TARGET0): $(OBJECTS) $(OBJECT0)
	@echo " Compiling $(APP0)"
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET0) $(LIB)"; $(CC) $^ -o $(TARGET0) $(LIB)

# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR)"; $(RM) -r $(BUILDDIR) $(BINDIR)

bla:
	@echo "$(BLA)"
	@echo "Target0: $(APP0) $(SOURCE0) $(OBJECT0) $(TARGET0)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# Spikes
#ticket:
#  $(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
