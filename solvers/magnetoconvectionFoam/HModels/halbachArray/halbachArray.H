#ifndef halbachArray_H
#define halbachArray_H

#include "HModel.H"

namespace Foam
{
namespace HModels
{

class halbachArray
:
    public HModel
{
private:

    //- Center of the middle magnet
    vector center_;

    //- Side length of each square magnet
    scalar magnetSide_;

    //- Edge-to-edge distance between adjacent magnets
    scalar gap_;

    //- Effective magnetization scale [A/m]
    dimensionedScalar H0_;


public:

    //- Runtime type information
    TypeName("halbachArray");


    // Constructors

        halbachArray
        (
            const dictionary& dict,
            const fvMesh& mesh
        );


    //- Destructor
    virtual ~halbachArray();


    // Member Functions

        virtual void H(volVectorField& HField);
};

}
}

#endif
