all : julia_handout.o bitmap.o
    mpicc julia_handout.o bitmap.o -lm -o julia_mpi
 
.PHONY : julia_handout.o
julia_handout.o : julia_handout.c
    mpicc -c julia_handout.c
 
.PHONY : bitmap.o
bitmap.o : bitmap.c
    gcc -c bitmap.c
 
.PHONY : clean
clean :
    rm -f *.o && rm -f julia_mpi julia.bmp result.out
