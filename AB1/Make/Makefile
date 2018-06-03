# file makefile
.PHONY: clean delete install
CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -g

# Direcotries for the filesystem
SRCDIR = src
OBJDIR = obj
BINDIR = bin
TSTDIR = test

# Filescollections
sources = $(wildcard $(SRCDIR)/*.c)
objects = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(sources:.c=.o))
executables = test/haw.exe test/hallo.exe test/bs.exe
deps = $(objects:.o=.d)

# creates the executables in a test folder
all: $(executables)
-include $(deps)

# dependencies are computing here
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@echo "Computing dependencies for $<"
	@mkdir -p $(@D)
	@$(CC) -MM -MT "$@ $(patsubst %.d,%.o,$@)" -MF $@ $<

# linking the object files to executables
$(TSTDIR)/%.exe: $(OBJDIR)/%.o $(OBJDIR)/print.o
	@echo "Linking $<...to $@"
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^

# compiling all the c files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "Compiling $<...to $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@ 

# creates executables and copies them to a bin folder
install: all
	@echo "Installing Programms"
	@mkdir -p $(BINDIR)
	@cp -u $(executables) $(BINDIR)

clean:
	@echo "Cleaning temporary files"
	rm -r -f $(TSTDIR) $(OBJDIR)

delete: clean
	@echo "Deleting executables"
	rm -r -f $(BINDIR)
	
