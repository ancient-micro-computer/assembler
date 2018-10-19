CC		= g++
CPPFLAGS	= -Wall
OBJS	= lib.o logutil.o assembler.o
TARGET	= assembler

all:	$(TARGET)
$(TARGET): $(OBJS)
	$(CC)  -o $(TARGET) $(OBJS)

%.cpp.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
