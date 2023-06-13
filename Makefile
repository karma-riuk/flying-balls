.PHONY: default
default: all

GTK_PACKAGES=gdk-pixbuf-2.0 gtk+-3.0
GTK_CFLAGS=$(shell pkg-config --cflags $(GTK_PACKAGES))
GTK_LIBS=$(shell pkg-config --libs $(GTK_PACKAGES))

# PROFILING_CFLAGS=-pg
CXXFLAGS=-Wall -g -O2 $(PROFILING_CFLAGS) $(GTK_CFLAGS)

LIBS=$(GTK_LIBS) -lm

PROGS=balls
OBJS=balls.o c_index.o game.o gravity.o spaceship.o main.o polygons.o polygon_generator.o collisions.o color.o

# dependencies (gcc -MM *.cc)
balls.o: balls.cc game.h balls.h vec2d.h gravity.h
c_index.o: c_index.cc balls.h vec2d.h game.h c_index.h
game.o: game.cc game.h
gravity.o: gravity.cc gravity.h balls.h vec2d.h game.h
main.o: main.cc game.h balls.h vec2d.h c_index.h gravity.h spaceship.h
spaceship.o: spaceship.cc balls.h vec2d.h game.h
stats.o: stats.cc
polygons.o: polygons.cc polygons.h vec2d.h polygon_generator.h color.h
polygon_generator.o: polygon_generator.cc polygon_generator.h
collisions.o: collisions.cc collisions.h vec2d.h
color.o: color.cc color.h


.PHONY: run
run: balls
	./balls polygons=1

.PHONY: all
all: $(PROGS)

balls: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

.PHONY: clean
clean:
	rm -f *.o $(PROGS) $(OBJS)
