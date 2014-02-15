/////////////////////////////////////////////////////////////////
// Ver.2.1.1    22.02.2007
//              AssignCopy - добавил чистку пространства от Len до AllocLen
// Ver.2.1.0     03.07.2006
//               ѕереход к LINUX

///////////////////////////////////////////////////////////////
// Ver.2.0.5     19.06.2006
//           - ѕоправки BitChar с ограничением длины строки
// Ver.2.0.4     03.05.2006
//           - ѕоправки по последней версии CBV
// Ver.2.0.1     21.03.2006
//           - √енераторы случайных перестроены на возврат *this
//             GetRandVu - внеклассовыq метод генерировани€ 32 бит вектора
//             ѕоправка в SetSize - сразу возврат, если новый равен старому
// Ver.2.0.0     10.03.2006   Russian == English
//              - ƒобавлены генераторы случайных
// Ver.1.0     11.02.2005
/////////////////////////////////////////////////////////////////

#include <limits.h>
#include <string>
#include <stdarg.h>
using namespace std;

#include "../ComBool/UlongBool.h"

#ifndef _LINUX
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#endif

//---------------------------------------------------------GetRandVu
//  ѕолучение псевдослучайного 32-разр€дного булева вектора 
//  Rgrain - глобальна€ перем.= тек.сост.генератора
//  Rgrain = (Rgrain*(5**13))[mod(2**31-1)]
//    Vect - 32-разр€дный базовый булев вектор 
//  *** ћодификаци€ “омашева - 27 €нвар€ 1999
//---------------------------------------------------------
CuBV GetRandVu()
{ //CuBV Vect(32,0);
//  BYTE Work[4];
//  unsigned long   f13 = 1220703125;  // f13 = 5**13
//  unsigned long  d, m = 0x7fffffff;  //  m  = 2**31-1
//  d= GetRgrain();
//#ifndef _LINUX
// __asm mov eax,d           //      
// __asm mul f13             //  Rgrain=(Rgrain*f13)[mod(2**31-1)]
// __asm div m               //  (непосредсвенно на ассемблере)
// __asm mov d,edx           //                              V.T.
//    
////  SetRgrain(d);     // Rgrain - глобальна€ перем.= тек.сост.генератора 
//  Work[0] = (BYTE)( d >> 16); Work[1] = (BYTE)( d >> 8); 
// __asm mov eax,d           //      
// __asm mul f13             //  Rgrain=(Rgrain*f13)[mod(2**31-1)]
// __asm div m               //  (непосредсвенно на ассемблере)
// __asm mov d,edx           //                              V.T.
//  
//  SetRgrain(d);     // Rgrain - глобальна€ перем.= тек.сост.генератора 
//  Work[2] = (BYTE)( d >> 16); Work[3] = (BYTE)( d >> 8); 
//  Vect = (const ULONG*) Work;
//#endif
//  return Vect;
return NULL;
}

//********************************************************   GenRbv
// √енераци€ псевдослучайного <nCol>-компонентного булева //
// вектора с равноверо€тным распределением нулей и единиц //
//    ћодификаци€ “омашева - 27 €нвар€ 1999
//---------------------------------------------------------
CuBV CuBV::GenRbv (int nCol)
{ int n,i;
  Empty();
  n = nCol/32;      //число обращений к генератору баз.вектора -1
  for (i=0; i<n; i++) Concat(GetRandVu());
  i = nCol - n*32;
  if (i!=0)  Concat((const ULONG*)GetRandVu(), i);
  return *this;
}

//********************************************************//
// √енераци€ псевдослучайного <nCol>-компонентного булева   //
// вектора со средним числом <nRang> единиц (дисперси€ 1/16)
// (при nRang>=nCol все компоненты вектора равны 1) 
//////////////////////////////////////////////////////////////

CuBV CuBV::GenRbvMid(int nCol, int nRang)
{
  if(nRang>=nCol)  { SetSize(nCol); One(); return *this; }
  CuBV  bv_Con (nCol, 1); 
  int d, e, i, j, log = 4; 
  CuBV Rbv(nCol,0), bv = Rbv, bvCon = Rbv;
// ‘ормирование вектора prob, управл€ющего получением 
// среднего числа nRang единиц в векторе vbDis         
  unsigned int prob, half = 1 << (sizeof(int) * 8 - 1);
  prob = (unsigned int)((2.0 * nRang / nCol) * half);// вычисление доли nCol/nRang
  for (i = nCol; i; i >>= 1) log++;  // вычисление log(nCol)
  prob+= (half >> (log));            // округление prob до 1/16
 
// ‘ормирование вектора.
  e = 1;
  for (j = 0; j <= log; j++)
  { if(e) { d =- 1; e = 0; bvCon = bv_Con; }
      for (; d < j; d++)
        bvCon &= bv.GenRbv(nCol); 
      if((half >> j) & prob) { Rbv |= bvCon; e = 1; }
  }
  *this = Rbv;
  return *this;
}

////////////////////////////////////////////////////////////
// √енераци€ псевдослучайного <nCol>-компонентного булева //
// вектора с фиксированным числом <nRang> единиц 
// (при nRang>=nCol все компоненты вектора равны 1) 
////////////////////////////////////////////////////////////

CuBV CuBV::GenRbvFix (int nCol, int nRang)
{
  SetSize(nCol);
  if(nRang>=nCol) { One(); return *this; }
  Zero();
  int j; unsigned int k; 
  for ( j = 0; j <nRang; j++)
  {
    k = GetRandN() % nCol;
    if (GetBitAt( k )) j--;
    SetBitAt( k, 1);
  } 
  return *this;
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Construction ///////////////////////////////////////

//****************************** Protected function ***********************************//

//          void AllocCopy(CuBV& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
void CuBV::AllocCopy(CuBV& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const
{
  int nNewLen = nCopyLen + nExtraLen;
  if (nNewLen == 0) { dest.Init(); }
  else {
    dest.AllocBuffer(nNewLen);
    memcpy(dest.m_bVect, &m_bVect[nCopyIndex], nCopyLen*sizeof(ULONG));
  }
}

//*************************************************************************************//

//------------------------------------------------------------CuBV()
CuBV::CuBV()   { Init();  }

//----------------------------------------------------------- CuBV(const CuBV& bv)
CuBV::CuBV(const CuBV& bv)
{ bv.AllocCopy(*this, bv.m_nLongLength, 0, 0);  
  m_nBitLength = bv.m_nBitLength; }

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// More sophisticated construction /////////////////////////////

//-------------------------------------------------- CuBV(int nLen, BOOL Fl);
CuBV::CuBV(int nLen, BOOL Fl/*=FALSE*/)
{
  int nLenLong, w;
  if (nLen < 1) Init();      // return empty vector if invalid repeat count
  else {
      nLenLong = LEN_LONG(nLen);
      AllocBuffer(nLenLong);
      if (Fl) {
        memset(m_bVect, BYTE_1, nLenLong*sizeof(ULONG));
        w = ADR_BITLONG(nLen);
        if (w) m_bVect[nLenLong-1] = (m_bVect[nLenLong-1] >> (S_4 - w)) << (S_4 - w);
      }
      else       memset(m_bVect, 0, nLenLong*sizeof(ULONG));
      m_nBitLength = nLen;
  }
}

//---------------------------------------------------- CuBV(const ULONG* pV, int nLenBit);
CuBV::CuBV(const ULONG* pV, int nLenBit)
{ 
  int nLen;
  if (nLenBit==0) Init();
  else   {
    nLen = LEN_LONG(nLenBit);
    AllocBuffer(nLen);
    memcpy(m_bVect, pV, nLen*sizeof(ULONG));
    m_nBitLength = nLenBit;
  }
}

//--------------------------------------------------------------- CuBV(const char* pch)
CuBV::CuBV(const char* pch)
{
  int nLen, nLenBit;
  if ((nLenBit = SafeStrlen(pch)) == 0) Init();
  else   {
    nLen = LEN_LONG(nLenBit);        // Translate from symbol to bit
    AllocBuffer(nLen);
    CharBit(pch);
    m_nBitLength = nLenBit;
  }
}

#ifndef _LINUX
/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Reading and writing //////////////////////////////////
//----------------------------------------------- BitChar(char One, char Zero)
CString CuBV::BitChar(char One, char Zero,int Max/*=0*/) const
{ int i,j=0;
  CString res('\0',m_nBitLength);
  for (i=0; i<m_nBitLength; i++, j++)
  {
    if (GetBitAt(i)) res.SetAt(j,One);
    else             res.SetAt(j,Zero);
    if (Max!=0)
      if ((Max==1)||((i+1)%Max == 0))
      { res.SetAt(++j,'\n'); }   //res.SetAt(++j,'\r'); 
  }
  return res;
}
#else
string CuBV::BitChar(char One, char Zero,int Max/*=0*/) const
{ int i,j=0;
string res;
res.resize('\0',m_nBitLength);
for (i=0; i<m_nBitLength; i++, j++)
{
  if (GetBitAt(i)) res[j] = One;
  else             res[j] = Zero;
  if (Max!=0)
    if ((Max==1)||((i+1)%Max == 0))
    { res[++j] = '\n'; } //res[++j] ='\r'; 
}
return res;
}

#endif


/////////////////////////////////////////////////////////////////////////////////////////
////////////////////// Assignment operator and functions ////////////////////////////////

//****************************** Protected function ***********************************//
//----------------------------------------------------------------- AllocBuffer(int nLen)
void CuBV::AllocBuffer(int nLenLong)
{
  ASSERT(nLenLong >= 0);
  if (nLenLong == 0)     {   Init();     }
  else   {
    m_bVect = new ULONG[nLenLong];       // may throw an exception
    m_nLongLength = m_nAllocLength = nLenLong;
    memset(m_bVect, 0, nLenLong*sizeof(ULONG));
  }
}

//----------------------------  void AssignCopy(int nLen, int nLenLong, const ULONG* pV)
void CuBV::AssignCopy(int nLen, int nLenLong, const ULONG* pV)
{ //--- check if it will fit
  if (nLenLong > m_nAllocLength) { // it won't fit, allocate another one
    Empty();
    AllocBuffer(nLenLong);
  }
  else   // 22.02.2007
   memset(m_bVect+nLenLong,0,(m_nAllocLength-nLenLong)*sizeof(ULONG));

  if (nLenLong != 0) memcpy(m_bVect, pV, nLenLong*sizeof(ULONG));
  m_nLongLength = nLenLong;
  m_nBitLength = nLen;
}

//------------------------------------------------------------- CharBit(const char* pch)
void CuBV::CharBit(const char* pch)
{ 
  int i, j, nLen = strlen(pch);
  for (j=0; j < nLen; j++) {
    for (i=0; (i < S_4) && *pch; i++) {
      if (*pch == '1') m_bVect[j] |= OB4[i];
      else { ASSERT(*pch == '0' || *pch == '-'|| *pch == '.'); }
      ++pch;
    }
  }
}

//------------------------------------------------- AssignChar(int nLen, const char* pch)
void CuBV::AssignChar(int nLen, const char* pch)  // nLen > 0 - fact!
{ //--- check if it will fit
  if (nLen == 0)     { Empty();  return; }
  int nLenLong = LEN_LONG(nLen);
  if (nLenLong > m_nAllocLength) { // it won't fit, allocate another one
    Empty();
    AllocBuffer(nLenLong);
    m_nBitLength = nLen;
  }
  else  memset(m_bVect, 0, nLenLong*sizeof(ULONG));

  CharBit(pch);
  m_nLongLength = nLenLong;
  m_nBitLength = nLen;
}

//-------------------------------- Extr(const ULONG* pV, int nLen, int nFirst, int nCount)
void CuBV::Extr(const ULONG* pV, int nLen, int nFirst, int nCount)
{ int i, j, nLenLong, l_bit, r_bit, AdrBeg;

  ASSERT(nCount >= 0);  ASSERT(nFirst >= 0);
  ASSERT((nCount + nFirst) <= nLen);
  nLenLong = LEN_LONG(nCount);
  AllocBuffer(nLenLong);
  m_nBitLength = nCount;
  if (nLen == nCount) 
   { memcpy(m_bVect, pV, nLenLong*sizeof(ULONG)); return; }
  AdrBeg = BIT_LONG(nFirst);
  memcpy(m_bVect, pV + AdrBeg, nLenLong*sizeof(ULONG));
  l_bit = ADR_BITLONG(nFirst);
  if (l_bit)    {
    r_bit = S_4 - l_bit;
    if (nLenLong == LEN_LONG(nLen)) j = nLenLong-1;
    else                            j = nLenLong;
    for (i=0; i<j; i++)
      m_bVect[i] = (pV[AdrBeg + i] << l_bit) | (pV[AdrBeg + i + 1] >> r_bit);
    if (j == nLenLong-1) m_bVect[j] = (pV[AdrBeg + j] << l_bit);
  }
  else memcpy(m_bVect, pV + AdrBeg, nLenLong*sizeof(ULONG));
  if (ADR_BITLONG(m_nBitLength)) {
    i = S_4 - ADR_BITLONG(m_nBitLength);
    m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> i) << i;
  }
}

//*************************************************************************************//

//--------------------------------------------------------------------------------- One()
void CuBV::One()
{ 
  int i;
  if (m_nBitLength == 0) return;
  ASSERT(m_nLongLength > 0);
  memset(m_bVect, BYTE_1, m_nLongLength*sizeof(ULONG));
  if ((i = ADR_BITLONG(m_nBitLength)) > 0 ) i = S_4 - i;
  m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> i) << i;
}

//-------------------------------------------------------- Extract(int nFirst,int nCount)
CuBV CuBV::Extract(int nFirst, int nCount)
{ 
  CuBV s; s.Extr(m_bVect, m_nBitLength, nFirst, nCount);  return s;
}



/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Concatination //////////////////////////////////////
//----------------------------------------------------- Concat(const ULONG* pV, int nLen)
void CuBV::Concat(const ULONG* pV, int nLen)
{
  int NewBitLen, NewLongLen, LongLen, OldLong, r_bit, l_bit, i, j;
  ULONG *work;
  NewBitLen = m_nBitLength + nLen;
  if (NewBitLen == 0) { Init(); return;}
  NewLongLen = LEN_LONG(NewBitLen);
  OldLong = m_nLongLength;
  r_bit = ADR_BITLONG(m_nBitLength);
  if (m_nAllocLength < NewLongLen)  {
    if (m_nLongLength) {
      work = new ULONG[m_nLongLength];
      memcpy(work, m_bVect, m_nLongLength*sizeof(ULONG));
      Empty();
      AllocBuffer(NewLongLen);
      memcpy(m_bVect, work, OldLong*sizeof(ULONG));
      delete [] work;
    }
    else { AllocBuffer(NewLongLen); }
  }
  else
   if(m_nLongLength < NewLongLen) {
     m_nLongLength = NewLongLen;
     m_bVect[m_nLongLength-1] = 0;
   }
  m_nBitLength = NewBitLen;
  LongLen = LEN_LONG(nLen);
  if (r_bit==0)  {
    memcpy(m_bVect + OldLong, pV, LongLen*sizeof(ULONG));
    r_bit = S_4 - ADR_BITLONG(m_nBitLength);
    if (r_bit == S_4) r_bit = 0;
    m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> r_bit) << r_bit;
    return;
  }
  l_bit = S_4 - r_bit; i = OldLong;
  m_bVect[i-1] |= pV[0] >> r_bit;
  for (j=0; j < LongLen - 1; j++)
    { m_bVect[i++] = (pV[j] << l_bit) | (pV[j+1] >> r_bit); }
  if (j*S_4 + l_bit < nLen)
    m_bVect[m_nLongLength-1] |= pV[LongLen-1] << l_bit;
  if (j = ADR_BITLONG(m_nBitLength)) {
    i = S_4 - j;
    m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> i) << i;
  }
}

//****************************** Protected function ***********************************//
//--------------------- Conc2(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
void CuBV::Conc2(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
{
  int NewBitLen, NewLongLen, LongLen, OldLong, r_bit, l_bit, i, j;

  NewBitLen = nLen1 + nLen2;
  if (NewBitLen == 0) { Init(); return;}
  NewLongLen = LEN_LONG(NewBitLen);
  if (m_nAllocLength < NewLongLen)  { Empty(); AllocBuffer(NewLongLen); }
  else { m_nLongLength = NewLongLen;  m_bVect[m_nLongLength-1] = 0; }

  m_nBitLength = NewBitLen;
  OldLong = LEN_LONG(nLen1);
  memcpy(m_bVect, pV1, OldLong*sizeof(ULONG));
  r_bit = ADR_BITLONG(nLen1);
  LongLen = LEN_LONG(nLen2);
  if (r_bit == 0)  { memcpy(m_bVect + OldLong, pV2, LongLen*sizeof(ULONG)); return;}
  l_bit = S_4 - r_bit; i = OldLong;
  m_bVect[OldLong-1] |= pV2[0] >> r_bit;
  for (j=0; j < LongLen-1; j++)
    { m_bVect[i++] = (pV2[j] << l_bit) | (pV2[j+1] >> r_bit); }
  if (j*S_4 + l_bit < nLen2)
    m_bVect[m_nLongLength-1] |= pV2[LongLen-1] << l_bit;
  if (j=ADR_BITLONG(m_nBitLength)) {
    i = S_4 - j;
    m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> i) << i;
  }
}
//*************************************************************************************//
//--------------------------------- Concat(BOOL bBit)
void CuBV::Concat(BOOL Bit/*=FALSE*/)
{ 
  int nLongLength, nLen = m_nBitLength + 1;
  if((nLongLength = LEN_LONG(nLen)) > m_nAllocLength)
    GetBuffer(nLen);
  m_nLongLength = nLongLength;
  m_nBitLength = nLen; SetBitAt(nLen-1, Bit);
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Advanced assignment operators /////////////////////////////////

//****************************** Protected function ***********************************//
//---------------------------------------------------------- LeftShiftInPlace(int nShift)
void CuBV::LeftShiftInPlace(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CuBV object
  int i, j, l_bit, r_bit, nNewLenLong, nNewLenBit, AdrBeg;

  if (nShift <= 0)       return;
  nNewLenBit = m_nBitLength - nShift;
  ASSERT(nNewLenBit >= 0);
  if (nNewLenBit==0)  { memset(m_bVect,0,m_nLongLength*sizeof(ULONG)); return; }
  nNewLenLong = LEN_LONG(nNewLenBit);
  AdrBeg = BIT_LONG(nShift);
  l_bit = ADR_BITLONG(nShift);
  if (l_bit)    {
    r_bit = S_4 - l_bit;  j=0;
    for (i = AdrBeg; i < m_nLongLength-1; i++)
      m_bVect[j++] = (m_bVect[i] << l_bit) | (m_bVect[i+1] >> r_bit);
    if ((j*S_4 + nShift) < m_nBitLength)
      m_bVect[nNewLenLong-1] <<=  l_bit;
  }
  else   memcpy(m_bVect, m_bVect + AdrBeg, nNewLenLong*sizeof(ULONG));
  i= m_nLongLength - nNewLenLong;
  memset(m_bVect + nNewLenLong, 0, i*sizeof(ULONG));
}

//--------------------------------------------------------- RightShiftInPlace(int nShift)
void CuBV::RightShiftInPlace(int nShift)
{  // RightShift bool vector  -- assume that 'this' is a new CuBV object
  int i, j, l_bit, r_bit, AdrBeg;
  ULONG *work;
  if (nShift <= 0) return;
  ASSERT((m_nBitLength - nShift) >= 0);
  if ((m_nBitLength - nShift)==0)  
   { memset(m_bVect, 0, m_nLongLength*sizeof(ULONG)); return; }
  work = new ULONG[m_nLongLength];       // may throw an exception
  memset(work, 0, m_nLongLength*sizeof(ULONG));
  AdrBeg = BIT_LONG(nShift);
  r_bit = ADR_BITLONG(nShift);
  if (r_bit)    {
    l_bit = S_4 - r_bit;  j=0;
    work[AdrBeg] = (m_bVect[0] >> r_bit);
    for (i = AdrBeg + 1; i < m_nLongLength; i++)
      { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
  }
  else memcpy(work + AdrBeg, m_bVect, (m_nLongLength - AdrBeg)*sizeof(ULONG));
  if (i=ADR_BITLONG(m_nBitLength))
    work[m_nLongLength-1] = (work[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
  memcpy(m_bVect, work, m_nLongLength*sizeof(ULONG));
  delete [] work;
}

//--------------------------------------------- DizInPlace(const ULONG* pV, int nLen)
void CuBV::DizInPlace(const ULONG* pV, int nLen)
{
  ASSERT(nLen >= 0);  ASSERT(nLen == m_nBitLength);
  for (int i = 0; i < m_nLongLength; i++) m_bVect[i] |= pV[i];
}

//--------------------------------------------- ConInPlace(const ULONG* pV, int nLen)
void CuBV::ConInPlace(const ULONG* pV, int nLen)
{
  ASSERT(nLen >= 0); ASSERT(nLen == m_nBitLength);
  for (int i=0; i < m_nLongLength; i++) m_bVect[i] &= pV[i];
}

//-------------------------------------------- Add2InPlace(const ULONG* pV, int nLen)
void CuBV::Add2InPlace(const ULONG* pV, int nLen)
{
  ASSERT(nLen >= 0); ASSERT(nLen == m_nBitLength);
  for (int i=0; i < m_nLongLength; i++) m_bVect[i] ^= pV[i];
}

//------------------------------------------ ConNotInPlace(const ULONG* pV, int nLen)
void CuBV::ConNotInPlace(const ULONG* pV, int nLen)
{
  int i;
  ASSERT(nLen >= 0);  ASSERT(nLen == m_nBitLength);
  for (i=0; i < m_nLongLength; i++) m_bVect[i] &= ~pV[i];
  i = ADR_BITLONG(nLen);
  if (i)  m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
}
//*************************************************************************************//

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Logic operations ////////////////////////////////////

//****************************** Protected function ***********************************//
//---------------------------------- LeftShiftVect(const ULONG* pV, int nLen, int nShift)
void CuBV::LeftShiftVect(const ULONG* pV, int nLen, int nShift)
{
  int j, i, l_bit, r_bit, nNewLenLong, nNewLenBit, nSrcLong, AdrBeg;
  ASSERT(nShift >= 0);
  nNewLenBit = nLen - nShift;
  ASSERT(nNewLenBit >= 0);
  nNewLenLong = LEN_LONG(nNewLenBit);
  nSrcLong = LEN_LONG(nLen);
  AllocBuffer(nSrcLong);
  m_nBitLength = nLen;
  if (nNewLenBit==0)  return;
  if (nShift==0)      { memcpy(m_bVect, pV, nSrcLong*sizeof(ULONG)); return; }
  AdrBeg = BIT_LONG(nShift);
  l_bit = ADR_BITLONG(nShift);
  if (l_bit)    {
    r_bit = S_4 - l_bit;  j=0;
    for (i = AdrBeg; i < nSrcLong-1; i++)
      m_bVect[j++] = (pV[i] << l_bit) | (pV[i+1] >> r_bit);
    if ((j*S_4 + nShift) < m_nBitLength)
      m_bVect[nNewLenLong-1] = pV[nSrcLong - 1] << l_bit;
  }
  else   memcpy(m_bVect, pV + AdrBeg, nNewLenLong*sizeof(ULONG));
  i = nSrcLong - nNewLenLong;
  memset(m_bVect + nNewLenLong, 0, i*sizeof(ULONG));
}

//-------------------------- RightShiftVect(const ULONG* pV, int nLen, int nShift)
void CuBV::RightShiftVect(const ULONG* pV, int nLen, int nShift)
{
  int i, j, l_bit, r_bit, nSrcLong, AdrBeg;
  ASSERT(nShift >= 0);  ASSERT((nLen - nShift) >= 0);
  nSrcLong = LEN_LONG(nLen);
  AllocBuffer(nSrcLong);
  m_nBitLength = nLen;
  if ((nLen - nShift) == 0) return;  //?????
  if (nShift==0)      { memcpy(m_bVect, pV, nSrcLong*sizeof(ULONG)); return; }
  AdrBeg = BIT_LONG(nShift);
  r_bit = ADR_BITLONG(nShift);
  if (r_bit)    {
    l_bit = S_4 - r_bit;  j=0;
    m_bVect[AdrBeg] = (pV[0] >> r_bit);
    for (i = AdrBeg + 1; i < nSrcLong; i++)
     { m_bVect[i] = (pV[j] << l_bit) | (pV[j+1] >> r_bit); j++; }
  }
  else memcpy(m_bVect + AdrBeg, pV, (nSrcLong - AdrBeg)*sizeof(ULONG));
  if (i=ADR_BITLONG(m_nBitLength))
    m_bVect[nSrcLong-1] = (m_bVect[nSrcLong-1] >> (S_4 - i)) << (S_4 - i);
}

//---------------------- Diz(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
void CuBV::Diz(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2); ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] | pV2[i];
}

//---------------------- Con(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
void CuBV::Con(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2);  ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] & pV2[i];
}

//--------------------- Add2(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
void CuBV::Add2(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2); ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] ^ pV2[i];
}

//------------------- ConNot(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
void CuBV::ConNot(const ULONG* pV1, int nLen1, const ULONG* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2);  ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] & ~pV2[i];
}

//---------------------------------------------------- Not(const ULONG* pV, int nLen)
void CuBV::Not(const ULONG* pV, int nLen)
{
  int i, LenLong;
  ASSERT(nLen >= 0);
  LenLong = LEN_LONG(nLen);
  AllocBuffer(LenLong);
  m_nBitLength = nLen;
  for (i=0; i < LenLong; i++) m_bVect[i] = ~pV[i];
  i = ADR_BITLONG(nLen);
  if (i)  m_bVect[LenLong-1] = (m_bVect[LenLong-1] >> (S_4-i)) << (S_4-i);
}
//*************************************************************************************//

//------------------------------------- operator <<(const CuBV& bv, int nShift)
CuBV operator <<(const CuBV& bv, int nShift)
{ 
  CuBV s; s.LeftShiftVect(bv.m_bVect, bv.m_nBitLength, nShift);  return s;
}

//------------------------------------- operator >>(const CuBV& bv, int nShift)
CuBV operator >>(const CuBV& bv, int nShift)
{ 
  CuBV s; s.RightShiftVect(bv.m_bVect, bv.m_nBitLength, nShift);  return s;
}

//-------------------------------- operator |(const CuBV& bv1,const CuBV& bv2)
CuBV operator |(const CuBV& bv1,const CuBV& bv2)
{ 
  CuBV s;
  s.Diz(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------- operator |(const CuBV& bv, const ULONG* pV)
CuBV operator |(const CuBV& bv, const ULONG* pV)
{
  CuBV s;
  s.Diz(bv.m_bVect, bv.m_nBitLength, pV, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator |(const CuBV& bv, const ULONG* pV)
CuBV operator |(const ULONG* pV, const CuBV& bv)
{ 
  CuBV s;
  s.Diz(pV, bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator &(const CuBV& bv1, const CuBV& bv2)
CuBV operator &(const CuBV& bv1, const CuBV& bv2)
{ 
  CuBV s;
  s.Con(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------- operator &(const ULONG* pV, const CuBV& bv)
CuBV operator &(const ULONG* pV, const CuBV& bv)
{
  CuBV s;
  s.Con(pV, bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator &(const CuBV& bv, const ULONG* pV)
CuBV operator &(const CuBV& bv, const ULONG* pV)
{ 
  CuBV s;
  s.Con(bv.m_bVect, bv.m_nBitLength, pV, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator ^(const CuBV& bv1,const CuBV& bv2)
CuBV operator ^(const CuBV& bv1, const CuBV& bv2)
{
  CuBV s;
  s.Add2(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------- operator ^(const ULONG* pV, const CuBV& bv)
CuBV operator ^(const ULONG* pV, const CuBV& bv)
{
  CuBV s;
  s.Add2(pV, bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator ^(const CuBV& bv, const ULONG* pV)
CuBV operator ^(const CuBV& bv, const ULONG* pV)
{
  CuBV s;
  s.Add2(bv.m_bVect, bv.m_nBitLength, pV, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator -(const CuBV& bv1, const CuBV& bv2)
CuBV operator -(const CuBV& bv1, const CuBV& bv2)
{
  CuBV s;
  s.ConNot(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------- operator -(const ULONG* pV, const CuBV& bv)
CuBV operator -(const ULONG* pV, const CuBV& bv)
{
  CuBV s;
  s.ConNot(pV, bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator -(const CuBV& bv, const ULONG* pV)
CuBV operator -(const CuBV& bv, const ULONG* pV)
{
  CuBV s;
  s.ConNot(bv.m_bVect, bv.m_nBitLength, pV, bv.m_nBitLength);
  return s;
}

//-------------------------------------------------- operator ~(const CuBV& bv)
CuBV operator ~(const CuBV& bv)
{ CuBV s; s.Not(bv.m_bVect, bv.m_nBitLength); return s; }

//-------------------------------------------------- Invert(const ULONG* pV)
void CuBV:: Invert(const ULONG* pV)
{ int i;
  for (i=0; i < m_nLongLength; i++) m_bVect[i] = ~pV[i];
  i = ADR_BITLONG(m_nBitLength);
  if (i)  m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
}

//------------------------------------------------------------- LoopLeftShift(int nShift)
void CuBV::LoopLeftShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CuBV object
  int i, j, l_bit, r_bit, nNewLenLong, nNewLenBit, nEndLenLong;
  ULONG *work;
  if (m_nLongLength==0 || nShift==0) return;
  ASSERT(nShift > 0);
  nNewLenBit = m_nBitLength - nShift;
  
  ASSERT(nNewLenBit > 0);
  nNewLenLong = LEN_LONG(nNewLenBit);
  
  work = new ULONG[m_nLongLength];       // may throw an exception
  memset(work, 0, m_nLongLength*sizeof(ULONG));
  i = BIT_LONG(nShift);
  l_bit = ADR_BITLONG(nShift);
  if (l_bit)    {
    r_bit = S_4 - l_bit;
    memcpy(work, m_bVect + i, (nNewLenLong + 1)*sizeof(ULONG));
    for (i=0; i<nNewLenLong; i++)
      work[i] = (work[i] << l_bit) | (work[i+1] >> r_bit);
    work[i] <<= l_bit;
  }
  else   memcpy(work, m_bVect + i, nNewLenLong*sizeof(ULONG));
  nEndLenLong = m_nLongLength - nNewLenLong;
  j = BIT_LONG(nNewLenBit);
  r_bit = ADR_BITLONG(nNewLenBit);
  if (r_bit == 0 )    { memcpy(work+j, m_bVect, nEndLenLong*sizeof(ULONG)); }
  else {
    l_bit = S_4 - r_bit;
    work[j] = (work[j] >> l_bit) << l_bit;
    work[j++] |= (m_bVect[0] >> r_bit);
    for (i=0; i<nEndLenLong; i++)
      work[j++] = (m_bVect[i] << l_bit) | (m_bVect[i+1] >> r_bit);
    j = S_4 - ADR_BITLONG(m_nBitLength);
    work[m_nLongLength-1] = (work[m_nLongLength-1] >> j)<<j;
  }
  memcpy(m_bVect, work, m_nLongLength*sizeof(ULONG));
  delete [] work;
}

//------------------------------------------------------------ LoopRightShift(int nShift)
void CuBV::LoopRightShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CuBV object
  int i, j, l_bit, r_bit, AdrBeg, AdrEnd, BitEnd;
  ULONG *work;
  if (nShift == 0) return;
  ASSERT(nShift > 0); ASSERT((m_nBitLength - nShift) >= 0);

  if ((m_nBitLength - nShift)==0)  
    { memset(m_bVect, 0, m_nLongLength*sizeof(ULONG)); return; }
  work = new ULONG[m_nLongLength];       // may throw an exception
  memset(work, 0, m_nLongLength*sizeof(ULONG));
  AdrBeg = BIT_LONG(nShift);
  AdrEnd = BIT_LONG(m_nBitLength - nShift);
  BitEnd = ADR_BITLONG(m_nBitLength);
  if (BitEnd < nShift) {
    l_bit = ADR_BITLONG(m_nBitLength - nShift);
    r_bit = S_4 - l_bit;     j = AdrEnd;
    for (i=0; i < AdrBeg + 1; i++)
      { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
  }
  else { l_bit = BitEnd - nShift; work[0] = m_bVect[AdrEnd] << l_bit; }
  r_bit = ADR_BITLONG(nShift); l_bit = S_4 - r_bit;
  work[AdrBeg] = (work[AdrBeg] >> l_bit) << l_bit;
  if (r_bit)    {
    j=0;
    work[AdrBeg] |= (m_bVect[0] >> r_bit);
    for (i=AdrBeg+1; i<m_nLongLength; i++)
      { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
  }
  else memcpy(work + AdrBeg, m_bVect, (m_nLongLength - AdrBeg)*sizeof(ULONG));
  if (i = ADR_BITLONG(m_nBitLength))
    work[m_nLongLength-1] = (work[m_nLongLength-1] >> (S_4-i)) << (S_4-i);
  memcpy(m_bVect, work, m_nLongLength*sizeof(ULONG));
  delete [] work;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// Operation of weighting,finding and casing ///////////////////////
//---------------------------------------------------------------------------- CountBit()
int CuBV::CountBit() const
{
  BYTE* pB = (BYTE*)m_bVect;
  int one=0, j, w = m_nLongLength<<2; 
  for (j=0; j < w; j++) one += TabC[pB[j]];
  return one;
}

//--------------------------------------------------------------- LeftOneLong(int nNext)
int CuBV::LeftOneLong(int nNext) const
{
  int i, j, pos = 0;
  ULONG val;
  ASSERT((m_nBitLength - nNext) > 0);
  if (nNext == -1) { i=0; j=0; val = m_bVect[i]; }
  else {
    i = BIT_LONG(nNext);  j=ADR_BITLONG(nNext); j++; 
    if (j == S_4)  {
      j = 0;  i++; 
      if (i == m_nLongLength)  return (-1);
      val = m_bVect[i];
    }
    else
      val = m_bVect[i] << j;
  }
  if (val)  {
m0: if (val & 0xff000000)  {
m3:   while (!(val & OB4[pos++]));
      return (nNext + pos);
    }
    else {
      pos +=8;
      if (val & 0x00ff0000) goto m3;
      pos +=8;
      if (val & 0x0000ff00) goto m3;
      pos +=8;
      goto m3;
    }
  }
  for (i++; i < m_nLongLength; i++)
    if (m_bVect[i])  {
      val = m_bVect[i];
      nNext = (i<<5) - 1;
      goto m0;
    }
  return (-1);
}

//-------------------------------------------------------------------- LeftOne(int nNext)
int CuBV::LeftOne(int nNext) const
{
  int offset, bit, pos = 0;
  ULONG val;
  ASSERT((m_nBitLength - nNext) > 0);
  if (m_nLongLength==0) return -1;
 
  if (nNext == -1) { offset=0; bit=0; val = m_bVect[0];}
  else {
    offset = BIT_LONG(nNext);  
    bit = ADR_BITLONG(nNext) + 1; 
    if (bit == S_4)  
      { bit = 0;  offset++; val = m_bVect[offset]; }
    else 
      val = m_bVect[offset] << bit;
    if (offset == m_nLongLength)  return (-1);
  }
  if (val!=0) {
m0: if (val & 0xffff0000)
      if (val & 0xff000000)
        if (val & 0xf0000000)
          if (val & 0xc0000000)
            if (val & 0x80000000)   return ((offset<<5)+bit);
            else                    return ((offset<<5)+bit+1);
          else
            if (val & 0x20000000)   return ((offset<<5)+bit+2);
            else                    return ((offset<<5)+bit+3);
        else
          if (val & 0x0c000000)
            if (val & 0x08000000)   return ((offset<<5)+bit+4);
            else                    return ((offset<<5)+bit+5);
          else
            if (val & 0x02000000)   return ((offset<<5)+bit+6);
            else                    return ((offset<<5)+bit+7);
      else
        if (val & 0x00f00000)
          if (val & 0x00c00000)
            if (val & 0x00800000)   return ((offset<<5)+bit+8);
            else                    return ((offset<<5)+bit+9);
          else
            if (val & 0x00200000)   return ((offset<<5)+bit+10);
            else                    return ((offset<<5)+bit+11);
        else
          if (val & 0x000c0000)
            if (val & 0x00080000)   return ((offset<<5)+bit+12);
            else                    return ((offset<<5)+bit+13);
          else
            if (val & 0x00020000)   return ((offset<<5)+bit+14);
            else                    return ((offset<<5)+bit+15);
    else
      if (val & 0x0000ff00)
        if (val & 0x0000f000)
          if (val & 0x0000c000)
            if (val & 0x00008000)   return ((offset<<5)+bit+16);
            else                    return ((offset<<5)+bit+17);
          else
            if (val & 0x00002000)   return ((offset<<5)+bit+18);
            else                    return ((offset<<5)+bit+19);
        else
          if (val & 0x00000c00) 
            if (val & 0x00000800)   return ((offset<<5)+bit+20);
            else                    return ((offset<<5)+bit+21);
          else
            if (val & 0x00000200)   return ((offset<<5)+bit+22);
            else                    return ((offset<<5)+bit+23);
      else 
        if (val & 0x000000f0)
          if (val & 0x000000c0)
            if (val & 0x00000080)   return ((offset<<5)+bit+24);
            else                    return ((offset<<5)+bit+25);
          else
            if (val & 0x00000020)   return ((offset<<5)+bit+26);
            else                    return ((offset<<5)+bit+27);
        else 
          if (val & 0x0000000c) 
            if (val & 0x00000008)   return ((offset<<5)+bit+28);
            else                    return ((offset<<5)+bit+29);
          else
            if (val & 0x00000002)   return ((offset<<5)+bit+30);
            else                    return ((offset<<5) + 31);
  }
  for (offset++; offset < m_nLongLength; offset++)
    if (m_bVect[offset])  {
      val = m_bVect[offset];
      bit = 0;
      goto m0;
    }
  return (-1);
}

/*
//------------------------------------------------------------------- RightOne(int nPrev)
int CuBV::RightOne(int nPrev) const
{
  int i, j, pos = S_4 - 1;
  ULONG val;
  ASSERT((m_nBitLength - nPrev) >= 0);
  if (m_nLongLength==0) return -1;
  if (nPrev == -1) { i = m_nLongLength - 1; j=0;  val = m_bVect[i];}
  else {
    i = BIT_LONG(nPrev);  
    j = S_4 - ADR_BITLONG(nPrev); 
    if (j == S_4)  { j = 0;  i--; val = m_bVect[i]; }
    else val = (m_bVect[i] >> j) << j;
  }
  if (val)  {
m0: if (val & 0x000000ff)  {
m3:    while (!(val & OB4[pos--]));
      return ((i*S_4)+pos+1);
    }
    else {
      pos -= 8;
      if (val & 0x0000ff00)  goto m3;
      pos -= 8;
      if (val & 0x00ff0000)  goto m3;
      pos -= 8;
      goto m3;
    }
  }
  for (j = i-1; j >= 0; j--)
    if(m_bVect[j])  {
      val = m_bVect[j];
      goto m0;
    }
  return (-1);
}
*/
//------------------------------------------------------------------- RightOne(int nPrev)
int CuBV::RightOne(int nPrev) const
{
  int offset, j, pos = S_4 - 1;
  ULONG val;
  if (m_nLongLength==0) return -1;
  ASSERT((m_nBitLength - nPrev) >= 0);
  if (nPrev == -1) { offset = m_nLongLength - 1; val = m_bVect[offset];}
  else {
    offset = BIT_LONG(nPrev);  
    j = S_4 - ADR_BITLONG(nPrev); 
    if (j == S_4)  { j = 0;  offset--; val = m_bVect[offset]; }
    else val = (m_bVect[offset] >> j) << j;
  }
  if (val!=0) {
m0: if (val& 0x0000ffff)
      if (val & 0x000000ff)
        if (val & 0x0000000f)  
          if (val & 0x00000003) 
            if (val & 0x00000001) { val&=0xfffffffe; return ((offset<<5)+31); }
            else                        { val&=0xfffffffc; return ((offset<<5)+30); }
          else
            if (val & 0x00000004) { val&=0xfffffff8; return ((offset<<5)+29); }
            else                        { val&=0xfffffff0; return ((offset<<5)+28); }
        else
          if (val & 0x00000030) 
            if (val & 0x00000010) { val&=0xffffffe0; return ((offset<<5)+27); }
            else                        { val&=0xffffffc0; return ((offset<<5)+26); }
          else
            if (val & 0x00000040) { val&=0xffffff80; return ((offset<<5)+25); }
            else                        { val&=0xffffff00; return ((offset<<5)+24); }
      else
        if (val & 0x00000f00)  
          if (val & 0x00000300) 
            if (val & 0x00000100) { val&=0xfffffe00; return ((offset<<5)+23); }
            else                        { val&=0xfffffc00; return ((offset<<5)+22); }
          else
            if (val & 0x00000400) { val&=0xfffff800; return ((offset<<5)+21); }
            else                        { val&=0xfffff000; return ((offset<<5)+20); }
        else
          if (val & 0x00003000) 
            if (val & 0x00001000) { val&=0xffffe000; return ((offset<<5)+19); }
            else                        { val&=0xffffc000; return ((offset<<5)+18); }
          else
            if (val & 0x00004000) { val&=0xffff8000; return ((offset<<5)+17); }
            else                        { val&=0xffff0000; return ((offset<<5)+16); }
    else
      if (val & 0x00ff0000)
        if (val & 0x000f0000)  
          if (val & 0x00030000) 
            if (val & 0x00010000) { val&=0xfffe0000; return ((offset<<5)+15); }
            else                        { val&=0xfffc0000; return ((offset<<5)+14); }
          else
            if (val & 0x00040000) { val&=0xfff80000; return ((offset<<5)+13); }
            else                        { val&=0xfff00000; return ((offset<<5)+12); }
        else
          if (val & 0x00300000) 
            if (val & 0x00100000) { val&=0xffe00000; return ((offset<<5)+11); }
            else                        { val&=0xffc00000; return ((offset<<5)+10); }
          else
            if (val & 0x00400000) { val&=0xff800000; return ((offset<<5)+9); }
            else                        { val&=0xff000000; return ((offset<<5)+8); }
      else
        if (val & 0x0f000000)  
          if (val & 0x03000000) 
            if (val & 0x01000000) { val&=0xfe000000; return ((offset<<5)+7); }
            else                        { val&=0xfc000000; return ((offset<<5)+6); }
          else
            if (val & 0x04000000) { val&=0xf8000000; return ((offset<<5)+5); }
            else                        { val&=0xf0000000; return ((offset<<5)+4); }
        else
          if (val & 0x30000000) 
            if (val & 0x10000000) { val&=0xe0000000; return ((offset<<5)+3); }
            else                        { val&=0xc0000000; return ((offset<<5)+2); }
          else
            if (val & 0x40000000) { val&=0x80000000; return ((offset<<5)+1); }
            else                        { val=0; return ((offset<<5)); }
  }
  for (offset--; offset >= 0; offset--)
    if(m_bVect[offset])  { val = m_bVect[offset]; goto m0; }
  return (-1);
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Comparing operations ///////////////////////////////////////

//****************************** Protected function ***********************************//
//--------------------------------------------- Equality(const ULONG* pV, int nLen) const
BOOL CuBV::Equality(const ULONG* pV, int nLen) const
{
  if (pV == NULL && m_bVect) return FALSE;
  if (m_nBitLength != nLen)  return FALSE;
  for (int i=0; i < m_nLongLength; i++)
    if (m_bVect[i] != pV[i]) return FALSE;
  return TRUE;
}

//-------------------------------------- Pogl(const ULONG* pV, int nLen, BOOL Dist) const
BOOL CuBV::Pogl(const ULONG* pV, int nLen, BOOL Dist) const
{ 
  int i, j=0;
  ASSERT(m_nBitLength == nLen);
  for (i=0; i < m_nLongLength; i++) {
    if (m_bVect[i] == pV[i]) j++;
    if (Dist) { if ((m_bVect[i] & pV[i]) != pV[i])   return FALSE; }
    else      { if ((m_bVect[i] & pV[i]) != m_bVect[i]) return FALSE; }
  }
  if (j == m_nLongLength) return FALSE;
  return TRUE;
}

//------------------------------------- PoglEq(const ULONG* pV, int nLen, BOOL Dist) const
BOOL CuBV::PoglEq(const ULONG* pV, int nLen, BOOL Dist) const
{
  ASSERT(m_nBitLength == nLen);
  for (int i=0; i < m_nLongLength; i++) {
    if (Dist) { if ((m_bVect[i] & pV[i]) != pV[i])   return FALSE; }
    else      { if ((m_bVect[i] & pV[i]) != m_bVect[i]) return FALSE; }
  }
  return TRUE;
}
//*************************************************************************************//

//------------------------------------------------------------------------------ IsZero()
BOOL CuBV::IsZero() const
{
  if (m_nLongLength == 0) return FALSE;
  for (int i=0; i < m_nLongLength; i++)  if (m_bVect[i]) return FALSE;
  return TRUE;
}

//------------------------------------------------------------------------------- IsOne()
BOOL CuBV::IsOne() const
{
  int i; ULONG LastLong;
  if (m_nLongLength == 0) return FALSE;
  for (i=0; i < m_nLongLength-1; i++)
    { LastLong = ~m_bVect[i]; if (LastLong) return FALSE; }
  i = S_4 - ADR_BITLONG(m_nBitLength);
  if (i != S_4) LastLong = (~m_bVect[m_nLongLength-1] >> i) << i;
  else          LastLong = ~m_bVect[m_nLongLength-1];
  if (LastLong) return FALSE;
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Input/output operations //////////////////////////////////////
//-------------------------------- operator <<(CDumpContext& dc, const CuBV& bv)
#ifdef _DEBUG
CDumpContext& operator <<(CDumpContext& dc, const CuBV& bv)
{
  CString res('\0',bv.m_nBitLength);
  for (int i=0; i < bv.m_nBitLength; i++)
    if (bv.GetBitAt(i)) res.SetAt(i,'1');
    else             res.SetAt(i,'0');
  dc << res; return dc;
}
#endif //_DEBUG

//------------------------------------- operator<<(CArchive& ar, const CuBV& bv)
#ifndef _LINUX
CArchive& operator<<(CArchive& ar, const CuBV& bv)
{
  ar << bv.m_nLongLength;
  ar << bv.m_nBitLength;
  ar.Write(bv.m_bVect, bv.m_nLongLength*sizeof(ULONG));
  return ar;
}

//------------------------------------- operator>>(CArchive& ar, const CuBV& bv)
CArchive& operator>>(CArchive& ar, CuBV& bv)
{
  bv.Empty();
  ar >> bv.m_nLongLength;
  ar >> bv.m_nBitLength;
  if (bv.m_nLongLength != 0) {
    bv.AllocBuffer(bv.m_nLongLength);
    if (ar.Read(bv.m_bVect, bv.m_nLongLength*sizeof(ULONG)) != bv.m_nLongLength*sizeof(ULONG))
      AfxThrowArchiveException(CArchiveException::endOfFile);
  }
  return ar;
}
#endif //not _LINUX

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Advanced dump of memmory /////////////////////////////////
//---------------------------------------------------------- GetBuffer(int nMinBufLength)

ULONG* CuBV::GetBuffer(int nMinBufLength)
{
  ASSERT(nMinBufLength >= 0);
  if (nMinBufLength > m_nBitLength) {
    ULONG* pbtOldData = m_bVect;
    int nOldLen = m_nLongLength, nOldBit = m_nBitLength;
    AllocBuffer(LEN_LONG(nMinBufLength));
    memcpy(m_bVect, pbtOldData, nOldLen*sizeof(ULONG));
    m_nLongLength = LEN_LONG(nMinBufLength);
    m_nBitLength = nMinBufLength;
    SafeDelete(pbtOldData);
  }
  return m_bVect;
}

//--------------------------------------------------------- ReleaseBuffer(int nNewLength)
void CuBV::ReleaseBuffer(int nNewLength)
{
  int nNewLong;
  if (nNewLength == -1) nNewLength = m_nBitLength;
  nNewLong = LEN_LONG(nNewLength);
  ASSERT(nNewLong <= m_nAllocLength);
  m_nLongLength = nNewLong;  m_nBitLength = nNewLength;
  if (nNewLength = ADR_BITLONG(m_nBitLength)) {
    nNewLength = S_4 - nNewLength;
    m_bVect[nNewLong - 1] = (m_bVect[nNewLong - 1] >> nNewLength) << nNewLength;
  }
}

//--------------------------------------------------------- GetBufferSetLength(int nNewLength)
ULONG* CuBV::SetSize(int nNewLength, int nNewAllocLength/*-1*/)
{
  if (nNewLength == m_nBitLength) return m_bVect;  // 20.03.2006
  return m_bVect;
  ASSERT(nNewLength >= 0);
  int i, nLen =(nNewAllocLength > 0)? nNewAllocLength : nNewLength;
  ASSERT(nNewLength <= nLen);
  int nOldLong = m_nLongLength;
  GetBuffer(nLen);                      //nLen - in bits
  m_nLongLength = LEN_LONG(nNewLength);
  m_nBitLength = nNewLength;

  if (i=ADR_BITLONG(m_nBitLength)) {
    i = S_4 - i;
    m_bVect[m_nLongLength-1]=(m_bVect[m_nLongLength-1] >> i) << i;
  }
  if (nOldLong > m_nLongLength)
    memset(m_bVect+m_nLongLength, 0, (nOldLong - m_nLongLength)*sizeof(ULONG));
  return m_bVect;
}


/////////////////////////////////////////////////////////////////////////////////////////
//***************************************************************************************
//-------------------------------------------------------------------
void CuBV::AssignDiz(int nBitLength, const ULONG* v1, const ULONG* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ULONG[nLen];       // may throw an exception
    m_nLongLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else 
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nLongLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nLongLength; i++) m_bVect[i] = v1[i] | v2[i];
}

//-------------------------------------------------------------------
void CuBV::AssignDiz(int nBitLength, int Num, ULONG* v1, ...)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ULONG[nLen];       // may throw an exception
    m_nLongLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else 
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nLongLength = nLen;
      m_nBitLength = nBitLength;
    }

  ULONG * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_bVect[i] = v[0][i];
    for (int j=1; j< Num; j++) {
      m_bVect[i] |= v[j][i];     
    }
  }
}

//-------------------------------------------------------------------
void CuBV::AssignCon(int nBitLength, const ULONG* v1, const ULONG* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ULONG[nLen];       // may throw an exception
    m_nLongLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else 
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nLongLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nLongLength; i++) m_bVect[i] = v1[i] & v2[i];
}

//-------------------------------------------------------------------
void CuBV::AssignCon(int nBitLength, int Num, ULONG* v1, ...)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ULONG[nLen];       // may throw an exception
    m_nLongLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else 
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nLongLength = nLen;
      m_nBitLength = nBitLength;
    }

  ULONG* v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_bVect[i] = v[0][i];
    for (int j=1; j< Num; j++) {
      m_bVect[i] &= v[j][i];     
    }
  }
}

//-------------------------------------------------------------------
void CuBV::AssignXor(int nBitLength, const ULONG* v1, const ULONG* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ULONG[nLen];       // may throw an exception
    m_nLongLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else 
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nLongLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nLongLength; i++) m_bVect[i] = v1[i] ^ v2[i];
}

//-------------------------------------------------------------------
void CuBV::AssignXor(int nBitLength, int Num, ULONG* v1, ...)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ULONG[nLen];       // may throw an exception
    m_nLongLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else 
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nLongLength = nLen;
      m_nBitLength = nBitLength;
    }

  ULONG* v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_bVect[i] = v[0][i];
    for (int j=1; j< Num; j++) {
      m_bVect[i] ^= v[j][i];     
    }
  }
}

//-------------------------------------------------------------------
void CuBV::AssignDif(int nBitLength, const ULONG* v1, const ULONG* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ULONG[nLen];       // may throw an exception
    m_nLongLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else 
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nLongLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nLongLength; i++) m_bVect[i] = v1[i] & ~v2[i];
}
