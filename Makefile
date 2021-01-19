FLAGS = -o
OBJ = addressOps.o hashedPageTable.o overflowList.o lruStack.o 2ndchancequeue.o

%.o: %.c
	gcc -g -c $< -o $@

main: main.o $(OBJ) 
	gcc $(FLAGS) main main.o $(OBJ)

clean: 
	rm -f main main.o $(OBJ)
