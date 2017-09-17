CFLAGS+= -std=c99
objects = julia_handout.o

myProgram : $(objects)
	mpicc $(objects) -o julia_mpi

.PHONY : clean
clean:
	rm julia_handout.o
