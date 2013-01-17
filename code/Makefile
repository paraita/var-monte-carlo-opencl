################################################################################
#
# PFE
# Author: Paraita Wohler <paraita.wohler@gmail.com>
#
################################################################################

# specifique a NEF
ifeq ($(shell uname -n), nef-devel)
OPENCL_INC	:= -I/usr/local/cuda-5.0/include
else
OPENCL_INC	:=
endif

# libs
BOOST_INC_OSX	 := -I/opt/local/include
BOOST_LIB_OSX	 := -L/opt/local/lib
OPENCL_LIB_OSX   := -framework OpenCL
OPENCL_LIB_LINUX := -lOpenCL
ifeq ($(shell uname), Linux)
OPENCL_LIB       := $(OPENCL_LIB_LINUX)
BOOST_INC	 :=
BOOST_LIB	 :=
else
OPENCL_LIB       := $(OPENCL_LIB_OSX)
BOOST_INC	 := $(BOOST_INC_OSX)
BOOST_LIB	 := $(BOOST_LIB_OSX)
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
NVIDIA_LINKLINE = $(LINK) -o $(TARGET) $(LOBJS) $(OPENCL_LIB) $(BOOST_LIB)


################################################################################
# Regles
################################################################################
%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -o $(OBJDIR)/$@ -c $< $(OPENCL_INC) $(BOOST_INC)

all: arch-cible $(TARGET)
	@echo "compilation OK"

arch-cible:
	@echo "compilation pour architecture $(shell uname)"

nef: all
	@echo "run sur nef ok"

$(TARGET): makedirs $(OBJS)
	$(NVIDIA_LINKLINE)

makedirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)
	@cp $(SRCDIR)/test_addition.cl $(BINDIR)/test_addition.cl

clean:
	@echo "uoti, ma ro'a !"
	@rm -Rf $(OBJDIR) $(BINDIR)

check-syntax:
	clang++ $(CFLAGS) -Wall -Wextra -fsyntax-only $(SRCDIR)/$(SRC) $(OPENCL_LIB)

.PHONY: clean mrproper check-syntax $(TARGET) $(BINDIR) $(SRCDIR) $(OBJDIR)