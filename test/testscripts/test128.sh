#!/bin/sh

#$ -N Particles
#$ -cwd
#$ -S /bin/sh
#$ -pe threads 32


pgmprefix="image"
dataprefix="timestep"
timesteps=300;

threads=128
echo -n > $threads.txt
mkdir /tmp/zberardo-particles
#cd /tmp/zberardo-particles
for((i = 0; i < 5; i++))
do {
/data/cosc462web/zberardo/parallelProg14/src/particleSimulator-omp input bob dt 0.001 numSteps $timesteps output /tmp/zberardo-particles/$dataprefix threads $threads time 1 >> $threads.txt
}
done