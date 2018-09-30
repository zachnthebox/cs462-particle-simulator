VPATH=src

all:
	@cd src; make 
	@cd test; make

simulation-omp:
	@cd test; make simulation-omp

simulation-mpi:
	@cd test; make simulation-mpi

simulation-special:
	@cd test; make simulation-special

simulation:
	@cd test; make simulation

clean:
	@cd src; make clean
	@cd test; make clean
	@rm -f particles.01.data Particles.*

release: clean 
	@rm -f particles.tgz
	@cd ..; tar cvfz particles.tgz parallelProg14; mv particles.tgz parallelProg14/