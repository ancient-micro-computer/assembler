CC=emcc
OBJS=assembler.o lib.o logutil.o
assembler: $(OBJS)
	$(CC) -o assembler.html $(OBJS)

assembler.o: assembler.cpp
	$(CC) -c assembler.cpp

lib.o: lib.cpp
	$(CC) -c lib.cpp

logutil.o: logutil.cpp
	$(CC) -c logutil.cpp

clean:
	rm -f assembler.html assembler.js $(OBJS)