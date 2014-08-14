// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
bool MinimumVolumeSphere3<InputType, ComputeType>::operator()(int numPoints,
    Vector3<InputType> const* points, Sphere3<InputType>& minimal)
{
    if (numPoints >= 1 && points)
    {
        // Function array to avoid switch statement in the main loop.
        std::function<Sphere3<ComputeType>(int)> update[5];
        update[1] = [this](int i) { return UpdateSupport1(i); };
        update[2] = [this](int i) { return UpdateSupport2(i); };
        update[3] = [this](int i) { return UpdateSupport3(i); };
        update[4] = [this](int i) { return UpdateSupport4(i); };

        // Process only the unique points.
        std::vector<int> permuted(numPoints);
        for (int i = 0; i < numPoints; ++i)
        {
            permuted[i] = i;
        }
        std::sort(permuted.begin(), permuted.end(),
            [points](int i0, int i1) { return points[i0] < points[i1]; });
        auto end = std::unique(permuted.begin(), permuted.end(),
            [points](int i0, int i1) { return points[i0] == points[i1]; });
        permuted.erase(end, permuted.end());
        numPoints = static_cast<int>(permuted.size());

        // Create a random permutation of the points.
        std::shuffle(permuted.begin(), permuted.end(),
            std::default_random_engine());

        // Convert to the compute type, which is a simple copy when
        // ComputeType is the same as InputType.
        mComputePoints.resize(numPoints);
        for (int i = 0; i < numPoints; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                mComputePoints[i][j] = points[permuted[i]][j];
            }
        }

        // Start with the first point.
        Sphere3<ComputeType> ctMinimal = ExactSphere1(0);
        mNumSupport = 1;
        mSupport[0] = 0;

        // The loop restarts from the beginning of the point list each time
        // the sphere needs updating.  Linus K�llberg (Computer Science at
        // M�lardalen University in Sweden) discovered that performance is
        // better when the remaining points in the array are processed before
        // restarting.  The points processed before the point that caused the
        // update are likely to be enclosed by the new sphere (or near the
        // sphere boundary) because they were enclosed by the previous sphere.
        // The chances are better that points after the current one will cause
        // growth of the bounding sphere.
        for (int i = 1 % numPoints, n = 0; i != n; i = (i + 1) % numPoints)
        {
            if (!SupportContains(i))
            {
                if (!Contains(i, ctMinimal))
                {
                    Sphere3<ComputeType> sphere = update[mNumSupport](i);
                    if (sphere.radius > ctMinimal.radius)
                    {
                        ctMinimal = sphere;
                        n = i;
                    }
                }
            }
        }

        for (int j = 0; j < 3; ++j)
        {
            minimal.center[j] = static_cast<InputType>(ctMinimal.center[j]);
        }
        minimal.radius = static_cast<InputType>(ctMinimal.radius);
        minimal.radius = sqrt(minimal.radius);

        for (int i = 0; i < mNumSupport; ++i)
        {
            mSupport[i] = permuted[mSupport[i]];
        }
        return true;
    }
    else
    {
        LogError("Input must contain points.");
        minimal.center = Vector3<InputType>::Zero();
        minimal.radius = std::numeric_limits<InputType>::max();
        return false;
    }
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType> inline
int MinimumVolumeSphere3<InputType, ComputeType>::GetNumSupport() const
{
    return mNumSupport;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType> inline
std::array<int, 4> const&
MinimumVolumeSphere3<InputType, ComputeType>::GetSupport() const
{
    return mSupport;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
bool MinimumVolumeSphere3<InputType, ComputeType>::Contains(int i,
    Sphere3<ComputeType> const& sphere) const
{
    // NOTE:  In this algorithm, sphere.radius is the *squared radius*.
    Vector3<ComputeType> diff = mComputePoints[i] - sphere.center;
    return Dot(diff, diff) <= sphere.radius;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
ExactSphere1(int i0) const
{
    Sphere3<ComputeType> minimal;
    minimal.center = mComputePoints[i0];
    minimal.radius = (ComputeType)0;
    return minimal;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
ExactSphere2(int i0, int i1) const
{
    Vector3<ComputeType> const& P0 = mComputePoints[i0];
    Vector3<ComputeType> const& P1 = mComputePoints[i1];
    Sphere3<ComputeType> minimal;
    minimal.center = ((ComputeType)0.5)*(P0 + P1);
    Vector3<ComputeType> diff = P1 - P0;
    minimal.radius = ((ComputeType)0.25)*Dot(diff, diff);
    return minimal;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
ExactSphere3(int i0, int i1, int i2) const
{
    // Compute the 3D circle containing P0, P1, and P2.  The center in
    // barycentric coordinates is C = x0*P0 + x1*P1 + x2*P2, where
    // x0 + x1 + x2 = 1.  The center is equidistant from the three points,
    // so |C - P0| = |C - P1| = |C - P2| = R, where R is the radius of the
    // circle.  From these conditions,
    //   C - P0 = x0*E0 + x1*E1 - E0
    //   C - P1 = x0*E0 + x1*E1 - E1
    //   C - P2 = x0*E0 + x1*E1
    // where E0 = P0 - P2 and E1 = P1 - P2, which leads to
    //   r^2 = |x0*E0 + x1*E1|^2 - 2*Dot(E0, x0*E0 + x1*E1) + |E0|^2
    //   r^2 = |x0*E0 + x1*E1|^2 - 2*Dot(E1, x0*E0 + x1*E1) + |E1|^2
    //   r^2 = |x0*E0 + x1*E1|^2
    // Subtracting the last equation from the first two and writing the
    // equations as a linear system,
    //
    // +-                     -++   -+       +-          -+
    // | Dot(E0,E0) Dot(E0,E1) || x0 | = 0.5 | Dot(E0,E0) |
    // | Dot(E1,E0) Dot(E1,E1) || x1 |       | Dot(E1,E1) |
    // +-                     -++   -+       +-          -+
    //
    // The following code solves this system for x0 and x1 and then evaluates
    // the third equation in r^2 to obtain r.

    Vector3<ComputeType> const& P0 = mComputePoints[i0];
    Vector3<ComputeType> const& P1 = mComputePoints[i1];
    Vector3<ComputeType> const& P2 = mComputePoints[i2];

    Vector3<ComputeType> E0 = P0 - P2;
    Vector3<ComputeType> E1 = P1 - P2;

    Matrix2x2<ComputeType> A;
    A(0, 0) = Dot(E0, E0);
    A(0, 1) = Dot(E0, E1);
    A(1, 0) = A(0, 1);
    A(1, 1) = Dot(E1, E1);

    ComputeType const half = (ComputeType)0.5;
    Vector2<ComputeType> B(half*A(0, 0), half*A(1, 1));

    Sphere3<ComputeType> minimal;
    Vector2<ComputeType> X;
    if (LinearSystem<ComputeType>::Solve(A, B, X))
    {
        ComputeType x2 = (ComputeType)1 - X[0] - X[1];
        minimal.center = X[0] * P0 + X[1] * P1 + x2 *P2;
        Vector3<ComputeType> tmp = X[0] * E0 + X[1] * E1;
        minimal.radius = Dot(tmp, tmp);
    }
    else
    {
        minimal.center = Vector3<ComputeType>::Zero();
        minimal.radius = (ComputeType)std::numeric_limits<InputType>::max();
    }
    return minimal;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
ExactSphere4(int i0, int i1, int i2, int i3) const
{
    // Compute the sphere containing P0, P1, P2, and p3.  The center in
    // barycentric coordinates is C = x0*P0 + x1*P1 + x2*P2 + x3*P3, where
    // x0 + x1 + x2 + x3 = 1.  The center is equidistant from the three
    // points, so |C - P0| = |C - P1| = |C - P2| = |C - P3| = R, where R is
    // the radius of the sphere.  From these conditions,
    //   C - P0 = x0*E0 + x1*E1 + x2*E2 - E0
    //   C - P1 = x0*E0 + x1*E1 + x2*E2 - E1
    //   C - P2 = x0*E0 + x1*E1 + x2*E2 - E2
    //   C - P3 = x0*E0 + x1*E1 + x2*E2
    // where E0 = P0 - P3, E1 = P1 - P3, and E2 = P2 - P3, which leads to
    //   r^2 = |x0*E0+x1*E1+x2*E2|^2 - 2*Dot(E0,x0*E0+x1*E1+x2*E2) + |E0|^2
    //   r^2 = |x0*E0+x1*E1+x2*E2|^2 - 2*Dot(E1,x0*E0+x1*E1+x2*E2) + |E1|^2
    //   r^2 = |x0*E0+x1*E1+x2*E2|^2 - 2*Dot(E2,x0*E0+x1*E1+x2*E2) + |E2|^2
    //   r^2 = |x0*E0+x1*E1+x2*E2|^2
    // Subtracting the last equation from the first three and writing the
    // equations as a linear system,
    //
    // +-                                -++   -+       +-          -+
    // | Dot(E0,E0) Dot(E0,E1) Dot(E0,E2) || x0 | = 0.5 | Dot(E0,E0) |
    // | Dot(E1,E0) Dot(E1,E1) Dot(E1,E2) || x1 |       | Dot(E1,E1) |
    // | Dot(E2,E0) Dot(E2,E1) Dot(E2,E2) || x2 |       | Dot(E2,E2) |
    // +-                                -++   -+       +-          -+
    //
    // The following code solves this system for x0, x1, and x2 and then
    // evaluates the fourth equation in r^2 to obtain r.

    Vector3<ComputeType> const& P0 = mComputePoints[i0];
    Vector3<ComputeType> const& P1 = mComputePoints[i1];
    Vector3<ComputeType> const& P2 = mComputePoints[i2];
    Vector3<ComputeType> const& P3 = mComputePoints[i3];

    Vector3<ComputeType> E0 = P0 - P3;
    Vector3<ComputeType> E1 = P1 - P3;
    Vector3<ComputeType> E2 = P2 - P3;

    Matrix3x3<ComputeType> A;
    A(0, 0) = Dot(E0, E0);
    A(0, 1) = Dot(E0, E1);
    A(0, 2) = Dot(E0, E2);
    A(1, 0) = A(0, 1);
    A(1, 1) = Dot(E1, E1);
    A(1, 2) = Dot(E1, E2);
    A(2, 0) = A(0, 2);
    A(2, 1) = A(1, 2);
    A(2, 2) = Dot(E2, E2);

    ComputeType const half = (ComputeType)0.5;
    Vector3<ComputeType> B(half*A(0, 0), half*A(1, 1), half*A(2, 2));

    Sphere3<ComputeType> minimal;
    Vector3<ComputeType> X;
    if (LinearSystem<ComputeType>::Solve(A, B, X))
    {
        ComputeType x3 = (ComputeType)1 - X[0] - X[1] - X[2];
        minimal.center = X[0] * P0 + X[1] * P1 + X[2] * P2 + x3 * P3;
        Vector3<ComputeType> tmp = X[0] * E0 + X[1] * E1 + X[2] * E2;
        minimal.radius = Dot(tmp, tmp);
    }
    else
    {
        minimal.center = Vector3<ComputeType>::Zero();
        minimal.radius = (ComputeType)std::numeric_limits<InputType>::max();
    }
    return minimal;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
UpdateSupport1(int i)
{
    Sphere3<ComputeType> minimal = ExactSphere2(mSupport[0], i);
    mNumSupport = 2;
    mSupport[1] = i;
    return minimal;
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
UpdateSupport2(int i)
{
    // Permutations of type 2, used for calling ExactSphere2(...).
    int const numType2 = 2;
    int const type2[numType2][2] =
    {
        {0, /*2*/ 1},
        {1, /*2*/ 0}
    };

    // Permutations of type 3, used for calling ExactSphere3(...).
    int const numType3 = 1;  // {0, 1, 2}

    Sphere3<ComputeType> sphere[numType2 + numType3];
    ComputeType minRSqr = (ComputeType)std::numeric_limits<InputType>::max();
    int iSphere = 0, iMinRSqr = -1;
    int k0, k1;

    // Permutations of type 2.
    for (int j = 0; j < numType2; ++j, ++iSphere)
    {
        k0 = mSupport[type2[j][0]];
        sphere[iSphere] = ExactSphere2(k0, i);
        if (sphere[iSphere].radius < minRSqr)
        {
            k1 = mSupport[type2[j][1]];
            if (Contains(k1, sphere[iSphere]))
            {
                minRSqr = sphere[iSphere].radius;
                iMinRSqr = iSphere;
            }
        }
    }

    // Permutations of type 3.
    k0 = mSupport[0];
    k1 = mSupport[1];
    sphere[iSphere] = ExactSphere3(k0, k1, i);
    if (sphere[iSphere].radius < minRSqr)
    {
        minRSqr = sphere[iSphere].radius;
        iMinRSqr = iSphere;
    }

    // For exact arithmetic, iMinRSqr >= 0, but for floating-point arithmetic,
    // round-off errors can lead to iMinRSqr == -1.  When this happens, just
    // use the sphere for the current support.
    if (iMinRSqr == -1)
    {
        iMinRSqr = iSphere;
    }

    switch (iMinRSqr)
    {
    case 0:
        mSupport[1] = i;
        break;
    case 1:
        mSupport[0] = i;
        break;
    case 2:
        mNumSupport = 3;
        mSupport[2] = i;
        break;
    }

    return sphere[iMinRSqr];
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
UpdateSupport3(int i)
{
    // Permutations of type 2, used for calling ExactSphere2(...).
    int const numType2 = 3;
    int const type2[numType2][3] =
    {
        {0, /*3*/ 1, 2},
        {1, /*3*/ 0, 2},
        {2, /*3*/ 0, 1}
    };

    // Permutations of type 3, used for calling ExactSphere3(...).
    int const numType3 = 3;
    int const type3[numType3][3] =
    {
        {0, 1, /*3*/ 2},
        {0, 2, /*3*/ 1},
        {1, 2, /*3*/ 0}
    };

    // Permutations of type 4, used for calling ExactSphere4(...).
    int const numType4 = 1;  // {0, 1, 2, 3}

    Sphere3<ComputeType> sphere[numType2 + numType3 + numType4];
    ComputeType minRSqr = (ComputeType)std::numeric_limits<InputType>::max();
    int iSphere = 0, iMinRSqr = -1;
    int k0, k1, k2;

    // Permutations of type 2.
    for (int j = 0; j < numType2; ++j, ++iSphere)
    {
        k0 = mSupport[type2[j][0]];
        sphere[iSphere] = ExactSphere2(k0, i);
        if (sphere[iSphere].radius < minRSqr)
        {
            k1 = mSupport[type2[j][1]];
            k2 = mSupport[type2[j][2]];
            if (Contains(k1, sphere[iSphere])
            &&  Contains(k2, sphere[iSphere]))
            {
                minRSqr = sphere[iSphere].radius;
                iMinRSqr = iSphere;
            }
        }
    }

    // Permutations of type 3.
    for (int j = 0; j < numType3; ++j, ++iSphere)
    {
        k0 = mSupport[type3[j][0]];
        k1 = mSupport[type3[j][1]];
        sphere[iSphere] = ExactSphere3(k0, k1, i);
        if (sphere[iSphere].radius < minRSqr)
        {
            k2 = mSupport[type3[j][2]];
            if (Contains(k2, sphere[iSphere]))
            {
                minRSqr = sphere[iSphere].radius;
                iMinRSqr = iSphere;
            }
        }
    }

    // Permutations of type 4.
    k0 = mSupport[0];
    k1 = mSupport[1];
    k2 = mSupport[2];
    sphere[iSphere] = ExactSphere4(k0, k1, k2, i);
    if (sphere[iSphere].radius < minRSqr)
    {
        minRSqr = sphere[iSphere].radius;
        iMinRSqr = iSphere;
    }

    // For exact arithmetic, iMinRSqr >= 0, but for floating-point arithmetic,
    // round-off errors can lead to iMinRSqr == -1.  When this happens, just
    // use the sphere for the current support.
    if (iMinRSqr == -1)
    {
        iMinRSqr = iSphere;
    }

    switch (iMinRSqr)
    {
    case 0:
        mNumSupport = 2;
        mSupport[1] = i;
        break;
    case 1:
        mNumSupport = 2;
        mSupport[0] = i;
        break;
    case 2:
        mNumSupport = 2;
        mSupport[0] = mSupport[2];
        mSupport[1] = i;
        break;
    case 3:
        mSupport[2] = i;
        break;
    case 4:
        mSupport[1] = i;
        break;
    case 5:
        mSupport[0] = i;
        break;
    case 6:
        mNumSupport = 4;
        mSupport[3] = i;
        break;
    }

    return sphere[iMinRSqr];
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
Sphere3<ComputeType> MinimumVolumeSphere3<InputType, ComputeType>::
UpdateSupport4(int i)
{
    // Permutations of type 2, used for calling ExactSphere2(...).
    int const numType2 = 4;
    int const type2[numType2][4] =
    {
        {0, /*4*/ 1, 2, 3},
        {1, /*4*/ 0, 2, 3},
        {2, /*4*/ 0, 1, 3},
        {3, /*4*/ 0, 1, 2}
    };

    // Permutations of type 3, used for calling ExactSphere3(...).
    int const numType3 = 6;
    int const type3[numType3][4] =
    {
        {0, 1, /*4*/ 2, 3},
        {0, 2, /*4*/ 1, 3},
        {0, 3, /*4*/ 1, 2},
        {1, 2, /*4*/ 0, 3},
        {1, 3, /*4*/ 0, 2},
        {2, 3, /*4*/ 0, 1}
    };

    // Permutations of type 4, used for calling ExactSphere4(...).
    int const numType4 = 4;
    int const type4[numType4][4] =
    {
        {0, 1, 2, /*4*/ 3},
        {0, 1, 3, /*4*/ 2},
        {0, 2, 3, /*4*/ 1},
        {1, 2, 3, /*4*/ 0}
    };

    Sphere3<ComputeType> sphere[numType2 + numType3 + numType4];
    ComputeType minRSqr = (ComputeType)std::numeric_limits<InputType>::max();
    int iSphere = 0, iMinRSqr = -1;
    int k0, k1, k2, k3;

    // Permutations of type 2.
    for (int j = 0; j < numType2; ++j, ++iSphere)
    {
        k0 = mSupport[type2[j][0]];
        sphere[iSphere] = ExactSphere2(k0, i);
        if (sphere[iSphere].radius < minRSqr)
        {
            k1 = mSupport[type2[j][1]];
            k2 = mSupport[type2[j][2]];
            k3 = mSupport[type2[j][3]];
            if (Contains(k1, sphere[iSphere])
            &&  Contains(k2, sphere[iSphere])
            &&  Contains(k3, sphere[iSphere]))
            {
                minRSqr = sphere[iSphere].radius;
                iMinRSqr = iSphere;
            }
        }
    }

    // Permutations of type 3.
    for (int j = 0; j < numType3; ++j, ++iSphere)
    {
        k0 = mSupport[type3[j][0]];
        k1 = mSupport[type3[j][1]];
        sphere[iSphere] = ExactSphere3(k0, k1, i);
        if (sphere[iSphere].radius < minRSqr)
        {
            k2 = mSupport[type3[j][2]];
            k3 = mSupport[type3[j][3]];
            if (Contains(k2, sphere[iSphere])
            &&  Contains(k3, sphere[iSphere]))
            {
                minRSqr = sphere[iSphere].radius;
                iMinRSqr = iSphere;
            }
        }
    }

    // Permutations of type 4.
    for (int j = 0; j < numType4; ++j, ++iSphere)
    {
        k0 = mSupport[type4[j][0]];
        k1 = mSupport[type4[j][1]];
        k2 = mSupport[type4[j][2]];
        sphere[iSphere] = ExactSphere4(k0, k1, k2, i);
        if (sphere[iSphere].radius < minRSqr)
        {
            k3 = mSupport[type4[j][3]];
            if (Contains(k3, sphere[iSphere]))
            {
                minRSqr = sphere[iSphere].radius;
                iMinRSqr = iSphere;
            }
        }
    }

    // For exact arithmetic, iMinRSqr >= 0, but for floating-point arithmetic,
    // round-off errors can lead to iMinRSqr == -1.  When this happens, just
    // use the sphere for the current support.
    if (iMinRSqr == -1)
    {
        iMinRSqr = iSphere;
    }

    switch (iMinRSqr)
    {
    case 0:
        mNumSupport = 2;
        mSupport[1] = i;
        break;
    case 1:
        mNumSupport = 2;
        mSupport[0] = i;
        break;
    case 2:
        mNumSupport = 2;
        mSupport[0] = mSupport[2];
        mSupport[1] = i;
        break;
    case 3:
        mNumSupport = 2;
        mSupport[0] = mSupport[3];
        mSupport[1] = i;
        break;
    case 4:
        mNumSupport = 3;
        mSupport[2] = i;
        break;
    case 5:
        mNumSupport = 3;
        mSupport[1] = i;
        break;
    case 6:
        mNumSupport = 3;
        mSupport[1] = mSupport[3];
        mSupport[2] = i;
        break;
    case 7:
        mNumSupport = 3;
        mSupport[0] = i;
        break;
    case 8:
        mNumSupport = 3;
        mSupport[0] = mSupport[3];
        mSupport[2] = i;
        break;
    case 9:
        mNumSupport = 3;
        mSupport[0] = mSupport[3];
        mSupport[1] = i;
        break;
    case 10:
        mSupport[3] = i;
        break;
    case 11:
        mSupport[2] = i;
        break;
    case 12:
        mSupport[1] = i;
        break;
    case 13:
        mSupport[0] = i;
        break;
    }

    return sphere[iMinRSqr];
}
//----------------------------------------------------------------------------
template <typename InputType, typename ComputeType>
bool MinimumVolumeSphere3<InputType, ComputeType>::SupportContains(int j)
    const
{
    for (int i = 0; i < mNumSupport; ++i)
    {
        if (j == mSupport[i])
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
