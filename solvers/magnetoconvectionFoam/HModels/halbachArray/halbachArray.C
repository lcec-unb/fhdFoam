#include "halbachArray.H"
#include "rectangularMagnet.H"
#include "addToRunTimeSelectionTable.H"


namespace Foam
{
namespace HModels
{

defineTypeNameAndDebug(halbachArray, 0);

addToRunTimeSelectionTable
(
    HModel,
    halbachArray,
    dictionary
);


halbachArray::halbachArray
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    HModel(dict, mesh),
    center_(Zero),
    magnetSide_(0.0),
    gap_(0.0),
    H0_
    (
        "H0",
        dimensionSet(0, -1, 0, 0, 0, 1, 0),
        0.0
    )
{
    const dictionary& coeffs =
        dict_.subDict("halbachArrayCoeffs");

    center_ = coeffs.get<vector>("center");
    magnetSide_ = coeffs.get<scalar>("magnetSide");
    gap_ = coeffs.getOrDefault<scalar>("gap", 0.0);
    H0_.value() = coeffs.get<scalar>("H0");

    if (magnetSide_ <= 0.0)
    {
        FatalIOErrorInFunction(coeffs)
            << "magnetSide must be greater than zero."
            << exit(FatalIOError);
    }

    if (gap_ < 0.0)
    {
        FatalIOErrorInFunction(coeffs)
            << "gap cannot be negative."
            << exit(FatalIOError);
    }

    if (H0_.value() < 0.0)
    {
        FatalIOErrorInFunction(coeffs)
            << "H0 cannot be negative."
            << exit(FatalIOError);
    }

    Info<< "halbachArray loaded:" << nl
        << "    H0         = " << H0_.value() << " A/m" << nl
        << "    center     = " << center_ << nl
        << "    magnetSide = " << magnetSide_ << " m" << nl
        << "    gap        = " << gap_ << " m" << endl;
}


halbachArray::~halbachArray()
{}


void halbachArray::H
(
    volVectorField& HField
)
{
    const vectorField& C = mesh_.C();

    const scalar halfSide = magnetSide_/2.0;
    const scalar pitch = magnetSide_ + gap_;

    // Magnetization directions from S to N, from left to right:
    //
    //     down, left, up, right, down
    //
    // This corresponds to the Halbach array in Kaneda et al. (2023).
    const vector magnetizationDirections[5] =
    {
        vector( 0.0, -1.0, 0.0),
        vector(-1.0,  0.0, 0.0),
        vector( 0.0,  1.0, 0.0),
        vector( 1.0,  0.0, 0.0),
        vector( 0.0, -1.0, 0.0)
    };

    forAll(HField, cellI)
    {
        vector totalField(Zero);

        for (label magnetI = 0; magnetI < 5; ++magnetI)
        {
            const scalar xCenter =
                center_.x() + (magnetI - 2)*pitch;

            const vector relativePosition
            (
                C[cellI].x() - xCenter,
                C[cellI].y() - center_.y(),
                0.0
            );

            const vector& localY =
                magnetizationDirections[magnetI];

            const vector localX
            (
                localY.y(),
               -localY.x(),
                0.0
            );

            const scalar xLocal =
                relativePosition & localX;

            const scalar yLocal =
                relativePosition & localY;

            const vector localField =
                rectangularMagnet::unitField
                (
                    xLocal,
                    yLocal,
                    halfSide,
                    halfSide
                );

            totalField +=
                localField.x()*localX
              + localField.y()*localY;
        }

        HField[cellI] = H0_.value()*totalField;
    }

    HField.correctBoundaryConditions();
}

}
}
