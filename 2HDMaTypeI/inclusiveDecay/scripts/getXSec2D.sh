#!/bin/bash
xsecDataFile=test.dat

#mh2=(200 300 400 500 600 700)
mh2=(200)
#mh3=(100 200 300 400 500 600 700)
mh3=(100)

workdir=/afs/cern.ch/work/s/syu/HHMET_study/MG5_aMC_v3_5_7/gg_hhxx_2HDMI
cd $workdir
echo "MH2" "WH" "WH/MH" "WA" "WA/MA" "Wa" "WHc" "Xsec" "Xsec*300/fb" >> $xsecDataFile
for h2 in "${mh2[@]}"
do
    seth2=$h2
    echo $seth2
    for h3 in "${mh3[@]}"
    do
	seth3=$h3
	echo $seth3
	
	cp -p Cards/param_card_paper.dat Cards/param_card.dat
	file=Cards/param_card.dat
	sed -i "" -e "s/MH2/"$seth2"/g" $file
	sed -i "" -e "s/MH3/"$seth3"/g" $file
	./bin/generate_events -f
	banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	dir=`ls -lrt Events | tail -1 | awk '{print $9}'` 
	echo $banner
	xsec=`grep -a Integrated $banner | awk '{print $6}'`
	decayH=`grep -a "DECAY  35" $banner | awk '{print $3}'`
	#    echo $decayH
	fracH=`awk -v a=$decayH -v b=$seth2 'BEGIN{print (a /b)}'`
	decayA=`grep -a "DECAY  36" $banner | awk '{print $3}'`
	fracA=`awk -v a=$decayA -v b=$seth3 'BEGIN{print (a /b)}'`
	decaya=`grep -a "DECAY  55" $banner | awk '{print $3}'`
	decayHc=`grep -a "DECAY  37" $banner | awk '{print $3}'`
	neve=`awk -v a=$xsec -v b=300000 'BEGIN{print (a*b)}'`
	echo $seth2 $seth3 $decayH $fracH $decayA $fracA $decaya $decayHc $xsec $neve >> $xsecDataFile
	newdir=run_13p6TeV_MA_${seth3}_MH2_${seth2}_tanbeta5_lambda3_2
	echo $dir $newdir
	mv Events/$dir Events/$newdir
	done
done

updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
