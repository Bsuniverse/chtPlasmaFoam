Info<< "\nCalculating dimensionless electric potential, ep\n" << endl;

scalar      initRes1 = 1.;

int         maxI = 0;


while ((initRes1 > 5e-5) && (maxI < 10000))
{
    maxI ++;
    Info<< "Iteration = " << maxI << nl << endl;
	
	forAll(fluidRegions, i)
	{
		volScalarField& epF = epFluid[i];
		volScalarField& epsF = epsFluid[i];

    	initRes1 = solve
    	(
        	fvm::laplacian(epsF, epF)
    	).initialResidual();
	}

	forAll(solidRegions, i)
	{
		volScalarField& epSo = epSolid[i];
		volScalarField& epsSo = epsSolid[i];

    	solve
    	(
        	fvm::laplacian(epsSo, epSo)
    	);
	}
}

if (maxI == 1000000)
{
    cerr << "Electric potential solver diverged!" << endl;
    exit(EXIT_FAILURE);
}


Info<< "\nCalculating dimensionless charge density, rhoc\n" << endl;

initRes1 = 1.;
maxI = 0;

while ((initRes1 > 1e-7) && (maxI < 10000))
{
    maxI ++;
    Info<< "Iteration = " << maxI << nl << endl;

	forAll(solidRegions, i)
	{
		volScalarField& rhocS = rhocSolid[i];
		volScalarField& epsSo = epsSolid[i];

    	solve
    	(
        	fvm::laplacian(epsSo, rhocS) 
    	);
	}

	forAll(fluidRegions, i)
	{
		volScalarField& rhocF = rhocFluid[i];
		volScalarField& lambdaF = lambdaFluid[i];
		volScalarField& epsF = epsFluid[i];

    	initRes1 = solve
    	(
        	fvm::laplacian(epsF, rhocF) 
            	== fvm::Sp(1. / (lambdaF * lambdaF), rhocF)
    	).initialResidual();
	}
}

if (maxI == 1000000)
{
    cerr << "Charge Density solver diverged!" << endl;
    exit(EXIT_FAILURE);
}

forAll(fluidRegions, i)
{
	volScalarField& epF = epFluid[i];

	Info<< "    Adding to EFluid\n" << endl;
	EFluid.set
	(
		i,
		new volVectorField
		(
			IOobject
			(
				"Elec",
				runTime.timeName(),
				fluidRegions[i],
				IOobject::READ_IF_PRESENT,
				IOobject::AUTO_WRITE
			),
			- fvc::grad(epF)
		)
	);

	dimensionedScalar rhocMax = rhocMaxf[i];
	dimensionedScalar epMax = epMaxf[i];

	Info<< "    Adding to fFluid\n" << endl;
	fFluid.set
	(
		i,
		new volVectorField
		(
			IOobject
			(
				"bodyforce",
				runTime.timeName(),
				fluidRegions[i],
				IOobject::READ_IF_PRESENT,
				IOobject::AUTO_WRITE
			),
			rhocMax * epMax * rhocFluid[i] * EFluid[i] / rhoFluid[i]
		)
	);

}
