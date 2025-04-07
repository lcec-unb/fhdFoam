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

#include "constant.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace HModels
{
    defineTypeNameAndDebug(constant, 0);
    addToRunTimeSelectionTable(HModel, constant, dictionary);
}



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

HModels::constant::constant
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    HModel(dict, mesh),
    Hval_("Hval",dimensionSet(0, -1, 0, 0, 0, 1, 0),Zero)
{
    // Retrieve the coefficients sub-dictionary.
    const dictionary& coeffs = dict_.subDict("constantCoeffs");
    Hval_.value() = coeffs.get<vector>("H");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

HModels::constant::~constant()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void HModels::constant::H
(
    volVectorField& HField
)
{        
    // Set each cell of the internal field of H to (Hmax, Hmax, Hmax).
    HField = Hval_;
    // Update the boundary conditions.
    // HField.correctBoundaryConditions();
}

}
// ************************************************************************* //
