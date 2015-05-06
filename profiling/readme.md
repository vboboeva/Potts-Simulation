# Profiling #

This is the first profiling of the code.

### computational time bottlenecks (using Gprof) ###

as we can see from the [GPROF OUTPUT](analysis.profiling) for Tneuro = 1
update_stato is called 3935010 times. I tried as I could to profile each line but without success


### Memory Consumption ###

Using valgrind this is the output of the total memory reserved vs size of the simulation (N)

![](memoryvsN.png)
