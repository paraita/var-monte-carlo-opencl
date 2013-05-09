################################################################################
#
# 
# Author: Paraita Wohler <paraita.wohler@gmail.com>
#
################################################################################


# libs (osx specific)
BOOST_INC_OSX	 := -I/opt/local/include
BOOST_LIB_OSX	 := -L/opt/local/lib -lboost_chrono-mt -lboost_system-mt
OPENCL_LIB_OSX   := -framework OpenCL
OPENCL_LIB_LINUX := -lOpenCL -lboost_chrono -lboost_system
ifeq ($(shell uname), Linux)
# libs (GNU/Linux specific)
OPENCL_LIB   := $(OPENCL_LIB_LINUX)
BOOST_INC	 := 
BOOST_LIB	 := -lOpenCL -lboost_chrono -lboost_system
else
OPENCL_LIB   := $(OPENCL_LIB_OSX)
BOOST_INC	 := $(BOOST_INC_OSX) 
BOOST_LIB	 := $(BOOST_LIB_OSX)
endif

# arborescence
SRCDIR  := src
BINDIR  := bin
OBJDIR  := obj

# Compilers
CC := clang++ -c
LINK := clang++
CFLAGS := -Wno-write-strings -DUNIX

# targets
EXECUTABLE := varcl
SRC := Actif.cpp Portefeuille.cpp utils.cpp tests.cpp CLManager.cpp main.cpp
OBJS := $(SRC:.cpp=.o)
LOBJS := $(patsubst %.o,$(OBJDIR)/%.o,$(OBJS))

# Build executable commands
TARGET  := $(BINDIR)/$(EXECUTABLE)
NVIDIA_LINKLINE = $(LINK) -o $(TARGET) $(LOBJS) $(OPENCL_LIB) $(BOOST_LIB)


################################################################################
# Rules
################################################################################
%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -o $(OBJDIR)/$@ -c $< $(OPENCL_INC) $(BOOST_INC)

all: clean arch-cible $(TARGET)
	@echo "Finished OK"

arch-cible:
	@echo "compilation on arch $(shell uname)"

plot: all
	@echo "execution"
	./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 983040 -p ./portefeuille.csv -t 1 -b
	@echo "plotting..."
	@gnuplot histogram.plot

$(TARGET): makedirs $(OBJS)
	$(NVIDIA_LINKLINE)

makedirs:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

clean:
	@echo "uoti, ma ro'a !"
	-@rm -Rf $(OBJDIR) $(BINDIR)
	-@rm -Rf monjob.pbs.*

check-syntax:
	clang++ $(CFLAGS) -Wall -Wextra -fsyntax-only $(SRCDIR)/$(SRC) $(OPENCL_LIB)

.PHONY: clean mrproper check-syntax $(TARGET) $(BINDIR) $(SRCDIR) $(OBJDIR)
