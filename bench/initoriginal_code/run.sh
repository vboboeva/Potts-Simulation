#Stacked histo percentage cycles
rm values.dat
echo "patterns initialization dynamics Trete N" > values.dat

for i in 500 1000 1500 2000
do

    echo $i
    sed -i -e "s/\(^#define N\)[0-9]*.*/\1 $i/" const_potts.h

    ./master.x | awk '/PGEN/{PG=$2} /INIT/{IN=$2} /UTIME/{UT=$2} /ENNE/{EN=$2}END{print PG/(UT/100),IN/(UT/100),UT/(UT/100),EN, '$i' }' >> values.dat

done



# rm values.dat
# echo "patterns initialization dynamics n S" > values.dat
#
# for i in 50 100 400 700
# do
#     echo $i
#     sed -i -e "s/\(^#define Cm\)[0-9]*.*/\1 $i/" const_potts.h
#
#     ./master.x | awk '/PGEN/{PG=$2} /INIT/{IN=$2} /UTIME/{UT=$2} /ENNE/{EN=$2}END{print PG,IN,UT,EN, '$i' }' >> values.dat
#     #./gen.out | awk '/PGEN/{PG=$2} END{print PG,'$i' }' >> values.dat
#
# done
