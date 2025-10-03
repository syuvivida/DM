#!/bin/bash
xsecDataFile=test.dat


mhs=(270 300 400 500 600 700 800 900 1000)
mdm=(150 200 300 400 500 600 700 800 900 1000)
mzp=(500 600 700 800 900 1000 1250 1500 1750 2000 2500 3000 3500 4000 4500 5000)
#mhs=(270)
#mdm=(150 200)
#mzp=(500)

workdir=/afs/cern.ch/work/s/syu/HHMET_study/MG5_aMC_v3_5_7/DarkHiggs_hh
#workdir=$PWD
cd $workdir

echo "MHs" "MDM" "MZp" "BR(Hs->hh)" "Xsec" >> $xsecDataFile
for hs in "${mhs[@]}"
do
    seths=$hs
    for dm in "${mdm[@]}"
    do
	setdm=$dm
	# impose mdm > mhs/2
	if ((dm<=hs/2))
	then
	    continue
        fi
	for zp in "${mzp[@]}"
	do
	    setzp=$zp
	    #impose mdm < mzp/2+100
	    if ((dm>=(zp/2+100)))
            then
                continue
            fi
	    echo $seths $setdm $setzp 
	    
            cp -p Cards/param_card_default.dat Cards/param_card.dat
            file=Cards/param_card.dat
            sed -i "" -e "s/MDarkHiggs/"$seths"/g" $file
            sed -i "" -e "s/MDarkMatter/"$setdm"/g" $file
            sed -i "" -e "s/MDarkPhoton/"$setzp"/g" $file
            ./bin/generate_events -f
	    banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	    dir=`ls -lrt Events | tail -1 | awk '{print $9}'` 
	    echo $banner
	    xsec=`grep -a Integrated $banner | awk '{print $6}'`
	    echo $xsec
	    decayHs=`grep -A 3 "DECAY  54" $banner | grep -a "25  25" | awk '{print $1}'`
	    echo $decayHs
	    echo $seths $setdm $setzp $decayHs $xsec >> $xsecDataFile
	    olddir=${dir}
	    newdir=darkHiggs_HsChiChi_13p6TeV_mHs_${seths}_mdm_${setdm}_mZp_${setzp}_sinTheta0p01
	    echo $olddir $newdir
	    mv Events/$olddir Events/$newdir
	    mv Events/$newdir/unweighted_events.lhe.gz 	Events/${newdir}/${newdir}.lhe.gz
	done
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
