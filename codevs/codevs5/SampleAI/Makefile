all: JI_NO_PAPOAI

CXX = g++

CXXFLAG = -std=c++11 -Wall

OBJ = Main.o skill.o

JI_NO_PAPOAI: Main.o skill.o
	$(CXX) $(CXXFLAG) -o ji_no_papoAI $(OBJ)

.cpp .o:
	gcc -c $<
