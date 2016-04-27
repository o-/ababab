TARGET = comp
LIBS =
CC = g++
CFLAGS = -O3 --std=c++11 -g -Wall
SUFFX = cpp

.PHONY: default all clean

default: $(TARGET)
all: default

SRCS    = $(wildcard *.$(SUFFX))
OBJECTS = $(patsubst %.$(SUFFX), %.o, $(SRCS))
HEADERS = $(wildcard *.h)
TEST_SRCS  = $(filter-out main.cpp,$(SRCS))
TEST_SRCS += $(wildcard tests/*.$(SUFFX))
TEST_OBJS  = $(patsubst %.$(SUFFX), %.o, $(TEST_SRCS))


%.o: %.$(SUFFX) $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

tests: $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -Wall $(LIBS) -lgtest -o tests/tests

check: tests
	tests/tests

clean:
	-rm -f *.o
	-rm -f tests/*.o
	-rm -f $(TARGET)
	-rm -f tests/tests
