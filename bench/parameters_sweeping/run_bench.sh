#!/bin/bash

:>bench_times.dat
cd ../

for i in {100..2000..100}
do
    sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/2))/" new_code/params.cfg
    sed -i -e "s/\(^#define N\)[0-9]*.*/\1 $i/" -e "s/\(^#define Cm\)[0-9]*.*/\1 $(($i/2))/" old_code/const_potts.h

    make
    cd new_code
    TNEW=$(./main.x | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')
    cd ..
    cd old_code
    TOLD=$(./master.x | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')
    cd ..

    echo "$i $TNEW $TOLD" >> parameters_sweeping/bench_times.dat
done
