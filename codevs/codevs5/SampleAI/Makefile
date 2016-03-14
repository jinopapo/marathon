all: JINOPAPOAI

CXX = g++

CXXFLAG = -std=c++11 -Wall

OBJ = Main.o skill.o
OBJ1 = ji_no_papoAI.o state.o

AI: Main.o skill.o
	$(CXX) $(CXXFLAG) -o AI $(OBJ)

JINOPAPOAI: ji_no_papoAI.o state.o
	$(CXX) $(CXXFLAG) -o ji_no_papoAI $(OBJ1)

.cpp .o:
	gcc -c $<
