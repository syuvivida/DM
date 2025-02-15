#!/bin/bash
xsecDataFile=test.dat

mh2=(470 500 550 600 650 700 750 800)                                                                                     
mh3=(230 250 300 350 400 450 500 550 600 650 700)                                                                                             


workdir=/afs/cern.ch/work/s/syu/HHMET_study/MG5_aMC_v3_5_7/simpleTest
cd $workdir

echo "MH" "MA" "BR(H->AA)" "BR(A->ah)" "BR(a->chi+chi)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
for h2 in "${mh2[@]}"
do
    seth2=$h2
    for h3 in "${mh3[@]}"
    do
        seth3=$h3
	if ((h3>=h2/2))
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
	echo $seth2 $seth3 $decayH $decayA $decaya $xsec $finalXSec >> $xsecDataFile
	olddir=${dir}_decayed_1
	newdir=resonant_HAAah_13p6TeV_MA_${seth3}_MH2_${seth2}_tanbeta5_lambda3_2
	echo $olddir $newdir
	mv Events/$olddir Events/$newdir
	olddir=${dir}
#	gzip Events/${olddir}/unweighted_events.lhe
	mv Events/${olddir}/*txt Events/$newdir/.
#	newdir=run_13p6TeV_MA_${seth3}_MH2_${seth2}_tanbeta5_lambda3_2
	#	mv Events/$olddir Events/$newdir
	rm -rf Events/${olddir}
	
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
