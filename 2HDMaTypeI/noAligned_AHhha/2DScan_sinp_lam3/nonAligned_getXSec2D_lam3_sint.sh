#!/bin/bash
xsecDataFile=test.dat
bound=`echo "sqrt(2)/2" | bc -l`
echo $bound
sinp=(0.1 0.2 0.3 0.5 $bound)
lam3=(2 3 4 6 8)

#sinp=(0.1)
#lam3=(2)



workdir=/Users/yush/2HDMa/MG5_aMC_v3_5_7/workdir_AHhha
cd $workdir

echo "sinp" "lambda3" "BR(A->Ha)" "BR(H->hh)" "BR(a->chi+chi)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
for setsinp in "${sinp[@]}"
do
    for setl3 in "${lam3[@]}"
    do
	echo $setsinp, $setl3
        cp -p Cards/param_card_paper.dat Cards/param_card.dat
        file=Cards/param_card.dat
        sed -i "" -e "s/SINP/"$setsinp"/g" $file
        sed -i "" -e "s/LAM3/"$setl3"/g" $file
        ./bin/generate_events -f
	banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	echo $banner
	xsec=`grep -a Integrated $banner | awk '{print $6}'`
	echo $xsec
	decayA=`grep -a "2    35  55" $banner | awk '{print $1}'`
	echo $decayA
	decayH=`grep -a "2    25  25" $banner | awk '{print $1}'`
	echo $decayH
	decaya=`grep -A 2 "DECAY  55" $banner | grep -a "52  -52" | awk '{print $1}'`
	echo $decaya
	finalXSec=`awk -v a=$xsec -v b=$decayA -v c=$decayH -v d=$decaya 'BEGIN{print (a*b*c*d)}'`
	echo $finalXSec
	echo $setsinp $setl3 $decayA $decayH $decaya $xsec $finalXSec >> $xsecDataFile
	roundsinp=`printf "%.1f" $setsinp`
	roundsinp=${roundsinp/\./p} 
	olddir=`ls -lrt Events | tail -1 | awk '{print $9}'`
	newdir=nonAlligned_AHhha_13p6TeV_MA410_MH260_tanbeta2_lambda3_${setl3}_sint${roundsinp}
	echo $olddir $newdir
	mv Events/$olddir Events/$newdir
	olddir=`ls -lrt Events | grep -a run | tail -1 | awk '{print $9}'`
	mv Events/${olddir}/*txt Events/$newdir/.
	rm -rf Events/${olddir}
	
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
