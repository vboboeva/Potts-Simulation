#!/bin/bash



##########################
#Computation profiling
##########################

#Change values
sed 's/<N>/600/g;s/<Trete>/1/g' const_potts.h.template > const_potts.h
make profile
./generator.x
./main.x
gprof -l -A -x main.x gmon.out > analysis.prof
rm -f gmon.out
make clean

##########################
#Memory profiling
##########################

> mem_usage.prof

for i in 500 1000 3000 7000 10000 20000;
do
    echo -e "Doing for $i instances\n"

    #Change values
    sed 's/<N>/'"$i"'/g;s/<Trete>/1/g' const_potts.h.template > const_potts.h

    #Compile with new values
    make all
    ./generator.x

    valgrind --tool=massif --massif-out-file=massif.out --stacks=yes --depth=1 ./main.x
    ms_print massif.out | tail -1 | sed 's/\,//g' | awk '{print "'"$i"' " $3}' >> mem_usage.prof
    rm -f massif.out
    make clean

done
