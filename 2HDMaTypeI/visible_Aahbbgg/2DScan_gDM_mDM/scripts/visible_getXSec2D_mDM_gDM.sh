#!/bin/bash
xsecDataFile=test.dat

gDM=(0.001 0.0025 0.005 0.01 0.025 0.05 0.1)
mh3=(400)
mh4=(200)
mdm=(10 50 100)

workdir=/Users/yush/2HDMa/MG5_aMC_v3_5_7/workdir_Aahbbgg
cd $workdir

echo "gDM" "MA" "Ma" "MDM" "BR(A->ah)" "BR(a->b+b~)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
for gdm in "${gDM[@]}"
do
   gXd=$gdm 
   for h3 in "${mh3[@]}"
   do
       seth3=$h3
       for h4 in "${mh4[@]}"
       do
           seth4=$h4
	   for dm in "${mdm[@]}"
	   do	    
               setdm=$dm
               cp -p Cards/param_card_paper.dat Cards/param_card.dat
               file=Cards/param_card.dat
               sed -i "" -e "s/MH3/"$seth3"/g" $file
               sed -i "" -e "s/MH4/"$seth4"/g" $file
               sed -i "" -e "s/MDM/"$setdm"/g" $file
               sed -i "" -e "s/GDM/"$gXd"/g" $file
               ./bin/generate_events -f
	       banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	       echo $banner
	       xsec=`grep -a Integrated $banner | awk '{print $6}'`
	       echo $xsec
	       decayA=`grep -a "2    25  55" $banner | awk '{print $1}'`
	       echo $decayA
	       decaya=`grep -A 5 "DECAY  55" $banner | grep -a "5  -5" | awk '{print $1}'`
	       echo $decaya
	       finalXSec=`awk -v a=$xsec -v b=0.00227 -v c=$decayA -v d=$decaya 'BEGIN{print (a*b*c*d)}'`
	       echo $finalXSec
	       echo $gXd $seth3 $seth4 $setdm $decayA $decaya $xsec $finalXSec >> $xsecDataFile
	       olddir=`ls -lrt Events | tail -1 | awk '{print $9}'`
	       newdir=visible_Aahbbgg_13TeV_gdm_${gXd}_mdm_${setdm}_MA_${seth3}_Ma_${seth4}_tanbeta5_lam3_6_sint0p2
	       echo $olddir $newdir
	       mv Events/$olddir Events/$newdir
	       olddir=`ls -lrt Events | grep -a run | tail -1 | awk '{print $9}'`
	       mv Events/${olddir}/*txt Events/$newdir/.
	       rm -rf Events/${olddir}
	   done
	done
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
