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
void Foam::HModels::rectangularMagnet::H(volVectorField& HField)
{
    const vectorField& C = mesh_.C();

    const scalar a = size_.x()/2.0;
    const scalar b = size_.y()/2.0;
    const scalar Hscale = polarity_*H0_.value();

    forAll(HField, celli)
    {
        const scalar x = C[celli].x() - center_.x();
        const scalar y = C[celli].y() - center_.y();

        HField[celli] = Hscale*unitField(x, y, a, b);
    }

    HField.correctBoundaryConditions();
}


Foam::vector Foam::HModels::rectangularMagnet::unitField
(
    const scalar x,
    const scalar y,
    const scalar a,
    const scalar b
)
{
    const scalar pi = constant::mathematical::pi;
    const scalar x1 = x - a;
    const scalar x2 = x + a;

    // Upper pole face (local y = +b)
    const scalar yTop = y - b;
    const scalar HxTop =
        (1.0/(4.0*pi))
       *log((sqr(x2) + sqr(yTop))/(sqr(x1) + sqr(yTop)));
    const scalar HyTop =
        (1.0/(2.0*pi))*(atan2(x2, yTop) - atan2(x1, yTop));

    // Lower pole face (local y = -b)
    const scalar yBot = y + b;
    const scalar HxBot =
        -(1.0/(4.0*pi))
       *log((sqr(x2) + sqr(yBot))/(sqr(x1) + sqr(yBot)));
    const scalar HyBot =
        -(1.0/(2.0*pi))*(atan2(x2, yBot) - atan2(x1, yBot));

    return vector(HxTop + HxBot, HyTop + HyBot, 0.0);
}
