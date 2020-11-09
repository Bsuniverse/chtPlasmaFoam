{
    volScalarField kappaEff
    (
        "kappaEff",
        turbulence.nu()/Pr + turbulence.nut()/Prt
    );

    fvScalarMatrix EEqn
    (
        fvm::ddt(T)
      + fvm::div(phi, T)
      - fvm::laplacian(kappaEff, T)
	  ==
	  	M_PI * eps * eps0 * omega * 0.0028 * magSqr(fvc::grad(ep) * epMax) * pos(std::sin(2 * M_PI * fplus.value() * runTime.value())) / rho / cp
    );

    EEqn.relax();

    EEqn.solve();

    Info<< "Min/max T:" << min(T)<< ' '
        << max(T) << endl;
}
