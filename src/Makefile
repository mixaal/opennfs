CXX=g++


DEFS=  -D_CLOUD_PARTICLE_RADIUS=2.5f \
       -D_CLOUD_PHASE_FUNCTION=1 \
       -D_ANTIALIASING=4 \
       -D_SCREEN_W=800 \
       -D_SCREEN_H=600

CXXFLAGS=-std=c++11 -DSDL_MAIN_HANDLED=1 -flto -march=native -funroll-loops -Wall -ggdb -Ofast -Iinclude/ $(DEFS)
LIBS= -lGLEW -lGL -lGLU -lSDL2 -lSDL2_image

MAIN=main
OBJECTS=main.o scene.o scene_handler.o window.o model.o tools.o light.o material.o skydome.o ephem.o cloud.o billboard.o matrix.o gauss.o field.o

$(MAIN): $(OBJECTS)
	$(CXX) -o $(MAIN) $(OBJECTS) $(LIBS)

clean:
	rm -rf $(OBJECTS) $(MAIN)

