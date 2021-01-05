FLAGS = -g -o
OBJ = addressOps.o

%.o: %.c
	gcc -c $< -o $@

tests/%.o: tests/%.c
	gcc -c $< -o $@

testSuite: tests/testSuite.o $(OBJ)
	gcc $(FLAGS) test tests/testSuite.o $(OBJ)

main: main.o 
	gcc $(FLAGS) main main.o

clean: 
	rm -f main main.o

clean_tests: 
	rm -f test tests/testSuite.o $(OBJ)
