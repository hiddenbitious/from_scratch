CXX           = g++
CC            = gcc

GL_PATH       = /usr/lib/x86_64-linux-gnu/mesa/
GLEW_PATH     = /usr/lib/x86_64-linux-gnu/

INCLUDES      = -ISFML-2.2/include/
PROGRAM       = from_scratch
REDUCE_ERRORS = -Wno-unused-parameter -Wno-unused-function \
					 -Wno-variadic-macros -Wno-long-long -Wno-error=vla -Wno-vla -Wno-error=unused-result
PEDANTIC      = -Wfloat-equal -Wall -pedantic -Wextra -Wfloat-equal -Werror -Winline -Wno-unused-result
CXXFLAGS      = -c -MMD -MP $(REDUCE_ERRORS) $(PEDANTIC)
CFLAGS        = $(CXXFLAGS) -std=c99
LDFLAGS       = -L$(GLEW_PATH) -L$(GL_PATH) -LSFML-2.2/lib/
LIBS          = -lm -lGL -lglut -lGLU -lGLEW -lpthread -lsfml-audio

.PHONY: Release
Release: CXXFLAGS += -O3
Release: CFLAGS += -O3
Release: LDFLAGS += -O3
Release: all

.PHONY: Profile
Profile: CXXFLAGS += -pg
Profile: CFLAGS += -pg
Profile: LDFLAGS += -pg
Profile: all

.PHONY: Debug
Debug: CXXFLAGS += -g -O0
Debug: CFLAGS += -g -O0
Debug: LDFLAGS += -g -O0
Debug: all

SOURCES = main.cpp bbox.cpp metaballs/cubeGrid.cpp quaternion.cpp \
		    math.cpp frustum.cpp vectors.cpp plane.cpp camera.cpp timer.cpp glsl/glsl.cpp \
		    bspTree.cpp bspNode.cpp bspHelperFunctions.cpp mesh.cpp \
		    objreader/objfile.cpp tgaLoader/tgaLoader.cpp \
		    map.cpp tile.cpp actor.cpp input.cpp \
		    battleMap/battleMap.cpp battleMap/battleObject.cpp \
		    battleMap/battleStaticObject.cpp battleMap/battleDynamicObject.cpp \
		    battleMap/battleEnemy.cpp battleMap/battlePlayer.cpp battleMap/battleTile.cpp

OBJECTS_CPP = $(SOURCES:.cpp=.o)
OBJECTS = $(OBJECTS_CPP:.c=.o)

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)

-include $(OBJECTS:.o=.d)

.cpp.o:
	$(CXX) $(INCLUDES) $(CXXFLAGS) $< -o $@

.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm *.o metaballs/*.o glsl/glsl.o objreader/objfile.o tgaLoader/tgaLoader.o battleMap/*.o $(PROGRAM)
	rm *.d metaballs/*.d glsl/glsl.d objreader/objfile.d tgaLoader/tgaLoader.d battleMap/*.d
