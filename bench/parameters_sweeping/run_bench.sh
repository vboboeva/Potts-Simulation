#!/bin/bash

# {500..1500..500}

for i in 1500
do
    :>bench_times_$i.dat
    echo "N   C   INIT_HC   DYN_HC   TOT_HC   INIT_LC   DYN_LC   TOT_LC   INIT_VLC   DYN_VLC   TOT_VLC   INIT_OLD   DYN_OLD   TOT_OLD"  >> bench_times_$i.dat

    for j in 2 3 4 5 7 8 10 13 17 21 27 34 41 55
    do

        sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/$j))/" lc/params.cfg
        sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/$j))/" vlc/params.cfg
        sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/$j))/" hc/params.cfg
        sed -i -e "s/\(^#define N\)[0-9]*.*/\1 $i/" -e "s/\(^#define Cm\)[0-9]*.*/\1 $(($i/$j))/" old_code/const_potts.h

        make
        cd hc
        TEMP=$(./main.x | grep "TOTAL UPDATE ELAPSED\|INITIALIZATION ELAPSED")
        TIHC=$(echo "$TEMP" | grep "INITIALIZATION ELAPSED" | awk '{print $4}')
        TUHC=$(echo "$TEMP" | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')

        cd ..
        cd lc
        TEMP=$(./main.x | grep "TOTAL UPDATE ELAPSED\|INITIALIZATION ELAPSED")
        TILC=$(echo "$TEMP" | grep "INITIALIZATION ELAPSED" | awk '{print $4}')
        TULC=$(echo "$TEMP" | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')

        cd ..
        cd vlc
        TEMP=$(./main.x | grep "TOTAL UPDATE ELAPSED\|INITIALIZATION ELAPSED")
        TIVLC=$(echo "$TEMP" | grep "INITIALIZATION ELAPSED" | awk '{print $4}')
        TUVLC=$(echo "$TEMP" | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')

        cd ..
        cd old_code
        TEMP=$(./master.x | grep "TOTAL UPDATE ELAPSED\|INIT1\|INIT2")
        TIOLD=$(echo "$TEMP" | grep "INIT" | awk '{ SUM += $2} END { print SUM }')
        TUOLD=$(echo "$TEMP" | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')

        cd ..

        echo "$i $(($i/$j)) $TIHC $TUHC $(($TIHC+$TUHC)) $TILC $TULC $(($TILC+$TULC)) $TIVLC $TUVLC $(($TIVLC+$TUVLC)) $TIOLD $TUOLD $(($TIOLD+$TUOLD))" >> bench_times_$i.dat

    done
done
