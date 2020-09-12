INCFLAGS  = -I ../vecmath/include
INCFLAGS += -I /usr/include/GL

LINKFLAGS = -L. -lRK4 -lglut -lGL -lGLU
CFLAGS    = -Wall -ansi
DEBUG 	 ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g -DDEBUG
else
	CFLAGS += -O2
endif
CC        = g++
SRCS      = $(wildcard *.cpp)
SRCS     += $(wildcard vecmath/src/*.cpp)
OBJS      = $(SRCS:.cpp=.o)
PROG      = a3

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)
