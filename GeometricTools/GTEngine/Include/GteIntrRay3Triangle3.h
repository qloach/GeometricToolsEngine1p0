// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteTIQuery.h"
#include "GteFIQuery.h"
#include "GteRay3.h"
#include "GteTriangle3.h"

namespace gte
{

template <typename Real>
class TIQuery<Real, Ray3<Real>, Triangle3<Real>>
{
public:
    struct Result
    {
        bool intersect;
    };

    Result operator()(Ray3<Real> const& ray, Triangle3<Real> const& triangle);
};

template <typename Real>
class FIQuery<Real, Ray3<Real>, Triangle3<Real>>
{
public:
    struct Result
    {
        Result();

        bool intersect;
        Real parameter;
        Real triangleBary[3];
        Vector3<Real> point;
    };

    Result operator()(Ray3<Real> const& ray, Triangle3<Real> const& triangle);
};

#include "GteIntrRay3Triangle3.inl"

}
