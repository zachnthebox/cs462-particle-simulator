#!/usr/bin/gnuplot

# Gnuplot script file for plotting data in file "timing.txt"
# This file is called   plotScript.gnu
# tutorials at http://www.duke.edu/~hpgavin/gnuplot.html
# http://linux.byexamples.com/archives/487/plot-your-graphs-with-command-line-gnuplot/

set terminal png
#set yrange auto
#set yrange [ 0.001 : 10 ]
set log x
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Timing test: 256/N Threads simulating 10,000 particles"
set xlabel "Number N threads"
set ylabel "Time (sec)"
set style data linespoints
set output 'timingPlot.png'
plot  "trail.0.txt" using 1:2  title "trial 1",\
			"trail.1.txt" using 1:2  title "trial 2",\
			"trail.2.txt" using 1:2  title "trial 3",\
			"trail.3.txt" using 1:2  title "trial 4",\
			"trail.4.txt" using 1:2  title "trial 5"
			
set title "Timing test: 16/N Processes with 16 threads each simulating 10,000 particles"
set output 'timingmpiPlot.png
set xlabel "Number N Processes with 16 threads each"
unset log x
plot  "mpitrail.0.txt" using 1:2  title "trial 1",\
			"mpitrail.1.txt" using 1:2  title "trial 2",\
			"mpitrail.2.txt" using 1:2  title "trial 3",\
			"mpitrail.3.txt" using 1:2  title "trial 4",\
			"mpitrail.4.txt" using 1:2  title "trial 5"