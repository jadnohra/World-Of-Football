/*
	Taken From geometrictools.com LGPL

	// Wild Magic Source Code
	// David Eberly
	// http://www.geometrictools.com
	// Copyright (c) 1998-2007
	//
	// This library is free software; you can redistribute it and/or modify it
	// under the terms of the GNU Lesser General Public License as published by
	// the Free Software Foundation; either version 2.1 of the License, or (at
	// your option) any later version.  The license is available for reading at
	// either of the locations:
	//     http://www.gnu.org/copyleft/lgpl.html
	//     http://www.geometrictools.com/License/WildMagicLicense.pdf
	//

	Modified for Witch Engine
*/

#include "../WEFrustum.h"
#include "../WEOBB.h"

namespace WE {

bool intersect(const Frustum& f, const OBB& b) {

	const Vector3* akA = b.coordFrame.row;
	const float* afE = b.extents.el;
	Vector3 kDiff;
	b.getCenter().subtract(f.origin(), kDiff);
	float afA[3], afB[3], afC[3], afD[3];
	float afNA[3], afNB[3], afNC[3], afND[3];
	float afNApLC[3], afNAmLC[3], afNBpUC[3], afNBmUC[3];
	float afLC[3], afLD[3], afUC[3], afUD[3], afLBpUA[3], afLBmUA[3];
	float fDdD, fR, fP, fMin, fMax, fMTwoLF, fMTwoUF, fLB, fUA, fTmp;
	int i, j;

	
	// M = D
	afD[2] = kDiff.dot(f.dVector());
	for (i = 0; i < 3; i++)
		afC[i] = akA[i].dot(f.dVector());
	fR = afE[0]*fabsf(afC[0]) + afE[1]*fabsf(afC[1]) + afE[2]*fabsf(afC[2]);
	if ( afD[2] + fR < f.dMin() || afD[2] - fR > f.dMax() )
		return false;
	
	// M = n*L - l*D
	for (i = 0; i < 3; i++)
	{
		afA[i] = akA[i].dot(f.lVector());
		afLC[i] = f.lBound()*afC[i];
		afNA[i] = f.dMin()*afA[i];
		afNAmLC[i] = afNA[i] - afLC[i];
	}
	
	afD[0] = kDiff.dot(f.lVector());
	fR = afE[0]*fabsf(afNAmLC[0]) + afE[1]*fabsf(afNAmLC[1]) + afE[2]*fabsf(afNAmLC[2]);
	afND[0] = f.dMin()*afD[0];
	afLD[2] = f.lBound()*afD[2];
	fDdD = afND[0] - afLD[2];
	fMTwoLF = f.m2lf();
	if ( fDdD + fR < fMTwoLF || fDdD > fR )
		return false;
	
	// M = -n*L - l*D
	for (i = 0; i < 3; i++)
		afNApLC[i] = afNA[i] + afLC[i];
	fR = afE[0]*fabsf(afNApLC[0]) +
		afE[1]*fabsf(afNApLC[1]) +
		afE[2]*fabsf(afNApLC[2]);
	fDdD = -(afND[0] + afLD[2]);
	if ( fDdD + fR < fMTwoLF || fDdD > fR )
		return false;
		
	// M = n*U - u*D
	for (i = 0; i < 3; i++)
	{
		afB[i] = akA[i].dot(f.uVector());
		afUC[i] = f.uBound()*afC[i];
		afNB[i] = f.dMin()*afB[i];
		afNBmUC[i] = afNB[i] - afUC[i];
	}
	
	afD[1] = kDiff.dot(f.uVector());
	fR = afE[0]*fabsf(afNBmUC[0]) + afE[1]*fabsf(afNBmUC[1]) + afE[2]*fabsf(afNBmUC[2]);
	afND[1] = f.dMin()*afD[1];
	afUD[2] = f.uBound()*afD[2];
	fDdD = afND[1] - afUD[2];
	fMTwoUF = f.m2uf();
	if ( fDdD + fR < fMTwoUF || fDdD > fR )
		return false;

	// M = -n*U - u*D
	for (i = 0; i < 3; i++)
		afNBpUC[i] = afNB[i] + afUC[i];
	fR = afE[0]*fabsf(afNBpUC[0]) + afE[1]*fabsf(afNBpUC[1]) + afE[2]*fabsf(afNBpUC[2]);
	fDdD = -(afND[1] + afUD[2]);
	if ( fDdD + fR < fMTwoUF || fDdD > fR )
		return false;

	// M = A[i]
	for (i = 0; i < 3; i++)
	{
		fP = f.lBound()*fabsf(afA[i]) +
			f.uBound()*fabsf(afB[i]);
		afNC[i] = f.dMin()*afC[i];
		fMin = afNC[i] - fP;
		if ( fMin < 0.0 )
			fMin *= f.dRatio();
		fMax = afNC[i] + fP;
		if ( fMax > 0.0 )
			fMax *= f.dRatio();
		fDdD = afA[i]*afD[0] + afB[i]*afD[1] + afC[i]*afD[2];
		if ( fDdD + afE[i] < fMin || fDdD - afE[i] > fMax )
			return false;
	}
	// M = Cross(L,A[i])
	for (i = 0; i < 3; i++)
	{
		fP = f.uBound()*fabsf(afC[i]);
		fMin = afNB[i] - fP;
		if ( fMin < 0.0 )
			fMin *= f.dRatio();
		fMax = afNB[i] + fP;
		if ( fMax > 0.0 )
			fMax *= f.dRatio();
		fDdD = -afC[i]*afD[1] + afB[i]*afD[2];
		fR = afE[0]*fabsf(afB[i]*afC[0]-afB[0]*afC[i]) +
				afE[1]*fabsf(afB[i]*afC[1]-afB[1]*afC[i]) +
				afE[2]*fabsf(afB[i]*afC[2]-afB[2]*afC[i]);

		if ( fDdD + fR < fMin || fDdD - fR > fMax )
			return false;
	}
	// M = Cross(U,A[i])
	for (i = 0; i < 3; i++)
	{
		fP = f.lBound()*fabsf(afC[i]);
		fMin = -afNA[i] - fP;
		if ( fMin < 0.0 )
			fMin *= f.dRatio();
		fMax = -afNA[i] + fP;
		if ( fMax > 0.0 )
			fMax *= f.dRatio();
		fDdD = afC[i]*afD[0] - afA[i]*afD[2];
		fR = afE[0]*fabsf(afA[i]*afC[0]-afA[0]*afC[i]) +
			afE[1]*fabsf(afA[i]*afC[1]-afA[1]*afC[i]) +
			afE[2]*fabsf(afA[i]*afC[2]-afA[2]*afC[i]);

		if ( fDdD + fR < fMin || fDdD - fR > fMax )
			return false;
	}
	// M = Cross(n*D+l*L+u*U,A[i])
	for (i = 0; i < 3; i++)
	{
		fLB = f.lBound()*afB[i];
		fUA = f.uBound()*afA[i];
		afLBpUA[i] = fLB + fUA;
		afLBmUA[i] = fLB - fUA;
	}
	for (i = 0; i < 3; i++)
	{
		fP = f.lBound()*fabsf(afNBmUC[i]) +
			f.uBound()*fabsf(afNAmLC[i]);
		fTmp = f.dMin()*afLBmUA[i];
		fMin = fTmp - fP;
		if ( fMin < 0.0 )
			fMin *= f.dRatio();
		fMax = fTmp + fP;
		if ( fMax > 0.0 )
			fMax *= f.dRatio();
		fDdD = -afD[0]*afNBmUC[i] + afD[1]*afNAmLC[i] + afD[2]*afLBmUA[i];
		fR = 0.0;
		for (j = 0; j < 3; j++)
		{
			fR += afE[j]*fabsf(-afA[j]*afNBmUC[i]+ afB[j]*afNAmLC[i]
			+ afC[j]*afLBmUA[i]);
		}
		if ( fDdD + fR < fMin || fDdD - fR > fMax )
			return false;
	}
	// M = Cross(n*D+l*L-u*U,A[i])
	for (i = 0; i < 3; i++)
	{
		fP = f.lBound()*fabsf(afNBpUC[i]) +
			f.uBound()*fabsf(afNAmLC[i]);
		fTmp = f.dMin()*afLBpUA[i];
		fMin = fTmp - fP;
		if ( fMin < 0.0 )
			fMin *= f.dRatio();
		fMax = fTmp + fP;
		if ( fMax > 0.0 )
			fMax *= f.dRatio();
		fDdD = -afD[0]*afNBpUC[i] + afD[1]*afNAmLC[i] + afD[2]*afLBpUA[i];
		fR = 0.0;
		for (j = 0; j < 3; j++)
		{
			fR += afE[j]*fabsf(-afA[j]*afNBpUC[i]+ afB[j]*afNAmLC[i]
			+ afC[j]*afLBpUA[i]);
		}
		if ( fDdD + fR < fMin || fDdD - fR > fMax )
			return false;
	}
	// M = Cross(n*D-l*L+u*U,A[i])
	for (i = 0; i < 3; i++)
	{
		fP = f.lBound()*fabsf(afNBmUC[i]) +
			f.uBound()*fabsf(afNApLC[i]);
		fTmp = -f.dMin()*afLBpUA[i];
		fMin = fTmp - fP;
		if ( fMin < 0.0 )
			fMin *= f.dRatio();
		fMax = fTmp + fP;
		if ( fMax > 0.0 )
			fMax *= f.dRatio();
		fDdD = -afD[0]*afNBmUC[i] + afD[1]*afNApLC[i] - afD[2]*afLBpUA[i];
		fR = 0.0;
		for (j = 0; j < 3; j++)
		{
			fR += afE[j]*fabsf(-afA[j]*afNBmUC[i]+ afB[j]*afNApLC[i]
			- afC[j]*afLBpUA[i]);
		}
		if ( fDdD + fR < fMin || fDdD - fR > fMax )
			return false;
	}
	// M = Cross(n*D-l*L-u*U,A[i])
	for (i = 0; i < 3; i++)
	{
		fP = f.lBound()*fabsf(afNBpUC[i]) +
			f.uBound()*fabsf(afNApLC[i]);
		fTmp = -f.dMin()*afLBmUA[i];
		fMin = fTmp - fP;
		if ( fMin < 0.0 )
			fMin *= f.dRatio();
		fMax = fTmp + fP;
		if ( fMax > 0.0 )
			fMax *= f.dRatio();
		fDdD = -afD[0]*afNBpUC[i] + afD[1]*afNApLC[i] - afD[2]*afLBmUA[i];
		fR = 0.0;
		for (j = 0; j < 3; j++)
		{
			fR += afE[j]*fabsf(-afA[j]*afNBpUC[i]+ afB[j]*afNApLC[i]
			- afC[j]*afLBmUA[i]);
		}
		if ( fDdD + fR < fMin || fDdD - fR > fMax )
			return false;
	}

	return true;
}

	
	/*

original

bool TestIntersection (const Box3& rkBox, const Frustum& rkFrustum)
{
const Vector3* akA = rkBox.Axes();
const Real* afE = rkBox.Extents();
Vector3 kDiff = rkBox.Center() - rkFrustum.Origin();
Real afA[3], afB[3], afC[3], afD[3];
Real afNA[3], afNB[3], afNC[3], afND[3];
Real afNApLC[3], afNAmLC[3], afNBpUC[3], afNBmUC[3];
Real afLC[3], afLD[3], afUC[3], afUD[3], afLBpUA[3], afLBmUA[3];
Real fDdD, fR, fP, fMin, fMax, fMTwoLF, fMTwoUF, fLB, fUA, fTmp;
int i, j;
// M = D
afD[2] = kDiff.Dot(rkFrustum.DVector());
for (i = 0; i < 3; i++)
afC[i] = akA[i].Dot(rkFrustum.DVector());
fR = afE[0]*Math::Abs(afC[0]) +
afE[1]*Math::Abs(afC[1]) +
afE[2]*Math::Abs(afC[2]);
if ( afD[2] + fR < rkFrustum.DMin() || afD[2] - fR > rkFrustum.DMax() )
return false;
// M = n*L - l*D
for (i = 0; i < 3; i++)
{
afA[i] = akA[i].Dot(rkFrustum.LVector());
afLC[i] = rkFrustum.LBound()*afC[i];
afNA[i] = rkFrustum.DMin()*afA[i];
afNAmLC[i] = afNA[i] - afLC[i];
}
afD[0] = kDiff.Dot(rkFrustum.LVector());
fR = afE[0]*Math::Abs(afNAmLC[0]) +
afE[1]*Math::Abs(afNAmLC[1]) +
afE[2]*Math::Abs(afNAmLC[2]);
afND[0] = rkFrustum.DMin()*afD[0];
afLD[2] = rkFrustum.LBound()*afD[2];
fDdD = afND[0] - afLD[2];
fMTwoLF = rkFrustum.GetMTwoLF();
if ( fDdD + fR < fMTwoLF || fDdD > fR )
return false;
// M = -n*L - l*D
for (i = 0; i < 3; i++)
afNApLC[i] = afNA[i] + afLC[i];
fR = afE[0]*Math::Abs(afNApLC[0]) +
afE[1]*Math::Abs(afNApLC[1]) +
afE[2]*Math::Abs(afNApLC[2]);
fDdD = -(afND[0] + afLD[2]);
if ( fDdD + fR < fMTwoLF || fDdD > fR )
return false;
// M = n*U - u*D
for (i = 0; i < 3; i++)
{
afB[i] = akA[i].Dot(rkFrustum.UVector());
afUC[i] = rkFrustum.UBound()*afC[i];
afNB[i] = rkFrustum.DMin()*afB[i];
afNBmUC[i] = afNB[i] - afUC[i];
}
afD[1] = kDiff.Dot(rkFrustum.UVector());
fR = afE[0]*Math::Abs(afNBmUC[0]) +
afE[1]*Math::Abs(afNBmUC[1]) +
afE[2]*Math::Abs(afNBmUC[2]);
afND[1] = rkFrustum.DMin()*afD[1];
afUD[2] = rkFrustum.UBound()*afD[2];
fDdD = afND[1] - afUD[2];
fMTwoUF = rkFrustum.GetMTwoUF();
if ( fDdD + fR < fMTwoUF || fDdD > fR )
return false;
// M = -n*U - u*D
for (i = 0; i < 3; i++)
afNBpUC[i] = afNB[i] + afUC[i];
fR = afE[0]*Math::Abs(afNBpUC[0]) +
afE[1]*Math::Abs(afNBpUC[1]) +
afE[2]*Math::Abs(afNBpUC[2]);
fDdD = -(afND[1] + afUD[2]);
if ( fDdD + fR < fMTwoUF || fDdD > fR )
return false;
// M = A[i]
for (i = 0; i < 3; i++)
{
fP = rkFrustum.LBound()*Math::Abs(afA[i]) +
rkFrustum.UBound()*Math::Abs(afB[i]);
afNC[i] = rkFrustum.DMin()*afC[i];
fMin = afNC[i] - fP;
if ( fMin < 0.0 )
fMin *= rkFrustum.GetDRatio();
fMax = afNC[i] + fP;
if ( fMax > 0.0 )
fMax *= rkFrustum.GetDRatio();
fDdD = afA[i]*afD[0] + afB[i]*afD[1] + afC[i]*afD[2];
if ( fDdD + afE[i] < fMin || fDdD - afE[i] > fMax )
return false;
}
// M = Cross(L,A[i])
for (i = 0; i < 3; i++)
{
fP = rkFrustum.UBound()*Math::Abs(afC[i]);
fMin = afNB[i] - fP;
if ( fMin < 0.0 )
fMin *= rkFrustum.GetDRatio();
fMax = afNB[i] + fP;
if ( fMax > 0.0 )
fMax *= rkFrustum.GetDRatio();
fDdD = -afC[i]*afD[1] + afB[i]*afD[2];
fR = afE[0]*Math::Abs(afB[i]*afC[0]-afB[0]*afC[i]) +
afE[1]*Math::Abs(afB[i]*afC[1]-afB[1]*afC[i]) +
afE[2]*Math::Abs(afB[i]*afC[2]-afB[2]*afC[i]);
if ( fDdD + fR < fMin || fDdD - fR > fMax )
return false;
}
// M = Cross(U,A[i])
for (i = 0; i < 3; i++)
{
fP = rkFrustum.LBound()*Math::Abs(afC[i]);
fMin = -afNA[i] - fP;
if ( fMin < 0.0 )
fMin *= rkFrustum.GetDRatio();
fMax = -afNA[i] + fP;
if ( fMax > 0.0 )
fMax *= rkFrustum.GetDRatio();
fDdD = afC[i]*afD[0] - afA[i]*afD[2];
fR = afE[0]*Math::Abs(afA[i]*afC[0]-afA[0]*afC[i]) +
afE[1]*Math::Abs(afA[i]*afC[1]-afA[1]*afC[i]) +
afE[2]*Math::Abs(afA[i]*afC[2]-afA[2]*afC[i]);
if ( fDdD + fR < fMin || fDdD - fR > fMax )
return false;
}
// M = Cross(n*D+l*L+u*U,A[i])
for (i = 0; i < 3; i++)
{
fLB = rkFrustum.LBound()*afB[i];
fUA = rkFrustum.UBound()*afA[i];
afLBpUA[i] = fLB + fUA;
afLBmUA[i] = fLB - fUA;
}
for (i = 0; i < 3; i++)
{
fP = rkFrustum.LBound()*Math::Abs(afNBmUC[i]) +
rkFrustum.UBound()*Math::Abs(afNAmLC[i]);
fTmp = rkFrustum.DMin()*afLBmUA[i];
fMin = fTmp - fP;
if ( fMin < 0.0 )
fMin *= rkFrustum.GetDRatio();
fMax = fTmp + fP;
if ( fMax > 0.0 )
fMax *= rkFrustum.GetDRatio();
fDdD = -afD[0]*afNBmUC[i] + afD[1]*afNAmLC[i] + afD[2]*afLBmUA[i];
fR = 0.0;
for (j = 0; j < 3; j++)
{
fR += afE[j]*Math::Abs(-afA[j]*afNBmUC[i]+ afB[j]*afNAmLC[i]
+ afC[j]*afLBmUA[i]);
}
if ( fDdD + fR < fMin || fDdD - fR > fMax )
return false;
}
// M = Cross(n*D+l*L-u*U,A[i])
for (i = 0; i < 3; i++)
{
fP = rkFrustum.LBound()*Math::Abs(afNBpUC[i]) +
rkFrustum.UBound()*Math::Abs(afNAmLC[i]);
fTmp = rkFrustum.DMin()*afLBpUA[i];
fMin = fTmp - fP;
if ( fMin < 0.0 )
fMin *= rkFrustum.GetDRatio();
fMax = fTmp + fP;
if ( fMax > 0.0 )
fMax *= rkFrustum.GetDRatio();
fDdD = -afD[0]*afNBpUC[i] + afD[1]*afNAmLC[i] + afD[2]*afLBpUA[i];
fR = 0.0;
for (j = 0; j < 3; j++)
{
fR += afE[j]*Math::Abs(-afA[j]*afNBpUC[i]+ afB[j]*afNAmLC[i]
+ afC[j]*afLBpUA[i]);
}
if ( fDdD + fR < fMin || fDdD - fR > fMax )
return false;
}
// M = Cross(n*D-l*L+u*U,A[i])
for (i = 0; i < 3; i++)
{
fP = rkFrustum.LBound()*Math::Abs(afNBmUC[i]) +
rkFrustum.UBound()*Math::Abs(afNApLC[i]);
fTmp = -rkFrustum.DMin()*afLBpUA[i];
fMin = fTmp - fP;
if ( fMin < 0.0 )
fMin *= rkFrustum.GetDRatio();
fMax = fTmp + fP;
if ( fMax > 0.0 )
fMax *= rkFrustum.GetDRatio();
fDdD = -afD[0]*afNBmUC[i] + afD[1]*afNApLC[i] - afD[2]*afLBpUA[i];
fR = 0.0;
for (j = 0; j < 3; j++)
{
fR += afE[j]*Math::Abs(-afA[j]*afNBmUC[i]+ afB[j]*afNApLC[i]
- afC[j]*afLBpUA[i]);
}
if ( fDdD + fR < fMin || fDdD - fR > fMax )
return false;
}
// M = Cross(n*D-l*L-u*U,A[i])
for (i = 0; i < 3; i++)
{
fP = rkFrustum.LBound()*Math::Abs(afNBpUC[i]) +
rkFrustum.UBound()*Math::Abs(afNApLC[i]);
fTmp = -rkFrustum.DMin()*afLBmUA[i];
fMin = fTmp - fP;
if ( fMin < 0.0 )
fMin *= rkFrustum.GetDRatio();
fMax = fTmp + fP;
if ( fMax > 0.0 )
fMax *= rkFrustum.GetDRatio();
fDdD = -afD[0]*afNBpUC[i] + afD[1]*afNApLC[i] - afD[2]*afLBmUA[i];
fR = 0.0;
for (j = 0; j < 3; j++)
{
fR += afE[j]*Math::Abs(-afA[j]*afNBpUC[i]+ afB[j]*afNApLC[i]
- afC[j]*afLBmUA[i]);
}
if ( fDdD + fR < fMin || fDdD - fR > fMax )
return false;
}
return true;
}
*/

}


