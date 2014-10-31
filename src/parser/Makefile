CXX=g++

CXXFLAGS=-std=c++11 -DSDL_MAIN_HANDLED=1 -flto -march=native -funroll-loops -Wall -ggdb -fsanitize=address #-Ofast
LIBS= -lpng -lasan

MAIN=main
OBJECTS=main.o nfs_path.o fam.o nfs_io.o bitmap.o

$(MAIN): $(OBJECTS)
	$(CXX) -o $(MAIN) $(OBJECTS) $(LIBS)

clean:
	rm -rf $(OBJECTS) $(MAIN) *mtl *obj *png

