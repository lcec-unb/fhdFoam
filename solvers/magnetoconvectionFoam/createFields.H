Info<< "Reading thermophysical properties\n" << endl;

//==============================================================================
// Primary solution fields
//==============================================================================

Info<< "Reading field T\n" << endl;
volScalarField T
(
    IOobject
    (
        "T",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field p_rgh\n" << endl;
volScalarField p_rgh
(
    IOobject
    (
        "p_rgh",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

#include "createPhi.H"

//==============================================================================
// Transport properties
//==============================================================================

#include "readTransportProperties.H"

//==============================================================================
// Turbulence model
//==============================================================================

Info<< "Creating turbulence model\n" << endl;
autoPtr<incompressible::turbulenceModel> turbulence
(
    incompressible::turbulenceModel::New(U, phi, laminarTransport)
);

//==============================================================================
// Thermal buoyancy auxiliary field
//==============================================================================

// Kinematic density used in the standard thermal Boussinesq buoyancy term
volScalarField rhok
(
    IOobject
    (
        "rhok",
        runTime.timeName(),
        mesh
    ),
    1.0 - beta*(T - TRef)
);

//==============================================================================
// Turbulent thermal diffusivity
//==============================================================================

Info<< "Reading field alphat\n" << endl;
volScalarField alphat
(
    IOobject
    (
        "alphat",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

//==============================================================================
// Gravity and hydrostatic pressure support
//==============================================================================

#include "readGravitationalAcceleration.H"
#include "readhRef.H"
#include "gh.H"

volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    p_rgh + rhok*gh
);

label pRefCell = 0;
scalar pRefValue = 0.0;

setRefCell
(
    p,
    p_rgh,
    pimple.dict(),
    pRefCell,
    pRefValue
);

if (p_rgh.needReference())
{
    p += dimensionedScalar
    (
        "p",
        p.dimensions(),
        pRefValue - getRefCellValue(p, pRefCell)
    );
}

mesh.setFluxRequired(p_rgh.name());

//==============================================================================
// Auxiliary models
//==============================================================================

#include "createMRF.H"
#include "createIncompressibleRadiationModel.H"
#include "createFvOptions.H"

//==============================================================================
// Imposed magnetic field H
//==============================================================================

Info<< "Creating magnetic field H\n" << endl;

volVectorField H
(
    IOobject
    (
        "H",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE,
        true
    ),
    mesh,
    dimensionedVector
    (
        "zero",
        dimensionSet(0, -1, 0, 0, 0, 1, 0),   // A/m
        vector::zero
    )
);

volScalarField H2over2
(
    IOobject
    (
        "H2over2",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE,
        true
    ),
    mesh,
    dimensionedScalar
    (
        "zero",
        dimensionSet(0, -2, 0, 0, 0, 2, 0),   // (A/m)^2
        0.0
    )
);

//==============================================================================
// Magnetic properties dictionary and magnetic-field model
//==============================================================================

Info<< "Reading magnetic properties\n" << endl;

IOdictionary magneticProperties
(
    IOobject
    (
        "magneticProperties",
        runTime.constant(),
        mesh,
        IOobject::MUST_READ,
        IOobject::NO_WRITE
    )
);

autoPtr<HModel> HPtr = HModel::New(magneticProperties, mesh);

//==============================================================================
// Magnetic convection model parameters and auxiliary magnetic fields
//==============================================================================

#include "createMagneticFields.H"
