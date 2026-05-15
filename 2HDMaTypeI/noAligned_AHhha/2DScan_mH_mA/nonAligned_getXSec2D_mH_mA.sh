#!/bin/bash
xsecDataFile=test.dat

mh3=(410 460 510 560 610 660 700 750 800)
mh2=(260 300 350 400 450 500 550 )
#mh3=(400)                                                                                     
#mh2=(260)


workdir=/Users/yush/2HDMa/MG5_aMC_v3_5_7/workdir_AHhha
cd $workdir

echo "MA" "MH" "BR(A->Ha)" "BR(H->hh)" "BR(a->chi+chi)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
for seth3 in "${mh3[@]}"
do
    for seth2 in "${mh2[@]}"
    do
	if ((seth3<(seth2+100)))
	then
	    break
	fi
	echo $seth2, $seth3
        cp -p Cards/param_card_paper.dat Cards/param_card.dat
        file=Cards/param_card.dat
        sed -i "" -e "s/MH2/"$seth2"/g" $file
        sed -i "" -e "s/MH3/"$seth3"/g" $file
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
	echo $seth3 $seth2 $decayA $decayH $decaya $xsec $finalXSec >> $xsecDataFile
	olddir=`ls -lrt Events | tail -1 | awk '{print $9}'`
	newdir=nonAlligned_AHhha_13p6TeV_MA_${seth3}_MH_${seth2}_tanbeta2_lambda3_2_sint0p1
	echo $olddir $newdir
	mv Events/$olddir Events/$newdir
	olddir=`ls -lrt Events | grep -a run | tail -1 | awk '{print $9}'`
	mv Events/${olddir}/*txt Events/$newdir/.
	rm -rf Events/${olddir}
	
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
