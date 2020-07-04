    const fvMesh& mesh = fluidRegions[i];

    volVectorField& U = UFluid[i];
	volVectorField& ffluid = fFluid[i];
    surfaceScalarField& phi = phiFluid[i];

	singlePhaseTransportModel& laminarTransport = laminarTransports[i];
    incompressible::turbulenceModel& turbulence = turbulenceFluid[i];

    volScalarField& p = pFluid[i];
	
	IOMRFZoneList& MRF = MRFfluid[i];
    fv::options& fvOptions = fluidFvOptions[i];

    pimpleNoLoopControl& pimple = pimples.pimple(i);

    scalar cumulativeContErr = cumulativeContErrs[i];
    dimensionedScalar eps0 = eps0s[i];
    dimensionedScalar epMax = epMaxs[i];
    dimensionedScalar rhocMax = rhocMaxs[i];
    dimensionedScalar omega = omegas[i];
	dimensionedScalar fplus = fpluses[i];

	label pRefCell = 0;   
	scalar pRefValue = 0.0;
	setRefCell(p, pimple.dict(), pRefCell, pRefValue);
	mesh.setFluxRequired(p.name());