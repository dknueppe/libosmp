# Makefile for building the OSMP library and complementary binaries
# part of the operatingsystems module @FH-Münster
# Author: Daniel Knüppe (@DanielKnueppe)

TARGETS = osmpclient osmprun testsuite
LIBRARY = osmp
LIB_DIR = osmplib
BUILD_DIR = build

INC_DIRS = $(wildcard */inc)
SRCS = $(foreach T, $(TARGETS), $(wildcard $T/src/*.c))
LIB_SRCS = $(foreach L, $(LIB_DIR), $(wildcard $L/src/*.c))

LIB_OBJS = 	$(foreach S, $(notdir $(LIB_SRCS)), $(BUILD_DIR)/$(basename $(S)).o)
OBJS = 	$(foreach S, $(notdir $(SRCS)), $(BUILD_DIR)/$(basename $(S)).o)
OBJS += $(LIB_OBJS)

CC = gcc
CC_FLAGS = -ggdb3 -O3 -Wall -Wextra -pthread -Wpedantic -std=gnu11 -Werror
LD_FLAGS = -lpthread -lrt
DEFINES =

RUN_TESTSUITE =$(BUILD_DIR)/testsuite

all : $(BUILD_DIR) $(BUILD_DIR)/lib$(LIBRARY).a $(OBJS) $(foreach T, $(TARGETS), $(BUILD_DIR)/$T)

$(BUILD_DIR) :
	mkdir $@

$(BUILD_DIR)/%.o : */src/%.c
	$(CC) -o $@ -c $^ $(CC_FLAGS) $(LD_FLAGS) $(DEFINES) \
	$(foreach D, $(INC_DIRS), -I$D) 

$(BUILD_DIR)/lib$(LIBRARY).a : $(LIB_OBJS)
	ar rcs $@ $^ 

###############################################################################
# This template in conjunction with the following foreach loop serves
# the purpose of creating make targets, that have different parameters
# but where every parameter can be determined through rules.
# also for some reason automatic variables don't work inside said template
###############################################################################
define TARGET_template
$1 : $2 $(BUILD_DIR)/lib$(LIBRARY).a
	$(CC) -o $1 $2 -L$(BUILD_DIR) -l$(LIBRARY) \
	$(CC_FLAGS) $(LD_FLAGS) $(DEFINES) \
	$(foreach D, $(INC_DIRS), -I$D)
endef

$(foreach T, $(TARGETS), $(eval $(call TARGET_template, $(BUILD_DIR)/$T, \
		$(BUILD_DIR)/$(basename $(notdir $(wildcard $T/src/*.c))).o)))
###############################################################################

test : all
	$(RUN_TESTSUITE)

clean :
	rm -rf $(BUILD_DIR)

.PHONY : all clean test