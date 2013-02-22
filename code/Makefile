################################################################################
#
# PFE
# Author: Paraita Wohler <paraita.wohler@gmail.com>
#
################################################################################


# libs
BOOST_INC_OSX	 := -I/opt/local/include
BOOST_LIB_OSX	 := -L/opt/local/lib -lboost_chrono-mt -lboost_system-mt
OPENCL_LIB_OSX   := -framework OpenCL
OPENCL_LIB_LINUX := -lOpenCL -lboost_chrono -lboost_system
ifeq ($(shell uname), Linux)
# specifique a NEF
ifeq ($(shell uname -n), nef-devel)
OPENCL_INC       := -I/usr/local/cuda-5.0/include
BOOST_INC        := -I/home/pwohler/boost_1_53_0/include
BOOST_LIB	 := -L/home/pwohler/boost_1_53_0/lib -lOpenCL -lboost_chrono -lboost_system
else
# linux en general
OPENCL_LIB       := $(OPENCL_LIB_LINUX)
BOOST_INC	 := 
BOOST_LIB	 := -lOpenCL -lboost_chrono -lboost_system
endif
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
CC := clang++ -c
LINK := clang++
CFLAGS := -Wno-write-strings -DUNIX

# targets
EXECUTABLE := pfe
SRC := Actif.cpp Portefeuille.cpp CLManager.cpp main.cpp
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
	-@rm -f monjob.pbs.*
	@echo "run sur nef"
	qsub monjob.pbs

bench: all
	@echo "run 1/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 2/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 3/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 4/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 5/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 6/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 7/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 8/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 9/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "run 10/10"
	-@./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 9830400 -p ./portefeuille.csv -t 1 >> trace.txt
	@echo "fin"

plot: all
	@echo "execution"
	./$(BINDIR)/$(EXECUTABLE) -c 0.99 -n 1000000 -p ./portefeuille.csv -t 1
	@echo "plot en cours..."
	@gnuplot histogram.plot

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
