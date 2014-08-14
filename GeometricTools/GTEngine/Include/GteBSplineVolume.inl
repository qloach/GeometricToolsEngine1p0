// Geometric Tools LLC, Redmond WA 98052
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// File Version: 1.0.0 (2014/08/11)

//----------------------------------------------------------------------------
template <int N, typename Real>
BSplineVolume<N, Real>::~BSplineVolume()
{
}
//----------------------------------------------------------------------------
template <int N, typename Real>
BSplineVolume<N, Real>::BSplineVolume(
    BasisFunctionInput<Real> const input[3],
    Vector<N, Real> const* controls)
    :
    mConstructed(false)
{
    if (!controls)
    {
        LogError("Invalid controls pointer.");
        return;
    }

    for (int i = 0; i < 3; ++i)
    {
        mNumControls[i] = input[i].numControls;
        mBasisFunction[i].Create(input[i]);
        if (!mBasisFunction[i])
        {
            // Errors were already generated during construction of the
            // basis functions.
            return;
        }
    }

    // The replication of control points for periodic splines is avoided
    // by wrapping the i-loop index in Evaluate.
    int numControls = mNumControls[0] * mNumControls[1] * mNumControls[2];
    mControls.resize(numControls);
    std::copy(controls, controls + numControls, mControls.begin());
    mConstructed = true;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
BSplineVolume<N, Real>::operator bool() const
{
    return mConstructed;
}
//----------------------------------------------------------------------------
template <int N, typename Real>
int BSplineVolume<N, Real>::GetNumControls(int dim) const
{
    return mNumControls[dim];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Vector<N, Real> const* BSplineVolume<N, Real>::GetControls() const
{
    return &mControls[0];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
BasisFunction<Real> const& BSplineVolume<N, Real>::GetBasisFunction(int dim)
    const
{
    return mBasisFunction[dim];
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Real BSplineVolume<N, Real>::GetMinDomain(int dim) const
{
    return mBasisFunction[dim].GetMinDomain();
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Real BSplineVolume<N, Real>::GetMaxDomain(int dim) const
{
    return mBasisFunction[dim].GetMaxDomain();
}
//----------------------------------------------------------------------------
template <int N, typename Real>
void BSplineVolume<N, Real>::Evaluate(Real u, Real v, Real w,
    unsigned int maxOrder, Vector<N, Real> values[10]) const
{
    if (!mConstructed)
    {
        // Errors were already generated during construction.
        for (int i = 0; i < 10; ++i)
        {
            values[i].MakeZero();
        }
        return;
    }

    int iumin, iumax, ivmin, ivmax, iwmin, iwmax;
    mBasisFunction[0].Evaluate(u, maxOrder, iumin, iumax);
    mBasisFunction[1].Evaluate(v, maxOrder, ivmin, ivmax);
    mBasisFunction[2].Evaluate(w, maxOrder, iwmin, iwmax);

    // Compute position.
    values[0] = Compute(0, 0, 0, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
    if (maxOrder >= 1)
    {
        // Compute first-order derivatives.
        values[1] =
            Compute(1, 0, 0, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
        values[2] =
            Compute(0, 1, 0, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
        values[3] =
            Compute(0, 0, 1, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
        if (maxOrder >= 2)
        {
            // Compute second-order derivatives.
            values[4] =
                Compute(2, 0, 0, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
            values[5] =
                Compute(0, 2, 0, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
            values[6] =
                Compute(0, 0, 2, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
            values[7] =
                Compute(1, 1, 0, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
            values[8] =
                Compute(1, 0, 1, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
            values[9] =
                Compute(0, 1, 1, iumin, iumax, ivmin, ivmax, iwmin, iwmax);
        }
    }
}
//----------------------------------------------------------------------------
template <int N, typename Real>
Vector<N, Real> BSplineVolume<N, Real>::Compute(unsigned int uOrder,
    unsigned int vOrder, unsigned int wOrder, int iumin, int iumax, int ivmin,
    int ivmax, int iwmin, int iwmax) const
{
    // The j*-indices introduce a tiny amount of overhead in order to handle
    // both aperiodic and periodic splines.  For aperiodic splines, j* = i*
    // always.

    int const numControls0 = mNumControls[0];
    int const numControls1 = mNumControls[1];
    int const numControls2 = mNumControls[2];
    Vector<N, Real> result;
    result.MakeZero();
    for (int iw = iwmin; iw <= iwmax; ++iw)
    {
        Real tmpw = mBasisFunction[2].GetValue(wOrder, iw);
        int jw = (iw >= numControls2 ? iw - numControls2 : iw);
        for (int iv = ivmin; iv <= ivmax; ++iv)
        {
            Real tmpv = mBasisFunction[1].GetValue(vOrder, iv);
            Real tmpvw = tmpv * tmpw;
            int jv = (iv >= numControls1 ? iv - numControls1 : iv);
            for (int iu = iumin; iu <= iumax; ++iu)
            {
                Real tmpu = mBasisFunction[0].GetValue(uOrder, iu);
                int ju = (iu >= numControls0 ? iu - numControls0 : iu);
                result += (tmpu * tmpvw) *
                    mControls[ju + numControls0*(jv + numControls1*jw)];
            }
        }
    }
    return result;
}
//----------------------------------------------------------------------------
