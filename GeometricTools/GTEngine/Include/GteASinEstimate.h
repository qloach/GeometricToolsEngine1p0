// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteACosEstimate.h"

// Approximations to asin(x) of the form f(x) = pi/2 - sqrt(1-x)*p(x)
// where the polynomial p(x) of degree D minimizes the quantity
// maximum{|acos(x)/sqrt(1-x) - p(x)| : x in [0,1]} over all
// polynomials of degree D.  We use the identity asin(x) = pi/2 - acos(x).

namespace gte
{

template <typename Real>
class ASinEstimate
{
public:
    // The input constraint is x in [0,1].  For example,
    //   float x; // in [0,1]
    //   float result = ASinEstimate<float>::Degree<3>(x);
    template <int D>
    inline static Real Degree(Real x);
};

#include "GteASinEstimate.inl"

}
