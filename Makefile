CC = gcc

TOPDIR := $(CURDIR)
OBJDIR := $(TOPDIR)/obj
BINDIR := $(TOPDIR)/bin
BIN    := sakura_modbus

CFLAGS = -m32 -g -O -Wall -I$(TOPDIR)/inc

SUBDIR := src

export CC TOPDIR OBJDIR BINDIR BIN CFLAGS

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$(1)" mkdir "$(1)"
    RM = if exist "$(1)" rmdir /s /q "$(1)"
else
    MKDIR = mkdir -p $(1)
    RM = rm -rf $(1)
endif

all:CHECKDIR $(SUBDIR) MAIN_OBJ LINK_BIN

CHECKDIR:
	@$(call MKDIR,$(OBJDIR))
	@$(call MKDIR,$(BINDIR))
$(SUBDIR):RUN
	make -C $@
RUN:

MAIN_OBJ: $(OBJDIR)/main.o
$(OBJDIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

LINK_BIN:
	$(CC) $(CFLAGS) -o $(BINDIR)/$(BIN) $(OBJDIR)/*.o $(LDFLAGS)

clean:
	@$(call RM,$(OBJDIR))
	@$(call RM,$(BINDIR))