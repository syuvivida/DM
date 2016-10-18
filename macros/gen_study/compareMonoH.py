import os
import sys
import os.path

for arg in sys.argv: 
    print arg


import ast

directory = sys.argv[1]
model = sys.argv[2]

outputtemp=directory
prefix=outputtemp.rsplit('/', 1)[1]
print prefix


dmrange =[1,10,20,50,100,150,500,1000]
medrange=[10,20,50,100,200,500,1000,2000,10000]
medrange2=[10,20]

## clean up files
for dm in dmrange:
    outfile = model+"_MDM" + str(dm) + ".txt"
    os.system('rm -rf '+outfile)    

for med    in medrange:
    outfile = model+"_MMED" + str(med) + ".txt"
    os.system('rm -rf '+outfile)
    foutmed = open(outfile,"w")
    for dm in dmrange:
        tmpMed = med
        if med == 2*dm:
            tmpMed = 2*dm - 5
        #print tmpMed, dm            
        outdmfile = model+"_MDM" + str(dm) + ".txt"
        foutdm = open(outdmfile,"a")

        filename=directory + "/output/"+prefix+"_" + model +"_MSC" + str(tmpMed) + "_MDM" + str(dm) + ".root"
        if not os.path.exists(filename):
            continue

        label='MDM='+str(dm) 
        foutmed.write(filename+" "+label+"\n")
        label='MMED='+str(tmpMed)
        foutdm.write(filename+" "+label+"\n")
        foutdm.close()
    foutmed.close()

