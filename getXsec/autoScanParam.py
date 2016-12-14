#! /usr/bin/env python
#-------------------------------------------------------------
# File: scanParam.py
# Created: 12 July 2016 Fang-Ying Tsai
# original script: https://github.com/syuvivida/DM/blob/v0.04/getXsec/scanParam.py
# Modified: 12 Nov 2016 Shu-Xiao Liu
#-------------------------------------------------------------  

import sys
import os
import glob
import fileinput
import subprocess


fileName = 'param_card.dat'
paramCard = glob.glob(fileName)


class cd:
    """Context manager for changing the current working directory"""
    def __init__(self, newPath):
        self.newPath = os.path.expanduser(newPath)

    def __enter__(self):
        self.savedPath = os.getcwd()
        os.chdir(self.newPath)

    def __exit__(self, etype, value, traceback):
        os.chdir(self.savedPath)

def replaceMZp(searchExp,replaceExp):
    for line in fileinput.input(fileName, inplace=1):
    	if line.lower().find('mzp')>0 and line.find('32')>0:
            line = line.replace(searchExp,replaceExp)
        sys.stdout.write(line)

def replaceMA0(searchExp,replaceExp):
    for line in fileinput.input(fileName, inplace=1):
    	if line.lower().find('ma0')>0 and line.find('28')>0:
            line = line.replace(searchExp,replaceExp)
 	if line.lower().find("mhn")>0 and line.find("26")>0:
	    line = line.replace(searchExp,replaceExp)
	if line.lower().find("mhp")>0 and line.find("27")>0:
	    line = line.replace(searchExp,replaceExp)
        sys.stdout.write(line)

'''def replaceMDM(searchExp,replaceExp):
    for line in fileinput.input(fileName, inplace=1):
    	if line.lower().find('mx')>0 and line.find('1000022')>0:
            line = line.replace(searchExp,replaceExp)
        sys.stdout.write(line)
'''

def getA0AutoWidth():
    for line in fileinput.input(paramCard, inplace=1):
        if line.find("ECAY  28")>0:
            A0Width = line.split("28 ")[1].split(" #")[0]
            line = line.replace(A0Width,"Auto")
        sys.stdout.write(line)

def getZpAutoWidth():
    for line in fileinput.input(paramCard, inplace=1):
        if line.find("ECAY  32")>0:
            ZpWidth = line.split("32 ")[1].split(" #")[0]
            line = line.replace(ZpWidth,"Auto")
        sys.stdout.write(line)

def gethAutoWidth():
    for line in fileinput.input(paramCard, inplace=1):
        if line.find("ECAY  25")>0:
            hWidth = line.split("25 ")[1].split(" #")[0]
            line = line.replace(hWidth,"Auto")
        sys.stdout.write(line)

def generateEvent():
    s = subprocess.Popen('./bin/generate_events -f', shell=True)
    s.wait()
    return s.returncode


'''def nDir(path):
	ndir = 0
	path0X = '../Events/run_0' 
	pathXX = '../Events/run_'
	for dirpath, dirname, filename in os.walk(path):
		ndir += len(dirname)
	
	if os.path.isdir(".git"):
		## ndir -= 5
		## if os.path.isdir('.git/logs'): ndir -= 1
		for dirpath, dirname, filename in os.walk('.git'): ndir -= len(dirname)
	if ndir < 10: testDir = path0X + str(ndir)
	else: testDir = pathXX + str(ndir)
	## guarantee code
	while not os.path.isdir(testDir):		##path):
		ndir -= 1
		if ndir < 10: testDir = path0X + str(ndir)
		else: testDir = pathXX + str(ndir)
		if ndir <= 0: break
	

	return ndir	'''

def nDir(path):
        nRun = 1
        outputfile = path + '/run_01'
	while os.path.exists(outputfile) == True:
                nRun += 1
                if nRun < 10: outputfile = path + '/run_0' + str(nRun)
                else: outputfile = path + '/run_' + str(nRun)
        nRun -= 1
        return nRun

def main():
    
    if not os.path.exists('../Bannerfile'):
        os.makedirs('../Bannerfile')
    errorinfo = []
    MzpList=["1.000000e+03","6.000000e+02","8.000000e+02","2.500000e+03","1.200000e+03","1.400000e+03","1.700000e+03","2.000000e+03","1.000000e+03"]
    MA0List=["3.000000e+02","4.000000e+02","5.000000e+02","6.000000e+02","7.000000e+02","8.000000e+02","3.000000e+02"]
    for i in range(len(MzpList)-1):
        replaceMZp(MzpList[i],MzpList[i+1])
        for j in range(len(MA0List)-1):
            getZpAutoWidth()
            getA0AutoWidth()
            replaceMA0(MA0List[j],MA0List[j+1]) 
            ## get the path of output directory
            Noutput = nDir('../Events')
            if Noutput+1 < 10: outputDir = 'Events/run_0' + str(Noutput+1)
            else: outputDir = 'Events/run_' + str(Noutput+1)
            with cd(".."):
                ## it tries 3 tims if it do not generate output directory
                for Runtime in range(3):  
                    print "Mass point\tMZp:" + MzpList[i+1] + "\tMA0:" + MA0List[j+1]
                    generateEvent()
                    ## check the output file exists or not and record the mass point when error occurs
                    if os.path.exists(outputDir): break
		    else: errorinfo.append('MZp:' + MzpList[i+1] + '\tMA0:' + MA0List[j+1])
                ## collect Banner files to new directory
                if Noutput+1 < 10: os.system("cp Events/run_0{}/run*.txt Bannerfile".format(str(Noutput+1)))
                else: os.system("cp Events/run_{}/run*.txt Bannerfile".format(str(Noutput+1))) 	
    
    print 'mass point of failed run'
    for error in range(len(errorinfo)):
        print errorinfo[error]
    
    os.chdir('../Bannerfile')
    #os.system('wget https://raw.githubusercontent.com/syuvivida/DM/v0.04/getXsec/getXsecTable.py')
    os.system('cp ../Cards/getXsecTable.py .')
    os.system('chmod 755 getXsecTable.py')
    print 'making figure'
    os.system('python getXsecTable.py')
    print 'copy path'
    print os.getcwd()+'/*.root'

if __name__ == "__main__":
   main()

