#!/bin/bash
xsecDataFile=test.dat

mzpN=(90 100 150 200 300 400 500 600 700 800 900 1000)
mdmN=(1 10 50 100 200 300 400 500 600 700 800 900 1000)
#mzpN=(90 100 150 200)
for mass_zp in "${mzpN[@]}"
do
    for mass_dm in "${mdmN[@]}"
    do
	setzp=$((mass_zp))
	setdm=$((mass_dm))
	if ((mass_zp/2==mass_dm)) 
	then setdm=$(( setdm + 5 ))
	fi
	if ((mass_dm>2*mass_zp))
	then
	    break
	fi
	echo $setzp, $setdm
	
	cp -p Cards/param_card_default.dat Cards/param_card.dat
	file=Cards/param_card.dat
	sed -i "" -e "s/MZP/"$setzp"/g" $file
	sed -i "" -e "s/MDM/"$setdm"/g" $file
	./bin/generate_events -f
	banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	echo $banner
	xsec=`grep -a Integrated $banner | awk '{print $6}'`
	echo $setzp $setdm $xsec >> $xsecDataFile

    done
done
