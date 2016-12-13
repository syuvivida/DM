import os
import sys
print sys.argv

name=sys.argv[1]
PID=sys.argv[2]
filename='width_'+name+ '_PID' + PID + '.txt'
tempfile='temp_'+filename
os.system('rm -rf '+filename)

dmrange =[1,10,20,50,100,150,500,1000]
medrange=[10,20,50,100,200,300,400,500,600,700,800,900,1000,1100,1200,1300,1400,1500,2000,10000]
for med    in medrange:
    for dm in dmrange:
        tmpMed = med
        if med == 2*dm:
            tmpMed = 2*dm - 5
#        print tmpMed, dm
        dirname = name+'_MSC'+str(tmpMed)+'_MDM'+str(dm)
        print dirname
        command = 'grep -a "DECAY  '+PID+'" '+dirname+'/Events/run_01/run_01_tag_1_banner.txt | awk \'{print "'+str(tmpMed)+' '+str(dm)+'",$3}\' >> '+tempfile
        print command
        os.system(command)
command="awk '{print $1,$2,$3,$3/$1}' "+tempfile + " > " + filename
os.system(command)

