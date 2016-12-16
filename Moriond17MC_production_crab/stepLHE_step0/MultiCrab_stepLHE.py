import os

### Preparing the dataset, crab and pset configuration files
def prepare(directory,nevents,njobs):
    if not os.path.exists(directory):
        print 'the directory', directory,'does not exist'
        quit()
    print "Processing the gridpacks in the directory:", directory

    if int(nevents) < 1:
        print "Number of events too small"
        quit()
    elif int(nevents) > 1000:
        print "Number of events too big"
        quit()
    print "Number of events per job is", nevents

    if int(njobs) < 1:
        print "Number of jobs too small"
        quit()
    print "Number of jobs is", njobs
    os.system('./outputText.sh '+directory)
    os.system('./replace_stepLHE.sh '+nevents+' '+njobs)


def submit():
    print "submitting"
    f = open('dataset.txt','r')
    for line in f:
        print line
        a=[]
        b=[]
        a,b = line.split()
        datasetdetail=[a,b]
        print datasetdetail
        name='crab submit -c crabConfig_stepLHE.py General.requestName=stepLHE-'+datasetdetail[1]+' JobType.pyCfgParams=\[\\\'tarball='+datasetdetail[0]+'\\\'\] Data.outputPrimaryDataset='+datasetdetail[1]
        print name
        os.system(name)


def status(crabdirname):
    import os
    os.system ("./Statusall.sh "+crabdirname)
    

## Add a help or usage function here 
def help() :
    print "this is under progress"

    


####################################################################################################################################################
####################################################################################################################################################
## this will control the functions   ##
## convert this to python main.      ##
####################################################################################################################################################
####################################################################################################################################################
import os
import sys
print sys.argv

## safety check
## improve this
if len(sys.argv) < 2 :
    print "insufficient options provided see help function "
    help()
    exit (1)




## prepare jobs
if sys.argv[1] == "prepare" :
    if len(sys.argv) != 5 :
        print "Usuage: python MultiCrab_stepLHE.py prepare dirName nEventsPerJob nJobs"
        exit (1)
    else:
        print "preparing for the submission"
        directory = sys.argv[2]
        nevents = sys.argv[3]
        njobs = sys.argv[4]
        prepare(directory,nevents,njobs)


## submit jobs 
if len(sys.argv) == 2 :
    if sys.argv[1] == "submit" :
        submit()


## check status of jobs 
## send the crab directory 
if len(sys.argv) == 3 : 
    if sys.argv[1] == "status" :
        crabdir = sys.argv[2]
        status(crabdir)




