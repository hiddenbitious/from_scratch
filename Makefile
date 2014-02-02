CC=clang++
CFLAGS=-c -Wall -O0 -g

GL_PATH=/usr/lib/x86_64-linux-gnu/mesa/
GLEW_PATH=/usr/lib/x86_64-linux-gnu/
GLUT_PATH=/usr/lib/x86_64-linux-gnu/

#LDFLAGS=-Wl,-rpath=../oglRenderer
LDFLAGS=
#LIBS=-L$(GLEW_PATH) -L$(GL_PATH) -lm -lGL -lglut -lGLU -lGLEW -L../oglRenderer -lOglRenderer
LIBS=-L$(GLEW_PATH) -L$(GL_PATH) -lm -lGL -lglut -lGLU -lGLEW -lpthread

SOURCES=main.cpp bbox.cpp metaballs/cubeGrid.cpp quaternion.cpp \
		  math.cpp frustum.cpp vectors.cpp plane.cpp camera.cpp timer.cpp glsl/glsl.cpp \
		  bspTree.cpp bspNode.cpp bspHelperFunctions.cpp map.cpp tile.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=from_scratch

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o metaballs/*.o glsl/glsl.o $(EXECUTABLE)
