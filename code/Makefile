################################################################################
#
# PFE
# Author: Paraita Wohler <paraita.wohler@gmail.com>
#
################################################################################

# libs
OPENCL_LIB_OSX   := -framework OpenCL
OPENCL_LIB_LINUX := -lOpenCL
ifeq ($(shell uname), Linux)
OPENCL_LIB       := $(OPENCL_LIB_LINUX)
else
OPENCL_LIB       := $(OPENCL_LIB_OSX)
endif

# arborescence
SRCDIR  := src
BINDIR  := bin
OBJDIR  := obj

# Compilers
CC := g++ -c
LINK := g++
CFLAGS := -Wno-write-strings -DUNIX

# targets
EXECUTABLE := pfe
SRC := CLManager.cpp main.cpp
OBJS := $(SRC:.cpp=.o)
LOBJS := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJS))

# Build executable commands
TARGET  := $(BINDIR)/$(EXECUTABLE)
NVIDIA_LINKLINE = $(LINK) -o $(TARGET) $(LOBJS) $(OPENCL_LIB)


################################################################################
# Regles
################################################################################
%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -o $(OBJDIR)/$@ -c $<

all: $(TARGET)
	@echo "compilation OK (target $(shell uname))"

$(TARGET): makedirs $(OBJS)
	$(NVIDIA_LINKLINE)

makedirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

clean:
	@echo "uoti, ma ro'a !"
	@rm -Rf $(OBJDIR) $(BINDIR)

check-syntax:
	clang++ $(CFLAGS) -Wall -Wextra -fsyntax-only $(SRCDIR)/$(SRC) $(OPENCL_LIB)

.PHONY: clean mrproper check-syntax $(TARGET) $(BINDIR) $(SRCDIR) $(OBJDIR)