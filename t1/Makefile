CC=g++
CFLAGS=-Wall -g -lpthread -MMD -MP
SRCS=$(wildcard *.cpp)
PROGS=$(patsubst %.cpp,%,$(SRCS))
DEPS=$(patsubst %.cpp,%.d,$(SRCS))

all: $(PROGS)

%: %.cpp $(CC) $(CFLAGS) -o $@ $< -include $(DEPS)

clean:
	$(RM) $(PROGS) $(DEPS)