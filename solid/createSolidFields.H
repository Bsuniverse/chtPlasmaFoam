// Initialise solid field pointer lists
PtrList<coordinateSystem> coordinates(solidRegions.size());
PtrList<solidThermo> thermos(solidRegions.size());
PtrList<fv::options> solidHeatSources(solidRegions.size());
PtrList<volScalarField> betavSolid(solidRegions.size());
PtrList<volSymmTensorField> aniAlphas(solidRegions.size());
// Initialise electrical field pointers lists
PtrList<volScalarField> epSolid(solidRegions.size());
PtrList<volScalarField> rhocSolid(solidRegions.size());
PtrList<volScalarField> epsSolid(solidRegions.size());
PtrList<dimensionedScalar> eps0s(solidRegions.size());
PtrList<dimensionedScalar> epMaxs(solidRegions.size());
PtrList<dimensionedScalar> rhocMaxs(solidRegions.size());
PtrList<dimensionedScalar> omegas(solidRegions.size());
PtrList<dimensionedScalar> fpluses(solidRegions.size());
// Populate solid field pointer lists
forAll(solidRegions, i)
{
    Info<< "*** Reading solid mesh thermophysical properties for region "
        << solidRegions[i].name() << nl << endl;

	Info<< "*** Reading transport properties for region " << solidRegions[i].name() << nl << endl;

//			Reading userdefine parameters			// 
    IOdictionary userDefinedParameters
    (
        IOobject
        (
            "userDefinedParameters",
            runTime.constant(),
            solidRegions[i],
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
	IOdictionary solidProperties
	(
		IOobject
		{
			"transportProperties",
			runTime.constant(),
			solidRegions[i],
			IOobject::MUST_READ_IF_MODIFIED,
			IOobject::NO_WRITE
		}
	);

    Info<< "    Adding to epSolid\n" << endl;
    epSolid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "ep",
                runTime.timeName(),
                solidRegions[i],
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
            ),
            solidRegions[i]
        )
    );

    Info<< "    Adding to rhocSolid\n" << endl;
    rhocSolid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "rhoc",
                runTime.timeName(),
                solidRegions[i],
				IOobject::MUST_READ,
				IOobject::AUTO_WRITE
            ),
            solidRegions[i]
        )
    );

    Info<< "    Adding to epsSolid\n" << endl;
    epsSolid.set
    (
        i,
        new volScalarField
        (
            IOobject
            (
                "epsField",
                runTime.timeName(),
                solidRegions[i]
            ),
            solidRegions[i],
            dimensionedScalar
            (
                dimensionSet(0, 0, 0, 0, 0, 0, 0),    
                readScalar(solidProperties.lookup("eps"))
            )
        )
    );

    Info<< "    Adding to thermos\n" << endl;
    thermos.set(i, solidThermo::New(solidRegions[i]));

    Info<< "    Adding fvOptions\n" << endl;
    solidHeatSources.set
    (
        i,
        new fv::options(solidRegions[i])
    );

    if (!thermos[i].isotropic())
    {
        Info<< "    Adding coordinateSystems\n" << endl;
        coordinates.set
        (
            i,
            coordinateSystem::New(solidRegions[i], thermos[i])
        );

        tmp<volVectorField> tkappaByCp =
            thermos[i].Kappa()/thermos[i].Cp();

        aniAlphas.set
        (
            i,
            new volSymmTensorField
            (
                IOobject
                (
                    "Anialpha",
                    runTime.timeName(),
                    solidRegions[i],
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                solidRegions[i],
                dimensionedSymmTensor
                (
                    "zero",
                    tkappaByCp().dimensions(),
                    Zero
                ),
                zeroGradientFvPatchSymmTensorField::typeName
            )
        );

        aniAlphas[i].primitiveFieldRef() =
            coordinates[i].R().transformVector(tkappaByCp());
        aniAlphas[i].correctBoundaryConditions();

    }

    IOobject betavSolidIO
    (
        "betavSolid",
        runTime.timeName(),
        solidRegions[i],
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    );

    if (betavSolidIO.typeHeaderOk<volScalarField>(true))
    {
        betavSolid.set
        (
            i,
            new volScalarField(betavSolidIO, solidRegions[i])
        );
    }
    else
    {
        betavSolid.set
        (
            i,
            new volScalarField
            (
                IOobject
                (
                    "betavSolid",
                    runTime.timeName(),
                    solidRegions[i],
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                solidRegions[i],
                dimensionedScalar(dimless, scalar(1))
            )
        );
    }
}
