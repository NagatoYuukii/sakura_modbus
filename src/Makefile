TARGET = $(OBJ)

SRC_FILES := $(wildcard *.c)

OBJ_FILE := $(OBJDIR)/$(TARGET)

all: $(OBJ_FILE)
$(OBJ_FILE): $(patsubst %.c, $(OBJDIR)/%.o, $(SRC_FILES))

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


