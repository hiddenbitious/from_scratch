CXX=clang++
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
		  bspTree.cpp bspNode.cpp bspHelperFunctions.cpp mesh.cpp \
		  objreader/objfile.cpp tgaLoader/tgaLoader.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=from_scratch

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm *.o metaballs/*.o glsl/glsl.o objreader/objfile.o tgaLoader/tgaLoader.o $(EXECUTABLE)
