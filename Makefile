
# Sources

SRCS = list.cpp ui.cpp main.cpp config.cpp map.cpp game.cpp
SRCS += screen_show_in_game.cpp screen_show_ranking.cpp

###################################################

# Project name
PROJ_NAME=test_sdl
OUTPATH=build

###################################################

BINPATH=
CC=$(BINPATH)gcc
CXX=$(BINPATH)g++
AS=$(BINPATH)as
OBJCOPY=$(BINPATH)objcopy
SIZE=$(BINPATH)size

###################################################
#sgcc -I /Library/Frameworks/SDL2.framework/Headers -framework SDL2 your_file.c
CXXFLAGS  = -O2 -g -Wall -fmessage-length=0
CXXFLAGS +=  -Wno-unused-function -Wno-unused-variable -Wno-unused-value -Wno-unused-but-set-variable
###################################################
ifeq ($(OS),Windows_NT)
    CXXFLAGS += -D WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
      CXXFLAGS += -D AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            CXXFLAGS += -D AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            CXXFLAGS += -D IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CXXFLAGS += -D LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CXXFLAGS += -D OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        CXXFLAGS += -D AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        CXXFLAGS += -D IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        CXXFLAGS += -D ARM
    endif
endif
###################################################

vpath %.cpp Src
vpath %.a Lib

###################################################

ROOT=$(shell pwd)

# Includes
ifeq (OSX,$(findstring OSX,$(CXXFLAGS)))
	CXXFLAGS += -IInc -ISDL2/osx/SDL2.framework/Headers
else ifeq (WIN32,$(findstring WIN32,$(CXXFLAGS)))
	CXXFLAGS += -IInc -ISDL2/include/SDL2
endif

# Library paths
ifeq (OSX,$(findstring OSX,$(CXXFLAGS)))
	LIBPATHS = -FSDL2/osx
else ifeq (WIN32,$(findstring WIN32,$(CXXFLAGS)))
	LIBPATHS = -LLib -LSDL2/lib
endif

# Libraries to link
ifeq (OSX,$(findstring OSX,$(CXXFLAGS)))
	LIBS = -framework SDL2
else ifeq (WIN32,$(findstring WIN32,$(CXXFLAGS)))
	LIBS = -lm -lpthreadGC2 -lsdl2
endif

ifeq (OSX,$(findstring OSX,$(CXXFLAGS)))
	EXE_NAME = $(PROJ_NAME)
else ifeq (WIN32,$(findstring WIN32,$(CXXFLAGS)))
	EXE_NAME = $(PROJ_NAME).exe
endif


# Extra includes
CXXFLAGS += 


# add files to build
SRCS += 

OBJS = $(SRCS:.cpp=.o)

###################################################

.PHONY: proj

all: proj
	$(SIZE) --format=berkeley $(OUTPATH)/$(EXE_NAME)

proj: $(OUTPATH)/$(EXE_NAME)

$(OUTPATH)/$(EXE_NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBPATHS) $(LIBS)
clean:
	rm -f *.o
	rm -f $(OUTPATH)/$(EXE_NAME)
#$(MAKE) clean -C Drivers 

###################################################
