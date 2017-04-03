SRCDIR		= src
OBJDIR		= obj
BINDIR		= bin
INCDIR		= include
OBJS         = roberto.o
EXE			= roberto

CXX         = mpiCC
CXXFLAGS    = -std=c++11 -Wall -g 

$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) -I$(INCDIR)