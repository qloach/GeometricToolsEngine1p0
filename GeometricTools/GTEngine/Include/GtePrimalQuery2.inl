// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
PrimalQuery2<Real>::PrimalQuery2()
    :
    mNumVertices(0),
    mVertices(nullptr)
{
}
//----------------------------------------------------------------------------
template <typename Real>
PrimalQuery2<Real>::PrimalQuery2(int numVertices,
    Vector2<Real> const* vertices)
    :
    mNumVertices(numVertices),
    mVertices(vertices)
{
}
//----------------------------------------------------------------------------
template <typename Real> inline
void PrimalQuery2<Real>::Set(int numVertices, Vector2<Real> const* vertices)
{
    mNumVertices = numVertices;
    mVertices = vertices;
}
//----------------------------------------------------------------------------
template <typename Real> inline
int PrimalQuery2<Real>::GetNumVertices() const
{
    return mNumVertices;
}
//----------------------------------------------------------------------------
template <typename Real> inline
Vector2<Real> const* PrimalQuery2<Real>::GetVertices() const
{
    return mVertices;
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToLine(int i, int v0, int v1) const
{
    return ToLine(mVertices[i], v0, v1);
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToLine(Vector2<Real> const& test, int v0, int v1)
    const
{
    Vector2<Real> const& vec0 = mVertices[v0];
    Vector2<Real> const& vec1 = mVertices[v1];

    Real x0 = test[0] - vec0[0];
    Real y0 = test[1] - vec0[1];
    Real x1 = vec1[0] - vec0[0];
    Real y1 = vec1[1] - vec0[1];
    Real x0y1 = x0*y1;
    Real x1y0 = x1*y0;
    Real det = x0y1 - x1y0;
    Real zero = (Real)0;

    return (det > zero ? +1 : (det < zero ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToLine(int i, int v0, int v1, int& order) const
{
    return ToLine(mVertices[i], v0, v1, order);
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToLine(Vector2<Real> const& test, int v0, int v1,
    int& order) const
{
    Vector2<Real> const& vec0 = mVertices[v0];
    Vector2<Real> const& vec1 = mVertices[v1];

    Real x0 = test[0] - vec0[0];
    Real y0 = test[1] - vec0[1];
    Real x1 = vec1[0] - vec0[0];
    Real y1 = vec1[1] - vec0[1];
    Real x0y1 = x0*y1;
    Real x1y0 = x1*y0;
    Real det = x0y1 - x1y0;
    Real zero = (Real)0;

    if (det > zero)
    {
        order = +3;
        return +1;
    }

    if (det < zero)
    {
        order = -3;
        return -1;
    }

    Real x0x1 = x0*x1;
    Real y0y1 = y0*y1;
    Real dot = x0x1 + y0y1;
    if (dot == zero)
    {
        order = -1;
    }
    else if (dot < zero)
    {
        order = -2;
    }
    else
    {
        Real x0x0 = x0*x0;
        Real y0y0 = y0*y0;
        Real sqrlen = x0x0 + y0y0;
        if (dot == sqrlen)
        {
            order = +1;
        }
        else if (dot > sqrlen)
        {
            order = +2;
        }
        else
        {
            order = 0;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToTriangle(int i, int v0, int v1, int v2) const
{
    return ToTriangle(mVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToTriangle(Vector2<Real> const& test, int v0, int v1,
    int v2) const
{
    int sign0 = ToLine(test, v1, v2);
    if (sign0 > 0)
    {
        return +1;
    }

    int sign1 = ToLine(test, v0, v2);
    if (sign1 < 0)
    {
        return +1;
    }

    int sign2 = ToLine(test, v0, v1);
    if (sign2 > 0)
    {
        return +1;
    }

    return ((sign0 && sign1 && sign2) ? -1 : 0);
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToCircumcircle(int i, int v0, int v1, int v2) const
{
    return ToCircumcircle(mVertices[i], v0, v1, v2);
}
//----------------------------------------------------------------------------
template <typename Real>
int PrimalQuery2<Real>::ToCircumcircle(Vector2<Real> const& test, int v0,
    int v1, int v2) const
{
    Vector2<Real> const& vec0 = mVertices[v0];
    Vector2<Real> const& vec1 = mVertices[v1];
    Vector2<Real> const& vec2 = mVertices[v2];

    Real x0 = vec0[0] - test[0];
    Real y0 = vec0[1] - test[1];
    Real s00 = vec0[0] + test[0];
    Real s01 = vec0[1] + test[1];
    Real t00 = s00*x0;
    Real t01 = s01*y0;
    Real z0 = t00 + t01;

    Real x1 = vec1[0] - test[0];
    Real y1 = vec1[1] - test[1];
    Real s10 = vec1[0] + test[0];
    Real s11 = vec1[1] + test[1];
    Real t10 = s10*x1;
    Real t11 = s11*y1;
    Real z1 = t10 + t11;

    Real x2 = vec2[0] - test[0];
    Real y2 = vec2[1] - test[1];
    Real s20 = vec2[0] + test[0];
    Real s21 = vec2[1] + test[1];
    Real t20 = s20*x2;
    Real t21 = s21*y2;
    Real z2 = t20 + t21;

    Real y0z1 = y0*z1;
    Real y0z2 = y0*z2;
    Real y1z0 = y1*z0;
    Real y1z2 = y1*z2;
    Real y2z0 = y2*z0;
    Real y2z1 = y2*z1;
    Real c0 = y1z2 - y2z1;
    Real c1 = y2z0 - y0z2;
    Real c2 = y0z1 - y1z0;
    Real x0c0 = x0*c0;
    Real x1c1 = x1*c1;
    Real x2c2 = x2*c2;
    Real term = x0c0 + x1c1;
    Real det = term + x2c2;
    return (det < (Real)0 ? 1 : (det > (Real)0 ? -1 : 0));
}
//----------------------------------------------------------------------------
template <typename Real>
typename PrimalQuery2<Real>::OrderType PrimalQuery2<Real>::ToLineExtended(
    Vector2<Real> const& P, Vector2<Real> const& Q0, Vector2<Real> const& Q1)
    const
{
    Real const zero((Real)0);

    Real x0 = Q1[0] - Q0[0];
    Real y0 = Q1[1] - Q0[1];
    if (x0 == zero && y0 == zero)
    {
        return ORDER_Q0_EQUALS_Q1;
    }

    Real x1 = P[0] - Q0[0];
    Real y1 = P[1] - Q0[1];
    if (x1 == zero && y1 == zero)
    {
        return ORDER_P_EQUALS_Q0;
    }

    Real x2 = P[0] - Q1[0];
    Real y2 = P[1] - Q1[1];
    if (x2 == zero && y2 == zero)
    {
        return ORDER_P_EQUALS_Q1;
    }

    // The theoretical classification relies on computing exactly the sign of
    // the determinant.  Numerical roundoff errors can cause misclassification.
    Real x0y1 = x0 * y1;
    Real x1y0 = x1 * y0;
    Real det = x0y1 - x1y0;

    if (det != zero)
    {
        if (det > zero)
        {
            // The points form a counterclockwise triangle <P,Q0,Q1>.
            return ORDER_POSITIVE;
        }
        else
        {
            // The points form a clockwise triangle <P,Q1,Q0>.
            return ORDER_NEGATIVE;
        }
    }
    else
    {
        // The points are collinear; P is on the line through Q0 and Q1.
        Real x0x1 = x0 * x1;
        Real y0y1 = y0 * y1;
        Real dot = x0x1 + y0y1;
        if (dot < zero)
        {
            // The line ordering is <P,Q0,Q1>.
            return ORDER_COLLINEAR_LEFT;
        }

        Real x0x0 = x0 * x0;
        Real y0y0 = y0 * y0;
        Real sqrLength = x0x0 + y0y0;
        if (dot > sqrLength)
        {
            // The line ordering is <Q0,Q1,P>.
            return ORDER_COLLINEAR_RIGHT;
        }

        // The line ordering is <Q0,P,Q1> with P strictly between Q0 and Q1.
        return ORDER_COLLINEAR_CONTAIN;
    }
}
//----------------------------------------------------------------------------
