EXECS=julia_mpi
MPICC?=mpicc

all: ${EXECS}

julia_mpi: julia_handout.c
	${MPICC} -o julia_mpi julia_handout.c

clean:
	rm ${EXECS}
