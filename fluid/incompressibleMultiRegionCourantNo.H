    scalar CoNum = -great;

    forAll(fluidRegions, regionI)
    {
        CoNum = max
        (
            incompressibleCourantNo
            (
                fluidRegions[regionI],
                runTime,
                phiFluid[regionI]
            ),
            CoNum
        );
    }
