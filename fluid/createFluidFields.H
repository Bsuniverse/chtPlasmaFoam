// Initialise fluid field pointer lists
PtrList<volScalarField> pFluid(fluidRegions.size());
PtrList<volVectorField> UFluid(fluidRegions.size());
PtrList<volScalarField> epFluid(fluidRegions.size());
PtrList<volScalarField> rhocFluid(fluidRegions.size());
PtrList<volScalarField> rhoFluid(fluidRegions.size());
PtrList<volScalarField> epsFluid(fluidRegions.size());
PtrList<volScalarField> lambdaFluid(fluidRegions.size());
PtrList<volVectorField> EFluid(fluidRegions.size());
PtrList<volVectorField> fFluid(fluidRegions.size());
PtrList<surfaceScalarField> phiFluid(fluidRegions.size());
PtrList<singlePhaseTransportModel> laminarTransports(fluidRegions.size());
PtrList<incompressible::turbulenceModel> turbulenceFluid(fluidRegions.size());
PtrList<dimensionedScalar> eps0s(fluidRegions.size());
PtrList<dimensionedScalar> epMaxs(fluidRegions.size());
PtrList<dimensionedScalar> rhocMaxs(fluidRegions.size());
PtrList<dimensionedScalar> omegas(fluidRegions.size());
PtrList<dimensionedScalar> fpluses(fluidRegions.size());

PtrList<IOMRFZoneList> MRFfluid(fluidRegions.size());
PtrList<fv::options> fluidFvOptions(fluidRegions.size());

// Populate fluid field pointer lists
forAll(fluidRegions, i)
{
	Info<< "*** Reading transport properties for region " << fluidRegions[i].name() << nl << endl;

//			Reading userdefine parameters			// 
    IOdictionary userDefinedParameters
    (
        IOobject
        (
            "userDefinedParameters",
            runTime.constant(),
            fluidRegions[i],
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );
	
    Info << "    permiticity of free space, eps0" << endl;
    eps0s.set
    (
        i,
        new dimensionedScalar
        (
            "eps0",
            dimensionSet(-1, -3, 4, 0, 0, 2, 0),
            userDefinedParameters.lookup("eps0")
        )
    );

    Info << "    electric potential coefficient of input sine signal, epMax" << endl;
    epMaxs.set
    (
        i,
        new dimensionedScalar
        (
            "epMax",
            dimensionSet(0, 0, 0, 0, 0, 0, 0),
            userDefinedParameters.lookup("epMax")
        )
    );

    Info << "    charge density coefficient of input sine signal, rhocMax" << endl;
    rhocMaxs.set
    (
        i,
        new dimensionedScalar
        (
            "rhocMax",
            dimensionSet(0, 0, 0, 0, 0, 0, 0),
            userDefinedParameters.lookup("rhocMax")
        )
    );

    Info << "    frequency of input sine signal, omega" << endl;
    omegas.set
    (
        i,
        new dimensionedScalar
        (
            "omega",
            dimensionSet(0, 0, -1, 0, 0, 0, 0),
            userDefinedParameters.lookup("omega")
        )
    );

    Info << "    control frequency, fplus" << endl;
    fpluses.set
    (
        i,
        new dimensionedScalar
        (
            "fplus",
            dimensionSet(0, 0, -1, 0, 0, 0, 0),
            userDefinedParameters.lookup("fplus")
        )
    );

// 				Reading transport parameters			//
	IOdictionary fluidProperties
	(
		IOobject
		{
			"transportProperties",
			runTime.constant(),
			fluidRegions[i],
			IOobject::MUST_READ_IF_MODIFIED,
			IOobject::NO_WRITE
		}
	);

    Info<< "    Adding to pFluid\n" << endl;
    pFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "p",
                runTime.timeName(),
                fluidRegions[i],
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            fluidRegions[i]   
        )
    );

    Info<< "    Adding to UFluid\n" << endl;
    UFluid.set
    (
        i,
        new volVectorField
        (
            IOobject
            (
                "U",
                runTime.timeName(),
                fluidRegions[i],
                IOobject::MUST_READ,
                IOobject::AUTO_WRITE
            ),
            fluidRegions[i]
        )
    );

    Info<< "    Adding to epFluid\n" << endl;
    epFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "ep",
                runTime.timeName(),
                fluidRegions[i],
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
            ),
            fluidRegions[i]
        )
    );

    Info<< "    Adding to rhocFluid\n" << endl;
    rhocFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "rhoc",
                runTime.timeName(),
                fluidRegions[i],
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
            ),
            fluidRegions[i]
        )
    );

    Info<< "    Adding to rhoFluid\n" << endl;
    rhoFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "rhoField",
                runTime.timeName(),
                fluidRegions[i]
            ),
            fluidRegions[i],
            dimensionedScalar
            (
                dimensionSet(1, -3, 0, 0, 0, 0, 0),    
                readScalar(fluidProperties.lookup("rho"))
            )
        )
    );
    Info<< "    Adding to epsFluid\n" << endl;
    epsFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "epsField",
                runTime.timeName(),
                fluidRegions[i]
            ),
            fluidRegions[i],
            dimensionedScalar
            (
                dimensionSet(0, 0, 0, 0, 0, 0, 0),    
                readScalar(fluidProperties.lookup("eps"))
            )
        )
    );

    Info<< "    Adding to lambdaFluid\n" << endl;
    lambdaFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "lambda",
                runTime.timeName(),
                fluidRegions[i]
            ),
            fluidRegions[i],
            dimensionedScalar
            (
			 	dimensionSet(0, 1, 0, 0, 0, 0, 0),
                readScalar(fluidProperties.lookup("lambda"))
            )
        )
    );

    Info<< "    Adding to phiFluid\n" << endl;
    phiFluid.set
    (
        i,
        new surfaceScalarField
        (
            IOobject
            (
                "phi",
                runTime.timeName(),
                fluidRegions[i],
                IOobject::READ_IF_PRESENT,
                IOobject::AUTO_WRITE
            ),
            linearInterpolate(UFluid[i])
                & fluidRegions[i].Sf()
        )
    );

	Info<< "    Adding to laminarTransport\n" << endl;
    laminarTransports.set
    (
        i,
        new singlePhaseTransportModel
        (
            UFluid[i],
            phiFluid[i]
        )
    );

    Info<< "    Adding to turbulenceFluid\n" << endl;
    turbulenceFluid.set
    (
        i,
        incompressible::turbulenceModel::New
        (
            UFluid[i],
            phiFluid[i],
            laminarTransports[i]
        ).ptr()
    );

	Info<< "    Adding MRF\n" << endl;
    MRFfluid.set
    (
        i,
        new IOMRFZoneList(fluidRegions[i])
    );

    Info<< "    Adding fvOptions\n" << endl;
    fluidFvOptions.set
    (
        i,
        new fv::options(fluidRegions[i])
    );

    turbulenceFluid[i].validate();
}
