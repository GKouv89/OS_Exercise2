FLAGS = -o
OBJ = addressOps.o hashedPageTable.o overflowList.o lruStack.o 2ndchancequeue.o

%.o: %.c
	gcc -g -c $< -o $@

tests/%.o: tests/%.c
	gcc -g -c $< -o $@

testSuite: tests/testSuite.o $(OBJ)
	gcc $(FLAGS) test tests/testSuite.o $(OBJ)

main: main.o $(OBJ) 
	gcc $(FLAGS) main main.o $(OBJ)

clean: 
	rm -f main main.o $(OBJ)

clean_tests: 
	rm -f test tests/testSuite.o $(OBJ)
