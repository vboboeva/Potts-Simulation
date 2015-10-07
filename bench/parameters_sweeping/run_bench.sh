#!/bin/bash

function run()
{
    dir=$1
    exe=$2
    pushd $dir || return
    $exe | awk '
        /INITIALIZATION ELAPSED/{TI=$4}
        /TOTAL UPDATE ELAPSED/{TU=$5}
        END{print TI,TU,TI+TU}
    '
    popd
}

# {500..1500..500}
#2 3 4 5 6 7 8 9 10 12 14 16 18 20 24 28 34 40 48 58
for i in 1000 1500 2000
do
    :>bench_times_$i.dat
    echo "N   C   INIT_HC   DYN_HC   TOT_HC   INIT_LC   DYN_LC   TOT_LC   INIT_VLC   DYN_VLC   TOT_VLC   INIT_OLD   DYN_OLD   TOT_OLD"  >> bench_times_$i.dat

    for j in 2 3 4 5 6 7 8 9 10 12 14 16 18 20 24 28 34 40 48 58
    do

        sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/$j))/" lc/params.cfg
        sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/$j))/" vlc/params.cfg
        sed -i -e "s/\(N *= *\).*/\1$i/" -e "s/\(C *= *\).*/\1$(($i/$j))/" hc/params.cfg
        sed -i -e "s/\(^#define N\)[0-9]*.*/\1 $i/" -e "s/\(^#define Cm\)[0-9]*.*/\1 $(($i/$j))/" old_code/const_potts.h

        make

        cd old_code
        TEMP=$(./master.x | grep "TOTAL UPDATE ELAPSED\|INIT1\|INIT2")
        TIOLD=$(echo "$TEMP" | grep "INIT" | awk '{ SUM += $2} END { print SUM }')
        TUOLD=$(echo "$TEMP" | grep "TOTAL UPDATE ELAPSED" | awk '{print $5}')

        HC="$(run hc ./main.x)"
        LC="$(run lc ./main.x)"
        VLC="$(run vlc ./main.x)"

        echo "$i $(($i/$j)) $HC $LC $VLC $TIOLD $TUOLD $(($TIOLD+$TUOLD))" # >> bench_times_$i.dat

    done
done
