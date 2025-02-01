#------------------------------------------------------------------------------
# bash script for compiling fhdFoam's solvers
#
#------------------------------------------------------------------------------
 
echo "Cleaning icomagFoam..."
echo ""
 cd solvers/icomagFoam && wclean 
echo "Cleaning mhtFoam..."
echo "" 
 cd ../mhtFoam && wclean  
echo "Cleaning magnetoconvectionFoam..."
echo "" 
 cd ../magnetoconvectionFoam && wclean  
 echo "Cleaning intermagFoam..."
echo "" 
 cd ../intermagFoam && wclean   
echo "" 
echo "All solvers have been successfully cleaned."