{
    while (pimple.correctNonOrthogonal())
    {
        fvScalarMatrix hEqn
        (
            fvm::ddt(betav*rho, h)
          - (
                thermo.isotropic()
              ? fvm::laplacian(betav*thermo.alpha(), h, "laplacian(alpha,h)")
              : fvm::laplacian(betav*taniAlpha(), h, "laplacian(alpha,h)")
            )
          ==
            fvOptions(rho, h)
		  + M_PI * epsSo * eps0Solid * omegaSolid * runTime.deltaT().value() * magSqr(fvc::grad(epSo) * epMaxSolid) * pos(std::sin(2 * M_PI * fplusSolid.value() * runTime.value()))
        );

        hEqn.relax();

        fvOptions.constrain(hEqn);

        hEqn.solve();

        fvOptions.correct(h);
    }
}

thermo.correct();

Info<< "Min/max T:" << min(thermo.T()).value() << ' '
    << max(thermo.T()).value() << endl;
