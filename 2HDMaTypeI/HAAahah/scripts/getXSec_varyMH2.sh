#!/bin/bash
xsecDataFile=test.dat


mh2=(470 500 550 600 650 700 750 800)
#mh2=(600)

echo "MH" "BR(H->AA)" "BR(A->ah)" "BR(a->chi chi)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
for l3 in "${mh2[@]}"
do
    setl3=$l3
    echo $setl3
	
    cp -p Cards/param_card_paper.dat Cards/param_card.dat
    file=Cards/param_card.dat
    sed -i "" -e "s/MH2/"$setl3"/g" $file
    ./bin/generate_events -f
    banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
    dir=`ls -lrt Events | tail -1 | awk '{print $9}'` 
    echo $banner
    xsec=`grep -a Integrated $banner | awk '{print $6}'`
    echo $xsec
    decayH=`grep -a "2    36  36" $banner | awk '{print $1}'`
    echo $decayH
    decayA=`grep -a "2    25  55" $banner | awk '{print $1}'`
    echo $decayA
    decaya=`grep -A 2 "DECAY  55" $banner | grep -a "52  -52" | awk '{print $1}'`
    echo $decaya
    finalXSec=`awk -v a=$xsec -v b=$decayH -v c=$decayA -v d=$decaya 'BEGIN{print (a*b*c*c*d*d)}'`
    echo $finalXSec
    echo $setl3 $decayH $decayA $decaya $xsec $finalXSec >> $xsecDataFile
    newdir=run_13TeV_MH2_${setl3}_tanbeta5_lambda3_2_sinp0p2
    olddir=${dir}_decayed_1
    echo $olddir $newdir
    mv Events/$olddir Events/$newdir
done

updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
