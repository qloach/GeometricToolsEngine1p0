// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
typename DCPQuery<Real, Vector3<Real>, Tetrahedron3<Real>>::Result
DCPQuery<Real, Vector3<Real>, Tetrahedron3<Real>>::operator()(
    Vector3<Real> const& point, Tetrahedron3<Real> const& tetrahedron)
{
    Result result;

    // Construct the planes for the faces of the tetrahedron.  The normals
    // are outer pointing, but specified not to be unit length.  We only need
    // to know sidedness of the query point, so we will save cycles by not
    // computing unit-length normals.
    Plane3<Real> planes[4];
    tetrahedron.GetPlanes(planes);

    // Determine which faces are visible to the query point.  Only these
    // need to be processed by point-to-triangle distance queries.
    result.sqrDistance = std::numeric_limits<Real>::max();
    result.tetrahedronClosestPoint = Vector3<Real>::Zero();
    for (int i = 0; i < 4; ++i)
    {
        if (Dot(planes[i].normal, point) >= planes[i].constant)
        {
            int indices[3] = { 0, 0, 0 };
            tetrahedron.GetFaceIndices(i, indices);
            Triangle3<Real> triangle(
                tetrahedron.v[indices[0]],
                tetrahedron.v[indices[1]],
                tetrahedron.v[indices[2]]);

            DCPQuery<Real, Vector3<Real>, Triangle3<Real>> query;
            auto ptResult = query(point, triangle);
            if (ptResult.sqrDistance < result.sqrDistance)
            {
                result.sqrDistance = ptResult.sqrDistance;
                result.tetrahedronClosestPoint =
                    ptResult.triangleClosestPoint;
            }
        }
    }

    if (result.sqrDistance == std::numeric_limits<Real>::max())
    {
        // The query point is inside the solid tetrahedron.  Report a zero
        // distance.  The closest points are identical.
        result.sqrDistance = (Real)0;
        result.tetrahedronClosestPoint = point;
    }
    result.distance = sqrt(result.sqrDistance);
    return result;
}
//----------------------------------------------------------------------------
