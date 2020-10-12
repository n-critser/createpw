
SRC=createpw
CXXFLAGS=-g -Wall -lpthread

all:
	gcc $(CXXFLAGS)  $(SRC).c  -o $(SRC)




clean:
	rm $(SRC)

