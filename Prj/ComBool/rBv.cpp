/////////////////////////////////////////////////////////////////
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1     17.03.2005
// Ver.1.0     08.03.2005
/////////////////////////////////////////////////////////////////
// Rare Boolean vector

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../ComBool/RareBool.h"

#ifndef _LINUX
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#endif

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Construction/Destruction ///////////////////////////////////////

//----------------------------------------------------------------------(1)
CrBV::CrBV()   { m_nVect.empty(); m_invert = TRUE; m_length = 0; }

//----------------------------------------------------------------------(2)
CrBV::CrBV(size_t Len, BOOL Inv)   { m_nVect.empty(); m_invert = Inv; m_length = Len; }

//----------------------------------------------------------------------(3)
CrBV::CrBV(const CrBV& rbv)
{
  m_nVect.insert(m_nVect.begin(), rbv.m_nVect.begin( ), rbv.m_nVect.end());
  m_invert = rbv.m_invert;
  m_length = rbv.m_length;
}

//----------------------------------------------------------------------(3a)
CrBV::CrBV(VECTOR* v, size_t Len, BOOL Inv)
{
  m_nVect.insert(m_nVect.begin(), v->begin( ), v->end());
  m_invert = Inv;
  m_length = Len;
}

//----------------------------------------------------------------------(4)
CrBV::CrBV(const CBV& bv, BOOL Inv /* == TRUE */)
{ 
  size_t i, j = 0;
  for (i = bv.LeftOne(-1); i>=0; i = bv.LeftOne(i)) {
    if (Inv == TRUE)
      m_nVect.push_back(i);
    else {
      for (; j<i; j++) 
        m_nVect.push_back(j);
      j++;
    }
  }
  m_invert = Inv;
  m_length = bv.GetBitLength();
}

//----------------------------------------------------------------------(5)
CrBV::CrBV(const char* pch, size_t Len, BOOL Inv)
{
  size_t i, j = 0, w;
  char *s = (char *)pch;
  w = strlen(s);
  m_invert = Inv;
  m_length = Len;
  while (w>0 && (*s <= '9' && *s >= '0')) {
    sscanf(s, "%u,%n", &j, &i);
    if (j >= m_length) return; 
    m_nVect.push_back(j);
    s +=i;
    w -=i;
  }
}

//----------------------------------------------------------------------(6)
CrBV::~CrBV()  { m_nVect.clear(); }

//=======================================================================================

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Reading and writing //////////////////////////////////
//----------------------------------------------------------------------(8)
BOOL CrBV::GetBitAt(size_t nIndex) const
{ 
  ASSERT(m_length > 0 && nIndex >=0 && nIndex < m_length);
  size_t Pos;
  if (m_invert == FALSE)
     if (m_nVect.size()==0) return TRUE;
     else  return !(Find(nIndex, Pos));
  else
    if (m_nVect.size()==0) return FALSE;
    else  return (Find(nIndex, Pos));
}

#ifndef _LINUX
//----------------------------------------------------------------------(9)
CString CrBV::BitChar(char One,char Zero) const
{
  size_t i;

  if (m_length==0) return "";
  if (m_nVect.size() == 0)
    if (m_invert == TRUE) {
      CString res('0',m_length);
      return res;
    }
    else {
      CString res('1',m_length);
      return res;
    }
  CString res('\0',m_length);
  VECTOR:: const_iterator it = m_nVect.begin();

  if (m_invert == TRUE) {
    for (i=0; i < m_length; i++) {
      if (i==*it) {
        res.SetAt(i,One);
        it++;
      }
      else   res.SetAt(i,Zero);
    }
  }
  else {
    for (i=0; i < m_length; i++) {
      if (i==*it) {
        res.SetAt(i,Zero);
        it++;
      }
      else   res.SetAt(i,One);
    }
  }
  return res;
}

//----------------------------------------------------------------------(10)
CString CrBV::BitList() const
{
  CString res, w;
  VECTOR:: const_iterator it = m_nVect.begin();

  if (m_invert == TRUE)     res = "1 : ";
  else          res = "0 : ";
  for (; it != m_nVect.end(); ++it) {
    w.Format("%u", *it);
    res += w + ", ";
  }
  return res;
}   
#else
//----------------------------------------------------------------------(9)
char* CrBV::BitChar(char One,char Zero) const
{
  size_t i;
  char* res;
  if (m_length==0) return NULL;
  res = (char*)malloc(m_length+1);
  if (m_nVect.size() == 0)
  {
    res[m_length]=0;
    if (m_invert == TRUE) memset(res,'0',m_length);
    else                  memset(res,'1',m_length);
    return res;
  }
  memset(res,'\0',m_length);
  VECTOR:: const_iterator it = m_nVect.begin();

  if (m_invert == TRUE) {
    for (i=0; i < m_length; i++) {
      if (i==*it) {
        res[i] = One;
        it++;
      }
      else   res[i] = Zero;
    }
  }
  else {
    for (i=0; i < m_length; i++) {
      if (i==*it) {
        res[i] = Zero;
        it++;
      }
      else   res[i] = One;
    }
  }
  return res;
}

//----------------------------------------------------------------------(10)
char* CrBV::BitList() const
{
  char* res;
  char w[16];
  size_t siz = m_nVect.size();
  res = (char*)malloc(siz*6);
  VECTOR:: const_iterator it = m_nVect.begin();

  if (m_invert == TRUE)     strcpy(res,"1 : ");
  else          strcpy(res,"0 : ");
  for (; it != m_nVect.end(); ++it) {
    sprintf(w,"%u, ", *it);
    strcat(res,w);
  }
  return res;
}   
#endif

//----------------------------------------------------------------------(11)
CrBV CrBV::Extract(size_t nFirst,size_t nCount)
{ 
  CrBV s;   // new vector
  size_t Last = nFirst + nCount; // last element
  ASSERT(nCount >= 0);  ASSERT(nFirst >= 0);
  ASSERT((nCount+nFirst) <= m_length);
  s.m_invert = m_invert;
  s.m_length = nCount;
  if (nCount==0 || m_nVect.size()==0) return s;
  size_t PosBeg, PosEnd;
  Find(nFirst, PosBeg);
  Find(Last, PosEnd);
  for (size_t k = PosBeg; k <PosEnd; k++)
    s.m_nVect.push_back(m_nVect[k]-nFirst);
  return s;
}

//----------------------------------------------------------------------(12)
void CrBV::SetBitAt(size_t nIndex,  BOOL bit)
{ 
  ASSERT(nIndex >= 0); ASSERT(nIndex < m_length);
  size_t Pos, Val;
  Val = Find(nIndex, Pos);
  VECTOR:: iterator it = m_nVect.begin()+Pos;
  if (m_invert == TRUE) {//    res = "1 : ";
    if (Val) {
      if (!bit) m_nVect.erase(it);
    }
    else {
      if (bit)  m_nVect.insert(it, nIndex);
    }
  }
  else {                   //   res = "0 : ";
    if (Val) {
      if (bit)  m_nVect.erase(it);
    }
    else {
      if (!bit) m_nVect.insert(it, nIndex);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////// Assignment operator and functions ////////////////////////////////
//----------------------------------------------------------------------(13)
const CrBV& CrBV::operator =(const CrBV& rbv)              //operator =
{ 
  m_nVect.clear();
  m_nVect.insert(m_nVect.begin(), rbv.m_nVect.begin( ), rbv.m_nVect.end());
  m_invert = rbv.m_invert;
  m_length = rbv.m_length;
  return *this;
}

//----------------------------------------------------------------------(14)
const CrBV& CrBV::operator=(const CBV& bv)
{
  size_t i, j = 0;
  m_invert = TRUE;
  m_length = bv.GetBitLength();
  m_nVect.clear();
  for (i = bv.LeftOne(-1); i>=0; i = bv.LeftOne(i)) {
    if (i >= m_length) return *this;
    m_nVect.push_back(i);
  }
  return *this;
}

//----------------------------------------------------------------------(15)
void CrBV::Reverse()
{
  size_t Count = m_length - m_nVect.size() + 10;
  size_t g;
  VECTOR temp;
  temp.reserve(m_nVect.size());
  temp.insert(temp.begin(), m_nVect.begin(),m_nVect.end());
  m_nVect.clear();
  m_nVect.reserve(Count);
  VECTOR:: const_iterator it1 = temp.begin();
  if (it1 == temp.end()) g = m_length;
  else g = *it1;
  for (size_t k=0; k < m_length; k++) {
    if (k<g) m_nVect.push_back(k);
    else {
      it1++;
      if (it1 == temp.end()) g = m_length;
      else g = *it1;
    }
  }
  m_invert = !m_invert;
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Logic operations //////////////////////////////////////

//----------------------------------------------------------------------(20)
void CrBV::Diz(const CrBV& rbv1, const CrBV& rbv2)
{ 
  ASSERT(rbv1.m_length == rbv2.m_length);
  m_length = rbv1.m_length;
  if (rbv1.m_invert==TRUE && rbv2.m_invert==TRUE) {
    m_invert=TRUE; SetU(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  }
  else {
    m_invert = FALSE;
    if (rbv1.m_invert==TRUE && rbv2.m_invert==FALSE)
      SetD(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
    else if (rbv1.m_invert==FALSE && rbv2.m_invert==TRUE)
      SetD(m_nVect, rbv2.m_nVect, rbv1.m_nVect);
    else
      SetI(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  }
}

//----------------------------------------------------------------------(21)
void CrBV::Con(const CrBV& rbv1, const CrBV& rbv2)
{ 
  ASSERT(rbv1.m_length == rbv2.m_length);
  m_length = rbv1.m_length;
  m_invert=TRUE;
  if (rbv1.m_invert==TRUE && rbv2.m_invert==TRUE) 
    SetI(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  else if (rbv1.m_invert==TRUE && rbv2.m_invert==FALSE) 
    SetD(m_nVect, rbv2.m_nVect, rbv1.m_nVect);
  else if (rbv1.m_invert==FALSE && rbv2.m_invert==TRUE) 
    SetD(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  else {
    m_invert=FALSE; SetU(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  }
}

//----------------------------------------------------------------------(22)
void CrBV::Xor(const CrBV& rbv1, const CrBV& rbv2)
{ 
  ASSERT(rbv1.m_length == rbv2.m_length);
  m_length = rbv1.m_length;
  if (rbv1.m_invert == rbv2.m_invert) {
    m_invert=TRUE;   SetA(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  }
  else {
    m_invert = FALSE;  SetA(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  }
}

//----------------------------------------------------------------------(23)
void CrBV::Dif(const CrBV& rbv1, const CrBV& rbv2)
{ 
  ASSERT(rbv1.m_length == rbv2.m_length);
  m_length = rbv1.m_length;
  if (rbv1.m_invert==TRUE && rbv2.m_invert==TRUE) {
    m_invert=TRUE; SetD(m_nVect, rbv2.m_nVect, rbv1.m_nVect);
  }
  else if (rbv1.m_invert==TRUE && rbv2.m_invert==FALSE) {
    m_invert=TRUE; SetI(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  }
  else if (rbv1.m_invert==FALSE && rbv2.m_invert==TRUE) {
    m_invert=FALSE;  SetU(m_nVect, rbv2.m_nVect, rbv1.m_nVect);
  }
  else {
    m_invert=TRUE; SetD(m_nVect, rbv1.m_nVect, rbv2.m_nVect);
  }
}


//----------------------------------------------------------------------(24)
CrBV operator |(const CrBV& bv1, const CrBV& bv2)
{ 
  CrBV s;  s.Diz(bv1, bv2); return s;
}

//----------------------------------------------------------------------(25)
CrBV operator &(const CrBV& bv1, const CrBV& bv2)
{ 
  CrBV s;  s.Con(bv1, bv2); return s;
}

//----------------------------------------------------------------------(26)
CrBV operator ^(const CrBV& bv1, const CrBV& bv2)
{ 
  CrBV s;  s.Xor(bv1, bv2); return s;
}

//----------------------------------------------------------------------(27)
CrBV operator -(const CrBV& bv1, CrBV& bv2)
{ 
  CrBV s;  s.Dif(bv1, bv2); return s;
}

//----------------------------------------------------------------------(28)
CrBV operator~(const CrBV& bv2)
{ 
  CrBV s(bv2);  s.Invert(); return s;
}


/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Shift operations //////////////////////////////////////
//----------------------------------------------------------------------(29)
CrBV operator<<(const CrBV& bv1, size_t nShift)
{ 
  CrBV s(bv1.m_length, bv1.m_invert);
  ASSERT(nShift >= 0 && nShift < s.m_length);
  size_t Pos;
  bv1.Find(nShift, Pos);
  for (; (unsigned) Pos < bv1.m_nVect.size(); Pos++)
    s.m_nVect.push_back(bv1.m_nVect[Pos] - nShift);
  if (bv1.m_invert==FALSE)
    for (Pos = s.m_length - nShift; Pos < s.m_length; Pos++)
      s.m_nVect.push_back(Pos);
  return s;
}
   
//----------------------------------------------------------------------(30)
CrBV operator>>(const CrBV& bv1, size_t nShift)
{ 
  CrBV s(bv1.m_length, bv1.m_invert);
  ASSERT(nShift >= 0 && nShift < s.m_length);
  size_t Pos, i;
  if (bv1.m_invert==FALSE)
    for (Pos = 0; Pos < nShift; Pos++)
      s.m_nVect.push_back(Pos);
  for (i=0; ((unsigned)i < bv1.m_nVect.size()) && (bv1.m_nVect[i]+nShift < s.m_length); i++)
    s.m_nVect.push_back(bv1.m_nVect[i] + nShift);
  return s;
}


//----------------------------------------------------------------------(31)
void CrBV::LoopLeftShift(size_t nShift)
{ 
  nShift %= m_length;
  if (nShift==0) return;
  VECTOR temp;
  temp.reserve(m_nVect.size());

  size_t Pos,g;
  Find(nShift, Pos);
  g = Pos;
  for (; (unsigned)Pos < m_nVect.size(); Pos++)
    temp.push_back(m_nVect[Pos] - nShift);
  for (Pos = 0; Pos < g; Pos++)
      temp.push_back(m_nVect[Pos] + m_length - nShift);
  
  m_nVect.assign(temp.begin(),temp.end());
  return;
}
   
//----------------------------------------------------------------------(32)
void CrBV::LoopRightShift(size_t nShift)
{ 
  nShift %= m_length;
  if (nShift==0) return;
  LoopLeftShift(m_length - nShift);
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Operators of advanced assignment ///////////////////////////

//----------------------------------------------------------------------(33)
const CrBV& CrBV::operator |=(const CrBV& bvr)
{
  CrBV temp(m_length, m_invert);
  temp.m_nVect.assign(m_nVect.begin(), m_nVect.end());
  Diz(temp,bvr);
  return *this;
}

//----------------------------------------------------------------------(34)
const CrBV& CrBV::operator &=(const CrBV& bvr)
{
  CrBV temp(m_length, m_invert);
  temp.m_nVect.assign(m_nVect.begin(), m_nVect.end());
  Con(temp,bvr);
  return *this;
}

//----------------------------------------------------------------------(35)
const CrBV& CrBV::operator ^=(const CrBV& bvr)
{
  CrBV temp(m_length, m_invert);
  temp.m_nVect.assign(m_nVect.begin(), m_nVect.end());
  Xor(temp,bvr);
  return *this;
}

//----------------------------------------------------------------------(36)
const CrBV& CrBV::operator -=(const CrBV& bvr)
{
  CrBV temp(m_length, m_invert);
  temp.m_nVect.assign(m_nVect.begin(), m_nVect.end());
  Dif(temp,bvr);
  return *this;
}

//----------------------------------------------------------------------(37)
const CrBV& CrBV::operator <<=(size_t nShift)
{
  CrBV temp(m_length, m_invert);
  temp.m_nVect.assign(m_nVect.begin(), m_nVect.end());
  m_nVect.clear();
  ASSERT(nShift >= 0 && nShift < m_length);
  size_t Pos;
  temp.Find(nShift, Pos);
  for (; (unsigned)Pos < temp.m_nVect.size(); Pos++)
    m_nVect.push_back(temp.m_nVect[Pos] - nShift);
  if (temp.m_invert==FALSE)
    for (Pos = m_length - nShift; Pos < m_length; Pos++)
      m_nVect.push_back(Pos);
  return *this;
}

//----------------------------------------------------------------------(38)
const CrBV& CrBV::operator >>=(size_t nShift)
{
  CrBV temp(m_length, m_invert);
  temp.m_nVect.assign(m_nVect.begin(), m_nVect.end());
  m_nVect.clear();
  ASSERT(nShift >= 0 && nShift < m_length);
  size_t Pos, i;
  if (temp.m_invert==FALSE)
    for (Pos = 0; Pos < nShift; Pos++)
      m_nVect.push_back(Pos);
  for (i=0; ((unsigned)i < temp.m_nVect.size()) && (temp.m_nVect[i] + nShift < m_length); i++)
    m_nVect.push_back(temp.m_nVect[i] + nShift);
  return *this;
}

//******************** Operations of weighting, finding and casing **********************
//----------------------------------------------------------------------(40)
size_t CrBV::LeftOne(size_t nNext /*= -1*/) const
{
  size_t i,j, Pos;
  if (nNext== -1) {
    if (m_invert) 
      if (m_nVect.size()>0) return m_nVect[0];
      else return -1;
    else {
      for (i=0; (unsigned)i<m_nVect.size(); i++)
        if (i != m_nVect[i]) return i;
      if (m_nVect.size() < (unsigned)m_length) return i;
      return -1;
    }
  }
  nNext++;
  if (nNext >= m_length) return -1;

  if (m_invert) {   // 1
    if (Find(nNext, Pos)) return nNext;
    if ((unsigned)Pos >= m_nVect.size()) return -1;
    return m_nVect[Pos];
  }
  // 0
  if (!Find(nNext, Pos))  return nNext;
  nNext++;
  for (j=Pos+1; (unsigned)j<m_nVect.size(); j++) {
    if (m_nVect[j] != nNext) return nNext;
    nNext++;
  } 
  if (nNext >= m_length) return -1;
  return nNext;
}


//----------------------------------------------------------------------(41)
size_t CrBV::RightOne(size_t nNext /*= -1*/) const
{
  size_t i,j, Pos;
  if (nNext== -1) {
    if (m_invert) 
      if (m_nVect.size()>0) return m_nVect[m_nVect.size()-1];
      else return -1;
    else {
      if (m_nVect.size() == m_length) return -1;

      for (j=1, i=m_nVect.size()-1; i>=0; i--, j++)
        if (m_length - j != m_nVect[i]) return (m_length - j);
      return -1;
    }
  }
  if (nNext == 0) return -1;
  nNext--;
  if (m_invert) {   // 1
    if (Find(nNext, Pos)) return nNext;
    if (Pos >= 1) return m_nVect[Pos-1];
     return -1;
  }

  // 0
  if (!Find(nNext, Pos))  return nNext;
  nNext--;
  if (Pos==0) return nNext;
  for (j=Pos-1; j>=0; j--) {
    if (m_nVect[j] != nNext) return nNext;
    nNext--;
  } 
  return nNext;
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Concatination //////////////////////////////////////

//----------------------------------------------------------------------(42)
void CrBV::Concat(const CrBV& rbv)
{
  if (m_invert == rbv.m_invert) {
    for (size_t i=0; (unsigned)i<rbv.m_nVect.size(); i++)
      m_nVect.push_back(rbv.m_nVect[i] + m_length);
  }
  else {
    size_t j=0, i;
    for (i=0; (unsigned)i<rbv.m_nVect.size(); i++, j++) {
      while (j < rbv.m_nVect[i])
        m_nVect.push_back(m_length + (j++));
    }
    for (; j<rbv.m_length; j++) {
      m_nVect.push_back(m_length + j);
    }
  }
  m_length += rbv.m_length;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Comparing operations ///////////////////////////////////////
BOOL CrBV::operator ==(const CrBV& rbv2)
{ 
  if (GetLength() != rbv2.GetLength()) return FALSE;
  if (GetForm() == rbv2.GetForm()) 
    return Equal11_00(&rbv2);
  else
    return Equal10_01(&rbv2);
}

//----------------------------------------------------------------------(47)
BOOL CrBV::operator !=(const CrBV& rbv2) 
{
  if (GetLength() != rbv2.GetLength()) return TRUE;
  if (GetForm() == rbv2.GetForm()) 
    return !Equal11_00(&rbv2);
  else
    return !Equal10_01(&rbv2);

}

//----------------------------------------------------------------------(48)
BOOL CrBV::operator >(const CrBV& rbv2)
{ 
  if (GetLength() != rbv2.GetLength()) return FALSE;
  if (GetForm() == rbv2.GetForm()) {  // Equal form
    if (Equal11_00(&rbv2)) return FALSE;
    if (GetForm())
      return (Absorb11(&rbv2));
    else
      return (Absorb00(&rbv2));
  }
  else {                                  // Not equal form
    if (Equal10_01(&rbv2)) return FALSE;  // is equal?
    return Con01(rbv2, TRUE);
  }
}

//----------------------------------------------------------------------(49)
BOOL CrBV::operator <(const CrBV& rbv2)
{ 
  if (GetLength() != rbv2.GetLength()) return FALSE;
  if (GetForm() == rbv2.GetForm()) {  // Equal form
    if (Equal11_00(&rbv2)) return FALSE;
    if (GetForm())
      return rbv2.Absorb11(this);
    else
      return rbv2.Absorb00(this);
  }
  else {                                      // Not equal form
    if (rbv2.Equal10_01(this)) return FALSE;  // is equal?
    return Con01(rbv2, FALSE);
  }
}

//----------------------------------------------------------------------(50)
BOOL CrBV::operator >=(const CrBV& rbv2)
{ 
  if (GetLength() != rbv2.GetLength()) return FALSE;
  if (GetForm() == rbv2.GetForm()) {  // Equal form
    if (GetForm())
      return Absorb11(&rbv2);
    else
      return Absorb00(&rbv2);
  }
  else {                                  // Not equal form
    if (Equal10_01(&rbv2)) return TRUE;   // is equal?
    return Con01(rbv2, TRUE);
  }
}

//----------------------------------------------------------------------(51)
BOOL CrBV::operator <=(const CrBV& rbv2)
{ 
  if (GetLength() != rbv2.GetLength()) return FALSE;
  if (GetForm() == rbv2.GetForm()) {  // Equal form
    if (GetForm())
      return rbv2.Absorb11(this);
    else
      return rbv2.Absorb00(this);
  }
  else {                                      // Not equal form
    if (rbv2.Equal10_01(this)) return TRUE;   // is equal?
    return Con01(rbv2, FALSE);
  }
}


#ifndef _LINUX
/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Input/output operations //////////////////////////////////////
#ifdef _DEBUG
//----------------------------------------------------------------------(52)
CDumpContext& operator<<(CDumpContext& dc, const CrBV& rbv)
{
  CString res;
  res.Format("Length = %u\n", rbv.m_length);
  res += rbv.BitList();
  dc << res; return dc;
}
#endif

//----------------------------------------------------------------------(53)
CArchive& operator<<(CArchive& ar, const CrBV& rbv)
{ ar << rbv.m_length;
  ar << (BYTE)rbv.m_invert;
  ar << rbv.m_nVect.size();
  for (size_t i=0; (unsigned)i < rbv.m_nVect.size(); i++)  ar << rbv.m_nVect[i];
  return ar;
}

//----------------------------------------------------------------------(54)
CArchive& operator>>(CArchive& ar, CrBV& rbv)
{
  size_t size, val;
  rbv.m_nVect.clear();
  ar >> rbv.m_length;
  ar >> size; rbv.m_invert = (size>0);
  ar >> size;
  if (size==0)  return ar;
  for (size_t i=0; i < size; i++)  {
    ar >> val; 
    rbv.m_nVect.push_back(val);
  }
  return ar;
}
#endif

//=====================================================================================//
//============================ Protected function =======================================
//----------------------------------------------------------------------(56)
BOOL CrBV::Find(size_t X, size_t& Pos) const
{
  size_t m, R=m_nVect.size() - 1, flag=0;
  Pos=0;
  while (Pos<=R && !flag)
  {
    m=(Pos+R)/2;
    if (m_nVect[m]==X) { Pos = m; return 1; }
    else if (m_nVect[m]<X)  Pos = m+1;
    else                    R   = m-1;
  }
  return flag;    // False, Pos - position for insert
}

//----------------------------------------------------------------------(57)
void CrBV::SetU(VECTOR& v0, const VECTOR& v1, const VECTOR& v2) 
{ // Union of sets
  v0.clear();
  if (v2.size() == 0) {
    v0.insert(v0.begin(), v1.begin(), v1.end()); return;
  }
  if (v1.size() == 0) {
    v0.insert(v0.begin(), v2.begin(), v2.end()); return;
  }
  
  VECTOR:: const_iterator it1 = v1.begin();
  VECTOR:: const_iterator it2 = v2.begin();
  while(it1!=v1.end()) {
    if (*it1 < *it2) {
      v0.push_back(*it1++);
    }
    else {
      v0.push_back(*it2);
      if (*it2 == *it1) it1++;
      it2++;
      if (it2 == v2.end()) {
        v0.insert(v0.end(), it1, v1.end()); return;
      }
    }
  }
  if (it2 != v2.end()) v0.insert(v0.end(), it2, v2.end());
}

//----------------------------------------------------------------------(58)
void CrBV::SetI(VECTOR& v0, const VECTOR& v1, const VECTOR& v2) 
{  // Intersection of sets
  v0.clear();
  if (v2.size() != 0 && v1.size() != 0) {
    VECTOR:: const_iterator it1 = v1.begin();
    VECTOR:: const_iterator it2 = v2.begin();
    while(it1!=v1.end()) {
      if (*it2 == *it1) {
        v0.push_back(*it2++);
        if (it2 == v2.end()) return;
        it1++;
      }
      else {
        if (*it1 < *it2) it1++;
        else {
          it2++;
          if (it2 == v2.end()) return;
        }
      }
    }
  }
}

//----------------------------------------------------------------------(59)
void CrBV::SetD(VECTOR& v0, const VECTOR& v1, const VECTOR& v2) 
{  // Второй без тех, которые есть в первом
  v0.clear();
  if (v2.size() == 0) {   // 0 is absent
    return;
  }
  else if (v1.size() == 0) {  // 1 is absent
    v0.insert(v0.begin(), v2.begin(), v2.end()); return;
  }
  
  VECTOR:: const_iterator it1 = v1.begin();
  VECTOR:: const_iterator it2 = v2.begin();
  while(it1!=v1.end()) {
    if (*it1 == *it2) {
      it1++;
      it2++;
      if ( it2 == v2.end()) return;
    }
    else {
      if (*it1 > *it2) {
        v0.push_back(*it2++);
        if ( it2 == v2.end()) return;
      }
      else
        it1++;
    }
  }
  if (it2 !=v2.end()) v0.insert(v0.end(), it2, v2.end());
}

//----------------------------------------------------------------------(60)
void CrBV::SetA(VECTOR& v0, const VECTOR& v1, const VECTOR& v2) 
{ // Все, что есть в списках, но не в обоих
  v0.clear();
  if (v2.size() == 0 ) {
     v0.insert(v0.begin(), v1.begin(), v1.end()); return;
  }
  if (v1.size() == 0 ) {
     v0.insert(v0.begin(), v2.begin(), v2.end()); return;
  }
  
  VECTOR:: const_iterator it1 = v1.begin();
  VECTOR:: const_iterator it2 = v2.begin();
  while (it1!=v1.end()) {
    if (*it1 < *it2) v0.push_back(*it1++);
    else {
      if (*it1 > *it2) {
        v0.push_back(*it2++);
        if (it2 == v2.end()) {
          v0.insert(v0.end(), it1, v1.end()); return;
        }
      }
      else {  // *it2 == *it1
        it1++;
        it2++;
        if (it2 == v2.end()) {
          v0.insert(v0.end(), it1, v1.end()); return;
        }
      }
    }
  }
  if (it2 != v2.end()) v0.insert(v0.end(), it2, v2.end());
}


//============================= Protected function ====================================//
//----------------------------------------------------------------------(61)
BOOL CrBV::Equal11_00(const CrBV* rbv) const
{
  if (rbv->m_nVect.size() != m_nVect.size()) return FALSE;
  
  for (size_t i=0; (unsigned)i < m_nVect.size(); i++)
    if (rbv->m_nVect[i] != m_nVect[i]) return FALSE;
  return TRUE;
}

//----------------------------------------------------------------------(62)
BOOL CrBV::Equal10_01(const CrBV* rbv) const
{
  if (rbv->m_nVect.size() != (m_length - m_nVect.size())) return FALSE;

  VECTOR:: const_iterator it1 = rbv->m_nVect.begin();
  VECTOR:: const_iterator it2 = m_nVect.begin();
  while(it1 != rbv->m_nVect.end()) {
    if (*it1 == *it2) return FALSE;
    if (*it1 < *it2) { it1++; }
    else {
      it2++;
      if (it2 == m_nVect.end()) return TRUE;
    }
  }
  return TRUE;
}

//----------------------------------------------------------------------(63)
BOOL CrBV::Absorb11(const CrBV* rbv) const   // >=
{
  size_t j=0;
  if (m_nVect.size() <= rbv->m_nVect.size()) return FALSE;
  if (rbv->m_nVect.size()==0) return TRUE;  
  VECTOR:: const_iterator it1 = m_nVect.begin();
  VECTOR:: const_iterator it2 = rbv->m_nVect.begin();
  while(it1!=m_nVect.end()) {
    if (*it1 == *it2) {
      it1++;
      it2++;
      if (it2 == rbv->m_nVect.end()) {
        if (it1 !=m_nVect.end()) return TRUE;
        else return FALSE;
      }
    }
    else {
      if (*it1 < *it2) { it1++; }
      else return FALSE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------(64)
BOOL CrBV::Absorb00(const CrBV* rbv) const  // >=
{
  size_t j=0;
  if (m_nVect.size() > rbv->m_nVect.size()) return FALSE;
  if (m_nVect.size()==0) return TRUE;  // First 111...111
  VECTOR:: const_iterator it1 = m_nVect.begin();
  VECTOR:: const_iterator it2 = rbv->m_nVect.begin();
  while(it1!=m_nVect.end()) {
    if (*it1 == *it2) {
      it1++;
      it2++;
      if (it2 == rbv->m_nVect.end()) {
        if (it1 !=m_nVect.end()) return FALSE;
        else return TRUE;
      }
    }
    else {
      if (*it1 < *it2) return FALSE;
      else { 
        it2++; 
        if (it2 == rbv->m_nVect.end()) return FALSE;
      }
    }
  }
  return FALSE;
}


//----------------------------------------------------------------------(65)
BOOL CrBV::Con01(const CrBV& rbv2, BOOL Opt)
{ 
  VECTOR Vect;    // TRUE
  if (m_invert==TRUE && rbv2.m_invert==FALSE) 
    SetD(Vect, rbv2.m_nVect, m_nVect);
  else
    SetD(Vect, m_nVect, rbv2.m_nVect);
//------------------------------------------
  if (Opt) {  // Vect(TRUE) compare with rbv2
    if (rbv2.GetForm()) { // TRUE-TRUE
        if (rbv2.m_nVect.size() != Vect.size()) return FALSE;
        for (size_t i=0; i < Vect.size(); i++)
          if (rbv2.m_nVect[i] != Vect[i]) return FALSE;
        return TRUE;
    }        
    else  { // TRUE-FALSE        return Equal10_01(&rbv2);
      if (rbv2.m_nVect.size() != (m_length - Vect.size())) return FALSE;
      VECTOR:: const_iterator it1 = rbv2.m_nVect.begin();
      VECTOR:: const_iterator it2 = Vect.begin();
      while(it1 != rbv2.m_nVect.end()) {
        if (*it1 == *it2) return FALSE;
        if (*it1 < *it2) { it1++; }
        else {
          it2++;
          if (it2 == Vect.end()) return TRUE;
        }
      }
      return TRUE;
    }

  }
  else {  // compare with this    
    if (GetForm()) { // TRUE-TRUE
        if (m_nVect.size() != Vect.size()) return FALSE;
        for (size_t i=0; i < Vect.size(); i++)
          if (m_nVect[i] != Vect[i]) return FALSE;
        return TRUE;
    }        
    else  { // TRUE-FALSE     return Equal10_01(&rbv1);
      if (m_nVect.size() != (m_length - Vect.size())) return FALSE;
      VECTOR:: const_iterator it1 = m_nVect.begin();
      VECTOR:: const_iterator it2 = Vect.begin();
      while(it1 != m_nVect.end()) {
        if (*it1 == *it2) return FALSE;
        if (*it1 < *it2) { it1++; }
        else {
          it2++;
          if (it2 == Vect.end()) return TRUE;
        }
      }
      return TRUE;
    }
  }
}
