CXX = g++

GL_PATH   = /usr/lib/x86_64-linux-gnu/mesa/
GLEW_PATH = /usr/lib/x86_64-linux-gnu/
#GLUT_PATH = /usr/lib/x86_64-linux-gnu/

#LDFLAGS=-Wl,-rpath=../oglRenderer
LDFLAGS =
#LIBS=-L$(GLEW_PATH) -L$(GL_PATH) -lm -lGL -lglut -lGLU -lGLEW -L../oglRenderer -lOglRenderer
LIBS   = -L$(GLEW_PATH) -L$(GL_PATH) -lm -lGL -lglut -lGLU -lGLEW -lpthread
CXXFLAGS = -c -Wall -O0 -g -MMD -Wall -Wno-unused-result

SOURCES = main.cpp bbox.cpp metaballs/cubeGrid.cpp quaternion.cpp \
		    math.cpp frustum.cpp vectors.cpp plane.cpp camera.cpp timer.cpp glsl/glsl.cpp \
		    bspTree.cpp bspNode.cpp bspHelperFunctions.cpp mesh.cpp \
		    objreader/objfile.cpp tgaLoader/tgaLoader.cpp \
		    map.cpp tile.cpp actor.cpp input.cpp \
		    battleMap/battleMap.cpp battleMap/battleObject.cpp \
		    battleMap/battleStaticObject.cpp battleMap/battleDynamicObject.cpp \
		    battleMap/battleEnemy.cpp battleMap/battlePlayer.cpp battleMap/battleTile.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = from_scratch
DEPS = $(SOURCES:.cpp=.d)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

-include $(DEPS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o metaballs/*.o glsl/glsl.o objreader/objfile.o tgaLoader/tgaLoader.o battleMap/*.o $(EXECUTABLE)
	rm *.d metaballs/*.d glsl/glsl.d objreader/objfile.d tgaLoader/tgaLoader.d battleMap/*.d
