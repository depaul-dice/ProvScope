EXEC = provScope
CC = g++
OBJ = funcTrace.o readFiles.o cfg.o Args.o tools.o Matrix.o Comparison.o ec.o subgraph.o # fsa/fsa.o regexGraph.o regexNode.o
DEPS = funcTrace.h readFiles.h cfg.h ec.h Args.h tools.h Matrix.h Comparison.h subgraph.h # fsa/fsa.h regexGraph.h regexNode.h

$(EXEC): provScope.cpp $(OBJ)  
	$(CC) -o $@ $^ -Wall -std=c++14 -lssl -lcrypto -I/usr/include/jsoncpp -ljsoncpp

debug: provScope.cpp funcTrace.cpp readFiles.cpp cfg.cpp Args.cpp tools.cpp Matrix.cpp Comparison.cpp ec.cpp subgraph.cpp # regexGraph.cpp regexNode.cpp
	$(CC) -o $@ $^ -Wall -std=c++14 -ggdb -lssl -lcrypto 

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< -Wall -std=c++14 
	
.PHONY: clean
clean:
	rm -f $(EXEC) debug $(OBJ) *.json
