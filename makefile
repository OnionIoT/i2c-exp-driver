
# main compiler
CC := gcc
# CC := clang --analyze # and comment out the linker last line for sanity

# define the directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin
LIBDIR := lib
PYLIBDIR := lib/python

# add lib directory
#LIB := -L$(LIBDIR) $(LIB)

# define common variables
SRCEXT := c
SOURCES := $(shell find $(SRCDIR) -maxdepth 1 -type f \( -iname "*.$(SRCEXT)" ! -iname "*main-*.$(SRCEXT)" \) )
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g # -Wall
INC := $(shell find $(INCDIR) -maxdepth 1 -type d -exec echo -I {}  \;)


# define specific binaries to create
LIBD := liboniondebug
SOURCE_LIBD := src/lib/onion-debug.$(SRCEXT)
OBJECT_LIBD := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIBD:.$(SRCEXT)=.o))
TARGET_LIBD := $(LIBDIR)/$(LIBD).so

LIB0 := libonioni2c
SOURCE_LIB0 := src/lib/onion-i2c.$(SRCEXT)
OBJECT_LIB0 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIB0:.$(SRCEXT)=.o))
TARGET_LIB0 := $(LIBDIR)/$(LIB0).so
LIB_LIB0 := -L$(LIBDIR) -loniondebug

LIB1 := libonionmcp23008
SOURCE_LIB1 := src/lib/onion-mcp23008-driver.$(SRCEXT)
OBJECT_LIB1 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIB1:.$(SRCEXT)=.o))
TARGET_LIB1 := $(LIBDIR)/$(LIB1).so
LIB_LIB1 := -L$(LIBDIR) -loniondebug -lonioni2c

LIB2 := libonionpwmexp
SOURCE_LIB2 := src/pwm-exp.$(SRCEXT)
OBJECT_LIB2 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIB2:.$(SRCEXT)=.o))
TARGET_LIB2 := $(LIBDIR)/$(LIB2).so
LIB_LIB2 := -L$(LIBDIR) -loniondebug -lonioni2c

LIB3 := libonionrelayexp
SOURCE_LIB3 := src/relay-exp.$(SRCEXT)
OBJECT_LIB3 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIB3:.$(SRCEXT)=.o))
TARGET_LIB3 := $(LIBDIR)/$(LIB3).so
LIB_LIB3 := -L$(LIBDIR) -loniondebug -lonioni2c -lonionmcp23008

LIB4 := libonionoledexp
SOURCE_LIB4 := src/oled-exp.$(SRCEXT)
OBJECT_LIB4 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_LIB4:.$(SRCEXT)=.o))
TARGET_LIB4 := $(LIBDIR)/$(LIB4).so
LIB_LIB4 := -L$(LIBDIR) -loniondebug -lonioni2c


APP0 := pwm-exp
SOURCE_APP0 := $(SRCDIR)/main-$(APP0).$(SRCEXT)
OBJECT_APP0 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_APP0:.$(SRCEXT)=.o))
LIB_APP0 := -L$(LIBDIR) -loniondebug -lonioni2c -lonionpwmexp
TARGET_APP0 := $(BINDIR)/$(APP0)

APP1 := relay-exp
SOURCE_APP1 := $(SRCDIR)/main-$(APP1).$(SRCEXT)
OBJECT_APP1 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_APP1:.$(SRCEXT)=.o))
LIB_APP1 := -L$(LIBDIR) -loniondebug -lonioni2c -lonionmcp23008 -lonionrelayexp
TARGET_APP1 := $(BINDIR)/$(APP1)

APP2 := oled-exp
SOURCE_APP2 := $(SRCDIR)/main-$(APP2).$(SRCEXT) $(SRCDIR)/$(APP2).$(SRCEXT)
OBJECT_APP2 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_APP2:.$(SRCEXT)=.o))
LIB_APP2 := -L$(LIBDIR) -loniondebug -lonioni2c -lonionoledexp
TARGET_APP2 := $(BINDIR)/$(APP2)


PYLIB1 := relayexp
SOURCE_PYLIB1 := src/python/relay-exp-module.$(SRCEXT)
OBJECT_PYLIB1 := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCE_PYLIB1:.$(SRCEXT)=.o))
TARGET_PYLIB1 := $(PYLIBDIR)/$(PYLIB1).so
LIB_PYLIB1 := -L$(LIBDIR) -loniondebug -lonioni2c -lonionmcp23008 -lonionrelayexp -lpython2.7
INC += "-I/usr/include/python2.7"

all: resp $(TARGET_LIBD) $(TARGET_LIB0) $(TARGET_LIB1) $(TARGET_LIB2) $(TARGET_LIB3) $(TARGET_LIB4) $(TARGET_APP0) $(TARGET_APP1) $(TARGET_APP2) $(TARGET_PYLIB1)

# libraries
$(TARGET_LIBD): $(OBJECT_LIBD)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^ $(LIB_LIBD)

$(TARGET_LIB0): $(OBJECT_LIB0)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^ $(LIB_LIB0)

$(TARGET_LIB1): $(OBJECT_LIB1)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^ $(LIB_LIB1)

$(TARGET_LIB2): $(OBJECT_LIB2)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^ $(LIB_LIB2)

$(TARGET_LIB3): $(OBJECT_LIB3)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^ $(LIB_LIB3)

$(TARGET_LIB4): $(OBJECT_LIB4)
	@echo " Compiling $@"
	@mkdir -p $(LIBDIR)
	$(CC) -shared -o $@  $^ $(LIB_LIB4)


# application binaries
$(TARGET_APP0): $(OBJECT_APP0)
	@echo " Compiling $(APP0)"
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(TARGET_APP0) $(LIB) $(LIB_APP0)

$(TARGET_APP1): $(OBJECT_APP1)
	@echo " Compiling $(APP1)"
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(TARGET_APP1) $(LIB) $(LIB_APP1)

$(TARGET_APP2): $(OBJECT_APP2)
	@echo " Compiling $(APP2)"
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	$(CC) $^ $(CFLAGS) $(LDFLAGS) -o $(TARGET_APP2) $(LIB) $(LIB_APP2)


$(TARGET_PYLIB1): $(OBJECT_PYLIB1)
	@echo " Compiling $@"
	@mkdir -p $(PYLIBDIR)
	$(CC) -shared -o $@  $^ $(LIB_PYLIB1)


# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR) $(BINDIR) $(LIBDIR)

bla:
	@echo "$(BLA)"
	@echo "TARGET_APP0: $(APP0) $(SOURCE_APP0) $(OBJECT_APP0) $(TARGET_APP0)"
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"

resp:
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIB: $(LIB)"

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# Spikes
#ticket:
#  $(CC) $(CFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
