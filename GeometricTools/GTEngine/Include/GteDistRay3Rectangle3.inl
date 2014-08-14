// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
typename DCPQuery<Real, Ray3<Real>, Rectangle3<Real>>::Result
DCPQuery<Real, Ray3<Real>, Rectangle3<Real>>::operator()(
    Ray3<Real> const& ray, Rectangle3<Real> const& rectangle)
{
    Result result;

    Line3<Real> line(ray.origin, ray.direction);
    DCPQuery<Real, Line3<Real>, Rectangle3<Real>> lrQuery;
    auto lrResult = lrQuery(line, rectangle);

    if (lrResult.lineParameter >= (Real)0)
    {
        result.distance = lrResult.distance;
        result.sqrDistance = lrResult.sqrDistance;
        result.rayParameter = lrResult.lineParameter;
        result.rectangleParameter[0] = lrResult.rectangleParameter[0];
        result.rectangleParameter[1] = lrResult.rectangleParameter[1];
        result.closestPoint[0] = lrResult.closestPoint[0];
        result.closestPoint[1] = lrResult.closestPoint[1];
    }
    else
    {
        DCPQuery<Real, Vector3<Real>, Rectangle3<Real>> prQuery;
        auto prResult = prQuery(ray.origin, rectangle);
        result.distance = prResult.distance;
        result.sqrDistance = prResult.sqrDistance;
        result.rayParameter = (Real)0;
        result.rectangleParameter[0] = prResult.rectangleParameter[0];
        result.rectangleParameter[1] = prResult.rectangleParameter[1];
        result.closestPoint[0] = ray.origin;
        result.closestPoint[1] = prResult.rectangleClosestPoint;
    }
    return result;
}
//----------------------------------------------------------------------------
