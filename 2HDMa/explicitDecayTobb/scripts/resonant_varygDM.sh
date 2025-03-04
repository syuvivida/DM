#!/bin/bash
xsecDataFile=test.dat

gDM=(0.001 0.0025 0.005 0.01 0.025 0.05 0.1)
mh2=(600)
mh4=(200 250 300 350 400 450)
mdm=(10 50 100 150 200 250 300 350)
workdir=/afs/cern.ch/work/s/syu/HHMET_study/MG5_aMC_v3_5_7/type2_visible_h3
cd $workdir

echo "gDM" "MA" "Ma" "MDM" "BR(A->ah)" "BR(a->b+b~)" "Xsec" "effective Xsec (Xsec*BR)" >> $xsecDataFile
for gdm in "${gDM[@]}"
do
   gXd=$gdm 
   for h2 in "${mh2[@]}"
   do
       seth2=$h2
       for h4 in "${mh4[@]}"
       do
           seth4=$h4
	   if ((h4>350))
	   then
	       break
	   fi
	   for dm in "${mdm[@]}"
	   do	    
               setdm=$dm
	       if ((dm>=h4*3/4))
	       then
		   break
               fi
	       echo $seth2, $seth4, $setdm, $gXd
               cp -p Cards/param_card_paper.dat Cards/param_card.dat
               file=Cards/param_card.dat
               sed -i "" -e "s/MH2/"$seth2"/g" $file
               sed -i "" -e "s/MH4/"$seth4"/g" $file
               sed -i "" -e "s/MDM/"$setdm"/g" $file
               sed -i "" -e "s/GDM/"$gXd"/g" $file
               ./bin/generate_events -f
	       banner=`ls -lrt Events | tail -1 | awk '{print "ls -lrt Events/"$9"/"$9"_tag_1_banner.txt"}' | bash | awk '{print $9}'`
	       dir=`ls -lrt Events | tail -1 | awk '{print $9}'` 
	       echo $banner
	       xsec=`grep -a Integrated $banner | awk '{print $6}'`
	       echo $xsec
	       decayA=`grep -a "2    25  55" $banner | awk '{print $1}'`
	       echo $decayA
	       decaya=`grep -A 5 "DECAY  55" $banner | grep -a "5  -5" | awk '{print $1}'`
	       echo $decaya
	       finalXSec=`awk -v a=$xsec -v c=$decayA -v d=$decaya 'BEGIN{print (a*c*d)}'`
	       echo $finalXSec
	       echo $gXd $seth2 $seth4 $setdm $decayA $decaya $xsec $finalXSec >> $xsecDataFile
	       olddir=${dir}_decayed_1
	       newdir=resonant_Aah_abb_13p6TeV_gdm_${gXd}_mdm_${setdm}_MA_${seth2}_Ma_${seth4}
	       echo $olddir $newdir
	       mv Events/$olddir Events/$newdir
	       olddir=${dir}
	       mv Events/${olddir}/*txt Events/$newdir/.
	       rm -rf Events/${olddir}
	   done
	done
    done
done
updateFile=updated_${xsecDataFile}
column -t $xsecDataFile > $updateFile
