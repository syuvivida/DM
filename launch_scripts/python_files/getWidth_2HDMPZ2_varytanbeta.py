import os
import sys
print sys.argv


higgsName = ['h1','h2','h3','hc','h4']
higgsPDG = [25, 35, 36, 37, 55]
medrange = [50,100,200,300,400,490,600]
tanbrange = ['0p3','0p5','0p7','1p0','1p5']

for name, pdg in zip(higgsName, higgsPDG):

    filename='2HDMPZ2_'+name+'width_MHP750_MA500_MDM1_sinp0p1_varytanbeta.txt'
    os.system('rm -rf '+filename)

    command = 'echo "tanb Ma Width(GeV)" >> '+filename
    os.system(command)
    command = 'echo "===============================" >> '+filename
    os.system(command)


    for tanb in tanbrange:
        for med in medrange:    
            dirname = '2HDMPZ2II_'+tanb+'_MA500_Ma'+str(med)+'_MDM1'
            print dirname
            tanbvalue = tanb.replace("p",".")
            command = 'grep -a "DECAY  '+str(pdg)+'" '+dirname+'/Events/run_01/run_01_tag_1_banner.txt | awk \'{print "'+str(tanbvalue)+' '+str(med)+'",$3}\' >> '+filename
            print command
            os.system(command)
