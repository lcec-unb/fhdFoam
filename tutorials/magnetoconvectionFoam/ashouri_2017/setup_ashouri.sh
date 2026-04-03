#!/bin/bash

echo ""
echo "==========================================="
echo " Setup simples - caso tipo Ashouri"
echo "==========================================="
echo ""

read -p "Entre com Ra_m   : " Ra_m
read -p "Entre com d*     : " d_star
read -p "Entre com beta_c : " beta_c
read -p "Entre com t_r    : " t_r

# --------------------------------------------------
# Valores de referência fixos
# --------------------------------------------------
D=0.05
Tc=300
Pr=1000
rho=1000
Br=1.2
alpha=1.0e-6
kB=1.380649e-23
mu0=1.2566370614e-6

# --------------------------------------------------
# Cálculos
# --------------------------------------------------
d=$(awk -v ds="$d_star" -v D="$D" 'BEGIN{printf "%.8e", ds*D}')
Th=$(awk -v tr="$t_r" -v Tc="$Tc" 'BEGIN{printf "%.8f", Tc*(1.0+tr)}')
nu=$(awk -v Pr="$Pr" -v alpha="$alpha" 'BEGIN{printf "%.8e", Pr*alpha}')
mu=$(awk -v rho="$rho" -v nu="$nu" 'BEGIN{printf "%.8e", rho*nu}')
Ms=$(awk -v Ram="$Ra_m" -v mu="$mu" -v alpha="$alpha" -v Br="$Br" -v D="$D" \
    'BEGIN{printf "%.8e", Ram*mu*alpha/(Br*D*D)}')
mParticle=$(awk -v bc="$beta_c" -v kB="$kB" -v Tc="$Tc" -v Br="$Br" \
    'BEGIN{printf "%.8e", bc*kB*Tc/Br}')
H0=$(awk -v Br="$Br" -v mu0="$mu0" 'BEGIN{printf "%.8e", Br/mu0}')
xc=$(awk -v D="$D" 'BEGIN{printf "%.8e", D/2.0}')
yc=$(awk -v D="$D" 'BEGIN{printf "%.8e", D/2.0}')

echo ""
echo "==========================================="
echo " Saída estimada"
echo "==========================================="
echo "D           = $D"
echo "d           = $d"
echo "xc          = $xc"
echo "yc          = $yc"
echo "Tc          = $Tc"
echo "Th          = $Th"
echo "Pr          = $Pr"
echo "alpha       = $alpha"
echo "nu          = $nu"
echo "rho         = $rho"
echo "mu          = $mu"
echo "Br          = $Br"
echo "H0          = $H0"
echo "Ms          = $Ms"
echo "mParticle   = $mParticle"
echo "kB          = $kB"
echo "==========================================="
echo ""
echo "Note que o d deverá ser alterado no blockMeshDict"
echo "e no magneticProperties. No blockMeshDict d = a."
echo "Já no magneticProperties d = magnet size."
