CC		= g++
CPPFLAGS	= -Wall
OBJS	= lib.o logutil.o assembler.o
DEST	= /usr/local/bin
TARGET	= ancientasm

all:	$(TARGET)
$(TARGET): $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS)

%.cpp.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

install:	$(TARGET)
	cp -p $(TARGET) $(DEST)
