#!/bin/bash
set -e

./Allclean.sh
cp -r 0.orig 0
blockMesh
magnetoconvectionFoam | tee log.magnetoconvectionFoam
