CC=g++
CFLAGS=-c -Wall -O3

GL_PATH=/usr/lib/x86_64-linux-gnu/mesa/
GLEW_PATH=/usr/lib/x86_64-linux-gnu/
GLUT_PATH=/usr/lib/x86_64-linux-gnu/

LDFLAGS=-Wl,-rpath=../oglRenderer
LIBS=-L$(GLEW_PATH) -L$(GL_PATH) -lm -lGL -lglut -lGLU -lGLEW -L../oglRenderer -lOglRenderer

SOURCES=main.cpp bbox.cpp metaballs/cubeGrid.cpp quaternion.cpp \
		math.cpp frustum.cpp vectors.cpp plane.cpp camera.cpp timer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=from_scratch

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o metaballs/*.o $(EXECUTABLE)
