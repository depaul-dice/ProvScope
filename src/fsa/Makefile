EXEC = fsa
CC = g++
OBJ = fsa.o
DEPS = fsa.h

object: $(OBJ)

$(EXEC): $(OBJ)
	$(CC) test.cc $(OBJ) -Wall -o $(EXEC)

debug: fsa.cc test.cc
	$(CC) $^ -Wall -g -o debug

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< -Wall 

.PHONY: clean
clean:
	rm -f $(EXEC) $(OBJ) debug
