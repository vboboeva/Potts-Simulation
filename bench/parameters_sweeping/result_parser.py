#!/usr/bin/python

import numpy as np
from scipy import stats

def print_array(a,filename):
    out_file = open(filename,"w")
    for rows in a:
        for values in rows:
            out_file.write(str(values)+" ")
        out_file.write("\n")

    out_file.close()


folders = ['1000','1500','2000']
connections = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 20, 28, 40, 58]

finalarray={'1000':[],'1500':[],'2000':[]}

for dir in folders:

    for conn in connections:
        in_file = open("results/"+ dir + "/bench_times_"+str(conn)+".dat","r")
        conndata = []
        for line in in_file:
            conndata.append(line.split())

        finalrow=[]
        for columns in range(0,14):

            column = [float(row[columns]) for row in conndata]
            mean = np.mean(column)
            finalrow.append(mean)

            if(columns > 1):
                ermin=min(column)
                ermax=max(column)

                finalrow.append(ermin)
                finalrow.append(ermax)


        finalarray[dir].append(finalrow)
        in_file.close()

    print_array(finalarray[dir],dir+".txt")

print(len(finalarray['1000'][0]))

#np.savetxt("1000.txt",finalarray['1000'][0],delimiter=' ')
