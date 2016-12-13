import os
import sys
print sys.argv

name=sys.argv[1]
filename='crossSection_'+name+'.txt'
os.system('rm -rf '+filename)

dmrange =[100]
med2range = [300,400,500,600,700,800]
medrange=[600,800,1000,1200,1400,1700,2000,2500]
for med in medrange:
  for med2 in med2range:							
    for dm in dmrange:	
        dirname = name+'_MZp'+str(med)+'_MA0'+str(med2)+'_MDM'+str(dm)
        print dirname
        command = 'grep -a pb '+dirname+'/Events/run_01/run_01_tag_1_banner.txt | awk \'{print "'+str(med)+' '+str(med2)+' '+str(dm)+'",$6}\' >> '+filename
        print command
        os.system(command)
