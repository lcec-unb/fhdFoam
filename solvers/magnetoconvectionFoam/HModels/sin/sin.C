/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2018 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "sin.H"
#include "mathematicalConstants.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace HModels
{
    defineTypeNameAndDebug(sinusoidal, 0);
    addToRunTimeSelectionTable(HModel, sinusoidal, dictionary);
}



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

HModels::sinusoidal::sinusoidal
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    HModel(dict,mesh),
    alpha_("alpha",dimensionSet(0, -1, 0, 0, 0, 1, 0),Zero),
    beta_(Zero)
{
    // Retrieve the coefficients sub-dictionary.
    const dictionary& coeffs = dict_.subDict("sinusoidalCoeffs");
    alpha_.value() = coeffs.get<vector>("alpha");
    beta_ = coeffs.get<vector>("beta");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

HModels::sinusoidal::~sinusoidal()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void HModels::sinusoidal::H
(
    volVectorField& HField
)
{        
    const scalar t = mesh_.time().value();
    forAll(HField,cellI)
    {
        vector a0 = cmptMultiply(mesh_.C()[cellI],alpha_.value());
        vector b0 = beta_*t;
        vector sinB0(sin(b0.x()), sin(b0.y()), sin(b0.z()));
        HField[cellI] = cmptMultiply(a0,sinB0);
    }
}

}
// ************************************************************************* //
