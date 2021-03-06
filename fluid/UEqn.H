// Solve the Momentum equation
	MRF.correctBoundaryVelocity(U);

    tmp<fvVectorMatrix> tUEqn
    (
        fvm::ddt(U) + fvm::div(phi, U)
	  + MRF.DDt(U)
	  - fvm::laplacian(nu, U)
      + turbulence.divDevReff(U)
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
			+ ffluid * M_PI * omega.value() * 0.000134 * pos(std::sin(2 * M_PI * fplus.value() * runTime.value()))
        );

        fvOptions.correct(U);
    }

    fvOptions.correct(U);
