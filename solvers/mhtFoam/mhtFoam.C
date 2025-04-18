/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    scalarTransportFoam

Group
    grpBasicSolvers

Description
    Passive scalar transport equation solver.

    \heading Solver details
    The equation is given by:

    \f[
        \ddt{T} + \div \left(\vec{U} T\right) - \div \left(D_T \grad T \right)
        = S_{T}
    \f]

    Where:
    \vartable
        T       | Passive scalar
        D_T     | Diffusion coefficient
        S_T     | Source
    \endvartable

    \heading Required fields
    \plaintable
        T       | Passive scalar
        U       | Velocity [m/s]
    \endplaintable

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "fvOptions.H"
#include "simpleControl.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Passive scalar transport equation solver."
    );

    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"
    #include "createFields.H"
    
    //ID.storePrevIter();
    simpleControl simple(mesh);

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nCalculating scalar transport\n" << endl;
    int winteger = winterval.value(); 
    int contador = 0;
    
    #include "CourantNo.H"
     while (simple.loop())
    {
       
        Info<< "Time = " << runTime.timeName() << nl << endl;
	
      int tempoint = floor(runTime.value());
   
      if (tempoint % winteger == 0){
         contador = contador + 1;     
         } 
      else
      {
      contador = 0;
      }        
      
      if (contador == 1){
 	Info<< "\nCalculating W field\n" << endl;
	#include "Weqn.H"
     }

	// Calculating the correction factor field
	
	fvScalarMatrix CORREqn
            (       
                fvm::ddt(corr)
              - fvm::laplacian(diff, corr)
             ==
                fvOptions(corr)                             
            );
	
	    CORREqn.relax();
            fvOptions.constrain(CORREqn);
            CORREqn.solve();
            fvOptions.correct(corr);
	
	
        while (simple.correctNonOrthogonal())
        {
            fvScalarMatrix TEqn
            (       
                fvm::ddt(T)
              + fvm::div(phi, T)
              - fvm::laplacian(DT, T)
- ID*(mu0*pi*fi*MD*H0*freq*qui_imag*corr)/((ID*rho2 + (1-ID)*rho1)*(ID*cp2 + (1-ID)*cp1))
              - W*((rhob*cb)/((ID*rho2 + (1-ID)*rho1)*(ID*cp2 + (1-ID)*cp1)))*(Tb-T) 
              - (1-ID)*Q1/((ID*rho2 + (1-ID)*rho1)*(ID*cp2 + (1-ID)*cp1))
              - ID*Q2/((ID*rho2 + (1-ID)*rho1)*(ID*cp2 + (1-ID)*cp1))
             ==
                fvOptions(T)                             
            );

            TEqn.relax();
            fvOptions.constrain(TEqn);
            TEqn.solve();
            fvOptions.correct(T);
 
        }
        
        runTime.write();
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
