// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
template <int D>
inline Real ASinEstimate<Real>::Degree(Real x)
{
    return (Real)GTE_C_HALF_PI - ACosEstimate<Real>::Degree<D>(x);
}
//----------------------------------------------------------------------------
