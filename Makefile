.DEFAULT_GOAL := all

.PHONY: PHONY_EXPLICIT
PHONY_EXPLICIT:

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

.SECONDARY: $(PROTOS_SOURCES) $(PROTOS_HEADERS)
.SECONDARY: $(PROTO_OBJECTS)

$(PROTODIR)/%.capnp.c++ $(PROTODIR)/%.capnp.h: $(PROTODIR)/%.capnp
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

OUTDIR = bin
.PHONY: clean-out
clean: clean-out
clean-out:
	-rm -f $(OUTDIR)/*

PROGRAM = keyboard
PROGRAM_NAME = $(PROGRAM)
PROGRAMS = $(notdir $(filter-out $(basename $(wildcard $(INCDIR)/*.h)),$(basename $(wildcard $(SRCDIR)/*.cpp)))) #Files which have headers are not programs
.PHONY: programs
programs:
	@for program in $(PROGRAMS); do \
		echo Invoking make for $$program...; \
		$(MAKE) $$program || exit $$?; \
	done

LIB = capnp kj ncursesw
LIBRARIES = $(LIB)

COMDIR = $(SRCDIR)
COM = $(notdir $(filter $(basename $(wildcard $(INCDIR)/*.h)),$(basename $(wildcard $(SRCDIR)/*.cpp)))) #Files which have headers are not programs
COMPONENTS = $(addprefix $(COMDIR)/,$(COM))
COMPONENT_SOURCES = $(addsuffix .cpp,$(COMPONENTS))
COMPONENT_HEADERS = $(addsuffix .h,$(COMPONENTS))

SRCDIR = src
SRC = $(addsuffix .cpp,$(PROGRAM)) #Propably we have only one real source: The main routine
SOURCES = $(addprefix $(SRCDIR)/,$(SRC)) $(COMPONENT_SOURCES) 

INCDIR = $(SRCDIR)
INC =
HEADERS = $(addprefix $(INCDIR)/,$(INC)) $(COMPONENT_HEADERS) $(PROTOS_HEADERS)

OBJDIR = obj
.PHONY: clean-obj
mostlyclean: clean-obj
clean-obj:
	-rm -f $(OBJDIR)/*.o
OBJ = $(SRC:.cpp=.o) $(addsuffix .o,$(COM))
OBJECTS = $(addprefix $(OBJDIR)/,$(OBJ)) $(PROTOS_OBJECTS)
$(OBJDIR)/%.o $(SRCDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -pv $(dir $@)
	@echo Compiling $@...
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(COMPILE_FLAGS) $< -o $@ -I$(INCDIR) -I$(PROTODIR)
.PHONY: objects
objects: $(OBJECTS)
.PHONY: clean-objects
mostlyclean: clean-objects
clean-objects:
	-rm -f $(OBJECTS)

OPTIONS = -Wall -Wextra -g
STD = c++11

LINK_OPTIONS = -Wall -Wextra
LINK_FLAGS = $(patsubst %,-l%,$(LIB)) --std=$(STD) $(OPTIONS) $(LINK_OPTIONS)

COMPILE_OPTIONS = 
COMPILE_FLAGS = --std=$(STD) $(OPTIONS) $(COMPILE_OPTIONS)

EXEC = $(PROGRAM_NAME)
EXECUTABLE = $(OUTDIR)/$(EXEC)
$(EXECUTABLE): $(OBJECTS)
	@mkdir -pv $(dir $@)
	@echo Linking...
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LINK_FLAGS) $^ -o $@
.PHONY: executable
all: executable
executable: $(EXECUTABLE)
.PHONY: clean-executable
mostlyclean: clean-executable
clean-executable:
	-rm -f $(EXECUTABLE)

###################
# Running a program

SERVER_HOST = ecovpn.dyndns.org
SERVER_PORT = 11224
RUN_SOCKET = /dev/tcp/$(SERVER_HOST)/$(SERVER_PORT)

TEAM_USERNAME = binary_cats
TEAM_HASH = qh8irf7ivs5ik1ex0i6i6ckypxxekiv
RUN_CAT_OPTIONS = 
RUN_PARAMETERS = "$(TEAM_USERNAME)" "$(TEAM_HASH)"
RUN_REDIRECTS = 
RUN_SOCKET_REDIRECTS = 3<&$$socket 4>&$$socket
RUN_PREPEND = 

.PHONY: run
run: $(EXECUTABLE)
	bash -c 'exec {socket}<>"$(RUN_SOCKET)" $(RUN_SOCKET_REDIRECTS); $(RUN_PREPEND) $< $(RUN_PARAMETERS) $(RUN_REDIRECTS)'

###############
# Misc commands

VARIABLE = VARIABLE #We like recursion

.PHONY: print_var
print_var:
	@echo '$(VARIABLE)=$(value $(VARIABLE))="$($(VARIABLE))"'

%: $(SRCDIR)/%.cpp
	$(MAKE) PROGRAM="$*"
