/////////////////////////////////////////////////////////////////
// Ver.2.1.2     10.03.2010
//               Поправлена функция int CsBV::RightOne(int nPrev) const
// Ver.2.1.1     22.01.2008
//               Добавлены функции конвертации данных базового класса булевых векторов
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1        11.02.2005
//                Change type 'unsigned int' on ULONG
// Ver.1.0        08.02.2005
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#ifdef _JOINT
#include "ShortBool.h"
#include "BaseBool.h"
#include "archive.h"
#else
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include "../Common/archive.h"
#endif

class CBV;
class CBM;
#ifndef _LINUX
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
#endif

//typedef unsigned long long size_t;

static size_t Rgrain = 1;
static BOOL NewRand = TRUE;

/////////////////////////////////////////////////////////////////////
//////////////////////// Construction/Destruction ///////////////////

//-------------------------------------------------------------------
CsBV::CsBV() { m_bVect = m_nBitLength = 0; }

//-------------------------------------------------------------------
CsBV::CsBV(const CsBV& bvSrc)
{
	m_bVect = bvSrc.m_bVect;
	m_nBitLength = bvSrc.m_nBitLength;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////// More sophisticated construction /////////

//-------------------------------------------------------------------
CsBV::CsBV(__int8 nLength, BOOL Fl)
{                      // return empty vector if invalid repeat count
	if (nLength < 1 || nLength > SIZE_SHORTBOOL) 
		m_bVect = m_nBitLength = 0;
	else
	{
		if (Fl)
			m_bVect = ONES >> (SIZE_SHORTBOOL - nLength) << (SIZE_SHORTBOOL - nLength);
		else
			m_bVect = 0;
		m_nBitLength = nLength;
	}
}

//-------------------------------------------------------------------
CsBV::CsBV(const size_t UintVal, __int8 Len)
{
	if (Len < 1 || Len > SIZE_SHORTBOOL) 
		m_bVect = m_nBitLength = 0;
	else
	{
		m_bVect = UintVal >> (SIZE_SHORTBOOL - Len) << (SIZE_SHORTBOOL - Len);
		m_nBitLength = Len;
	}
}

//-------------------------------------------------------------------
CsBV::CsBV(const char* pch)
{
	__int8 nLenBit;
	if ((nLenBit = (__int8)SafeStrlen(pch)) == 0)
		m_bVect = m_nBitLength = 0;
	else
	{
		CharBit(nLenBit, pch);
		m_nBitLength = nLenBit;
	}
}

//-------------------------------------------------------------------
CsBV::~CsBV() {}

//////////////////////////////////////////////////////////// GetRandN
// Получение очередного псевдослучайного целого числа     //
// 32 или 16-разрядного(Романов - переход к 32 от 16 - 10.03.2006)
// Корректно работающий вариант Томашева - 27 января 1999 //
////////////////////////////////////////////////////////////
size_t GetRandN()

{
	size_t	f13	= (size_t)1220703125 * (size_t)1220703125;  // f13 = 5**26
	size_t	m	= 0x7fffffffffffffff;  //  m  = 2**31-1
	size_t  w;

	size_t x1 = (size_t)Rgrain;
	x1 = (x1*f13)%m;
	w = (x1 << 16) >> 32;
	x1 = (x1*f13)%m;
	w = (w << 32) | ((x1 << 16) >> 32);
	Rgrain = (size_t)x1;
	return (w);
}

CsBV CsBV::GenRbvN(__int8 n){
	m_nBitLength = n;
	m_bVect = GetRandN();
	return *this; 
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////// Reading and writing //////////////
#ifndef _LINUX
//-------------------------------------------------------------------
CString CsBV::BitChar(char One, char Zero)
{
	CString res('\0', m_nBitLength);
	for (__int8 i = 0; i < m_nBitLength; i++)
		if (GetBitAt(i))
			res.SetAt(i, One);
		else
			res.SetAt(i, Zero);
		return res;
}
#else
//-------------------------------------------------------------------
char* CsBV::BitChar(char One, char Zero)
{ int i;
  char * res;
  res = (char*)malloc(m_nBitLength+1);
  
  for (i=0; i<m_nBitLength; i++)
    if (GetBitAt(i)) res[i] = One;
    else             res[i] = Zero;
  res[m_nBitLength]= 0;
  return res;
}
#endif

/////////////////////////////////////////////////////////////////////
////////////////////// Assignment operator and functions ////////////

//-------------------------------------------------------------------
void CsBV::CharBit(__int8 nLen, const char* pch)
{
	Zero();
	for (__int8 j = 0; j < nLen; j++)
	{
		if (pch[j] == '1')
			m_bVect |= sOB[j];
		else 
			ASSERT(pch[j] == '0' || pch[j] == '-' || pch[j] == '.');
	}
}

//-------------------------------------------------------------------
void CsBV::One()
{
	if (m_nBitLength == 0)
		return;
	m_bVect = ONES >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
}

//-------------------------------------------------------------------
CsBV CsBV::Extract(__int8 nFirst, __int8 nCount)
{
	CsBV s;
	ASSERT(nFirst >= 0 && nFirst < SIZE_SHORTBOOL);
	ASSERT(nCount > 0);
	ASSERT(nFirst + nCount <= SIZE_SHORTBOOL);

	s.m_bVect = m_bVect >> (SIZE_SHORTBOOL - (nFirst + nCount)) << (SIZE_SHORTBOOL - nCount);
	s.m_nBitLength = nCount;
	return s;
}


/////////////////////////////////////////////////////////////////////
//////////////////////////////////// Concatination //////////////////

//****************************** Protected function *****************//
//-------------------------------------------------------------------
void CsBV::Conc2(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2)
{
	m_nBitLength = Len1 + Len2;
	ASSERT(m_nBitLength <= SIZE_SHORTBOOL);
	if (m_nBitLength == 0) 
	{
		m_bVect = 0;
		return;
	}
	m_bVect = Vect1 | (Vect2 >> (SIZE_SHORTBOOL - Len2) << (SIZE_SHORTBOOL - m_nBitLength));
}

//-------------------------------------------------------------------
void CsBV::Concat(BOOL Bit/*=FALSE*/)
{
	Concat(Bit ? sOB[0] : 0, 1);
}

//-------------------------------------------------------------------
void CsBV::Concat(const size_t Vect, __int8 Len)
{
	__int8 NewBitLen = m_nBitLength + Len;
	ASSERT(NewBitLen <= SIZE_SHORTBOOL);
	if (NewBitLen == 0) 
	{
		m_bVect = m_nBitLength = 0;
		return;
	}
	m_bVect |= Vect >> (SIZE_SHORTBOOL - Len) << (SIZE_SHORTBOOL - NewBitLen);
	m_nBitLength = NewBitLen;
	return;
}

//-------------------------------------------------------------------
void CsBV::Concat(const CsBV& bv)
{
	Concat(bv.m_bVect, bv.m_nBitLength);
	return;
}

//-------------------------------------------------------------------
void CsBV::Concat(const CsBV& bv1, const CsBV& bv2)
{
	Conc2(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
	return;
}


/////////////////////////////////////////////////////////////////////
//////////////////////// Advanced assignment operators //////////////

//-------------------------------------------------------------------
void CsBV::LeftShiftInPlace(__int8 nShift)
{
	ASSERT(nShift >= 0);
	if (nShift >= m_nBitLength)
	{
		m_bVect = m_nBitLength = 0;
		return;
	}
	m_bVect <<= nShift;
}

//-------------------------------------------------------------------
void CsBV::RightShiftInPlace(__int8 nShift)
{
	ASSERT(nShift >= 0);
	if (nShift >= m_nBitLength)
	{
		m_bVect = m_nBitLength = 0;
		return;
	}
	m_bVect = m_bVect >> (SIZE_SHORTBOOL - m_nBitLength + nShift) << (SIZE_SHORTBOOL - m_nBitLength);
}

//-------------------------------------------------------------------
const CsBV&  CsBV::operator <<= (__int8 nShift)
{
	LeftShiftInPlace(nShift);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator >>= (__int8 nShift)
{
	RightShiftInPlace(nShift);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator |= (const CsBV& bv)
{
	DizInPlace(bv.m_bVect, bv.m_nBitLength);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator |= (const size_t Vect)
{
	ASSERT(m_nBitLength > 0); 
	DizInPlace(Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength), m_nBitLength);    
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator &= (const CsBV& bv)
{
	ConInPlace(bv.m_bVect, bv.m_nBitLength);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator &= (const size_t Vect)
{
	ASSERT(m_nBitLength > 0); 
	ConInPlace(Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength), m_nBitLength);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator ^= (const CsBV& bv)
{
	Add2InPlace(bv.m_bVect, bv.m_nBitLength);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator ^= (const size_t Vect)
{
	ASSERT(m_nBitLength > 0); 
	Add2InPlace(Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength), m_nBitLength);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator -= (const CsBV& bv)
{
	ConNotInPlace(bv.m_bVect, bv.m_nBitLength);
	return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator -= (const size_t Vect)
{
	ASSERT(m_nBitLength > 0); 
	ConNotInPlace(Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength), m_nBitLength);    
	return *this;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////// Logic operations /////////////////

//-------------------------------------------------------------------
void CsBV::LeftShiftVect(const size_t Vect, __int8 Len, __int8 nShift)
{
	ASSERT(nShift >= 0);
	if (nShift >= Len)
	{
		m_bVect = m_nBitLength = 0;
		return;
	}
	m_bVect = Vect << nShift;
	m_nBitLength = Len;
}

//-------------------------------------------------------------------
void CsBV::RightShiftVect(const size_t Vect, __int8 Len, __int8 nShift)
{
	ASSERT(nShift >= 0);
	if (nShift >= Len)
	{
		m_bVect = m_nBitLength = 0;
		return;
	}
	m_bVect = Vect >> (SIZE_SHORTBOOL - Len + nShift) << (SIZE_SHORTBOOL - Len);
	m_nBitLength = Len;
}

//-------------------------------------------------------------------
void CsBV::Diz(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2)
{
	ASSERT(Len1 == Len2);
	m_bVect = Vect1 | Vect2;
	m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::Con(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2)
{
	ASSERT(Len1 == Len2);
	m_bVect = Vect1 & Vect2;
	m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::Add2(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2)
{
	ASSERT(Len1 == Len2);
	m_bVect = Vect1 ^ Vect2;
	m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::ConNot(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2)
{
	ASSERT(Len1 == Len2);
	m_bVect = Vect1 & ~Vect2;
	m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::Not(const size_t Vect, __int8 Len)
{
	m_nBitLength = Len;
	m_bVect = ~Vect >> (SIZE_SHORTBOOL - Len) << (SIZE_SHORTBOOL - Len);
}

//-------------------------------------------------------------------
STD(CsBV) operator << (const CsBV& bv, __int8 nShift)
{
	CsBV s;
	s.LeftShiftVect(bv.m_bVect, bv.m_nBitLength, nShift);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator >> (const CsBV& bv, __int8 nShift)
{
	CsBV s;
	s.RightShiftVect(bv.m_bVect, bv.m_nBitLength, nShift);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator | (const CsBV& bv1, const CsBV& bv2)
{
	CsBV s;
	s.Diz(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator | (const CsBV& bv, const size_t Vect)
{
	CsBV s;
	s.Diz(
		bv.m_bVect,
		bv.m_nBitLength,
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator | (const size_t Vect, const CsBV& bv)
{
	CsBV s;
	s.Diz(
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength,
		bv.m_bVect,
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator & (const CsBV& bv1, const CsBV& bv2)
{
	CsBV s;
	s.Con(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator & (const size_t Vect, const CsBV& bv)
{
	CsBV s;
	s.Con(
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength,
		bv.m_bVect,
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator & (const CsBV& bv, const size_t Vect)
{
	CsBV s;
	s.Con(bv.m_bVect,
		bv.m_nBitLength, 
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ^ (const CsBV& bv1, const CsBV& bv2)
{
	CsBV s;
	s.Add2(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ^ (const size_t Vect, const CsBV& bv)
{
	CsBV s;
	s.Add2(
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength,
		bv.m_bVect,
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ^ (const CsBV& bv, const size_t Vect)
{
	CsBV s;
	s.Add2(
		bv.m_bVect,
		bv.m_nBitLength, 
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator - (const CsBV& bv1, const CsBV& bv2)
{
	CsBV s;
	s.ConNot(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator - (const size_t Vect, const CsBV& bv)
{
	CsBV s;
	s.ConNot(
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength,
		bv.m_bVect,
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator - (const CsBV& bv, const size_t Vect)
{
	CsBV s;
	s.ConNot(
		bv.m_bVect,
		bv.m_nBitLength, 
		Vect >> (SIZE_SHORTBOOL - bv.m_nBitLength) << (SIZE_SHORTBOOL - bv.m_nBitLength), 
		bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ~ (const CsBV& bv)
{
	CsBV s;
	s.Not(bv.m_bVect, bv.m_nBitLength);
	return s;
}

//-------------------------------------------------------------------
void CsBV::LoopLeftShift(__int8 nShift)
{
	ASSERT(nShift >= 0);
	nShift %= m_nBitLength;
	size_t w = m_bVect >> (SIZE_SHORTBOOL - nShift);
	m_bVect <<= nShift;
	m_bVect |= w << (SIZE_SHORTBOOL - m_nBitLength);
}

//-------------------------------------------------------------------
void CsBV::LoopRightShift(__int8 nShift)
{
	ASSERT(nShift >= 0);
	nShift %= m_nBitLength;
	size_t w = m_bVect << (m_nBitLength - nShift);
	m_bVect = (m_bVect >> (SIZE_SHORTBOOL - m_nBitLength + nShift)) << (SIZE_SHORTBOOL - m_nBitLength);
	m_bVect |= w;
}


/////////////////////////////////////////////////////////////////////
////////////////////// Operation of weighting,finding and casing ////
//-------------------------------------------------------------------
__int8 CsBV::CountBit() const
{
	BYTE* pB = (BYTE*)&m_bVect;
	return (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]] + sTabC[pB[4]] + sTabC[pB[5]] + sTabC[pB[6]] + sTabC[pB[7]]);
}

//-------------------------------------------------------------------
__int8 CsBV::LeftOne(__int8 nNext) const
{
	ASSERT((nNext >= -1) && (m_nBitLength > nNext));
	for (__int8 i = nNext + 1; i < m_nBitLength; i++)
		if (m_bVect & sOB[i])
			return i;
	return (-1);
}

//-------------------------------------------------------------------
__int8 CsBV::RightOne(__int8 nPrev) const
{
	ASSERT(m_nBitLength >= nPrev); // && (nPrev > 0)
	if (nPrev == -1)
		nPrev = m_nBitLength;   // Добавлено 24.03.2010
	for (__int8 i = nPrev - 1; i >= 0; i--)
		if (m_bVect & sOB[i])
			return i;
	return (-1);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////// Comparing operations ////////////////////
//-------------------------------------------------------------------
BOOL CsBV::Equality(const size_t Vect, __int8 Len) const
{
	if (m_nBitLength != Len)
		return FALSE;
	if (m_bVect != (Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength)))
		return FALSE;
	return TRUE;
}

//-------------------------------------------------------------------
BOOL CsBV::Pogl(const size_t Vect, __int8 Len, BOOL Dist) const
{
	ASSERT(m_nBitLength == Len);
	size_t V = Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	if (Dist)
		if ((m_bVect & V) != V)
			return FALSE; 
	else if ((m_bVect & V) != m_bVect)
		return FALSE; 
	if (m_bVect == V)
		return FALSE; 
	return TRUE;
}

//-------------------------------------------------------------------
BOOL CsBV::PoglEq(const size_t Vect, __int8 Len, BOOL Dist) const
{
	ASSERT(m_nBitLength == Len);
	size_t V = Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	if (Dist)
		if ((m_bVect & V) != V)
			return FALSE; 
	else if ((m_bVect & V) != m_bVect)
		return FALSE; 
	return TRUE;
}
//*****************************************************************//
//-------------------------------------------------------------------
BOOL CsBV::IsZero() const
{
	if (m_nBitLength == 0)
		return FALSE;
	return (m_bVect == 0);
}

//-------------------------------------------------------------------
BOOL CsBV::IsOne() const
{
	if (m_nBitLength == 0)
		return FALSE;
	return (((~m_bVect) >> (SIZE_SHORTBOOL - m_nBitLength)) == 0);
}

//-------------------------------------------------------------------
STD(BOOL) operator == (const CsBV& bv1, const CsBV& bv2)
{ return bv1.Equality(bv2.m_bVect, bv2.m_nBitLength); }

//-------------------------------------------------------------------
STD(BOOL) operator == (const CsBV& bv, const size_t Vect)
{ return bv.Equality(Vect, bv.m_nBitLength); }

//-------------------------------------------------------------------
STD(BOOL) operator == (const  size_t Vect, const CsBV& bv)
{ return bv.Equality(Vect, bv.m_nBitLength); }

//-------------------------------------------------------------------

STD(BOOL) operator != (const CsBV& bv1, const CsBV& bv2)
{ return !(bv1.Equality(bv2.m_bVect, bv2.m_nBitLength)); }

//-------------------------------------------------------------------
STD(BOOL) operator != (const CsBV& bv, const  size_t Vect)
{ return !(bv.Equality(Vect, bv.m_nBitLength));}

//-------------------------------------------------------------------
STD(BOOL) operator != (const  size_t Vect, const CsBV& bv)
{ return !(bv.Equality(Vect, bv.m_nBitLength)); }

//-------------------------------------------------------------------
STD(BOOL) operator > (const CsBV& bv1, const CsBV& bv2)
{ return (bv1.Pogl(bv2.m_bVect, bv2.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator > (const CsBV& bv, const  size_t Vect)
{ return (bv.Pogl(Vect, bv.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator > (const  size_t Vect, const CsBV& bv)
{ return (bv.Pogl(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator < (const CsBV& bv1, const CsBV& bv2)
{ return (bv2.Pogl(bv1.m_bVect, bv1.m_nBitLength,TRUE));}

//-------------------------------------------------------------------
STD(BOOL) operator < (const CsBV& bv, const size_t Vect)
{ return (bv.Pogl(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator < (const  size_t Vect, const CsBV& bv)
{ return (bv.Pogl(Vect, bv.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator >= (const CsBV& bv1, const CsBV& bv2)
{ return (bv1.PoglEq(bv2.m_bVect, bv2.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator >= (const CsBV& bv, const size_t Vect)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator >= (const size_t Vect, const CsBV& bv)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator <= (const CsBV& bv1, const CsBV& bv2)
{ return (bv2.PoglEq(bv1.m_bVect, bv1.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator <= (const CsBV& bv, const size_t Vect)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator <= (const size_t Vect, const CsBV& bv)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,TRUE)); }


//-------------------------------------------------------------------
// Конвертирование булева вектора bv из стандартного
// представления в "короткое"(bvS)  

//void CsBV::ToShort(CBV &bv)
/*void CsBV::ToShort(CBV bv)
{
	ASSERT(bv.GetBitLength() <= SIZE_SHORTBOOL);
	Empty();
	m_nBitLength = (__int8)bv.GetBitLength();
	for(__int8 i = 1; i * SIZE_BYTE <= m_nBitLength; i++)
		m_bVect |= (size_t)(bv.GetByteAt(i)) << (SIZE_SHORTBOOL - (i * SIZE_BYTE));
	return;
}
*/

//-------------------------------------------------------------------
// Конвертирование булева вектора из "короткого"
// представления (bvS) в стандартное (bv)  

/*
void  CsBV::FromShort(CBV &bv)
{
	bv.SetSize(m_nBitLength);
	for(__int8 i = 0; i * SIZE_BYTE < m_nBitLength; i++)
		bv.SetByteAt(i, (BYTE)((m_bVect << (i * SIZE_BYTE)) >> (SIZE_SHORTBOOL - SIZE_BYTE)));
	return;
}
*/


void  CsBV::SetSize(__int8 nNewLength)
{
	ASSERT(nNewLength >= 0);
	ASSERT(nNewLength <= SIZE_SHORTBOOL - 1);
	if (m_nBitLength > nNewLength)
	{
		size_t l = 0;
		for (__int8 i = 0; i < nNewLength; i++)
			l |= sOB[i];
		m_bVect &= l;
	}
	m_nBitLength = nNewLength;
	return;
}
#ifndef _LINUX
/////////////////////////////////////////////////////////////////////
///////////////////////// Input/output operations ///////////////////
//-------------------------------------------------------------------
#ifdef _DEBUG
CDumpContext& AFXAPI operator << (CDumpContext& dc, const CsBV& bv)
{
	CString res('\0', bv.m_nBitLength);
	for (__int8 i = 0; i < bv.m_nBitLength; i++)
		if (bv.GetBitAt(i))
			res.SetAt(i, '1');
		else
			res.SetAt(i, '0');
	dc << res;
	return dc;
}
#endif //_DEBUG

//-------------------------------------- AFXAPI operator<<(CArchive& ar, const CsBV& bv)
CArchive& AFXAPI operator << (CArchive& ar, const CsBV& bv)
{
	ar << (__int8)bv.m_nBitLength;
	ar << (size_t)bv.m_bVect;
	return ar;
}

//-------------------------------------- AFXAPI operator>>(CArchive& ar, const CsBV& bv)
CArchive& AFXAPI operator>>(CArchive& ar, CsBV& bv)
{
	ar >> (__int8)bv.m_nBitLength;
	ar >> (size_t)bv.m_bVect;
	return ar;
}

#endif //_LINUX
