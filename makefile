
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
LIBDIR := lib

# define common variables
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -maxdepth 1 -type f \( -iname "*.$(SRCEXT)" ! -iname "*main-*.$(SRCEXT)" \) )
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
SOURCE0 := $(SRCDIR)/main-$(APP0).$(SRCEXT) $(SRCDIR)/$(APP0).$(SRCEXT)
OBJECT0 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE0:.$(SRCEXT)=.o))
TARGET0 := $(BINDIR)/$(APP0)

APP1 := relay-exp
SOURCE1 := $(SRCDIR)/main-$(APP1).$(SRCEXT) $(SRCDIR)/$(APP1).$(SRCEXT)
OBJECT1 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE1:.$(SRCEXT)=.o))
TARGET1 := $(BINDIR)/$(APP1)

LIB0 := libonioni2c
SOURCE_LIB0 := src/lib/onion-i2c.$(SRCEXT)
OBJECT_LIB0 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIB0:.$(SRCEXT)=.o))
TARGET_LIB0 := $(LIBDIR)/$(LIB0).so

LIB1 := libonionmcp23008
SOURCE_LIB1 := src/lib/onion-mcp23008-driver.$(SRCEXT)
OBJECT_LIB1 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIB1:.$(SRCEXT)=.o))
TARGET_LIB1 := $(LIBDIR)/$(LIB1).so


all: $(TARGET_LIB0) $(TARGET_LIB1) $(TARGET0) $(TARGET1)

# libraries
$(TARGET_LIB0): $(OBJECT_LIB0)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^

$(TARGET_LIB1): $(OBJECT_LIB1)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^ -L$(LIBDIR) -lonioni2c

# application binaries
$(TARGET0): $(OBJECT0)
	@echo " Compiling $(APP0)"
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET0) $(LIB) -L$(LIBDIR) -lonioni2c"; $(CC) $^ -o $(TARGET0) $(LIB) -L$(LIBDIR) -lonioni2c

$(TARGET1): $(OBJECT1)
	@echo " Compiling $(APP1)"
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET1) $(LIB)"; $(CC) $^ -o $(TARGET1) $(LIB) -L$(LIBDIR) -lonioni2c -lonionmcp23008


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
