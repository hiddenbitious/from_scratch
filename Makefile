CC=g++
CFLAGS=-c -Wall -O3

GL_PATH=/usr/lib
GLEW_PATH=/usr/lib
GLUT_PATH=/usr/lib

LDFLAGS=-Wl,-rpath=../oglRenderer
LIBS=-L$(GLEW_PATH) -L$(GL_PATH) -lm -lGL -lglut -lGLU -lGLEW -L../oglRenderer -lOglRenderer

SOURCES=main.cpp quaternion.cpp meshgroup.cpp bbox.cpp metaballs/cubeGrid.cpp trimesh.cpp \
		bsphere.cpp mesh.cpp math.cpp frustum.cpp bspHelperFunctions.cpp vectors.cpp \
		plane.cpp 3dsReader/my_3ds_additions.cpp 3dsReader/3dsReader.cpp \
		bspNode.cpp box.cpp camera.cpp bspTree.cpp timer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=from_scratch

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o 3dsReader/*.o
