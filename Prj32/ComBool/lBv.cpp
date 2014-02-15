/////////////////////////////////////////////////////////////////
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1.1    08.03.2005   Russian == English
/////////////////////////////////////////////////////////////////

#include <limits.h>
#include <stdarg.h>
#include "../ComBool/LongBool.h"

#ifndef _LINUX
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
#else
#undef _DEBUG
#endif

//***************************************************************************************
//***************************************************************************************
// Class   ClBV
//***************************************************************************************

//******************************* Constructors\Destructor *******************************

//-----------------------------------------------------------
ClBV::ClBV()   { Init();  }

//-----------------------------------------------------------
ClBV::ClBV(CBV& bvSrc)
{ 
  m_bVect = (ULONG*)((BYTE*)bvSrc);
  m_nBitLength = bvSrc.GetBitLength();
  m_nLongLength = m_nBitLength/32;
  m_nByteRest = bvSrc.GetByteLength() - m_nLongLength*4;
}

//-----------------------------------------------------------
ClBV::ClBV(const BYTE* pbt, int nLenBit)
{
  m_bVect = (ULONG*)pbt;
  m_nBitLength = nLenBit;
  m_nLongLength = nLenBit/32;
  m_nByteRest = LEN_BYTE(nLenBit) - m_nLongLength*4;
}


//***************************************************************************************
//******************************* Disjunctions *******************************
CBV operator |(const ClBV& bv, const BYTE* pbt)
{ 
  CBV s(BYTE(0),bv.m_nBitLength);
  int i, k;
  BYTE* pBv = (BYTE*)s;
  ULONG* pV = (ULONG *)pBv;
  ULONG* pV1 = (ULONG*)pbt;
  for (i=0; i<bv.m_nLongLength; i++)
    pV[i] = pV1[i] | bv.m_bVect[i];
  if (bv.m_nByteRest!=0) {
    ULONG val = pV1[i] | bv.m_bVect[i];
    if (bv.m_nByteRest==4)
      pV[i] = val;
    else {
      BYTE * pB = (BYTE *)&val;
      pBv = (BYTE*)(pV + i);
      for (k=0; k<bv.m_nByteRest; k++)
        pBv[k] = pB[k];
    }
  }
  return s;
}


//***************************************************************************************
//******************************* Conjunctions *******************************
CBV operator &(const ClBV& bv, const BYTE* pbt)
{ 
  CBV s(BYTE(0),bv.m_nBitLength);
  int i, k;
  BYTE* pBv = (BYTE*)s;
  ULONG* pV = (ULONG *)pBv;
  ULONG* pV1 = (ULONG*)pbt;
  for (i=0; i<bv.m_nLongLength; i++)
    pV[i] = pV1[i] & bv.m_bVect[i];
  if (bv.m_nByteRest!=0) {
    ULONG val = pV1[i] & bv.m_bVect[i];
    if (bv.m_nByteRest==4)
      pV[i] = val;
    else {
      BYTE * pB = (BYTE *)&val;
      pBv = (BYTE*)(pV + i);
      for (k=0; k<bv.m_nByteRest; k++)
        pBv[k] = pB[k];
    }
  }
  return s;
}

//***************************************************************************************
//******************************* Disjunction with ************************
CBV operator ^(const ClBV& bv, const BYTE* pbt)
{ 
  CBV s(BYTE(0),bv.m_nBitLength);
  int i, k;
  BYTE* pBv = (BYTE*)s;
  ULONG* pV = (ULONG *)pBv;
  ULONG* pV1 = (ULONG*)pbt;
  for (i=0; i<bv.m_nLongLength; i++)
    pV[i] = pV1[i] ^ bv.m_bVect[i];
  if (bv.m_nByteRest!=0) {
    ULONG val = pV1[i] ^ bv.m_bVect[i];
    if (bv.m_nByteRest==4)
      pV[i] = val;
    else {
      BYTE * pB = (BYTE *)&val;
      pBv = (BYTE*)(pV + i);
      for (k=0; k<bv.m_nByteRest; k++)
        pBv[k] = pB[k];
    }
  }
  return s;
}


//***************************************************************************************
//******************************* Subtractions *******************************
CBV operator -(const ClBV& bv, const BYTE* pbt)
{ 
  CBV s(BYTE(0),bv.m_nBitLength);
  int i, k;
  BYTE* pBv = (BYTE*)s;
  ULONG* pV = (ULONG *)pBv;
  ULONG* pV1 = (ULONG*)pbt;
  for (i=0; i<bv.m_nLongLength; i++)
    pV[i] = bv.m_bVect[i] & ~pV1[i];
  if (bv.m_nByteRest!=0) {
    ULONG val = bv.m_bVect[i] & ~pV1[i];
    k = 32 - ADR_BIT(bv.m_nBitLength);
    if (k<32)
      val = (val >> k) << k;
    if (bv.m_nByteRest==4)
      pV[i] = val;
    else {
      BYTE * pB = (BYTE *)&val;
      pBv = (BYTE*)(pV + i);
      for (k=0; k<bv.m_nByteRest; k++)
        pBv[k] = pB[k];
    }
  }
  return s;
}

//-----------------------------------------------------------
CBV operator -(const BYTE* pbt, const ClBV& bv)
{ 
  CBV s(BYTE(0),bv.m_nBitLength);
  int i, k;
  BYTE* pBv = (BYTE*)s;
  ULONG* pV = (ULONG *)pBv;
  ULONG* pV1 = (ULONG*)pbt;
  for (i=0; i<bv.m_nLongLength; i++)
    pV[i] = pV1[i] & ~bv.m_bVect[i];
  if (bv.m_nByteRest!=0) {
    ULONG val = pV1[i] & ~bv.m_bVect[i];
    k = 32 - ADR_BIT(bv.m_nBitLength);
    if (k<32)
      val = (val >> k) << k;
    if (bv.m_nByteRest==4)
      pV[i] = val;
    else {
      BYTE * pB = (BYTE *)&val;
      pBv = (BYTE*)(pV + i);
      for (k=0; k<bv.m_nByteRest; k++)
        pBv[k] = pB[k];
    }
  }
  return s;
}


//***************************************************************************************
//******************************* Invertions *******************************
CBV operator ~(const ClBV& bv)
{ 
  CBV s(BYTE(0),bv.m_nBitLength);
  int i, k;
  BYTE* pBv = (BYTE*)s;
  ULONG* pV = (ULONG *)pBv;
  for (i=0; i<bv.m_nLongLength; i++) 
    pV[i] = ~bv.m_bVect[i];
  if (bv.m_nByteRest!=0) {
    ULONG val = ~bv.m_bVect[i];
    k = 32 - ADR_BIT(bv.m_nBitLength);
    if (k<32)
      val = (val >> k) << k;
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(pV + i);
    for (k=0; k<bv.m_nByteRest; k++)
      pBv[k] = pB[k];
  }
  return s;
}

//-----------------------------------------------------
void ClBV::Invert(const ULONG* pwt)
{ 
  int i, k;
  for (i=0; i<m_nLongLength; i++) m_bVect[i] = ~pwt[i];
  if (m_nByteRest!=0) {
    ULONG val = ~pwt[i];
    k = 32 - ADR_BIT(m_nBitLength);
    if (k<32)
      val = (val >> k) << k;
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//***************************************************************************************

//-----------------------------------------------------
BOOL ClBV::IsZero() const
{ 
  int i, k;
  if (m_nBitLength == 0) return FALSE;
  for (i=0; i<m_nLongLength; i++)  if (m_bVect[i]) return FALSE;
  if (m_nByteRest!=0) {
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (k=0; k<m_nByteRest; k++)
      if (pBv[k]) return FALSE;
  }
  return TRUE;
}

//-----------------------------------------------------
BOOL ClBV::IsOne() const
{ 
  int i, k;
  if (m_nBitLength == 0) return FALSE;
  for (i=0; i<m_nLongLength; i++) { 
    if (m_bVect[i] != 0xffffffff) return FALSE; 
  }
  if (m_nByteRest!=0) {
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (k=0; k<m_nByteRest-1; k++)
      if (pBv[k]!=0xff) return FALSE;
    if (~pBv[k]>>(8-(m_nBitLength & 7))) return FALSE; 
  }
  return TRUE;
}


//***************************************************************************************
//******************************* Internal functions*********************
//-----------------------------------------------------------
void ClBV::DisInPlace(const ULONG* Vect1,int BitLen1)
{
  int i;
  ASSERT(BitLen1 >= 0);
  ASSERT(BitLen1 == m_nBitLength);

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = m_bVect[i] | Vect1[i];
  if (m_nByteRest!=0) {
    ULONG val = m_bVect[i] | Vect1[i];
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-----------------------------------------------------------
void ClBV::ConInPlace(const ULONG* Vect1,int BitLen1)
{
  int i;
  ASSERT(BitLen1 >= 0);
  ASSERT(BitLen1 == m_nBitLength);

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = m_bVect[i] & Vect1[i];
  if (m_nByteRest!=0) {
    ULONG val = m_bVect[i] & Vect1[i];
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-----------------------------------------------------------
void ClBV::AddInPlace(const ULONG* Vect1,int BitLen1)
{
  int i;
  ASSERT(BitLen1 >= 0);
  ASSERT(BitLen1 == m_nBitLength);

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = m_bVect[i] ^ Vect1[i];
  if (m_nByteRest!=0) {
    ULONG val = m_bVect[i] ^ Vect1[i];
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//----------------------------------------------------------
void ClBV::ConNotInPlace(const ULONG* Vect1, int BitLen1)
{
  int i, k;
  ASSERT(BitLen1 >= 0);
  ASSERT(BitLen1 == m_nBitLength);

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = m_bVect[i] & ~Vect1[i];
  if (m_nByteRest!=0) {
    ULONG val = m_bVect[i] & ~Vect1[i];
    k = 32 - ADR_BIT(m_nBitLength);
    if (k<32)
      val = (val >> k) << k;
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-----------------------------------------------------------
BOOL ClBV::Equality(const ULONG* Vect2,int BitLen2) const
{
  int i;
  ASSERT(m_nBitLength==BitLen2);
  if (Vect2==NULL && m_bVect) return FALSE;
  for (i=0;i<m_nLongLength;i++)
    if (m_bVect[i]!=Vect2[i]) return FALSE;
  if (m_nByteRest!=0) {
    BYTE * pB = (BYTE *)(Vect2 + i);
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      if (pBv[k] != pB[k]) return FALSE;
  }
  return TRUE;
}

//-----------------------------------------------------------
BOOL ClBV::Pogl(const ULONG* Vect2,int BitLen2, BOOL Dist) const
{
  int i,j=0,jb=0;
  ASSERT(m_nBitLength==BitLen2);
  for (i=0;i<m_nLongLength;i++) {
    if (m_bVect[i] == Vect2[i]) j++;
    else
      if (Dist) { if ((m_bVect[i] & Vect2[i]) != Vect2[i])   return FALSE; }
      else      { if ((m_bVect[i] & Vect2[i]) != m_bVect[i]) return FALSE; }
  }
  if (m_nByteRest!=0) {
    BYTE * pB = (BYTE *)(Vect2 + i);
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      if (pBv[k] == pB[k]) jb++;
      else
        if (Dist) { if ((pBv[k] & pB[k]) != pB[k])  return FALSE; }
        else      { if ((pBv[k] & pB[k]) != pBv[k]) return FALSE; }
  }

  if (j==m_nLongLength && jb==m_nByteRest) return FALSE;
  return TRUE;
}

//-----------------------------------------------------------
BOOL ClBV::PoglEq(const ULONG* Vect2,int BitLen2,BOOL Dist) const
{ 
  int i;
  ASSERT(m_nBitLength==BitLen2);
  for (i=0;i<m_nLongLength;i++) {
    if (Dist) { if ((m_bVect[i] & Vect2[i]) != Vect2[i])   return FALSE; }
    else      { if ((m_bVect[i] & Vect2[i]) != m_bVect[i]) return FALSE; }
  }
  if (m_nByteRest!=0) {
    BYTE * pB = (BYTE *)(Vect2 + i);
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      if (Dist) { if ((pBv[k] & pB[k]) != pB[k])  return FALSE; }
      else      { if ((pBv[k] & pB[k]) != pBv[k]) return FALSE; }
  }
  return TRUE;
}

//-----------------------------------------------------------
inline void ClBV::Init()                                        // Init() (protect)
  { m_nBitLength = m_nLongLength = m_nByteRest = 0;
    m_bVect = NULL;
  }


//***************************************************************************************
//***************************************************************************************
//-------------------------------------------------------------------
void ClBV::AssignDiz(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nBitLength >= 0);
  ASSERT(nBitLength == m_nBitLength);
  int i;
  ULONG *pv1 = (ULONG*)v1;
  ULONG *pv2 = (ULONG*)v2;

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = pv1[i] | pv2[i];
  if (m_nByteRest!=0) {
    ULONG val = pv1[i] | pv2[i];
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-------------------------------------------------------------------
void ClBV::AssignDiz(int nBitLength, int Num, BYTE* v1, ...)
{
  ASSERT(nBitLength >= 0);
  ASSERT(nBitLength == m_nBitLength);

  int i,j;
  ULONG * v[10];
  va_list marker;
  v[0] = (ULONG*)v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (j=1; j< Num; j++) {
    v[j] = (ULONG*)va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (i=0; i<m_nLongLength; i++) {
    m_bVect[i] = v[0][i];
    for (j=1; j< Num; j++) {
      m_bVect[i] |= v[j][i];     
    }
  }
  if (m_nByteRest!=0) {
    ULONG val = v[0][i];
    for (j=1; j< Num; j++) {
      val |= v[j][i];     
    }
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-------------------------------------------------------------------
void ClBV::AssignCon(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nBitLength >= 0);
  ASSERT(nBitLength == m_nBitLength);
  int i;
  ULONG *pv1 = (ULONG*)v1;
  ULONG *pv2 = (ULONG*)v2;

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = pv1[i] & pv2[i];
  if (m_nByteRest!=0) {
    ULONG val = pv1[i] & pv2[i];
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-------------------------------------------------------------------
void ClBV::AssignCon(int nBitLength, int Num, BYTE* v1, ...)
{
  ASSERT(nBitLength >= 0);
  ASSERT(nBitLength == m_nBitLength);

  int i,j;
  ULONG * v[10];
  va_list marker;
  v[0] = (ULONG*)v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (j=1; j< Num; j++) {
    v[j] = (ULONG*)va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (i=0; i<m_nLongLength; i++) {
    m_bVect[i] = v[0][i];
    for (j=1; j< Num; j++) {
      m_bVect[i] &= v[j][i];     
    }
  }
  if (m_nByteRest!=0) {
    ULONG val = v[0][i];
    for (j=1; j< Num; j++) {
      val &= v[j][i];     
    }
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-------------------------------------------------------------------
void ClBV::AssignXor(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nBitLength >= 0);
  ASSERT(nBitLength == m_nBitLength);
  int i;
  ULONG *pv1 = (ULONG*)v1;
  ULONG *pv2 = (ULONG*)v2;

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = pv1[i] ^ pv2[i];
  if (m_nByteRest!=0) {
    ULONG val = pv1[i] ^ pv2[i];
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-------------------------------------------------------------------
void ClBV::AssignXor(int nBitLength, int Num, BYTE* v1, ...)
{
  ASSERT(nBitLength >= 0);
  ASSERT(nBitLength == m_nBitLength);

  int i,j;
  ULONG* v[10];
  va_list marker;
  v[0] = (ULONG*)v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (j=1; j< Num; j++) {
    v[j] = (ULONG*)va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (i=0; i<m_nLongLength; i++) {
    m_bVect[i] = v[0][i];
    for (j=1; j< Num; j++) {
      m_bVect[i] ^= v[j][i];     
    }
  }
  if (m_nByteRest!=0) {
    ULONG val = v[0][i];
    for (j=1; j< Num; j++) {
      val ^= v[j][i];     
    }
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}

//-------------------------------------------------------------------
void ClBV::AssignDif(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nBitLength >= 0);
  ASSERT(nBitLength == m_nBitLength);
  int i;
  ULONG *pv1 = (ULONG*)v1;
  ULONG *pv2 = (ULONG*)v2;

  for (i=0; i<m_nLongLength; i++) 
    m_bVect[i] = pv1[i] & ~pv2[i];
  if (m_nByteRest!=0) {
    ULONG val = pv1[i] & ~pv2[i];
    BYTE * pB = (BYTE *)&val;
    BYTE * pBv = (BYTE *)(m_bVect + i);
    for (int k=0; k<m_nByteRest; k++)
      pBv[k] = pB[k];
  }
}
