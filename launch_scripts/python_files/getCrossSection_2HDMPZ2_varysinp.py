import os
import sys
print sys.argv


filename='2HDMPZ2_crossSection_MHP750_MA500_MDM1_varysinp.txt'
os.system('rm -rf '+filename)

command = 'echo "sinp Ma crossSection(pb)" >> '+filename
print command
os.system(command)
command = 'echo "===============================" >> '+filename
os.system(command)

medrange = [50,100,200,300,400,490,600]
sinprange = ['0p1','0p3','0p5000','0p7071','0p8660']
for sinp in sinprange:
    for med in medrange:    
        dirname = '2HDMPZ2II_'+sinp+'_MA500_Ma'+str(med)+'_MDM1'
        print dirname
        sinpvalue = sinp.replace("p",".")
        command = 'grep -a pb '+dirname+'/Events/run_01/run_01_tag_1_banner.txt | awk \'{print "'+str(sinpvalue)+' '+str(med)+'",$6}\' >> '+filename
        print command
        os.system(command)
