#!/bin/bash
# Run minimize_nonAligned on a grid of (mA, mH) and collect the best parameters in one table.
# usage: bash getBestPar_mA_mH.sh [output.dat] [extra minimizer options, e.g. --fracWA 0.2 --dM 0 0]
out=${1:-bestPar_nonAligned_AHhha.dat}
shift
opts="$@"
ma=140                                   # must match --ma (default 140)

mA_list=(410 460 510 560 610 660 700 750 800)
mH_list=(255 260 300 350 400 450 500 550 600 650 700)

echo "mA mH ma mHc sinp sinbma tanb lam3 laP1 laP2 xsecBR_pb sigma_ggA_pb BR_AHa BR_Hhh BR_achichi GammaA_over_mA GammaH_over_mH drho maxLambda allowed unit_over_8pi bounded_margin" > $out
for mA in "${mA_list[@]}"; do
  for mH in "${mH_list[@]}"; do
    if (( mA < mH + ma + 10 )); then continue; fi   # A -> H a needs mA > mH + ma
    echo "mA = $mA, mH = $mH"
    ./minimize_nonAligned $mA $mH $opts | grep "^RESULT" | cut -d' ' -f2- >> $out
  done
done
command -v column >/dev/null && column -t $out > updated_$out
echo "wrote $out (and the aligned updated_$out); plot with: python3 plot_bestPar_2D.py $out"
