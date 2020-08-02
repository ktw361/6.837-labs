INCFLAGS  = -I /usr/include/GL
INCFLAGS += -I ../vecmath/include

LINKFLAGS = -lglut -lGL -lGLU
LINKFLAGS += -L ../vecmath/lib -lvecmath

CFLAGS    = -Wall -ansi -DSOLN
DEBUG 	 ?= 1
ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g -DDEBUG
else
	CFLAGS += -O2
endif
CC        = g++
SRCS      = main.cpp parse.cpp curve.cpp surf.cpp camera.cpp
OBJS      = $(SRCS:.cpp=.o)
PROG      = a1

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm $(OBJS) $(PROG)
