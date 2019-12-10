TARGET_EXEC ?= bf

CC := gcc
CFLAGS := -std=c99 -Wall -Wextra -Werror -Wmissing-declarations \
	-Wmissing-prototypes -Werror-implicit-function-declaration -Wreturn-type \
	-Wparentheses -Wunused -Wold-style-definition -Wundef -Wshadow \
	-Wstrict-prototypes -Wswitch-default -Wunreachable-code -MMD -MP
CLIBS := 

OUT_DIR  ?= ./build
SRC_DIRS ?= ./src
INC_DIRS ?= ./include

SRCS := $(shell find $(SRC_DIRS) -name '*.c' 2>&1 | grep -v "Permission denied")
OBJS := $(SRCS:%=$(OUT_DIR)/%.o)
DEPS := $(OBJS:.o=.d)	

INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CFLAGS += $(INC_FLAGS)

all: $(OUT_DIR)/$(TARGET_EXEC)

$(OUT_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(CLIBS)

$(OUT_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(CLIBS)

debug: CFLAGS += -g
debug: $(OUT_DIR)/$(TARGET_EXEC)

.PHONY: clean

clean:
	$(RM) -r $(OUT_DIR)

-include $(DEPS)
