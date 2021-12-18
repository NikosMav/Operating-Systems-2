#some name types for readable make file
OB = lib/parent.o lib/aux.o lib/child.o
OBPAR = lib/parent.o lib/aux.o
OBCHILD = lib/child.o lib/aux.o
HEADERS = headers/aux.h
EXECPAR = parent
EXECCHILD = child
COMP = g++
FLAGS = -std=c++11 -g -Wall -c -ggdb3 -lrt -pthread -lcrypto 
LRT = -pthread -lrt -lcrypto
#executable
all: parent child

lib/aux.o: source/aux.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/aux.cpp
	mv aux.o lib/aux.o

lib/parent.o: source/parent.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/parent.cpp
	mv parent.o lib/parent.o

lib/child.o: source/child.cpp $(HEADERS)
	$(COMP) $(FLAGS) source/child.cpp
	mv child.o lib/child.o

parent: $(OBPAR)
		$(COMP) -g $(OBPAR) -o $(EXECPAR) $(LRT)

child: $(OBCHILD)
		$(COMP) -g $(OBCHILD) -o $(EXECCHILD) $(LRT)

#  cleaning command
clean :
	rm -f $(OB) $(EXECPAR) $(EXECCHILD)