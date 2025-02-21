#!/bin/bash
xsecDataFile=test.dat

mh2=(600 1000)
mh4=(100 150 200 250 300 350 400 450)
mdm=(100 150 200 250 300 350 400 450 500)
workdir=/afs/cern.ch/work/s/syu/HHMET_study/MG5_aMC_v3_5_7/type2_visible_h3
cd $workdir

echo "MA" "BR(A->ah)" "BR(a->b+b~)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
for h2 in "${mh2[@]}"
do
    seth2=$h2
    for h4 in "${mh4[@]}"
    do
        seth4=$h4
	for dm in "${mdm[@]}"
	do	    
            setdm=$dm
	    if ((dm<=h4/2))
	    then
		continue
            fi
	    echo $seth2, $seth4, $setdm
            cp -p Cards/param_card_paper.dat Cards/param_card.dat
            file=Cards/param_card.dat
            sed -i "" -e "s/MH2/"$seth2"/g" $file
            sed -i "" -e "s/MH4/"$seth4"/g" $file
            sed -i "" -e "s/MDM/"$setdm"/g" $file
            ./bin/generate_events -f
	    banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	    dir=`ls -lrt Events | tail -1 | awk '{print $9}'` 
	    echo $banner
	    xsec=`grep -a Integrated $banner | awk '{print $6}'`
	    echo $xsec
	    decayA=`grep -a "2    25  55" $banner | awk '{print $1}'`
	    echo $decayA
	    decaya=`grep -A 3 "DECAY  55" $banner | grep -a "5  -5" | awk '{print $1}'`
	    echo $decaya
	    finalXSec=`awk -v a=$xsec -v c=$decayA -v d=$decaya 'BEGIN{print (a*c*d)}'`
	    echo $finalXSec
	    echo $seth2 $seth4 $setdm $decayA $decaya $xsec $finalXSec >> $xsecDataFile
	    olddir=${dir}_decayed_1
	    newdir=resonant_Aah_abb_13p6TeV_mdm_${setdm}_MA_${seth2}_Ma_${seth4}
	    echo $olddir $newdir
	    mv Events/$olddir Events/$newdir
	    olddir=${dir}
#	gzip Events/${olddir}/unweighted_events.lhe
	    mv Events/${olddir}/*txt Events/$newdir/.
	    rm -rf Events/${olddir}
	done
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
