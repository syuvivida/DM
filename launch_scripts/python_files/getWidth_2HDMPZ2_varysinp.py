import os
import sys
print sys.argv


higgsName = ['h1','h2','h3','hc','h4']
higgsPDG = [25, 35, 36, 37, 55]
medrange = [50,100,200,300,400,490,600]
sinprange = ['0p1','0p3','0p5000','0p7071','0p8660']

for name, pdg in zip(higgsName, higgsPDG):

    filename='2HDMPZ2_'+name+'width_MHP750_MA500_MDM1_varysinp.txt'
    os.system('rm -rf '+filename)

    command = 'echo "sinp Ma Width(GeV)" >> '+filename
    os.system(command)
    command = 'echo "===============================" >> '+filename
    os.system(command)


    for sinp in sinprange:
        for med in medrange:    
            dirname = '2HDMPZ2II_'+sinp+'_MA500_Ma'+str(med)+'_MDM1'
            print dirname
            sinpvalue = sinp.replace("p",".")
            command = 'grep -a "DECAY  '+str(pdg)+'" '+dirname+'/Events/run_01/run_01_tag_1_banner.txt | awk \'{print "'+str(sinpvalue)+' '+str(med)+'",$3}\' >> '+filename
            print command
            os.system(command)
