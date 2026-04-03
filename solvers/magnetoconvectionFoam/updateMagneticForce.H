//==============================================================================
// Magnetic body-force update
//==============================================================================

// The imposed magnetic field H is stationary and is computed only once.
// The magnetic body force, however, can be evaluated using:
//
// 1) a Boussinesq-like pyromagnetic approximation
// 2) a Langevin-based magnetization model
//
// In the Langevin branch, a small lower bound is imposed on alphaL
// to avoid singularities in coth(alphaL) - 1/alphaL.

if (useBoussinesqTM)
{
    magneticForce =
        (Foam::constant::electromagnetic::mu0/rho)
       *chi0*beta_m*(T - TRef)
       *fvc::grad(H2over2);
}
else
{
    Hmag = mag(H);

    alphaL =
        max
        (
            (Foam::constant::electromagnetic::mu0*mParticle*Hmag)/(kB*T),
            alphaMin
        );

    M =
        Ms
       *(
            cosh(alphaL)/sinh(alphaL)
          - 1.0/alphaL
        );

    magneticForce =
        (Foam::constant::electromagnetic::mu0/rho)
       *M
       *fvc::grad(Hmag);
}
