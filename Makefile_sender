
CFLAGS=-std=c++11 -I /usr/local
CC=g++


TARGETS=sender
UTILS=util.o 

all: $(TARGETS)

$(UTILS): %.o: %.cpp %.hpp
	$(CC) $(CFLAGS) -c $<

%.o: %.cpp util.hpp
	$(CC) $(CFLAGS)  -c $< 


$(TARGETS): %:%.o util.o
	$(CC) $(CFLAGS) $^ -o $@



run:
	./sender

.PHONY:	clean

clean:
	rm *.o $(HELPERS) $(TARGETS) 
