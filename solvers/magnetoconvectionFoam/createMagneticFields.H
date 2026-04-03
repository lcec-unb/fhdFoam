//==============================================================================
// Magnetic convection model selector
//==============================================================================

// true  -> current Boussinesq-like pyromagnetic model
// false -> Langevin-based magnetization model
const Switch useBoussinesqTM
(
    magneticProperties.lookup("useBoussinesqTM")
);

//==============================================================================
// Magnetic-fluid properties
//==============================================================================

const dictionary& magFluidDict = magneticProperties.subDict("magneticFluid");

//------------------------------------------------------------------------------
// Parameters for current Boussinesq-like model
//------------------------------------------------------------------------------

dimensionedScalar beta_m("beta_m", dimless/dimTemperature, magFluidDict);
dimensionedScalar chi0("chi0", dimless, magFluidDict);

//------------------------------------------------------------------------------
// Parameters for Langevin-based model
//------------------------------------------------------------------------------

dimensionedScalar Ms
(
    "Ms",
    dimensionSet(0, -1, 0, 0, 0, 1, 0),   // A/m
    magFluidDict
);

dimensionedScalar mParticle
(
    "mParticle",
    dimensionSet(0, 2, 0, 0, 0, 1, 0),    // A.m2
    magFluidDict
);

dimensionedScalar kB
(
    "kB",
    dimensionSet(1, 2, -2, -1, 0, 0, 0),  // J/K
    magFluidDict
);

dimensionedScalar alphaMin
(
    "alphaMin",
    dimless,
    magFluidDict.lookupOrDefault<scalar>("alphaMin", SMALL)
);

//==============================================================================
// Auxiliary magnetic fields for force evaluation
//==============================================================================

// Magnitude of imposed magnetic field
volScalarField Hmag
(
    IOobject
    (
        "Hmag",
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
        dimensionSet(0, -1, 0, 0, 0, 1, 0),   // A/m
        0.0
    )
);

// Langevin parameter alpha = mu0*m*H/(kB*T)
volScalarField alphaL
(
    IOobject
    (
        "alphaL",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE,
        true
    ),
    mesh,
    dimensionedScalar("zero", dimless, 0.0)
);

// Magnetization magnitude M = Ms*L(alpha)
volScalarField M
(
    IOobject
    (
        "M",
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
        dimensionSet(0, -1, 0, 0, 0, 1, 0),   // A/m
        0.0
    )
);

// Final magnetic body-force contribution written as acceleration
volVectorField magneticForce
(
    IOobject
    (
        "magneticForce",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE,
        true
    ),
    mesh,
    dimensionedVector("zero", dimAcceleration, vector::zero)
);
