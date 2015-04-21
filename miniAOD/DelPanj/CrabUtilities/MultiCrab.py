import os

f = open('datasetdetails.txt','r')
for line in f:
    print line
    a=[]
    b=[]
    c=[]
    d=[]
    a,b,c,d = line.split()
    datasetdetail=[a,b,c,d]
    print datasetdetail
    os.system('crab submit General.requestName='+datasetdetail[0]+' JobType.psetName='+datasetdetail[1]+' Data.inputDataset='+datasetdetail[2]+' Data.unitsPerJob='+datasetdetail[3])
    #name =  'crab submit General.requestName='+datasetdetail[0]+' JobType.psetName='+datasetdetail[1]+' Data.inputDataset='+datasetdetail[2]+' Data.unitsPerJob='+datasetdetail[3]
    #print name 


