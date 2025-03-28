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

Class
    Foam::HModels::sinusoidal

Description

    Reference:
    \verbatim

    \endverbatim

SourceFiles
    sinusoidal.C

\*---------------------------------------------------------------------------*/

#ifndef sinusoidal_H
#define sinusoidal_H

#include "HModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace HModels
{

/*---------------------------------------------------------------------------*\
                           Class sinusoidal Declaration
\*---------------------------------------------------------------------------*/

class sinusoidal
:
    public HModel
{   
private:
    // Read Hmax as a dimensionedScalar.
    dimensionedVector alpha_;

    vector            beta_;

public:

    //- Runtime type information
    TypeName("sinusoidal");


    // Constructors

        //- Construct from a dictionary and a phase list
        sinusoidal
        (
            const dictionary& dict,
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~sinusoidal();


    // Member Functions
        //- Return true if this the model has a temporal
        // dependence
        virtual bool HTemporalVariation() const
        {
            return true;
        }

        //- Return the relative permeability of phase
        virtual void H(volVectorField& HField);    

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace HModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
