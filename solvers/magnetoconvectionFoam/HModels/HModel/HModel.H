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
    Foam::HModel

Description

SourceFiles
    HModel.C
    newHModel.C

\*---------------------------------------------------------------------------*/

#ifndef HModel_H
#define HModel_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "volFields.H"
#include "surfaceFields.H"
#include "surfaceInterpolate.H"
#include "dictionary.H"
#include "runTimeSelectionTables.H"

namespace Foam
{
/*---------------------------------------------------------------------------*\
                           Class HModel Declaration
\*---------------------------------------------------------------------------*/

class HModel
{
protected:

    // Reference to the top-level dictionary (e.g., from transportProperties
    // or a custom dict in constant/ or system/)
    const dictionary& dict_;

    const fvMesh& mesh_;

public:

    //- Runtime type information
    TypeName("HModel");


    // Declare runtime construction

        declareRunTimeSelectionTable
        (
            autoPtr,
            HModel,
            dictionary,
            (
                const dictionary& dict,
                const fvMesh& mesh
            ),
            (dict,mesh)
        );


    // Constructors

        //- Construct with dictionary name
        HModel
        (
            const dictionary& dict, 
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~HModel();


    // Selectors

        //- Returns a reference to the selected Kr model
        static autoPtr<HModel> New
        (
            const dictionary& dict,
            const fvMesh& mesh
        );


    // Member Functions
        //- Return true if this the model has a temporal
        // dependence
        virtual bool HTemporalVariation() const
        {
            return false;
        }

        //- Return the term used in the momentum equations 
        // where Kr is the relative permeability
        virtual void H(volVectorField& HField) = 0;

        //- Dummy write for regIOobject
        bool writeData(Ostream& os) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
