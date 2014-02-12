/////////////////////////////////////////////////////////////////
// Ver.2.1.2    28.03.2008
//              SetOnes и SetZeros - исправлены для реализации в другой интерпретации
//              в виде функций AddOnes и AddZeros
// Ver.2.1.1    22.02.2007
//              AssignCopy - добавил чистку пространства от Len до AllocLen
//
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.2.0.4     19.05.2006
//               Поправки BitChar с ограничением длины строки
// Ver.2.0.3     02.04.2006
//               Поправка старой ошибки в SetZeros 
// Ver.2.0.2     21.03.2006
//               - Генераторы случайных перестроены на возврат *this
//               Поправка в SetSize - сразу возврат, если новый равен старому
//               Поправка в SetVect - разрешена запись в пустой вектор
// Ver.2.0.0     11.03.2006   Russian == English
//               - Добавлены генераторы случайных
// Ver.1.0.0     18.02.2005

/////////////////////////////////////////////////////////////////


#include <limits.h>
#include <string>
#include "../Common/BaseBool.h"
#include "../ComBool/UlongBool.h"
#include "../ComBool/UlongTern.h"

#ifndef _LINUX
// For Win
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

#else  //_LINUX
// For Linux
#undef _DEBUG

#endif //_LINUX

using namespace std; 
/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// Генерация псевдослучайного <nCol>-компонентного троичного //
// вектора с равномерным распределением значений '-','0'и'1' //
// по компонентам c вероятностями p(-)=1/2 и p(0)=p(1)=1/4.  //
///////////////////////////////////////////////////////////////

CuTV CuTV::GenRtv(int nCol)
{
  CuBV Rbv, Rbv_1;
  Rbv.GenRbv (nCol);
  Rbv_1 = Rbv & Rbv_1.GenRbv (nCol);
  SetSize(nCol);
  SetVect(Rbv_1, Rbv & ~Rbv_1);
  return *this;
}

///////////////////////////////////////////////////////////////
// Генерация псевдослучайного <nCol>-компонентного троичного //
// вектора со средним числом <nRang> определенных компонент  //
// (дисперсия 1/32), принимающих значение'0'или'1'c вероятн.1/2
// (при nRang>=nCol все комп. вектора имеют определенное значение) 
///////////////////////////////////////////////////////////////

CuTV CuTV::GenRtvMid(int nCol, int nRang)
{
  CuBV Rbv, Rbv_1; 
  Rbv.GenRbvMid(nCol, nRang); 
  Rbv_1 = Rbv & Rbv.GenRbv(nCol);
  SetSize(nCol);
  SetVect(Rbv_1, Rbv & ~Rbv_1);
  return *this;
}

///////////////////////////////////////////////////////////////
// Генерация псевдослучайного <nCol>-компонентного троичного //
// вектора с фиксированным числом <nRang> определенных компо-//
// нент, принимающих значение '0' или '1' c вероятностью 1/2 
// (при nRang>=nCol все комп. вектора имеют определенное значение) 
///////////////////////////////////////////////////////////////

CuTV CuTV::GenRtvFix (int nCol, int nRang)
{
  CuBV Rbv, Rbv_1; 
  Rbv.GenRbvFix(nCol, nRang); 
  Rbv_1 = Rbv & Rbv.GenRbv(nCol);
  SetSize(nCol);
  SetVect(Rbv_1, Rbv & ~Rbv_1);
  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Construction/Destruction ///////////////////////////////////////

//------------------------ CuTV ----------------------------------------------------------
CuTV::CuTV()   { Init(); }

//------------------------ CuTV (copy of CuTV) --------------------------------------------
CuTV::CuTV(const CuTV& tvSrc)
{
 if (tvSrc.m_nLongLength == 0) Init();
 else {
   AllocBuffer(tvSrc.m_nLongLength);
   memcpy(m_bVect1, tvSrc.m_bVect1,tvSrc.m_nLongLength*sizeof(ULONG));
   memcpy(m_bVect0, tvSrc.m_bVect0,tvSrc.m_nLongLength*sizeof(ULONG));
   m_nBitLength = tvSrc.m_nBitLength;
 }
}

//------------------------ CuTV (copy of boolean vectors pair) -----------------------------
CuTV::CuTV(const CuBV& bv1Src, const CuBV& bv0Src)
{ 
  int len = bv1Src.GetLongLength();
  ASSERT(bv1Src.GetBitLength() == bv0Src.GetBitLength());
  if (len == 0) Init();
  else {
    AllocBuffer(len);
    memcpy(m_bVect1, (ULONG*)bv1Src,len*sizeof(ULONG));
    memcpy(m_bVect0, (ULONG*)bv0Src,len*sizeof(ULONG));
    m_nBitLength = bv1Src.GetBitLength();
  }
}

//------------------------ CuTV (array of bits) ------------------------------------------
CuTV::CuTV(int nLenBit, char value)
{
  int nLenLong, w;
  ULONG a1, a0;
  if (nLenBit < 1) Init();      // return empty vector if invalid repeat count
  else {
    nLenLong = LEN_LONG(nLenBit);
    AllocBuffer(nLenLong);
    m_nBitLength = nLenBit;
     switch (value) {
       case '1': a1 = LONG_1; a0 = 0; break;
       case '0': a1 = 0; a0 = LONG_1; break;
       case '-': a1 = 0; a0 = 0; break;
       case '+': a1 = 1; a0 = 1; break;
        default: return;
     }
    memset(m_bVect1,a1, nLenLong*sizeof(ULONG));
    memset(m_bVect0,a0, nLenLong*sizeof(ULONG));
    if (w = ADR_BITLONG(nLenBit)) {
      m_bVect1[nLenLong-1] = (m_bVect1[nLenLong-1]>>(S_4-w))<<(S_4-w);
      m_bVect0[nLenLong-1] = (m_bVect0[nLenLong-1]>>(S_4-w))<<(S_4-w);
    }
  }
}

//------------------------ CuTV (copy of byte arrays pair) -------------------------------
CuTV::CuTV(const ULONG* pbt1, const ULONG* pbt0, int nLenBit)
{
  int nLenLong;
  if (nLenBit == 0) Init();
  else   {
    nLenLong = LEN_LONG(nLenBit);
    AllocBuffer(nLenLong);
    memcpy(m_bVect1, pbt1, nLenLong*sizeof(ULONG));
    memcpy(m_bVect0, pbt0, nLenLong*sizeof(ULONG));
    m_nBitLength = nLenBit;
  }
}

//------------------------ CuTV (copy of chararray) ---------------------------------------
CuTV::CuTV(const char* pch)
{
  int nLenLong, nLenBit;
  if ((nLenBit = strlen((char*)pch)) == 0) Init();
  else   {
    nLenLong = LEN_LONG(nLenBit);        // Translate from symbol to bit
    AllocBuffer(nLenLong);
    CharBit(nLenLong,pch);
    m_nBitLength = nLenBit;
  }
}

//-------------------------~CuTV() -------------------------------------------------------
CuTV::~CuTV()       //  free any attached data
{ if (m_bVect1 != NULL) delete [] m_bVect1; }


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Data Reading ////////////////////////////////////////////////

//------------------------- GetBitAt ----------------------------------------------------
char CuTV::GetBitAt(int nIndex) const
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  ULONG a1, a0;
  a1=m_bVect1[BIT_LONG(nIndex)] & OB4[ADR_BITLONG(nIndex)];
  a0=m_bVect0[BIT_LONG(nIndex)] & OB4[ADR_BITLONG(nIndex)];
  if(a1 != 0 && a0 == 0)  return '1';
  if(a0 != 0 && a1 == 0)  return '0';
  if(a0 == 0 && a1 == 0)  return '-';
  return '+';
}

//----------------------------  Extract ------------------------------------------------
CuTV CuTV::Extract(int nFirst, int nCount)
{ CuTV s;
 s.Extr(m_bVect1, m_bVect0, m_nBitLength, nFirst, nCount);  return s;
}

#ifndef _LINUX
CString CuTV::BitChar(char One, char Zero, char Dont, char Rest, int Max)
{ int i,j=0;
CString res('\0',m_nBitLength);
for (i=0; i < m_nBitLength; i++, j++)
{
  switch (GetBitAt(i)) {
  case '1': res.SetAt(j,One); break;
  case '0': res.SetAt(j,Zero); break;
  case '-': res.SetAt(j,Dont); break;
  default:  res.SetAt(j,Rest);
  }
  if (Max!=0)
    if ((Max==1)||((i+1)%Max == 0))
    { res.SetAt(++j,'\n'); }   //res.SetAt(++j,'\r'); 
}
return res;
}
#else
//------------------------- BitChar ----------------------------------------------------
string CuTV::BitChar(char One, char Zero, char Dont, char Rest, int Max)
{ int i,j=0;
  string res;
  res.resize('\0',m_nBitLength);
  for (i=0; i < m_nBitLength; i++, j++)
  {
    switch (GetBitAt(i)) {
     case '1': res[j] = One; break;
     case '0': res[j] = Zero; break;
     case '-': res[j] = Dont; break;
     default:  res[j] = Rest;
    }
    if (Max!=0)
      if ((Max==1)||((i+1)%Max == 0))
      { res[++j] = '\n'; }  //res[++j] = '\r'; 
  }
  return res;
}
#endif

//-------------------------------------------------------------------
CuBV CuTV::GetDefs() const
{ CuBV bv(m_bVect0, m_nBitLength);
 int i;
 for(i=0; i < m_nLongLength; i++)
    bv.SetLongAt(i, m_bVect1[i]|m_bVect0[i]);
 return bv;
}

//------------------------- CuBV GetUnDefs -----------------------------------------------
CuBV CuTV::GetUnDefs() const
{ CuBV bv(m_bVect0, m_nBitLength);
 int i;
 for(i=0; i < m_nLongLength-1; i++)
    bv.SetLongAt(i,~(m_bVect1[i]|m_bVect0[i]));
 ULONG maska = LONG_1;
 if ((i = ADR_BITLONG(m_nBitLength)) > 0 ) {
   i = S_4 - i; maska = LONG_1 << i; }
 bv.SetLongAt(m_nLongLength-1,
 (~(m_bVect1[m_nLongLength-1]|m_bVect0[m_nLongLength-1]))&maska);
  return bv;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Data Writing ////////////////////////////////////////////////

//---------------------------- SetBitAt ------------------------------------------------
void CuTV::SetBitAt(int nIndex, char symb)
{ ASSERT(nIndex >= 0 && nIndex < m_nBitLength);
  int i, j;
  i=BIT_LONG(nIndex); j=ADR_BITLONG(nIndex);
  switch (symb) {
     case '1': m_bVect1[i] |=OB4[j]; m_bVect0[i] &=~OB4[j];break;
     case '0': m_bVect0[i] |=OB4[j]; m_bVect1[i] &=~OB4[j]; break;
     case '-': m_bVect1[i] &=~OB4[j]; m_bVect0[i] &=~OB4[j]; break;
     default:  m_bVect1[i] |=OB4[j]; m_bVect0[i] |=OB4[j];
    }
}

//--------------------------- SetVect ---------------------------------------------------
void CuTV::SetVect(CuBV bv1, CuBV bv0)  //new  =-1-неудача
{ 
  int len=bv1.GetLongLength();
  ASSERT(bv1.GetBitLength() == bv0.GetBitLength() && len <= m_nAllocLength);
  if (len == 0) Init();
  else {
    SetSize(bv1.GetBitLength());        // 21.03.2006
    memcpy(m_bVect1,(ULONG*)bv1,len*sizeof(ULONG));
    memcpy(m_bVect0,(ULONG*)bv0,len*sizeof(ULONG));
  }
}

//--------------------------- SetOnes ---------------------------------------------------
void CuTV::SetOnes(const CuBV& bv)
{int i;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nLongLength; i++) {
   m_bVect1[i]=bv.GetLongAt(i); m_bVect0[i]&=~bv.GetLongAt(i);
 }
}

//--------------------------- SetZeros ----------------------------------------------------
void CuTV::SetZeros(const CuBV& bv)
{int i;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nLongLength; i++) {
   m_bVect0[i]=bv.GetLongAt(i); m_bVect1[i]&=~bv.GetLongAt(i);
 }
}

//--------------------------- AddOnes ---------------------------------------------------
void CuTV::AddOnes(const CuBV& bv)
{int i;
ASSERT(m_nBitLength==bv.GetBitLength());
for (i=0; i < m_nLongLength; i++) {
  m_bVect1[i] |= bv.GetLongAt(i); m_bVect0[i] &= ~bv.GetLongAt(i);
}
}

//--------------------------- AddZeros ----------------------------------------------------
void CuTV::AddZeros(const CuBV& bv)
{int i;
ASSERT(m_nBitLength==bv.GetBitLength());
for (i=0; i < m_nLongLength; i++) {
  m_bVect0[i] |= bv.GetLongAt(i); m_bVect1[i] &= ~bv.GetLongAt(i);
}
}

//--------------------------- SetUnDefs -------------------------------------------------
void CuTV::SetUnDefs(const CuBV& bv)
{int i;
 ULONG maska;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nLongLength; i++) {
     maska=~bv.GetLongAt(i);
   m_bVect1[i]&=maska; m_bVect0[i]&=maska;
 }
}

//--------------------------- SetOnes ---------------------------------------------------
void CuTV::SetOnes(const ULONG* bv)
{int i;
  for (i=0; i < m_nLongLength; i++) {
   m_bVect1[i]=bv[i]; m_bVect0[i]&=~bv[i];
 }
}

//--------------------------- SetZeros ----------------------------------------------------
void CuTV::SetZeros(const ULONG* bv)
{int i;
 for (i=0; i < m_nLongLength; i++) {
   m_bVect0[i]=bv[i]; m_bVect1[i]&=~bv[i];
 }
}

//--------------------------- AddOnes ---------------------------------------------------
void CuTV::AddOnes(const ULONG* bv)
{int i;
for (i=0; i < m_nLongLength; i++) {
  m_bVect1[i] |= bv[i]; m_bVect0[i] &= ~bv[i];
}
}

//--------------------------- AddZeros ----------------------------------------------------
void CuTV::AddZeros(const ULONG* bv)
{int i;
for (i=0; i < m_nLongLength; i++) {
  m_bVect0[i] |= bv[i]; m_bVect1[i] &= ~bv[i];
}
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// Operators and functions of assignment ///////////////////////

//--------------------------- Clear -----------------------------------------------------
void CuTV::Clear(char symb)
{ ASSERT(m_nLongLength > 0);
 ULONG maska = LONG_1;
 int i;
 if ((i = ADR_BITLONG(m_nBitLength)) > 0 ) {
   i = S_4 - i; maska = LONG_1 << i; }
 memset(m_bVect1, BYTE_1, (m_nLongLength-1)*sizeof(ULONG)); 
 m_bVect1[m_nLongLength-1] = maska;
 memset(m_bVect0, BYTE_1, (m_nLongLength-1)*sizeof(ULONG)); 
 m_bVect0[m_nLongLength-1] = maska;
switch (symb) {
 case '1': memset(m_bVect0,0, m_nLongLength*sizeof(ULONG)); break;
 case '0': memset(m_bVect1,0, m_nLongLength*sizeof(ULONG)); break;
 case '-': memset(m_bVect1,0, m_nLongLength*sizeof(ULONG)); 
           memset(m_bVect0,0, m_nLongLength*sizeof(ULONG));break;
 }
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Operators and functions of expanded assignment ////////////

const CuTV&  CuTV::operator <<=(int nShift)
{ int i, j, l_bit, r_bit, nNewLenLong, nNewLenBit,AdrBeg;
 ASSERT(nShift >= 0 && m_nBitLength >= nShift);
 nNewLenBit = m_nBitLength - nShift;
 if (nNewLenBit==0) {
   memset(m_bVect1,0, m_nLongLength*sizeof(ULONG));
   memset(m_bVect0,0, m_nLongLength*sizeof(ULONG)); return *this;
 }
 if (nShift==0) return *this;
 nNewLenLong = LEN_LONG(nNewLenBit);
 AdrBeg = BIT_LONG(nShift);
 l_bit = ADR_BITLONG(nShift);
 if (l_bit)    {
   r_bit = S_4 - l_bit;  j=0;
   for (i=AdrBeg; i<m_nLongLength-1; i++, j++) {
     m_bVect1[j] = (m_bVect1[i] << l_bit) | (m_bVect1[i+1] >> r_bit);
     m_bVect0[j] = (m_bVect0[i] << l_bit) | (m_bVect0[i+1] >> r_bit);
   }
   if ((j*S_4) < nShift) {
     m_bVect1[nNewLenLong-1] <<= l_bit;
     m_bVect0[nNewLenLong-1] <<= l_bit;
   }
 }
 else {
   memcpy(m_bVect1, m_bVect1+AdrBeg, nNewLenLong*sizeof(ULONG));
   memcpy(m_bVect0, m_bVect0+AdrBeg, nNewLenLong*sizeof(ULONG));
 }
 i= m_nLongLength - nNewLenLong;
 memset(m_bVect1+nNewLenLong, 0, i*sizeof(ULONG));
 memset(m_bVect0+nNewLenLong, 0, i*sizeof(ULONG));
 return *this;
}

//--------------------------- operator >> -----------------------------------------------
const CuTV& CuTV::operator >>=(int nShift)
{int i, j, l_bit, r_bit,AdrBeg;
 ULONG *work1, *work0;
 ASSERT(nShift >= 0);
 ASSERT((m_nBitLength - nShift) >= 0);
 if (nShift == 0) return *this;
 if ((m_nBitLength - nShift)==0)  {
   memset(m_bVect1,0, m_nLongLength*sizeof(ULONG));
   memset(m_bVect0,0, m_nLongLength*sizeof(ULONG)); return *this;
   }                                            // may throw an exception
 work1 = new ULONG[m_nLongLength<<1]; work0 = work1 + m_nLongLength;
 memset(work1,0, (m_nLongLength<<1)*sizeof(ULONG));
 AdrBeg = BIT_LONG(nShift);
 r_bit = ADR_BITLONG(nShift);
 if (r_bit)    {
   l_bit = S_4 - r_bit;  j=0; work1[AdrBeg] = (m_bVect1[0] >> r_bit);
   work0[AdrBeg] = (m_bVect0[0] >> r_bit);
   for (i=AdrBeg+1; i<m_nLongLength; i++) {
     work1[i] = (m_bVect1[j] << l_bit) | (m_bVect1[j+1] >> r_bit); j++;
     work0[i] = (m_bVect0[j] << l_bit) | (m_bVect0[j+1] >> r_bit); j++;
   }
 }
 else {
   memcpy(work1, m_bVect1, (m_nLongLength-AdrBeg)*sizeof(ULONG));
   memcpy(work0, m_bVect0, (m_nLongLength-AdrBeg)*sizeof(ULONG));
 }
 if (i=ADR_BITLONG(m_nBitLength)) {
   work1[m_nLongLength-1] = (work1[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
   work0[m_nLongLength-1] = (work0[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
 }
 memcpy(m_bVect1,work1, m_nLongLength*sizeof(ULONG)); 
 memcpy(m_bVect0,work0, m_nLongLength*sizeof(ULONG));
 delete [] work1;
 return *this;
}

//--------------------------- operator &= -----------------------------------------------
const CuTV& CuTV::operator &=(const ULONG* maska)
{ ASSERT(m_nBitLength > 0);
 int i;
 for (i=0; i<m_nLongLength; i++) {
   m_bVect1[i] &= maska[i];
   m_bVect0[i] &= maska[i];
 }
 return *this;
}

//--------------------------- operator &= -----------------------------------------------
const CuTV& CuTV::operator &=(const CuBV& maska)
{ ASSERT(m_nBitLength == maska.GetBitLength());
 int i;
 for (i=0; i<m_nLongLength; i++) {
   m_bVect1[i] &= maska.GetLongAt(i);
   m_bVect0[i] &= maska.GetLongAt(i);
 }
 return *this;
}

//--------------------------- operator &= -----------------------------------------------
const CuTV& CuTV::operator &=(const CuTV& tv)
{ ASSERT(m_nBitLength == tv.GetBitLength());
 int i;
 for (i=0; i < m_nLongLength; i++) {
   m_bVect1[i] |= tv.m_bVect1[i];
   m_bVect0[i] |= tv.m_bVect0[i];
 }
 return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Logic operations //////////////////////////////////////////
//--------------------------- operator << -----------------------------------------------

//---------------------------  operator << ---------------------------------------
CuTV  operator <<(const CuTV& tv, int nShift)
{ CuTV s;
 s.LeftShiftVect(tv.m_bVect1, tv.m_bVect0, tv.m_nBitLength, nShift);  return s;
}

//---------------------------  operator >> ---------------------------------------
CuTV  operator >>(const CuTV& tv, int nShift)
{ CuTV s;
 s.RightShiftVect(tv.m_bVect1,tv.m_bVect0,tv.m_nBitLength, nShift);  return s;
}

//---------------------------  operator &= ----------------------------------------
CuTV  operator&=(const CuTV& tv, const ULONG* maska)
{
 CuTV tvTag(tv);
 tvTag &= maska;
 return tvTag;
}

//---------------------------  operator &= ----------------------------------------
CuTV  operator&=(const CuTV& tv, const CuBV& maska)
{ASSERT(tv.m_nBitLength == maska.GetBitLength());
 CuTV tvTag(tv);
 tvTag &= maska;
 return tvTag;
}

//---------------------------  operator &= ----------------------------------------
CuTV  operator&=(const CuTV& tv1, const CuTV& tv2)
{ASSERT(tv1.m_nBitLength == tv2.GetBitLength());
 CuTV tvTag(tv1);
 tvTag &= tv2;
 return tvTag;
}

//--------------------------- LoopLeftShift --------------------------------------------
void CuTV::LoopLeftShift(int nShift)
{  // LeftShift ternary vector  -- assume that 'this' is a new CuTV object
 int i, j, l_bit, r_bit, nNewLenLong, nNewLenBit, nEndLenLong;
 ULONG *work1, *work0;
 ASSERT(nShift >= 0);
 nNewLenBit = m_nBitLength - nShift;
 ASSERT(nNewLenBit >= 0);
 if (nNewLenBit==0 || nShift==0) return;
 nNewLenLong = LEN_LONG(nNewLenBit);
 work1 = new ULONG[m_nLongLength<<1]; work0=work1+m_nLongLength;// may throw an exception
 memset(work1,0, (m_nLongLength<<1)*sizeof(ULONG));
 i = BIT_LONG(nShift);
 l_bit = ADR_BITLONG(nShift);
 if (l_bit)    {
   r_bit = S_4 - l_bit;
   memcpy(work1, m_bVect1+i, (nNewLenLong+1)*sizeof(ULONG));
   memcpy(work0, m_bVect0+i, (nNewLenLong+1)*sizeof(ULONG));
   for (i=0; i<nNewLenLong; i++)  {
     work1[i] = (work1[i] << l_bit) | (work1[i+1] >> r_bit);
     work0[i] = (work0[i] << l_bit) | (work0[i+1] >> r_bit);
   }
   work1[i] <<= l_bit; work0[i] <<= l_bit;
 }
 else  {
   memcpy(work1, m_bVect1+i, nNewLenLong*sizeof(ULONG));
   memcpy(work0, m_bVect0+i, nNewLenLong*sizeof(ULONG));
 }
 nEndLenLong = m_nLongLength - nNewLenLong;
 j = BIT_LONG(nNewLenBit);
 r_bit = ADR_BITLONG(nNewLenBit);
 if (r_bit == 0 ) {
   memcpy(work1+j, m_bVect1, nEndLenLong*sizeof(ULONG));
   memcpy(work0+j, m_bVect0, nEndLenLong*sizeof(ULONG));
 }
 else {
   l_bit = S_4 - r_bit;
   work1[j] = (work1[j] >> l_bit) << l_bit;
   work0[j] = (work0[j] >> l_bit) << l_bit;
   work1[j++] |= (m_bVect1[0] >> r_bit);
   work0[j++] |= (m_bVect0[0] >> r_bit);
   for (i=0; i<nEndLenLong; i++) {
     work1[j++] = (m_bVect1[i] << l_bit) | (m_bVect1[i+1] >> r_bit);
     work0[j++] = (m_bVect0[i] << l_bit) | (m_bVect0[i+1] >> r_bit);
   }
   j = S_4 - ADR_BITLONG(m_nBitLength);
   work1[m_nLongLength-1] = (work1[m_nLongLength-1] >> j)<<j;
   work0[m_nLongLength-1] = (work0[m_nLongLength-1] >> j)<<j;
 }
 memcpy(m_bVect1,work1, m_nLongLength*sizeof(ULONG)); 
 memcpy(m_bVect0,work0, m_nLongLength*sizeof(ULONG));
 delete [] work1;
}

//--------------------------- LoopRightShift -------------------------------------------
void CuTV::LoopRightShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CuTV object
 int i, j, l_bit, r_bit,AdrBeg,AdrEnd,BitEnd;
 ULONG *work1, *work0;
 ASSERT(nShift >= 0 && m_nBitLength >= nShift);
 if (nShift == 0) return;
 if ((m_nBitLength - nShift)==0) {
   memset(m_bVect1,0, m_nLongLength*sizeof(ULONG));
   memset(m_bVect0,0, m_nLongLength*sizeof(ULONG)); return;
 }
 work1 = new ULONG[m_nLongLength<<1]; work0=work1+m_nLongLength;// may throw an exception
 memset(work1,0, (m_nLongLength<<1)*sizeof(ULONG));
 AdrBeg = BIT_LONG(nShift);
 AdrEnd = BIT_LONG(m_nBitLength - nShift);
 BitEnd = ADR_BITLONG(m_nBitLength);
 if (BitEnd < nShift) {
   l_bit = ADR_BITLONG(m_nBitLength - nShift);
   r_bit = S_4 - l_bit;     j = AdrEnd;
   for (i=0; i<AdrBeg+1; i++, j++) {
     work1[i] = (m_bVect1[j] << l_bit) | (m_bVect1[j+1] >> r_bit);
     work0[i] = (m_bVect0[j] << l_bit) | (m_bVect0[j+1] >> r_bit);
   }
 }
 else {
   l_bit = BitEnd - nShift; work1[0] = m_bVect1[AdrEnd] << l_bit;
   work0[0] = m_bVect0[AdrEnd] << l_bit;
 }
 r_bit = ADR_BITLONG(nShift); l_bit = S_4 - r_bit;
 work1[AdrBeg] = (work1[AdrBeg] >> l_bit) << l_bit;
 work0[AdrBeg] = (work0[AdrBeg] >> l_bit) << l_bit;
 if (r_bit)    {
   j=0;
   work1[AdrBeg] |= (m_bVect1[0] >> r_bit);
   work0[AdrBeg] |= (m_bVect0[0] >> r_bit);
   for (i=AdrBeg+1; i < m_nLongLength; i++, j++) {
     work1[i] = (m_bVect1[j] << l_bit) | (m_bVect1[j+1] >> r_bit);
     work0[i] = (m_bVect0[j] << l_bit) | (m_bVect0[j+1] >> r_bit);
   }
 }
 else {
   memcpy(work1+AdrBeg, m_bVect1, (m_nLongLength-AdrBeg)*sizeof(ULONG));
   memcpy(work0+AdrBeg, m_bVect0, (m_nLongLength-AdrBeg)*sizeof(ULONG));
 }
 if (i=ADR_BITLONG(m_nBitLength)) {
   work1[m_nLongLength-1] = (work1[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
   work0[m_nLongLength-1] = (work0[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
 }
 memcpy(m_bVect1,work1, m_nLongLength*sizeof(ULONG)); 
 memcpy(m_bVect0,work0, m_nLongLength*sizeof(ULONG));
 delete [] work1;
}

//--------------------------- InvertDefBit ----------------------------------------------
void CuTV::InvertDefs()
{ASSERT(m_nLongLength > 0);
    int i;
 ULONG a;
 for(i=0; i < m_nLongLength; i++) {
   a=m_bVect0[i]; m_bVect0[i]=m_bVect1[i]; m_bVect1[i]=a;
 }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// Logic Operations of weighting, finding and casing ///////////////

//--------------------------- CountDefs -------------------------------------------------
int CuTV::CountDefs() const
{
  BYTE* pB1 = (BYTE*)m_bVect1;
  BYTE* pB0 = (BYTE*)m_bVect0;
  int one=0, j, w = m_nLongLength<<2; 
  for (j=0; j < w; j++) one += TabC[pB1[j] | pB0[j]];
  return one;

}

//--------------------------- CountUnDefs -----------------------------------------------
int CuTV::CountUnDefs() const
{ return m_nBitLength - CountDefs(); }

//--------------------------- CountOnes -------------------------------------------------
int CuTV::CountOnes() const
{
  BYTE* pB1 = (BYTE*)m_bVect1;
  int one=0, j, w = m_nLongLength<<2; 
  for (j=0; j < w; j++) one += TabC[pB1[j]];
  return one;

}

//--------------------------- CountZeros -------------------------------------------------
int CuTV::CountZeros() const
{ 
  BYTE* pB0 = (BYTE*)m_bVect0;
  int one=0, j, w = m_nLongLength<<2; 
  for (j=0; j < w; j++) one += TabC[pB0[j]];
  return one;
}

//--------------------------- LeftUnDef -------------------------------------------------
int CuTV::LeftUnDef(int nNext) const
{int i, j,pos = 0;
 ULONG ch, vec, ch_last;
 ASSERT(m_nBitLength >= nNext);
 if(m_nBitLength == (nNext+1)) return (-1);
 i=ADR_BITLONG(m_nBitLength);
 ch_last=((~(m_bVect1[m_nLongLength-1]|m_bVect0[m_nLongLength-1]))>>(S_4-i))<<(S_4-i);
 if (nNext == -1) { i=0; j=0; }
 else {i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext); j++;}
 if (j == S_4) { j=0;  i++; }
 if (i == m_nLongLength)  return (-1);
 ch=(i == m_nLongLength-1)?ch_last:((~(m_bVect1[i]|m_bVect0[i])) << j);
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j < m_nLongLength-1; j++)
   if ((vec=~(m_bVect1[j]|m_bVect0[j])))  {
     while (!(vec & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 if (j < m_nLongLength)
   if(ch_last) {
     while (!(ch_last & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//--------------------------- LeftDef ---------------------------------------------------
int CuTV::LeftDef(int nNext) const
{int i, j,pos = 0;
 ULONG ch, vec;
 ASSERT(m_nBitLength >= nNext);
 if (nNext == -1) { i=0;j=0; }
 else {i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext); j++;}
 if (j==S_4)  {  j=0;  i++; }
 if (i == m_nLongLength)  return (-1);
 ch = (m_bVect1[i]|m_bVect0[i]) << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j < m_nLongLength;j++)
   if ((vec=m_bVect1[j]|m_bVect0[j]))  {
     while (!(vec & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//--------------------------- LeftOne ---------------------------------------------------
int CuTV::LeftOne(int nNext) const
{int i, j,pos = 0;
 ULONG ch;
 ASSERT(m_nBitLength >= nNext);
 if (nNext == -1) {i=0;j=0;}
 else {i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext); j++;}
 if (j==S_4)  {j=0;  i++; }
 if (i==m_nLongLength)  return (-1);
 ch = m_bVect1[i] << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nLongLength;j++)
   if (m_bVect1[j])  {
     while (!(m_bVect1[j] & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//--------------------------- LeftZero ---------------------------------------------------
int CuTV::LeftZero(int nNext) const
{int i, j,pos = 0;
 ULONG ch;
 ASSERT(m_nBitLength >= nNext);
 if (nNext == -1) {i=0;j=0;}
 else {i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext); j++;}
 if (j==S_4)  {j=0;  i++; }
 if (i==m_nLongLength)  return (-1);
 ch = m_bVect0[i] << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j<m_nLongLength; j++)
   if (m_bVect0[j])  {
     while (!(m_bVect0[j] & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//--------------------------- RightUnDef ------------------------------------------------
int CuTV::RightUnDef(int nPrev) const
{int i, j, pos = S_4-1, ii;
 ULONG ch, vec;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) { i=m_nLongLength-1; j=0; }
 else {i=BIT_LONG(nPrev);  j=S_4 - ADR_BITLONG(nPrev);}
 if (j==S_4)  {j=0;  i--; }
 ch = ~(m_bVect1[i] | m_bVect0[i]);
 if(i == m_nLongLength-1) {
   ii=ADR_BITLONG(m_nBitLength);
   ch=(ch>>(S_4-ii))<<(S_4-ii);
 }
 ch = (ch>> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j>=0; j--)
   if((vec=~(m_bVect1[j] | m_bVect0[j])))  {
     while (!(vec & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//--------------------------- RightDef --------------------------------------------------
int CuTV::RightDef(int nPrev) const
{int i, j,pos = S_4 - 1;
 ULONG ch, vec;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) {i=m_nLongLength-1;j=0;}
 else {i=BIT_LONG(nPrev);  j=S_4 - ADR_BITLONG(nPrev);}
 if (j==S_4)  {j=0;  i--; }
 ch = ((m_bVect1[i] | m_bVect0[i])>> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if((vec=m_bVect1[j] | m_bVect0[j]))  {
     while (!(vec & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//--------------------------- RightOne --------------------------------------------------
int CuTV::RightOne(int nPrev) const
{int i, j,pos = S_4 - 1;
 ULONG ch;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) {i=m_nLongLength-1;j=0;}
 else {i=BIT_LONG(nPrev);  j=S_4 - ADR_BITLONG(nPrev);}
 if (j==S_4)  {j=0;  i--; }
 ch = (m_bVect1[i] >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if(m_bVect1[j])  {
     while (!(m_bVect1[j] & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//--------------------------- RightZero --------------------------------------------------
int CuTV::RightZero(int nPrev) const
{int i, j,pos = S_4 - 1;
 ULONG ch;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) {i=m_nLongLength-1;j=0;}
 else {i=BIT_LONG(nPrev);  j=S_4 - ADR_BITLONG(nPrev);}
 if (j==S_4)  {j=0;  i--; }
 ch = (m_bVect0[i] >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if(m_bVect0[j])  {
     while (!(m_bVect0[j] & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Operations of concatinations ///////////////////////////////

//--------------------------- Concat --------------------------------------------------
void CuTV::Concat(char symb/*='-'*/)
{ int nLongLength, nLen=m_nBitLength+1;
  if((nLongLength = LEN_LONG(nLen)) > m_nAllocLength)
    GetBuffer(nLen);
  m_nLongLength = nLongLength;
  m_nBitLength = nLen; SetBitAt(nLen-1,symb);
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Comparing operations ///////////////////////////////////////

//--------------------------- IsCorrect -------------------------------------------------
BOOL CuTV::IsCorrect() const
{ int i;
 for (i=0; i < m_nLongLength; i++)
   if ((m_bVect1[i] & m_bVect0[i]) != 0) return FALSE;  //12.12.2002
 return TRUE;
}

//--------------------------- IsBool ----------------------------------------------------
BOOL CuTV::IsBool() const
{ int i;
 ULONG LastB;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 for (i=0; i < m_nLongLength-1; i++)
   if((m_bVect1[i] | m_bVect0[i]) != LONG_1) return FALSE;
 if((i = ADR_BITLONG(m_nBitLength)) == 0) LastB = LONG_1;
 else LastB=~(LONG_1 >> i);
 if((m_bVect1[i] | m_bVect0[i]) != LastB) return FALSE;
 return TRUE;
}

//--------------------------- IsTrivial -------------------------------------------------
BOOL CuTV::IsTrivial() const
{ int i;
 if (m_nLongLength == 0) return FALSE;      // 27.05.02
 for (i=0; i < m_nLongLength; i++)
   if (m_bVect1[i] != 0 || m_bVect0[i] != 0) return FALSE;
 return TRUE;
}

//----------------- IsOne ---------------------------------------------------------------
BOOL CuTV::IsOne() const
{int i;
 ULONG maska;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1 << (S_4 - i)) : LONG_1;
 for (i=0; i < m_nLongLength-1; i++)
  if ((m_bVect1[i] ^LONG_1) != 0) return FALSE;
 if((m_bVect1[i] ^maska) != 0)  return FALSE;
 return TRUE;
}

//----------------- IsZero --------------------------------------------------------------
BOOL CuTV::IsZero() const
{int i;
 ULONG maska;
 if (m_nLongLength == 0) return FALSE;     // 27.05.02
 maska=((i = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1 << (S_4 - i)) : LONG_1;
 for (i=0; i < m_nLongLength-1; i++)
  if ((m_bVect0[i] ^LONG_1) != 0) return FALSE;
 if((m_bVect0[i] ^maska) != 0)  return FALSE;
 return TRUE;
}

//--------------------------- ExistOne --------------------------------------------------
BOOL CuTV::ExistOne() const
{ int i;
 for (i=0; i < m_nLongLength; i++)  if (m_bVect1[i]) return TRUE;
 return FALSE;
}

//--------------------------- ExistZero -------------------------------------------------
BOOL CuTV::ExistZero() const
{ int i;
 for (i=0; i < m_nLongLength; i++)  if (m_bVect0[i]) return TRUE;
 return FALSE;
}

//--------------------------- IsOrthog --------------------------------------------------
BOOL CuTV::IsOrthog(const CuTV& tv) const
{ int i;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=0; i < m_nLongLength; i++)
   if ((m_bVect1[i] & tv.m_bVect0[i])||(m_bVect0[i] & tv.m_bVect1[i]) != 0)
     return TRUE;
 return FALSE;
}

//--------------------------- IsOrthog --------------------------------------------------
BOOL CuTV::IsOrthog(const CuBV& bv) const
{ int i;
 ULONG* bv1=(ULONG*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nLongLength; i++)
   if ((m_bVect1[i] & ~bv1[i]) || (m_bVect0[i] & bv1[i]) != 0)
     return TRUE;
 return FALSE;
}

//--------------------------- IsNeighb --------------------------------------------------
BOOL CuTV::IsNeighb(const CuTV& tv) const   //соседство
{ int i, j;
 ULONG  mas1, mas0;
 BYTE* pm= (BYTE*)&mas1;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=j=0; i < m_nLongLength && j < 2; i++) {
   if ((mas1 = m_bVect1[i] ^ tv.m_bVect1[i]) !=
       (mas0 = m_bVect0[i] ^ tv.m_bVect0[i])) return FALSE;
   j += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//   j+=COUNTBIT(mas1);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsNeighb --------------------------------------------------
BOOL CuTV::IsNeighb(const CuBV& bv) const
{ int i, j;
 ULONG  mas1, mas0, *bvS=(ULONG*)bv;
 BYTE* pm= (BYTE*)&mas1;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=j=0; i < m_nLongLength && j < 2; i++) {
   if ((mas1 = m_bVect1[i] ^ bvS[i]) !=
       (mas0 = m_bVect0[i] ^ (~bvS[i]))) return FALSE;
   j += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//   j+=COUNTBIT(mas1);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsAdjac --------------------------------------------------
BOOL CuTV::IsAdjac(const CuTV& tv) const    //смежность
{ int i, j;
 ULONG  maska;
 BYTE* pm= (BYTE*)&maska;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=j=0; i < m_nLongLength && j < 2; i++) {
   if ((maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i])) != 0)
     j += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//     j+=COUNTBIT(maska);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsAdjac --------------------------------------------------
BOOL CuTV::IsAdjac(const CuBV& bv) const
{ int i, j;
 ULONG  maska, *bvS=(ULONG*)bv;;
 BYTE* pm= (BYTE*)&maska;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=j=0; i < m_nLongLength && j < 2; i++) {
   if ((maska=(m_bVect1[i] & ~bvS[i]) | (m_bVect0[i] & bvS[i])) != 0)
     j += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//     j+=COUNTBIT(maska);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsCover --------------------------------------------------
BOOL CuTV::IsCover(const CuTV& tv) const
{ int i, j;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=j=0; i < m_nLongLength; i++)
   if ((((m_bVect1[i] & tv.m_bVect1[i])^m_bVect1[i])!= 0) ||
       (((m_bVect0[i] & tv.m_bVect0[i])^m_bVect0[i])) != 0) return FALSE;
 return TRUE;
}

//--------------------------- IsCover --------------------------------------------------
BOOL CuTV::IsCover(const CuBV& bv) const
{ int i;
 ULONG *bvS=(ULONG*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nLongLength; i++)
   if ((m_bVect1[i] & ~bvS[i]) || (m_bVect0[i] & bvS[i]))
     return FALSE;
 return TRUE;
}

//--------------------------- IsIntersec ------------------------------------------------
BOOL CuTV::IsIntersec(const CuTV& tv) const
{ int i;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=0; i < m_nLongLength; i++)
   if ((m_bVect1[i] & tv.m_bVect0[i])||(m_bVect0[i] & tv.m_bVect1[i]))
     return FALSE;
 return TRUE;
}

//--------------------------- operator == -----------------------------------------------
BOOL  operator ==(const CuTV& tv1, const CuTV& tv2)
{ return tv1.Equality(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength);}

//--------------------------- operator == -----------------------------------------------
BOOL  operator ==(const CuTV& tv1, const CuBV& bv2)
{ return tv1.EqualBool((ULONG*)bv2,bv2.GetBitLength());}

//--------------------------- operator == -----------------------------------------------
BOOL  operator ==(const CuBV& bv1, const CuTV& tv2)
{ return tv2.EqualBool((ULONG*)bv1,bv1.GetBitLength());}

//--------------------------- operator != -----------------------------------------------
BOOL  operator !=(const CuTV& tv1, const CuTV& tv2)
{ return !(tv1.Equality(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength));}

//--------------------------- operator != -----------------------------------------------
BOOL  operator !=(const CuTV& tv1, const CuBV& bv2)
{ return !(tv1.EqualBool((ULONG*)bv2,bv2.GetBitLength()));}

//--------------------------- operator != -----------------------------------------------
BOOL  operator !=(const CuBV& bv1, const CuTV& tv2)
{ return !(tv2.EqualBool((ULONG*)bv1,bv1.GetBitLength()));}

//--------------------------- operator > ------------------------------------------------
BOOL  operator >(const CuTV& tv1, const CuTV& tv2)
{ return (tv1.Pogl(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength)) ; }

//--------------------------- operator < ------------------------------------------------
BOOL  operator <(const CuTV& tv1, const CuTV& tv2)
{ return (tv2.Pogl(tv1.m_bVect1,tv1.m_bVect0,tv1.m_nBitLength)) ; }

//--------------------------- operator >= -----------------------------------------------
BOOL  operator>=(const CuTV& tv1, const CuTV& tv2)
{ return tv1.IsCover(tv2); }

//--------------------------- operator <= -----------------------------------------------
BOOL  operator<=(const CuTV& tv1, const CuTV& tv2)
{ return tv2.IsCover(tv1); }

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Analizing operations ///////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Operations on intervals ///////////////////////////////////////
//--------------------------- NumNeighb -------------------------------------------------
int CuTV::NumNeighb(const CuTV& tv) const   //номер компоненты, по кот.соседство,иначе -1
{ int i, num;
 ULONG  maska;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=0, num=-1; i < m_nLongLength; i++) {
   if ((m_bVect1[i] == tv.m_bVect1[i]) && (m_bVect0[i] == tv.m_bVect0[i])) continue;
   if(num != -1) return -1;
   maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]);
   while (!(maska & OB4[++num]));
   if((maska<<(num+1)) != 0) return -1;
 }
 if(num != -1) return ((i*S_4)+num);
 else return -1;
}

//--------------------------- NumNeighb -------------------------------------------------
int CuTV::NumNeighb(const CuBV& bv) const   //номер компоненты, по кот.соседство,иначе -1
{ int i, num;
 ULONG  maska, *bvS=(ULONG*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0, num=-1; i < m_nLongLength; i++) {
   if ((m_bVect1[i] == bvS[i]) && (m_bVect0[i] == ~bvS[i])) continue;
   if(num != -1) return -1;
   maska=(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i]);
   while (!(maska & OB4[++num]));
   if((maska<<(num+1)) != 0) return -1;
 }
 if(num != -1) return ((i*S_4)+num);
 else return -1;
}

//--------------------------- NumAdjacent -----------------------------------------------
int CuTV::NumAdjac(const CuTV& tv) const   //номер компоненты, по кот.сьежны,иначе -1
{ int i, j=0, num;
  ULONG  maska;
  BYTE* pm= (BYTE*)&maska;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  for (i=0, num=-1; i < m_nLongLength; i++) {
    if ((maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i])) != 0)
      j += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//     j+=COUNTBIT(maska);
   if(num != -1) return -1;
   while (!(maska & OB4[++num]));
   if((maska<<(num+1)) != 0) return -1;
 }
 if(num != -1) return ((i*S_4)+num);
 else return -1;
}

//--------------------------- NumAdjacent -----------------------------------------------
int CuTV::NumAdjac(const CuBV& bv) const   //номер компоненты, по кот.сьежны,иначе -1
{ int i, j=0, num;
  ULONG  maska, *bvS=(ULONG*)bv;
  BYTE* pm= (BYTE*)&maska;
  ASSERT(m_nBitLength==bv.GetBitLength());
  for (i=0, num=-1; i < m_nLongLength; i++) {
    if ((maska=(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i])) != 0)
      j += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//      j+=COUNTBIT(maska);
  if(num != -1) return -1;
  while (!(maska & OB4[++num]));
  if((maska<<(num+1)) != 0) return -1;
  }
  if(num != -1) return ((i*S_4)+num);
  else return -1;
}

//--------------------------- Orthogon --------------------------------------------------
CuBV CuTV::Orthogon(const CuTV& tv) const    //мн-во ортогон.компонент
{
  int i;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CuBV bvTag(m_nBitLength, FALSE);
  for (i=0; i < m_nLongLength; i++)
    bvTag.SetLongAt(i,(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]));
  return bvTag;
}

//--------------------------- Orthogon --------------------------------------------------
CuBV CuTV::Orthogon(const CuBV& bv) const    //мн-во ортогон.компонент
{ 
  int i;
  ASSERT(m_nBitLength==bv.GetBitLength());
  ULONG *bvS=(ULONG*)bv;
  CuBV bvTag(m_nBitLength, FALSE);
  for (i=0; i < m_nLongLength; i++)
    bvTag.SetLongAt(i,(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i]));
  return bvTag;
}

//--------------------------- Intersec --------------------------------------------------
CuTV CuTV::Intersec(const CuTV& tv) const    //рез-т миним.покрытия,если ортю- NULL
{ 
  int i;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CuTV tvTag(m_nBitLength,TRUE);      // ?????????
  for (i=0; i < m_nLongLength; i++) {
    if(((m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i])) != 0) {
      tvTag.Empty(); break;
    }
    tvTag.m_bVect1[i]=(m_bVect1[i] | tv.m_bVect1[i]);
    tvTag.m_bVect0[i]=(m_bVect0[i] | tv.m_bVect0[i]);
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTV::Concens(const CuTV& tv) const    //рез-т обобщ.склеивани
{ 
  int i;
  ULONG maska;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CuTV tvTag(m_nBitLength);
  for (i=0; i < m_nLongLength; i++) {
    maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]);
    tvTag.m_bVect1[i]=(m_bVect1[i] | tv.m_bVect1[i])^maska;
    tvTag.m_bVect0[i]=(m_bVect0[i] | tv.m_bVect0[i])^maska;
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTV::Concens(const CuBV& bv) const    //рез-т обобщ.склеивани
{ 
  int i;
  ULONG maska, *bvS=(ULONG*)bv;;
  ASSERT(m_nBitLength==bv.GetBitLength());
  CuTV tvTag(m_nBitLength);
  for (i=0; i < m_nLongLength; i++) {
    maska=(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i]);
    tvTag.m_bVect1[i]=(m_bVect1[i] | bvS[i])^maska;
    tvTag.m_bVect0[i]=(m_bVect0[i] | ~bvS[i])^maska;
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTV::MinCover(const CuTV& tv) const    //рез-т миним.покрыти
{ 
  int i;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CuTV tvTag(m_nBitLength);
  for (i=0; i < m_nLongLength; i++) {
    tvTag.m_bVect1[i]=(m_bVect1[i] & tv.m_bVect1[i]);
    tvTag.m_bVect0[i]=(m_bVect0[i] & tv.m_bVect0[i]);
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTV::MinCover(const CuBV& bv) const    //рез-т миним.покрыти
{ 
  int i;
  ULONG *bvS=(ULONG*)bv;
  ASSERT(m_nBitLength==bv.GetBitLength());
  CuTV tvTag(m_nBitLength);
  for (i=0; i < m_nLongLength; i++) {
    tvTag.m_bVect1[i]=(m_bVect1[i] & bvS[i]);
    tvTag.m_bVect0[i]=(m_bVect0[i] & ~bvS[i]);
  }
  return tvTag;
}

//--------------------------- CuTVtoCuBM --------------------------------------------------
CuBM CuTV::CuTVtoCuBM() const
{
  int i, j, nex, ves, count, newR;
  CuBV maska(m_nBitLength, FALSE);
  ULONG *maskaB=(ULONG *)maska;
  for(i=0; i < m_nLongLength; i++)
    maskaB[i]=~(m_bVect1[i] | m_bVect0[i]);
  if ((i = ADR_BITLONG(m_nBitLength)) > 0 ) {
    i = S_4 - i; maskaB[m_nLongLength-1] &= (LONG_1 << i);
  }
  ves=maska.CountBit();
  CuBM bmTag(1<<ves, m_nBitLength); bmTag.SetSize(0, m_nBitLength);
  bmTag.Add(m_bVect1);
  for(i=0, count=1, nex=-1; i < ves; i++) {
    nex=maska.LeftOne(nex);
    for(j=0, newR=count; j < count; j++, newR++) {
      bmTag.Add(bmTag, j); bmTag.SetBitAt(newR, nex,1);
    }
    count<<=1;
  }
  return bmTag;
}

//////////////////// Operations on TERNARY VECTOR PARTS /////////////////////////////////
/////////////////////////// Logic operations ////////////////////////////////////////////
//--------------------------- OrInPlace -------------------------------------------------
void CuTV::OrInPlace(BOOL Part, const CuBV& bv)
{
  int i;
  ASSERT(bv.GetBitLength() == m_nBitLength);
  for (i=0; i < m_nLongLength; i++)
    if(Part) m_bVect1[i] |= bv.GetLongAt(i);
    else  m_bVect0[i] |= bv.GetLongAt(i);
  return;
}

//--------------------------- AndInPlace ------------------------------------------------
void CuTV::AndInPlace(BOOL Part, const CuBV& bv)
{
  int i;
  ASSERT(bv.GetBitLength() == m_nBitLength);
  for (i=0; i < m_nLongLength; i++)
    if(Part) m_bVect1[i] &= bv.GetLongAt(i);
    else  m_bVect0[i] &= bv.GetLongAt(i);
  return;
}

//--------------------------- XorInPlace -------------------------------------------------
void CuTV::XorInPlace(BOOL Part, const CuBV& bv)
{
  int i;
  ASSERT(bv.GetBitLength() == m_nBitLength);
  for (i=0; i < m_nLongLength; i++)
    if(Part) m_bVect1[i] ^= bv.GetLongAt(i);
    else  m_bVect0[i] ^= bv.GetLongAt(i);
  return;
}

//--------------------------- SubInPlace -------------------------------------------------
void CuTV::SubInPlace(BOOL Part, const CuBV& bv)
{
  ASSERT(bv.GetBitLength() == m_nBitLength);
  for (int i=0; i < m_nLongLength; i++)
    if(Part) m_bVect1[i] &= ~bv.GetLongAt(i);
    else  m_bVect0[i] &= ~bv.GetLongAt(i);
}

//--------------------------- InvertBitsInPlace -------------------------------------------
void CuTV::InvertBitsInPlace(BOOL Part)
{
  int i;
  ULONG maska;
  ASSERT(m_nBitLength != 0);
  maska=((i=ADR_BITLONG(m_nBitLength)) == 0)? LONG_1:(~(LONG_1>>i));
  if(Part) {
    for (i=0; i < m_nLongLength; i++)
      m_bVect1[i]=~m_bVect1[i];
    m_bVect1[m_nLongLength-1] &= maska;
  }
  else {
    for (i=0; i < m_nLongLength; i++)
      m_bVect0[i]= ~m_bVect0[i];
    m_bVect0[m_nLongLength-1] &= maska;
  }
  return;
}

//--------------------------- InvertBits ------------------------------------------------
CuBV CuTV::InvertBits(BOOL Part) const
{int i;
 ULONG maska = LONG_1;
 ASSERT(m_nBitLength != 0);
 i=ADR_BITLONG(m_nBitLength);
 maska=(i == 0)? LONG_1:(~(LONG_1 >> i));
 CuBV bvTag(m_nBitLength, FALSE);
 if(Part) {
   for (i=0; i < m_nLongLength-1; i++)
     bvTag.SetLongAt(i,~m_bVect1[i]);
   bvTag.SetLongAt(m_nLongLength-1,(~m_bVect1[i]&maska));
 }
 else {
   for (i=0; i < m_nLongLength-1; i++)
     bvTag.SetLongAt(i,~m_bVect0[i]);
   bvTag.SetLongAt(m_nLongLength-1,(~m_bVect0[i]&maska));
 }
 return bvTag;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Input/output operations //////////////////////////////////////
//----------------  operator <<(CDumpContext& dc, const CuTV& tv) ------------------
#ifdef _DEBUG
CDumpContext&  operator <<(CDumpContext& dc, const CuTV& tv)
{int i;
 CString res('\0',tv.m_nBitLength);
 for (i=0; i < tv.m_nBitLength; i++)
   switch(tv.GetBitAt(i)) {
   case '1': res.SetAt(i,'1'); break;
   case '0': res.SetAt(i,'0'); break;
   case '-': res.SetAt(i,'-'); break;
   default:  res.SetAt(i,'+');  }
 dc << res; return dc;
}
#endif //_DEBUG

#ifndef _LINUX
//----------------  operator<<(CArchive& ar, const CuTV& tv) -----------------------
CArchive&  operator<<(CArchive& ar, const CuTV& tv)
{ar << tv.m_nLongLength;
 ar << tv.m_nBitLength;
 ar.Write(tv.m_bVect1, tv.m_nLongLength*sizeof(ULONG));
 ar.Write(tv.m_bVect0, tv.m_nLongLength*sizeof(ULONG));
 return ar;
}

//----------------  operator>>(CArchive& ar, const CuTV& tv) -----------------------
CArchive&  operator>>(CArchive& ar, CuTV& tv)
{tv.Empty();
 ar >> tv.m_nLongLength;
 ar >> tv.m_nBitLength;
 if (tv.m_nLongLength != 0) {
   tv.AllocBuffer(tv.m_nLongLength);
   if (ar.Read(tv.m_bVect1, tv.m_nLongLength*sizeof(ULONG)) != 
               tv.m_nLongLength*sizeof(ULONG))
     AfxThrowArchiveException(CArchiveException::endOfFile);
   if (ar.Read(tv.m_bVect0, tv.m_nLongLength*sizeof(ULONG)) != 
               tv.m_nLongLength*sizeof(ULONG))
     AfxThrowArchiveException(CArchiveException::endOfFile);
 }
 return ar;
}
#endif //not _LINUX

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Advanced dump of memmory /////////////////////////////////
//---------------- GetBuffer(int nMinBufLength) -----------------------------------------

ULONG* CuTV::GetBuffer(int nMinBufLength)
{ ASSERT(nMinBufLength >= 0);
 if (nMinBufLength > m_nBitLength) {
 ULONG* pbtOldData1 = m_bVect1;
 ULONG* pbtOldData0 = m_bVect0;
 int nOldLen = m_nLongLength; int nOldBit = m_nBitLength;
 AllocBuffer(LEN_LONG(nMinBufLength));
 memcpy(m_bVect1, pbtOldData1, nOldLen*sizeof(ULONG));
 memcpy(m_bVect0, pbtOldData0, nOldLen*sizeof(ULONG));
 m_nLongLength = nOldLen; m_nBitLength = nOldBit;
 if (pbtOldData1 != NULL) delete [] pbtOldData1;
 }
 return m_bVect1;
}

//---------------- ReleaseBuffer(int nNewLength) ----------------------------------------
void CuTV::ReleaseBuffer(int nNewLength)
{ int nNewLong;
 if (nNewLength == -1) nNewLength = m_nBitLength;
 nNewLong = LEN_LONG(nNewLength);
 ASSERT(nNewLong <= m_nAllocLength);
 m_nLongLength = nNewLong;
 m_nBitLength = nNewLength;
}

// 10.02.2002------ Romanov
//----------------- SetSize(int nNewLength, int nNewAllocLength)--------------------
ULONG* CuTV::SetSize(int nNewLength, int nNewAllocLength/*-1*/)
{ 
  if (nNewLength == m_nBitLength) return m_bVect1;  //21.03.2006
  ASSERT(nNewLength >= 0);
  int nLen, i;
  int nOldLong = m_nLongLength;
  nLen=(nNewAllocLength > 0)? nNewAllocLength : nNewLength;
  ASSERT(nNewLength <=nLen);
  GetBuffer(nLen);
  m_nLongLength = LEN_LONG(nNewLength);
  m_nBitLength = nNewLength;
//  new 10.02.2001
  if (i=ADR_BITLONG(m_nBitLength)) {
     m_bVect1[m_nLongLength-1]=(m_bVect1[m_nLongLength-1]>>(S_4-i))<<(S_4-i);
     m_bVect0[m_nLongLength-1]=(m_bVect0[m_nLongLength-1]>>(S_4-i))<<(S_4-i);
  }
  if (nOldLong > m_nLongLength) {
    memset(m_bVect1+m_nLongLength,0, (nOldLong-m_nLongLength)*sizeof(ULONG));
    memset(m_bVect0+m_nLongLength,0, (nOldLong-m_nLongLength)*sizeof(ULONG));
  }
  return m_bVect1;
}


//********************* Protected functions **********************************************//
//---------------------- AllocBuffer------------------------------------------------------
void CuTV::AllocBuffer(int nLen)
{ASSERT(nLen >= 0);
 ASSERT(nLen <= ULONG_MAX - 1);    // max size (enough room for 1 extra)
 if (nLen == 0)     {   Init();     }
 else   {
   m_bVect1 = new ULONG[nLen<<1];       // may throw an exception
   m_bVect0 = m_bVect1 + nLen;
   m_nLongLength = nLen;
   m_nAllocLength = nLen;
   memset(m_bVect1,0, (nLen<<1)*sizeof(ULONG));
 }
}

//--------------------- AllocCopy ------------------------------------------------------
void CuTV::AllocCopy(CuTV& dest, int nCopyLen, int nCopyIndex, int nExtraLen)
{int nNewLen = nCopyLen + nExtraLen;
 if (nNewLen == 0) { dest.Init(); }
 else {
   dest.AllocBuffer(nNewLen);
   memcpy(dest.m_bVect1, &m_bVect1[nCopyIndex], nCopyLen*sizeof(ULONG));
   memcpy(dest.m_bVect0, &m_bVect0[nCopyIndex], nCopyLen*sizeof(ULONG));
 }
}

//---------------------- AssignChar ------------------------------------------------------
void CuTV::AssignChar(int nLenBit, const char* pch)
{ int nLenLong = LEN_LONG(nLenBit);
  if (nLenLong > m_nAllocLength) {
    delete [] m_bVect1;
    AllocBuffer(nLenLong);
    m_nBitLength = nLenBit;
  }
  else {
    memset(m_bVect1,0, nLenLong*sizeof(ULONG));
    memset(m_bVect0,0, nLenLong*sizeof(ULONG));
  }
  CharBit(nLenLong,pch);
  m_nLongLength = nLenLong;
  m_nBitLength = nLenBit;
}

//----------------------- AssignCopy -----------------------------------------------------
void CuTV::AssignCopy(int nLenBit, int nLenLong, const ULONG* pbtSrcDat1, const ULONG* pbtSrcDat0)
{ //--- check if it will fit
  if (nLenLong > m_nAllocLength) { // it won't fit, allocate another one
    Empty();
    AllocBuffer(nLenLong);
  }
  else {
    memset(m_bVect1+nLenLong, 0,
           (m_nAllocLength-nLenLong)*sizeof(ULONG));   // 22.02.2007
    memset(m_bVect0+nLenLong,0,
           (m_nAllocLength-nLenLong)*sizeof(ULONG));   // 22.02.2007
  }

  if (nLenLong != 0) {
    memcpy(m_bVect1, pbtSrcDat1, nLenLong*sizeof(ULONG));
    memcpy(m_bVect0, pbtSrcDat0, nLenLong*sizeof(ULONG));
  }
  m_nLongLength = nLenLong;
  m_nBitLength = nLenBit;
  return;
}

//----------------------- CharBit --------------------------------------------------------
void CuTV::CharBit(int nLenLong, const char* pch)
{ int i, j;
  for (j=0; j < nLenLong; j++) {
    for (i=0; i < S_4 && *pch; i++) {
      if (*pch=='1' || *pch=='+') m_bVect1[j] |= OB4[i];
      else
        if (*pch=='0' || *pch=='+') m_bVect0[j] |= OB4[i];
      ++pch;
    }
  }
}

//--------------------------- Conc-------------------------------------------------------
void CuTV::Conc(const ULONG* SrcVect1, const ULONG* SrcVect0, int SrcBitLen)
{ int NewBitLen, NewLongLen, SrcLongLen, OldLong, r_bit, l_bit, i, j;
  ULONG *work1, *work0;
 NewBitLen = m_nBitLength + SrcBitLen;
 if (NewBitLen == 0) { Init(); return;}
 NewLongLen = LEN_LONG(NewBitLen);
 OldLong=m_nLongLength;
 r_bit = ADR_BITLONG(m_nBitLength);
 if (m_nAllocLength < NewLongLen)  {
   if (m_nLongLength) {
     work1 = new ULONG[m_nLongLength<<1];
     work0 = work1+m_nLongLength;
     memcpy(work1, m_bVect1, m_nLongLength*sizeof(ULONG));
     memcpy(work0, m_bVect0, m_nLongLength*sizeof(ULONG));
     Empty();
     AllocBuffer(NewLongLen);
     memcpy(m_bVect1, work1, OldLong*sizeof(ULONG)); 
     memcpy(m_bVect0, work0, OldLong*sizeof(ULONG));
     delete [] work1;
   }
   else { AllocBuffer(NewLongLen); }
 }
 else                                   // new 16.11.99
  if(m_nLongLength < NewLongLen) {      // new 16.11.99
    m_nLongLength = NewLongLen;
    m_bVect1[m_nLongLength-1]=0;        // new 16.11.99
    m_bVect0[m_nLongLength-1]=0;        // new 16.11.99
  }
 m_nBitLength = NewBitLen;
 SrcLongLen = LEN_LONG(SrcBitLen);
 if (r_bit==0)  {
   memcpy(m_bVect1+OldLong, SrcVect1, SrcLongLen*sizeof(ULONG));
   memcpy(m_bVect0+OldLong, SrcVect0, SrcLongLen*sizeof(ULONG));  return;
 }
 l_bit = S_4-r_bit; i=OldLong;
 m_bVect1[i-1] |= SrcVect1[0]>>r_bit; m_bVect0[i-1] |= SrcVect0[0]>>r_bit;
 for (j=0; j < SrcLongLen-1; j++) {
   m_bVect1[i] = (SrcVect1[j] << l_bit) | (SrcVect1[j+1] >> r_bit); // Monday 01 February 0100 (13:04:55) Cher
   m_bVect0[i++] = (SrcVect0[j] << l_bit) | (SrcVect0[j+1] >> r_bit);
 }
 if (j*S_4+l_bit < SrcBitLen) {
   m_bVect1[m_nLongLength-1] |= SrcVect1[SrcLongLen-1] << l_bit;
   m_bVect0[m_nLongLength-1] |= SrcVect0[SrcLongLen-1] << l_bit;
 }
 if (j=ADR_BITLONG(m_nBitLength)) {
   i = S_4 - j;
   m_bVect1[m_nLongLength-1] = (m_bVect1[m_nLongLength-1] >> i) << i;
   m_bVect0[m_nLongLength-1] = (m_bVect0[m_nLongLength-1] >> i) << i;
 }
}

//--------------------------- Conc2 -------------------------------------------------------
void CuTV::Conc2(const ULONG* Vect11, const ULONG* Vect10, int SrcBitLen1,
const ULONG* Vect21, const ULONG* Vect20, int SrcBitLen2)
{ int NewBitLen, NewLongLen, SrcLongLen, OldLong, r_bit, l_bit, i, j;
 NewBitLen = SrcBitLen1 + SrcBitLen2;
 if (NewBitLen == 0) { Init(); return;}
 NewLongLen = LEN_LONG(NewBitLen);
 if (m_nAllocLength < NewLongLen)  {
   Empty(); AllocBuffer(NewLongLen);
 }
 else                                   // new 16.11.99
  if(m_nLongLength < NewLongLen) {      // new 16.11.99
    m_nLongLength = NewLongLen;
    m_bVect1[m_nLongLength-1]=0;        // new 16.11.99
    m_bVect0[m_nLongLength-1]=0;        // new 16.11.99
 }
 m_nBitLength = NewBitLen;
 OldLong = LEN_LONG(SrcBitLen1);
 memcpy(m_bVect1, Vect11, OldLong*sizeof(ULONG));
 memcpy(m_bVect0, Vect10, OldLong*sizeof(ULONG));
 r_bit = ADR_BITLONG(SrcBitLen1);
 SrcLongLen = LEN_LONG(SrcBitLen2);
 if (r_bit==0)  {
   memcpy(m_bVect1+OldLong, Vect21, SrcLongLen*sizeof(ULONG));
   memcpy(m_bVect0+OldLong, Vect20, SrcLongLen*sizeof(ULONG)); return;
 }
 l_bit = S_4-r_bit; i=OldLong;
 m_bVect1[OldLong-1] |= Vect21[0]>>r_bit;
 m_bVect0[OldLong-1] |= Vect20[0]>>r_bit;
 for (j=0; j<SrcLongLen-1; j++) {
   m_bVect1[i++] = (Vect21[j] << l_bit) | (Vect21[j+1] >> r_bit);
   m_bVect0[i++] = (Vect20[j] << l_bit) | (Vect20[j+1] >> r_bit);
 }
 if (j*S_4+l_bit < SrcBitLen2) {
   m_bVect1[m_nLongLength-1] |= Vect21[SrcLongLen-1] << l_bit;
   m_bVect0[m_nLongLength-1] |= Vect20[SrcLongLen-1] << l_bit;
 }
 if (j=ADR_BITLONG(m_nBitLength)) {
   i = S_4 - j;
   m_bVect1[m_nLongLength-1] = (m_bVect1[m_nLongLength-1] >> i) << i;
   m_bVect0[m_nLongLength-1] = (m_bVect0[m_nLongLength-1] >> i) << i;
 }
}

//--------------------------- EqualBool --------------------------------------------------
BOOL CuTV::EqualBool(const ULONG* Vect, int BitLen) const
{int i;
 if ((Vect==NULL && m_bVect1 != NULL) || (m_nBitLength != BitLen)) return FALSE;
 for (i=0;i < m_nLongLength;i++)
   if ((m_bVect1[i] != Vect[i]) || (m_bVect0[i] != ~Vect[i])) return FALSE;
 return TRUE;
}

//--------------------------- Equality --------------------------------------------------
BOOL CuTV::Equality(const ULONG* Vect1, const ULONG* Vect0, int BitLen) const
{int i;
 if ((Vect1==NULL && m_bVect1 != NULL) || (m_nBitLength != BitLen)) return FALSE;
 for (i=0;i < m_nLongLength;i++)
   if ((m_bVect1[i] != Vect1[i]) || (m_bVect0[i] != Vect0[i])) return FALSE;
 return TRUE;
}

//--------------------------- Extr ------------------------------------------------------
void CuTV::Extr(const ULONG* SrcVect1, const ULONG* SrcVect0, int SrcBitLen, int nFirst, int nCount)
{ int i, j, nLenLong, l_bit, r_bit,AdrBeg;
 ASSERT(nCount >= 0);  ASSERT(nFirst >= 0);
 ASSERT((nCount+nFirst) <= SrcBitLen);
 nLenLong = LEN_LONG(nCount);
 AllocBuffer(nLenLong);
 m_nBitLength = nCount;
 if (SrcBitLen == nCount) {
   memcpy((ULONG*)m_bVect1,SrcVect1, nLenLong*sizeof(ULONG));
   memcpy((ULONG*)m_bVect0,SrcVect0, nLenLong*sizeof(ULONG)); return;
 }
 AdrBeg = BIT_LONG(nFirst);
 memcpy((ULONG*)m_bVect1,SrcVect1+AdrBeg, nLenLong*sizeof(ULONG));
 memcpy((ULONG*)m_bVect0,SrcVect0+AdrBeg, nLenLong*sizeof(ULONG));
 l_bit = ADR_BITLONG(nFirst);
 if (l_bit) {
   r_bit = S_4 - l_bit;
   if (nLenLong == LEN_LONG(SrcBitLen)) j=nLenLong-1;   //new, 4.12.97
   else                                 j=nLenLong;     //new, 4.12.97
   for (i=0; i < nLenLong; i++) {
     m_bVect1[i] = (SrcVect1[AdrBeg+i] << l_bit) | (SrcVect1[AdrBeg+i+1] >> r_bit);
     m_bVect0[i] = (SrcVect0[AdrBeg+i] << l_bit) | (SrcVect0[AdrBeg+i+1] >> r_bit);
   }
   if (j == nLenLong-1) {
     m_bVect1[j] = (SrcVect1[AdrBeg+j] << l_bit);  //new, 4.12.97
     m_bVect0[j] = (SrcVect0[AdrBeg+j] << l_bit);  //new, 4.12.97
   }
 }
 else {
   memcpy((ULONG*)m_bVect1,SrcVect1+AdrBeg, nLenLong*sizeof(ULONG));
   memcpy((ULONG*)m_bVect0,SrcVect0+AdrBeg, nLenLong*sizeof(ULONG));
 }
 if (ADR_BITLONG(m_nBitLength)) {
   i = S_4 - ADR_BITLONG(m_nBitLength);
   m_bVect1[m_nLongLength-1] = (m_bVect1[m_nLongLength-1] >> i) << i;
   m_bVect0[m_nLongLength-1] = (m_bVect0[m_nLongLength-1] >> i) << i;
 }
}

//--------------------------- LeftShiftVect ---------------------------------------------
void CuTV::LeftShiftVect(const ULONG* SrcVect1, const ULONG* SrcVect0, int nSrcLen, int nShift)
{int j, i, l_bit, r_bit, nNewLenLong, nNewLenBit, nSrcLong,AdrBeg;
 ASSERT(nShift >= 0 && nSrcLen >= nShift);
 nNewLenBit = nSrcLen - nShift;
 nNewLenLong = LEN_LONG(nNewLenBit);
 nSrcLong = LEN_LONG(nSrcLen);
 AllocBuffer(nSrcLong);
 m_nBitLength = nSrcLen;
 if (nNewLenBit==0)  return;
 if (nShift==0) {
   memcpy(m_bVect1,SrcVect1, nSrcLong*sizeof(ULONG));
   memcpy(m_bVect0,SrcVect0, nSrcLong*sizeof(ULONG)); return;
 }
 AdrBeg = BIT_LONG(nShift);
 l_bit = ADR_BITLONG(nShift);
 if (l_bit)    {
   r_bit = S_4 - l_bit;  j=0;
   for (i=AdrBeg; i<nSrcLong-1; i++) {
     m_bVect1[j++] = (SrcVect1[i] << l_bit) | (SrcVect1[i+1] >> r_bit);
     m_bVect0[j++] = (SrcVect0[i] << l_bit) | (SrcVect0[i+1] >> r_bit);
   }
   if ((j*S_4) < nShift) {
     m_bVect1[nNewLenLong-1] = SrcVect1[nSrcLong-1] << l_bit;
     m_bVect0[nNewLenLong-1] = SrcVect0[nSrcLong-1] << l_bit;
   }
 }
 else {
   memcpy(m_bVect1, SrcVect1+AdrBeg, nNewLenLong*sizeof(ULONG));
   memcpy(m_bVect0, SrcVect0+AdrBeg, nNewLenLong*sizeof(ULONG));
 }
 i= nSrcLong - nNewLenLong;
 memset(m_bVect1+nNewLenLong, 0, i*sizeof(ULONG));
 memset(m_bVect0+nNewLenLong, 0, i*sizeof(ULONG));
}

//--------------------------- Pogl ------------------------------------------------------
BOOL CuTV::Pogl(const ULONG* Vect1, const ULONG* Vect0, int BitLen) const
{ int i, j=0;
 ASSERT(m_nBitLength==BitLen);
 for (i=0; i < m_nLongLength; i++) {
   if ((m_bVect1[i] == Vect1[i]) && (m_bVect0[i] == Vect0[i])) continue;
   if ((m_bVect1[i] & Vect1[i]) != m_bVect1[i])   return FALSE;
   if ((m_bVect0[i] & Vect0[i]) != m_bVect0[i])   return FALSE;
   j=1;
 }
 if (j == 0) return FALSE;
 return TRUE;
}

//-------------------------- RightShiftVect --------------------------------------------
void CuTV::RightShiftVect(const ULONG* SrcVect1, const ULONG* SrcVect0, int nSrcLen, int nShift)
{int i, j, l_bit, r_bit, nSrcLong,AdrBeg;
 ASSERT(nShift >= 0 && nSrcLen >= nShift);
 nSrcLong = LEN_LONG(nSrcLen);
 AllocBuffer(nSrcLong);
 m_nBitLength = nSrcLen;
 if ((nSrcLen - nShift)==0)  return;
 if (nShift==0) {
   memcpy(m_bVect1,SrcVect1, nSrcLong*sizeof(ULONG));
   memcpy(m_bVect0,SrcVect0, nSrcLong*sizeof(ULONG)); return;
 }
 AdrBeg = BIT_LONG(nShift);
 r_bit = ADR_BITLONG(nShift);
 if (r_bit)    {
   l_bit = S_4 - r_bit;  j=0;
   m_bVect1[AdrBeg] = (SrcVect1[0] >> r_bit);
   m_bVect0[AdrBeg] = (SrcVect0[0] >> r_bit);
   for (i=AdrBeg+1; i<nSrcLong; i++) {
     m_bVect1[i] = (SrcVect1[j] << l_bit) | (SrcVect1[j+1] >> r_bit);
     m_bVect0[i] = (SrcVect0[j] << l_bit) | (SrcVect0[j+1] >> r_bit); j++;
   }
 }
 else {
   memcpy(m_bVect1+AdrBeg, SrcVect1, (nSrcLong-AdrBeg)*sizeof(ULONG));
   memcpy(m_bVect0+AdrBeg, SrcVect0, (nSrcLong-AdrBeg)*sizeof(ULONG));
 }
 if (i=ADR_BITLONG(m_nBitLength)) {
   m_bVect1[nSrcLong-1] = (m_bVect1[nSrcLong-1] >> (S_4 - i)) << (S_4 - i);
   m_bVect0[nSrcLong-1] = (m_bVect0[nSrcLong-1] >> (S_4 - i)) << (S_4 - i);
 }
}
