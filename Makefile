FLAGS = -g -o
OBJ = addressOps.o

%.o: %.c
	gcc -c %.c

testSuite: testSuite.o $(OBJ)
	gcc $(FLAGS) test testSuite.o $(OBJ)

clean_tests: 
	rm -f test testSuite.o $(OBJ)
