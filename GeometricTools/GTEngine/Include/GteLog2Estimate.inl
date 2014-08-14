// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <typename Real>
template <int D>
inline Real Log2Estimate<Real>::Degree(Real x)
{
    Real t = x - (Real)1;  // t in (0,1]
    return Evaluate(degree<D>(), t);
}
//----------------------------------------------------------------------------
template <typename Real>
template <int D>
inline Real Log2Estimate<Real>::DegreeRR(Real x)
{
    int p;
    Real y = frexp(x, &p);  // y in [1/2,1)
    y = ((Real)2)*y;  // y in [1,2)
    --p;
    Real poly = Degree<D>(y);
    Real result = poly + (Real)p;
    return result;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<1>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG1_C1;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<2>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG2_C2;
    poly = (Real)GTE_C_LOG2_DEG2_C1 + poly * t;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<3>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG3_C3;
    poly = (Real)GTE_C_LOG2_DEG3_C2 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG3_C1 + poly * t;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<4>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG4_C4;
    poly = (Real)GTE_C_LOG2_DEG4_C3 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG4_C2 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG4_C1 + poly * t;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<5>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG5_C5;
    poly = (Real)GTE_C_LOG2_DEG5_C4 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG5_C3 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG5_C2 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG5_C1 + poly * t;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<6>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG6_C6;
    poly = (Real)GTE_C_LOG2_DEG6_C5 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG6_C4 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG6_C3 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG6_C2 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG6_C1 + poly * t;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<7>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG7_C7;
    poly = (Real)GTE_C_LOG2_DEG7_C6 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG7_C5 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG7_C4 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG7_C3 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG7_C2 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG7_C1 + poly * t;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
template <typename Real>
inline Real Log2Estimate<Real>::Evaluate(degree<8>, Real t)
{
    Real poly;
    poly = (Real)GTE_C_LOG2_DEG8_C8;
    poly = (Real)GTE_C_LOG2_DEG8_C7 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG8_C6 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG8_C5 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG8_C4 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG8_C3 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG8_C2 + poly * t;
    poly = (Real)GTE_C_LOG2_DEG8_C1 + poly * t;
    poly = poly * t;
    return poly;
}
//----------------------------------------------------------------------------
