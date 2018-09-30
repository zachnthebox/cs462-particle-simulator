all: particlemaker

particlemaker: particlemaker.c
	@gcc -o particlemaker particlemaker.c

simulation-omp: particlemaker
	@cd ../src; make particleSimulator-omp particleToPgm particleToPpm
	./simulate-omp

simulation-mpi: particlemaker
	@cd ../src; make particleSimulator-mpi particleToPgm particleToPpm
	./simulate-mpi

simulation-special: particlemaker
	@cd ../src; make particleSimulator-special particleToPgm particleToPpm
	./simulate-special

simulation: particlemaker
	@cd ../src; make particleSimulator-serial particleToPgm particleToPpm
	./simulate

clean: 
	@rm -f particlemaker particle.01.data