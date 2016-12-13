import os
import sys
print sys.argv

PID=sys.argv[1]
name='2HDM'
filename='width_'+name+ '_PID' + PID + '.txt'
tempfile='temp_'+filename
os.system('rm -rf '+filename)

dmrange =[100]
med2range = [300,400,500,600,700,800]
medrange=[600,800,1000,1200,1400,1700,2000,2500]
for med in medrange:
  for med2 in med2range:							
    for dm in dmrange:	
        dirname = name+'_MZp'+str(med)+'_MA0'+str(med2)+'_MDM'+str(dm)
        print dirname
        command = 'grep -a "DECAY  '+PID+'" '+dirname+'/Events/run_01/run_01_tag_1_banner.txt | awk \'{print "'+str(med)+' '+str(med2)+' '+str(dm)+'",$3}\' >> '+tempfile
        print command
        os.system(command)
command="awk '{print $1,$2,$3,$4,$4/$1}' "+tempfile + " > " + filename
os.system(command)

