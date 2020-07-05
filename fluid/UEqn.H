// Solve the Momentum equation
	MRF.correctBoundaryVelocity(U);

    tmp<fvVectorMatrix> tUEqn
    (
        fvm::ddt(U) + fvm::div(phi, U)
	  + MRF.DDt(U)
      + turbulence.divDevRhoReff(U)
     ==
        fvOptions(U)
    );
    fvVectorMatrix& UEqn = tUEqn.ref();

    UEqn.relax();

    fvOptions.constrain(UEqn);

    if (pimple.momentumPredictor())
    {
        solve
        (
            UEqn
         ==
		 	- fvc::grad(p)
			+ ffluid * omega.value() * runTime.deltaT().value() * pos(std::sin(2 * M_PI * fplus.value() * runTime.value()))
        );

        fvOptions.correct(U);
    }

    fvOptions.correct(U);
