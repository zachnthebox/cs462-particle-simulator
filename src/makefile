VPATH=includes

all: particleSimulator-serial particleToPgm particleSimulator-omp particleSimulator-mpi particleSimulator-special particleToPpm

particleSimulator-omp: parseFlags.o particleSimulator-omp.c
	@gcc -o particleSimulator-omp particleSimulator-omp.c parseFlags.o -lm -fopenmp -Iincludes

particleSimulator-serial: parseFlags.o particleSimulator-serial.c
	@gcc -o particleSimulator-serial particleSimulator-serial.c parseFlags.o -lm -fopenmp -Iincludes

particleSimulator-special: parseFlags.o particleSimulator-special.c
	@gcc -o particleSimulator-special particleSimulator-special.c parseFlags.o -lm -fopenmp -Iincludes

particleSimulator-mpi: parseFlags.o particleSimulator-mpi.c
	@mpicc -openmp -o particleSimulator-mpi particleSimulator-mpi.c parseFlags.o -limf -lm -Iincludes 

particleToPgm: particleToPgm.c parseFlags.o
	@gcc -o particleToPgm particleToPgm.c parseFlags.o -Iincludes -lm

particleToPpm: particleToPpm.c parseFlags.o
	@gcc -o particleToPpm particleToPpm.c parseFlags.o -Iincludes -lm

parseFlags.o: parseFlags.c parseFlags.h
	@gcc -c parseFlags.c -Iincludes

clean: 
	@rm -f particleSimulator-omp particleSimulator-serial parseFlags.o particleToPgm particleSimulator-mpi particleSimulator-special particleToPpm