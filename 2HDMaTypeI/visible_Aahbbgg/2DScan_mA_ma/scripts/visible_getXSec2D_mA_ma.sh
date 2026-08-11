#!/bin/bash
xsecDataFile=test.dat
widthDataFile=width.dat
mh3=(240 280 300 320 350 400 450 500 550 600 650 700)
# cross section < 0.01fb for mh3>700
mh4=(50 60 70 80 90 95 100 150 200 250 300)



workdir=/Users/yush/2HDMa/MG5_aMC_v3_5_7/workdir_Aahbbgg
cd $workdir

echo "MA" "Ma" "BR(A->ah)" "BR(a->bb)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
echo "MA" "Ma" "widthA" "widtha" "FracWA" "FracWa" >> $widthDataFile
for h3 in "${mh3[@]}"
do
    seth3=$h3
    for h4 in "${mh4[@]}"
    do
        seth4=$h4
	if ((h3<(h4+125)))
	then
	    break
	fi
	echo $seth3, $seth4
        cp -p Cards/param_card_paper.dat Cards/param_card.dat
        file=Cards/param_card.dat
        sed -i "" -e "s/MH3/"$seth3"/g" $file
        sed -i "" -e "s/MH4/"$seth4"/g" $file
        ./bin/generate_events -f
	banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	echo $banner
	xsec=`grep -a Integrated $banner | awk '{print $6}'`
	echo $xsec
	decayA=`grep -A 10 "DECAY  36" $banner | grep -a "25  55" | awk '{print $1}'`
	echo $decayA
	decaya=`grep -A 2 "DECAY  55" $banner | grep -a "5  -5" | awk '{print $1}'`
	echo $decaya
	finalXSec=`awk -v a=$xsec -v b=$decayA -v c=$decaya -v d=0.00227 'BEGIN{print (a*b*c*d)}'`
	echo $finalXSec
	echo $seth3 $seth4 $decayA $decaya $xsec $finalXSec >> $xsecDataFile

	#get width
	massA=`grep -A 10 "BLOCK MASS" $banner | grep -a 36  | awk '{print $2}'`
	massa=`grep -A 10 "BLOCK MASS" $banner | grep -a 55  | awk '{print $2}'`
	widthA=`grep "DECAY  36" $banner | awk '{print $3}'`
	widtha=`grep "DECAY  55" $banner | awk '{print $3}'`
	FracWA=`awk -v a=$massA -v b=$widthA 'BEGIN{print (b/a)}'`
	FracWa=`awk -v a=$massa -v b=$widtha 'BEGIN{print (b/a)}'`
	echo $seth3 $seth4 $widthA $widtha $FracWA $FracWa >> $widthDataFile
	
	olddir=`ls -lrt Events | tail -1 | awk '{print $9}'`
	newdir=visible_Aahbbgg_13TeV_MA_${seth3}_Ma_${seth4}_tanbeta5_lambda3_6_sint0p2
	echo $olddir $newdir
	mv Events/$olddir Events/$newdir
	olddir=`ls -lrt Events | grep -a run | tail -1 | awk '{print $9}'`
	mv Events/${olddir}/*txt Events/$newdir/.
	rm -rf Events/${olddir}
	
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile


updateFile=updated_${widthDataFile}
column -t $widthDataFile > $updateFile
