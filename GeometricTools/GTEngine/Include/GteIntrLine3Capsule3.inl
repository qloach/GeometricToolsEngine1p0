// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
typename TIQuery<Real, Line3<Real>, Capsule3<Real>>::Result
TIQuery<Real, Line3<Real>, Capsule3<Real>>::operator()(
    Line3<Real> const& line, Capsule3<Real> const& capsule)
{
    Result result;
    DCPQuery<Real, Line3<Real>, Segment3<Real>> lsQuery;
    auto lsResult = lsQuery(line, capsule.segment);
    result.intersect = (lsResult.distance <= capsule.radius);
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
typename FIQuery<Real, Line3<Real>, Capsule3<Real>>::Result
FIQuery<Real, Line3<Real>, Capsule3<Real>>::operator()(
    Line3<Real> const& line, Capsule3<Real> const& capsule)
{
    Result result;
    DoQuery(line.origin, line.direction, capsule, result);
    for (int i = 0; i < result.numIntersections; ++i)
    {
        result.point[i] = line.origin + result.parameter[i] * line.direction;
    }
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
void FIQuery<Real, Line3<Real>, Capsule3<Real>>::DoQuery(
    Vector3<Real> const& lineOrigin, Vector3<Real> const& lineDirection,
    Capsule3<Real> const& capsule, Result& result)
{
    // Create a coordinate system for the capsule.  In this system, the
    // capsule segment center C is the origin and the capsule axis direction
    // W is the z-axis.  U and V are the other coordinate axis directions.
    // If P = x*U+y*V+z*W, the cylinder containing the capsule wall is
    // x^2 + y^2 = r^2, where r is the capsule radius.  The finite cylinder
    // that makes up the capsule minus its hemispherical end caps has z-values
    // |z| <= e, where e is the extent of the capsule segment.  The top
    // hemisphere cap is x^2+y^2+(z-e)^2 = r^2 for z >= e, and the bottom
    // hemisphere cap is x^2+y^2+(z+e)^2 = r^2 for z <= -e.

    Vector3<Real> segOrigin, segDirection;
    Real segExtent;
    capsule.segment.GetCenteredForm(segOrigin, segDirection, segExtent);
    Vector3<Real> basis[3];  // {W, U, V}
    basis[0] = segDirection;
    ComputeOrthogonalComplement(1, basis);
    Real rSqr = capsule.radius * capsule.radius;

    // Convert incoming line origin to capsule coordinates.
    Vector3<Real> diff = lineOrigin - segOrigin;
    Vector3<Real> P(Dot(basis[1], diff), Dot(basis[2], diff),
        Dot(basis[0], diff));

    // Get the z-value, in capsule coordinates, of the incoming line's
    // unit-length direction.
    Real dz = Dot(basis[0], lineDirection);
    if (std::abs(dz) == (Real)1)
    {
        // The line is parallel to the capsule axis.  Determine whether the
        // line intersects the capsule hemispheres.
        Real radialSqrDist = rSqr - P[0]*P[0] - P[1]*P[1];
        if (radialSqrDist < (Real)0)
        {
            // The line is outside the cylinder of the capsule, so there is no
            // intersection.
            result.intersect = false;
            result.numIntersections = 0;
            return;
        }

        // The line intersects the hemispherical caps.
        result.intersect = true;
        result.numIntersections = 2;
        Real zOffset = sqrt(radialSqrDist) + segExtent;
        if (dz > (Real)0)
        {
            result.parameter[0] = -P[2] - zOffset;
            result.parameter[1] = -P[2] + zOffset;
        }
        else
        {
            result.parameter[0] = P[2] - zOffset;
            result.parameter[1] = P[2] + zOffset;
        }
        return;
    }

    // Convert the incoming line unit-length direction to capsule coordinates.
    Vector3<Real> D(Dot(basis[1], lineDirection),
        Dot(basis[2], lineDirection), dz);

    // Test intersection of line P+t*D with infinite cylinder x^2+y^2 = r^2.
    // This reduces to computing the roots of a quadratic equation.  If
    // P = (px,py,pz) and D = (dx,dy,dz), then the quadratic equation is
    //   (dx^2+dy^2)*t^2 + 2*(px*dx+py*dy)*t + (px^2+py^2-r^2) = 0
    Real a0 = P[0]*P[0] + P[1]*P[1] - rSqr;
    Real a1 = P[0]*D[0] + P[1]*D[1];
    Real a2 = D[0]*D[0] + D[1]*D[1];
    Real discr = a1*a1 - a0*a2;
    if (discr < (Real)0)
    {
        // The line does not intersect the infinite cylinder.
        result.intersect = false;
        result.numIntersections = 0;
        return;
    }

    Real root, inv, tValue, zValue;
    result.numIntersections = 0;
    if (discr > (Real)0)
    {
        // The line intersects the infinite cylinder in two places.
        root = sqrt(discr);
        inv = ((Real)1)/a2;
        tValue = (-a1 - root)*inv;
        zValue = P[2] + tValue*D[2];
        if (std::abs(zValue) <= segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
        }

        tValue = (-a1 + root)*inv;
        zValue = P[2] + tValue*D[2];
        if (std::abs(zValue) <= segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
        }

        if (result.numIntersections == 2)
        {
            // The line intersects the capsule wall in two places.
            result.intersect = true;
            return;
        }
    }
    else
    {
        // The line is tangent to the infinite cylinder but intersects the
        // cylinder in a single point.
        tValue = -a1/a2;
        zValue = P[2] + tValue*D[2];
        if (std::abs(zValue) <= segExtent)
        {
            result.intersect = true;
            result.numIntersections = 1;
            result.parameter[0] = tValue;
            // Used by derived classes.
            result.parameter[1] = result.parameter[0];
            return;
        }
    }

    // Test intersection with bottom hemisphere.  The quadratic equation is
    //   t^2 + 2*(px*dx+py*dy+(pz+e)*dz)*t + (px^2+py^2+(pz+e)^2-r^2) = 0
    // Use the fact that currently a1 = px*dx+py*dy and a0 = px^2+py^2-r^2.
    // The leading coefficient is a2 = 1, so no need to include in the
    // construction.
    Real PZpE = P[2] + segExtent;
    a1 += PZpE*D[2];
    a0 += PZpE*PZpE;
    discr = a1*a1 - a0;
    if (discr > (Real)0)
    {
        root = sqrt(discr);
        tValue = -a1 - root;
        zValue = P[2] + tValue*D[2];
        if (zValue <= -segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
            if (result.numIntersections == 2)
            {
                result.intersect = true;
                if (result.parameter[0] > result.parameter[1])
                {
                    std::swap(result.parameter[0], result.parameter[1]);
                }
                return;
            }
        }

        tValue = -a1 + root;
        zValue = P[2] + tValue*D[2];
        if (zValue <= -segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
            if (result.numIntersections == 2)
            {
                result.intersect = true;
                if (result.parameter[0] > result.parameter[1])
                {
                    std::swap(result.parameter[0], result.parameter[1]);
                }
                return;
            }
        }
    }
    else if (discr == (Real)0)
    {
        tValue = -a1;
        zValue = P[2] + tValue*D[2];
        if (zValue <= -segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
            if (result.numIntersections == 2)
            {
                result.intersect = true;
                if (result.parameter[0] > result.parameter[1])
                {
                    std::swap(result.parameter[0], result.parameter[1]);
                }
                return;
            }
        }
    }

    // Test intersection with top hemisphere.  The quadratic equation is
    //   t^2 + 2*(px*dx+py*dy+(pz-e)*dz)*t + (px^2+py^2+(pz-e)^2-r^2) = 0
    // Use the fact that currently a1 = px*dx+py*dy+(pz+e)*dz and
    // a0 = px^2+py^2+(pz+e)^2-r^2.  The leading coefficient is a2 = 1, so
    // no need to include in the construction.
    a1 -= ((Real)2)*segExtent*D[2];
    a0 -= ((Real)4)*segExtent*P[2];
    discr = a1*a1 - a0;
    if (discr > (Real)0)
    {
        root = sqrt(discr);
        tValue = -a1 - root;
        zValue = P[2] + tValue*D[2];
        if (zValue >= segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
            if (result.numIntersections == 2)
            {
                result.intersect = true;
                if (result.parameter[0] > result.parameter[1])
                {
                    std::swap(result.parameter[0], result.parameter[1]);
                }
                return;
            }
        }

        tValue = -a1 + root;
        zValue = P[2] + tValue*D[2];
        if (zValue >= segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
            if (result.numIntersections == 2)
            {
                result.intersect = true;
                if (result.parameter[0] > result.parameter[1])
                {
                    std::swap(result.parameter[0], result.parameter[1]);
                }
                return;
            }
        }
    }
    else if (discr == (Real)0)
    {
        tValue = -a1;
        zValue = P[2] + tValue*D[2];
        if (zValue >= segExtent)
        {
            result.parameter[result.numIntersections++] = tValue;
            if (result.numIntersections == 2)
            {
                result.intersect = true;
                if (result.parameter[0] > result.parameter[1])
                {
                    std::swap(result.parameter[0], result.parameter[1]);
                }
                return;
            }
        }
    }

    if (result.numIntersections == 1)
    {
        // Used by derived classes.
        result.parameter[1] = result.parameter[0];
    }
}
//----------------------------------------------------------------------------
