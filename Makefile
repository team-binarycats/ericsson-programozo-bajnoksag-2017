.DEFAULT_GOAL := all

###############
# Basic targets

.PHONY: install
install: #dist
	@echo Not implemented >&2
	@exit 1

.PHONY: dist
dist: #all check
	@echo Not implemented >&2
	@exit 1

.PHONY: check
check: all

.PHONY: all
all:

.PHONY: mostlyclean
mostlyclean:

.PHONY: clean
clean: mostlyclean

######################
# Cap'n Proto handling

PROTODIR := protokoll
PROTOOBJDIR := obj/proto
PROTOS_FILES := $(wildcard $(PROTODIR)/*.capnp)
PROTOS_SOURCES = $(addsuffix .c++,$(PROTOS_FILES))
PROTOS_HEADERS = $(addsuffix .h,$(PROTOS_FILES))
PROTOS_OBJ = $(addsuffix .o,$(basename $(notdir $(PROTOS_FILES))))
PROTOS_OBJECTS = $(addprefix $(PROTOOBJDIR)/,$(PROTOS_OBJ))

CAPNP := capnp
CAPNPC := c++
CAPNP_OPTIONS := 
CAPNP_FLAGS = -o$(CAPNPC) $(CAPNP_OPTIONS)

PROTO_COMPILE_OPTIONS := 
PROTO_COMPILE_FLAGS = --std=c++11 $(PROTO_COMPILE_OPTIONS)

.PHONY: $(PROTODIR)/%.capnp.h
$(PROTODIR)/%.capnp.h: $(PROTODIR)/%.capnp $(PROTODIR)/%.capnp.c++
	@[ -f $@ -a ! $@ -ot $< ] || (echo "Error: $@ doesn't seems to got generated. Please run make $@ -B manually!"; false)

$(PROTODIR)/%.capnp.c++: $(PROTODIR)/%.capnp
	@mkdir -pv $(dir $@)
	@echo Generating protocol source $@...
	$(CAPNP) compile $(CAPNP_FLAGS) $<

$(PROTOOBJDIR)/%.o: $(PROTODIR)/%.capnp.c++ $(PROTODIR)/%.capnp.h
	@mkdir -pv $(dir $@)
	@echo Creating object file from $<...
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(PROTO_COMPILE_FLAGS) $< -o $@ -I$(PROTODIR)


.PHONY: proto
all: proto
proto: $(PROTOS_SOURCES)

.PHONY: compile-proto
compile-proto: $(PROTOS_OBJECTS)


.PHONY: clean-proto-src
clean: clean-proto-src
clean-proto-src:
	-rm -f $(PROTODIR)/*.capnp.{c++,h}

.PHONY: clean-proto-obj
mostlyclean: clean-proto-obj
clean-proto-obj:
	-rm -f $(PROTOOBJDIR)/*.o

##############
# Main program

SRC := main.cpp
OPTIONS := -Wall -Wextra -g
HEADERS = $(PROTOS_HEADERS)
STD := c++11
PROGRAM_NAME := client
LINK_OPTIONS := -Wall -Wextra
COMPILE_OPTIONS := 

OBJDIR := obj
OUTDIR := bin
SRCDIR := src
INCDIR := src

OBJ = $(SRC:.cpp=.o)
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJ)) $(PROTOS_OBJECTS)
EXECUTABLE = $(OUTDIR)/$(PROGRAM_NAME)
LINK_FLAGS = $(OPTIONS) $(LINK_OPTIONS)
COMPILE_FLAGS = $(OPTIONS) $(COMPILE_OPTIONS)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -pv $(dir $@)
	@echo Linking...
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OPTIONS) --std=$(STD) $(LINK_FLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -pv $(dir $@)
	@echo Compiling $@...
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(OPTIONS) --std=$(STD) $(COMPILE_FLAGS) $< -o $@ -I$(INCDIR) -I$(PROTODIR)


.PHONY: executable
all: executable
executable: $(EXECUTABLE)

.PHONY: clean-obj clean-out clean-executable
mostlyclean: clean-obj clean-out clean-executable
clean-obj:
	-rm -f $(OBJDIR)/*.o
clean-out:
	-rm -f $(OUTDIR)/*
clean-executable:
	-rm -f $(EXECUTABLE)
