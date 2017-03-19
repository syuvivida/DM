import os
import sys
print sys.argv


filename='2HDMPZ2_crossSection_MHP750_MA500_MDM1_sinp0p1_varytanbeta.txt'
os.system('rm -rf '+filename)

command = 'echo "tanbeta Ma crossSection(pb)" >> '+filename
print command
os.system(command)
command = 'echo "===============================" >> '+filename
os.system(command)


medrange = [50,100,200,300,400,490,600]
tanbrange = ['0p3','0p5','0p7','1p0','1p5']
for tanb in tanbrange:
    for med in medrange:    
        dirname = '2HDMPZ2II_'+tanb+'_MA500_Ma'+str(med)+'_MDM1'
        print dirname
        tanbvalue = tanb.replace("p",".")
        command = 'grep -a pb '+dirname+'/Events/run_01/run_01_tag_1_banner.txt | awk \'{print "'+str(tanbvalue)+' '+str(med)+'",$6}\' >> '+filename
        print command
        os.system(command)
