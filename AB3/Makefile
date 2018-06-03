# Makefile for appl and mmanage
.PHONY: clean 

CC	 = /usr/bin/gcc
CFLAGS	 = -pthread -ggdb -Wall -std=gnu99 $(DFLAGS)
ifdef VMEM_PAGESIZE
# Supported page sizes 8 16 32 64
CFLAGS	+= -DVMEM_PAGESIZE=$(VMEM_PAGESIZE)
endif

LDFLAGS =  -lrt -lpthread 

SRCDIR   = ./src
OBJDIR   = ./obj
BINDIR   = ./bin
DOCDIR   = ./html

EXEFILES     = mmanage vmappl # Anwendungen
srcfiles     = $(wildcard $(SRCDIR)/*.c) # all src files
toolfiles    = $(patsubst %,$(SRCDIR)/%.c,$(EXEFILES))  # src files containing main
modulefiles  = $(filter-out $(toolfiles),$(srcfiles)) # modules uesd by tools; does not contain main 
deps         = $(subst $(SRCDIR)/,$(OBJDIR)/,$(srcfiles:.c=.d))

all: $(patsubst %,$(BINDIR)/%,$(EXEFILES))

debug:
	make clean
	make DFLAGS=-DDEBUG_MESSAGES

-include $(deps)

# Build all test programs

# calculate dependcies
$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@echo "computing dependencies for  $<"
	@mkdir -p $(@D)
	@$(CC) -MM -MT "$@ $(patsubst %.d,%.o,$@)" -MF $@ $<

# compile a module
$(OBJDIR)/%.o: $(SRCDIR)/%.c 
	@echo "compiling $< ..."
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS)  -c $< -o $@

# link an executable
$(BINDIR)/% : $(OBJDIR)/%.o $(subst $(SRCDIR)/,$(OBJDIR)/,$(modulefiles:.c=.o)) 
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -r -f $(OBJDIR) $(BINDIR) 
	rm -rf *.o mmanage vmappl logfile.txt pagefile.bin
	rm -rf /dev/shm/sem.sem_wakeup_mmanager_vm_simulation
	rm -rf /dev/shm/sem.sem_wakeup_vmapp_vm_simulation
	sudo ipcrm -ashm

doc: clean
	rm -rf $(DOCDIR)
	doxygen Doxyfile

# EOF
