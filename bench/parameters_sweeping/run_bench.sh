#!/bin/bash

for j in 2 5 8 10
do
    :>bench_times_$j.dat
    echo "S   C   INIT_NEW   DYN_NEW   TOT_NEW   INIT_OLD   DYN_OLD   TOT_OLD"  >> bench_times_$j.dat
    for i in {100..2000..100}
    do


        sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/$j))/" new_code/params.cfg
        sed -i -e "s/\(^#define N\)[0-9]*.*/\1 $i/" -e "s/\(^#define Cm\)[0-9]*.*/\1 $(($i/$j))/" old_code/const_potts.h

        make
        cd new_code
        TEMP=$(./main.x | grep "TOTAL UPDATE ELAPSED\|INITIALIZATION ELAPSED")
        TINEW=$(echo "$TEMP" | grep "INITIALIZATION ELAPSED" | awk '{print $4}')
        TUNEW=$(echo "$TEMP" | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')

        cd ..
        cd old_code
        TEMP=$(./master.x | grep "TOTAL UPDATE ELAPSED\|INIT1\|INIT2")
        TIOLD=$(echo "$TEMP" | grep "INIT" | awk '{ SUM += $2} END { print SUM }')
        TUOLD=$(echo "$TEMP" | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')

        cd ..

        echo "$i $(($i/$j)) $TINEW $TUNEW $(($TINEW+$TUNEW)) $TIOLD $TUOLD $(($TIOLD+$TUOLD))" >> bench_times_$j.dat

    done
done
