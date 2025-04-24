#------------------------------------------------------------------------------
# Running fhdFoam's GUI
#
#------------------------------------------------------------------------------

echo "fhdFoam has the following solvers:"
echo ""
echo "1 - mhtFoam"
echo "2 - magnetoconvectionFoam"
echo "3 - intermagFoam"
echo "4 - icomagFoam"
echo "Select the solver you want to use: " 
read option


cd etc
source bashrc
cd ..
case $option in
1)
python3 scripts/pythonscripts/mhtFoam_pre.py 
;;
2)
echo "We do not have a GUI for this solver yet."
;;
3)
echo "We do not have a GUI for this solver yet."
;;
4)
echo "We do not have a GUI for this solver yet."
;;
esac
