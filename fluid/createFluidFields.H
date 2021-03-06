// Initialise fluid field pointer lists
PtrList<singlePhaseTransportModel> thermoFluid(fluidRegions.size());
PtrList<volScalarField> pFluid(fluidRegions.size());
PtrList<volScalarField> TFluid(fluidRegions.size());
PtrList<volVectorField> UFluid(fluidRegions.size());
PtrList<volScalarField> rhoFluid(fluidRegions.size());
PtrList<surfaceScalarField> phiFluid(fluidRegions.size());
PtrList<volScalarField> KFluid(fluidRegions.size());
PtrList<dimensionedScalar> nuFluid(fluidRegions.size());
PtrList<dimensionedScalar> cpFluid(fluidRegions.size());
PtrList<dimensionedScalar> PrFluid(fluidRegions.size());
PtrList<dimensionedScalar> PrtFluid(fluidRegions.size());
PtrList<incompressible::turbulenceModel> turbulenceFluid(fluidRegions.size());
// initialise electrical field pointer lists
PtrList<volScalarField> epsFluid(fluidRegions.size());
PtrList<volScalarField> lambdaFluid(fluidRegions.size());
PtrList<volVectorField> EFluid(fluidRegions.size());
PtrList<volVectorField> fFluid(fluidRegions.size());
PtrList<volScalarField> epFluid(fluidRegions.size());
PtrList<volScalarField> rhocFluid(fluidRegions.size());
PtrList<dimensionedScalar> eps0f(fluidRegions.size());
PtrList<dimensionedScalar> epMaxf(fluidRegions.size());
PtrList<dimensionedScalar> rhocMaxf(fluidRegions.size());
PtrList<dimensionedScalar> omegaf(fluidRegions.size());
PtrList<dimensionedScalar> fplusf(fluidRegions.size());

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
            IOobject::NO_WRITE)
    );
	
    Info << "    permiticity of free space, eps0" << endl;
    eps0f.set
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
    epMaxf.set
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
    rhocMaxf.set
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
    omegaf.set
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
    fplusf.set
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

    Info<< "*** Reading fluid mesh thermophysical properties for region "
        << fluidRegions[i].name() << nl << endl;

    Info<< "    Adding to KFluid\n" << endl;
    KFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "K",
                runTime.timeName(),
                fluidRegions[i]
            ),
            fluidRegions[i],
            dimensionedScalar
            (
                dimensionSet(1, 1, -3, -1, 0, 0, 0),    
                readScalar(fluidProperties.lookup("K"))
            )
        )
    );

    Info<< "    Adding to TFluid\n" << endl;
    TFluid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "T",
                runTime.timeName(),
                fluidRegions[i],
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            fluidRegions[i]   
        )
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

	Info<< "    Adding to thermoFluid\n" << endl;
    thermoFluid.set
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
            thermoFluid[i]
        ).ptr()
    );

    PrFluid.set
    (
    	i,
    	new dimensionedScalar
		(
         "Pr",
         dimensionSet(0, 0, 0, 0, 0, 0, 0),
		 thermoFluid[i].lookup("Pr")
		)
    );
    // Turbulent Prandtl number
    PrtFluid.set
    (
    	i,
    	new dimensionedScalar
		(
         "Prt",
         dimensionSet(0, 0, 0, 0, 0, 0, 0),
		 thermoFluid[i].lookup("Prt")
		)
    );

    // Kynematic viscosity
    nuFluid.set
    (
    	i,
    	new dimensionedScalar
		(
         "nu",
         dimensionSet(0, 2, -1, 0, 0, 0, 0),
		 thermoFluid[i].lookup("nu")
		)
    );

    cpFluid.set
    (
    	i,
    	new dimensionedScalar
		(
         "cp",
         dimensionSet(0, 2, -2, -1, 0, 0, 0),
		 thermoFluid[i].lookup("cp")
		)
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
