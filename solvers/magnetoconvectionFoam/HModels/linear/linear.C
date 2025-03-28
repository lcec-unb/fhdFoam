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

#include "linear.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace HModels
{
    defineTypeNameAndDebug(linear, 0);
    addToRunTimeSelectionTable(HModel, linear, dictionary);
}



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

HModels::linear::linear
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    HModel(dict,mesh),
    C0_("C0",dimensionSet(0, -1, 0, 0, 0, 1, 0),Zero)
{
    // Retrieve the coefficients sub-dictionary.
    const dictionary& coeffs = dict_.subDict("linearCoeffs");
    C0_.value() = coeffs.get<vector>("C0");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

HModels::linear::~linear()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void HModels::linear::H
(
    volVectorField& HField
)
{        
    forAll(HField,cellI)
    {
        HField[cellI] = cmptMultiply(mesh_.C()[cellI],C0_.value());
    }
}

}
// ************************************************************************* //
