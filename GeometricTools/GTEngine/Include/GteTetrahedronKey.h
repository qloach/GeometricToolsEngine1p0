// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

#pragma once

#include "GteFeatureKey.h"
#include <array>

namespace gte
{

template <bool Ordered>
class TetrahedronKey : public FeatureKey<4, Ordered>
{
public:
    // An ordered tetrahedron has V[0] = min(v0,v1,v2,v3).  Let {u1,u2,u3} be
    // the set of inputs excluding the one assigned to V[0] and define
    // V[1] = min(u1,u2,u3).  Choose (V[1],V[2],V[3]) to be a permutation of
    // (u1,u2,u3) so that the final storage is one of
    //   (v0,v1,v2,v3), (v0,v2,v3,v1), (v0,v3,v1,v2)
    //   (v1,v3,v2,v0), (v1,v2,v0,v3), (v1,v0,v3,v2)
    //   (v2,v3,v0,v1), (v2,v0,v1,v3), (v2,v1,v3,v0)
    //   (v3,v1,v0,v2), (v3,v0,v2,v1), (v3,v2,v1,v0)
    // The idea is that if v0 corresponds to (1,0,0,0), v1 corresponds to
    // (0,1,0,0), v2 corresponds to (0,0,1,0), and v3 corresponds to
    // (0,0,0,1), the ordering (v0,v1,v2,v3) corresponds to the 4x4 identity
    // matrix I; the rows are the specified 4-tuples.  The permutation
    // (V[0],V[1],V[2],V[3]) induces a permutation of the rows of the identity
    // matrix to form a permutation matrix P with det(P) = 1 = det(I).
    //
    // An unordered tetrahedron stores a permutation of (v0,v1,v2,v3) so that
    // V[0] < V[1] < V[2] < V[3].
    TetrahedronKey(int v0 = -1, int v1 = -1, int v2 = -1, int v3 = -1);

    // Indexing for the vertices of the triangle opposite a vertex.  The
    // triangle opposite vertex j is
    //   <oppositeFace[j][0], oppositeFace[j][1], oppositeFace[j][2]>
    // and is listed in counterclockwise order when viewed from outside the
    // tetrahedron.
    static std::array<int, 3> const oppositeFace[4];
};

}
