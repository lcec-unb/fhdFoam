#include "rectangularMagnet.H"
#include "addToRunTimeSelectionTable.H"
#include "mathematicalConstants.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace HModels
{
    defineTypeNameAndDebug(rectangularMagnet, 0);
    addToRunTimeSelectionTable(HModel, rectangularMagnet, dictionary);
}
}

// * * * * * * * * * * * * * * * * Constructors * * * * * * * * * * * * * * //

Foam::HModels::rectangularMagnet::rectangularMagnet
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    HModel(dict, mesh),
    H0_("H0", dimensionSet(0,-1,0,0,0,1,0), 0.0),
    center_(Zero),
    size_(Zero),
    polarity_(1.0),
    eps_(1e-12)
{
    const dictionary& coeffs =
        dict_.subDict("rectangularMagnetCoeffs");

    H0_.value() = coeffs.get<scalar>("H0");
    center_     = coeffs.get<vector>("center");
    size_       = coeffs.get<vector>("size");
    polarity_   = coeffs.getOrDefault<scalar>("polarity", 1.0);
    eps_        = coeffs.getOrDefault<scalar>("eps", 1e-12);

    Info<< "rectangularMagnet loaded: "
        << "H0=" << H0_.value()
        << ", center=" << center_
        << ", size=" << size_
        << endl;
}

// * * * * * * * * * * * * * * * * Destructor * * * * * * * * * * * * * * * //

Foam::HModels::rectangularMagnet::~rectangularMagnet()
{}

// * * * * * * * * * * * * * * * Member Functions * * * * * * * * * * * * * //
void Foam::HModels::rectangularMagnet::H
(
    volVectorField& HField
)
{
    const vectorField& C = mesh_.C();

    const scalar xc = center_.x();
    const scalar yc = center_.y();

    const scalar a = size_.x()/2.0;   // meia largura
    const scalar b = size_.y()/2.0;   // meia altura

    const scalar Hscale = polarity_ * H0_.value();

    forAll(HField, cellI)
    {
        // Coordenadas relativas ao centro do ímã
        scalar x = C[cellI].x() - xc;
        scalar y = C[cellI].y() - yc;

        scalar x1 = x - a;
        scalar x2 = x + a;

        // ==============================
        // Face superior (y = +b)
        // ==============================
        scalar yTop = y - b;

        scalar Hx_top =
            (1.0/(4.0*constant::mathematical::pi)) *
            log( (sqr(x2) + sqr(yTop)) /
                 (sqr(x1) + sqr(yTop)) );

        scalar Hy_top =
            (1.0/(2.0*constant::mathematical::pi)) *
            ( atan2(x2, yTop) - atan2(x1, yTop) );

        // ==============================
        // Face inferior (y = -b)
        // ==============================
        scalar yBot = y + b;

        scalar Hx_bot =
           -(1.0/(4.0*constant::mathematical::pi)) *
            log( (sqr(x2) + sqr(yBot)) /
                 (sqr(x1) + sqr(yBot)) );

        scalar Hy_bot =
           -(1.0/(2.0*constant::mathematical::pi)) *
            ( atan2(x2, yBot) - atan2(x1, yBot) );

        scalar Hx = Hx_top + Hx_bot;
        scalar Hy = Hy_top + Hy_bot;

        HField[cellI] = Hscale * vector(Hx, Hy, 0.0);
    }

    HField.correctBoundaryConditions();
}
