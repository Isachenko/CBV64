/////////////////////////////////////////////////////////////////
// Ver.2.1.1    22.02.2007
//              AssignCopy - добавил чистку пространства от Len до AllocLen
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

///////////////////////////////////////////////////////////////
// Ver.2.0.5     19.06.2006
//           - Поправки BitChar с ограничением длины строки
// Ver.2.0.4     03.05.2006
//           - Поправки по последней версии CBV
// Ver.2.0.1     21.03.2006
//           - Генераторы случайных перестроены на возврат *this
//             GetRandVu - внеклассовыq метод генерирования 32 бит вектора
//             Поправка в SetSize - сразу возврат, если новый равен старому
// Ver.2.0.0     10.03.2006   Russian == English
//              - Добавлены генераторы случайных
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

static size_t Rgrain =1;
static BOOL NewRand = TRUE;

///////////////////////////////////////////////////////////
// Установить новое "зерно" для базового генератора      //
///////////////////////////////////////////////////////////

void SetRgrain(size_t NewRgrain ) {Rgrain = NewRgrain;}

///////////////////////////////////////////////////////////
// Взять текущее "зерно" базового генератора             //
///////////////////////////////////////////////////////////

size_t GetRgrain() {return (Rgrain);}

///////////////////////////////////////////////////////////
// Установить новый режим генерации                      //
///////////////////////////////////////////////////////////
void SetRandMode(BOOL Fl/* = TRUE*/) {NewRand = Fl; }

///////////////////////////////////////////////////////////
// Взять текущий режим генерации                         //
///////////////////////////////////////////////////////////
BOOL GetRandMode() { return NewRand; }

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

//---------------------------------------------------------GetRandVu
//  Получение псевдослучайного 32-разрядного булева вектора 
//  Rgrain - глобальная перем.= тек.сост.генератора
//  Rgrain = (Rgrain*(5**13))[mod(2**31-1)]
//    Vect - 32-разрядный базовый булев вектор 
//  *** Модификация Томашева - 27 января 1999
//---------------------------------------------------------



CuBV GetRandVu()
{
	size_t data = GetRandN();
	return *(new CuBV((BYTE *)&data, 64));

}

//********************************************************   GenRbv
// Генерация псевдослучайного <nCol>-компонентного булева //
// вектора с равновероятным распределением нулей и единиц //
//    Модификация Томашева - 27 января 1999
//---------------------------------------------------------
CuBV CuBV::GenRbv (size_t nCol)
{ 
  Empty();
  size_t a, b, hh, h, i, j, n; 
  a = 32;
  b=a/8;  hh=a-8;  
  n=(nCol/a)+((nCol%a)?1:0);
  size_t nn;
  m_nBitLength = n*a;
  m_nByteLength = m_nAllocLength = LEN_BYTE(n*a);
  m_bVect = new unsigned char[m_nByteLength];
  for (i=0; i<n-1; i++) 
  {
    nn=GetRandN(); 
    for(h=hh, j=0; j<b; j++, h-=8)
      m_bVect[b*i+j] =  (nn>>h & 255); 
  } 

  nn=(GetRandN()>> (a-nCol)) << (a-nCol); 
  for(h=hh, j=0; j<b; j++, h-=8)
    m_bVect[b*i+j] =  (nn>>h & 255); 
  
  	if (m_nBitLength != nCol)
	{
    m_nBitLength = nCol;
    m_nByteLength = LEN_BYTE(nCol);
  }
  return *this;
}

//*********************************** GenRbv ****************
// Ускоренное генерирование псевдослучайного <n>-компонентного 
// булева вектора с равновероятным распределением нулей и единиц 
//---------------------------------------------------------
CuBV CuBV::GenRbvN(int n)
{ int i, k;
  size_t *Syn;
  size_t c, d, m1;
  BYTE * pr;
  Empty();
  m_nBitLength = n;
  m_nByteLength = m_nAllocLength = LEN_BYTE(n);
  m_bVect = new unsigned char[m_nByteLength];
  k = LEN_LONG(n);
  Syn = (size_t *) (m_bVect);
  for(i=0; i<k-1; i++)
    Syn[i] = GetRandN();
  //m = S_4 - ADR_BITLONG(n);

  //Syn[i] = GetRandN() << m >> m;
  c = i * S_1;
  m1 = m_nByteLength - c;
	if (m1 == S_1)
		Syn[i] = GetRandN();
	else
	{
  d = GetRandN();
  pr = m_bVect + c;
	for(i=0; i < m1; i++){
		pr[i] = d << (i * S_1) >> (S_4-S_1);
	}
	}
	 if (ADR_BIT(m_nBitLength)) {
    i = S_1 - ADR_BIT(m_nBitLength);
    m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >> i) << i;
	 }
  return *this;  
}

//********************************************************//
// Генерация псевдослучайного <nCol>-компонентного булева   //
// вектора со средним числом <nRang> единиц (дисперсия 1/16)
// (при nRang>=nCol все компоненты вектора равны 1) 
//////////////////////////////////////////////////////////////

CuBV CuBV::GenRbvMid(int nCol, int nRang)
{
  if(nRang>=nCol)  { SetSize(nCol); One(); return *this; }
  CuBV  bv_Con (nCol, 1); 
  int d, e, i, j, log = 4; 
  CuBV Rbv(nCol,0), bv = Rbv, bvCon = Rbv;
// Формирование вектора prob, управляющего получением 
// среднего числа nRang единиц в векторе vbDis         
  unsigned int prob, half = 1 << (sizeof(int) * 8 - 1);
  prob = (unsigned int)((2.0 * nRang / nCol) * half);// вычисление доли nCol/nRang
  for (i = nCol; i; i >>= 1) log++;  // вычисление log(nCol)
  prob+= (half >> (log));            // округление prob до 1/16
 
// Формирование вектора.
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
// Генерация псевдослучайного <nCol>-компонентного булева //
// вектора с фиксированным числом <nRang> единиц 
// (при nRang>=nCol все компоненты вектора равны 1) 
////////////////////////////////////////////////////////////

CuBV CuBV::GenRbvFix (int nCol, int nRang)
{
  SetSize(nCol);
  if(nRang>=nCol) { One(); return *this; }
  Zero();
  size_t j; size_t k; 
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
    memcpy(dest.m_bVect, &m_bVect[nCopyIndex], nCopyLen*sizeof(ptrdiff_t));
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
        memset(m_bVect, BYTE_1, nLenLong*sizeof(ptrdiff_t));
        w = ADR_BITLONG(nLen);
        if (w) m_bVect[nLenLong-1] = (m_bVect[nLenLong-1] >> (BITS_IN_WORD - w)) << (BITS_IN_WORD - w);
      }
      else       memset(m_bVect, 0, nLenLong*sizeof(ptrdiff_t));
      m_nBitLength = nLen;
  }
}

//---------------------------------------------------- CuBV(const ptrdiff_t* pV, int nLenBit);
CuBV::CuBV(const ptrdiff_t* pV, int nLenBit)
{ 
  int nLen;
  if (nLenBit==0) Init();
  else   {
    nLen = LEN_LONG(nLenBit);
    AllocBuffer(nLen);
    memcpy(m_bVect, pV, nLen*sizeof(ptrdiff_t));
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
    m_bVect = new ptrdiff_t[nLenLong];       // may throw an exception
    m_nLongLength = m_nAllocLength = nLenLong;
    memset(m_bVect, 0, nLenLong*sizeof(ptrdiff_t));
  }
}

//----------------------------  void AssignCopy(int nLen, int nLenLong, const ptrdiff_t* pV)
void CuBV::AssignCopy(int nLen, int nLenLong, const ptrdiff_t* pV)
{ //--- check if it will fit
  if (nLenLong > m_nAllocLength) { // it won't fit, allocate another one
    Empty();
    AllocBuffer(nLenLong);
  }
  else   // 22.02.2007
   memset(m_bVect+nLenLong,0,(m_nAllocLength-nLenLong)*sizeof(ptrdiff_t));

  if (nLenLong != 0) memcpy(m_bVect, pV, nLenLong*sizeof(ptrdiff_t));
  m_nLongLength = nLenLong;
  m_nBitLength = nLen;
}

//------------------------------------------------------------- CharBit(const char* pch)
void CuBV::CharBit(const char* pch)
{ 
  int i, j, nLen = (int) strlen(pch);
  for (j=0; j < nLen; j++) {
    for (i=0; (i < BITS_IN_WORD) && *pch; i++) {
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
  else  memset(m_bVect, 0, nLenLong*sizeof(ptrdiff_t));

  CharBit(pch);
  m_nLongLength = nLenLong;
  m_nBitLength = nLen;
}

//-------------------------------- Extr(const ptrdiff_t* pV, int nLen, int nFirst, int nCount)
void CuBV::Extr(const ptrdiff_t* pV, int nLen, int nFirst, int nCount)
{ int i, j, nLenLong, l_bit, r_bit, AdrBeg;

  ASSERT(nCount >= 0);  ASSERT(nFirst >= 0);
  ASSERT((nCount + nFirst) <= nLen);
  nLenLong = LEN_LONG(nCount);
  AllocBuffer(nLenLong);
  m_nBitLength = nCount;
  if (nLen == nCount) 
   { memcpy(m_bVect, pV, nLenLong*sizeof(ptrdiff_t)); return; }
  AdrBeg = BIT_LONG(nFirst);
  memcpy(m_bVect, pV + AdrBeg, nLenLong*sizeof(ptrdiff_t));
  l_bit = ADR_BITLONG(nFirst);
  if (l_bit)    {
    r_bit = BITS_IN_WORD - l_bit;
    if (nLenLong == LEN_LONG(nLen)) j = nLenLong-1;
    else                            j = nLenLong;
    for (i=0; i<j; i++)
      m_bVect[i] = (pV[AdrBeg + i] << l_bit) | (pV[AdrBeg + i + 1] >> r_bit);
    if (j == nLenLong-1) m_bVect[j] = (pV[AdrBeg + j] << l_bit);
  }
  else memcpy(m_bVect, pV + AdrBeg, nLenLong*sizeof(ptrdiff_t));
  if (ADR_BITLONG(m_nBitLength)) {
    i = BITS_IN_WORD - ADR_BITLONG(m_nBitLength);
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
  memset(m_bVect, BYTE_1, m_nLongLength*sizeof(ptrdiff_t));
  if ((i = ADR_BITLONG(m_nBitLength)) > 0 ) i = BITS_IN_WORD - i;
  m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> i) << i;
}

//-------------------------------------------------------- Extract(int nFirst,int nCount)
CuBV CuBV::Extract(int nFirst, int nCount)
{ 
  CuBV s; s.Extr(m_bVect, m_nBitLength, nFirst, nCount);  return s;
}



/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Concatination //////////////////////////////////////
//----------------------------------------------------- Concat(const ptrdiff_t* pV, int nLen)
void CuBV::Concat(const ptrdiff_t* pV, int nLen)
{
  int NewBitLen, NewLongLen, LongLen, OldLong, r_bit, l_bit, i, j;
  ptrdiff_t *work;
  NewBitLen = m_nBitLength + nLen;
  if (NewBitLen == 0) { Init(); return;}
  NewLongLen = LEN_LONG(NewBitLen);
  OldLong = m_nLongLength;
  r_bit = ADR_BITLONG(m_nBitLength);
  if (m_nAllocLength < NewLongLen)  {
    if (m_nLongLength) {
      work = new ptrdiff_t[m_nLongLength];
      memcpy(work, m_bVect, m_nLongLength*sizeof(ptrdiff_t));
      Empty();
      AllocBuffer(NewLongLen);
      memcpy(m_bVect, work, OldLong*sizeof(ptrdiff_t));
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
    memcpy(m_bVect + OldLong, pV, LongLen*sizeof(ptrdiff_t));
    r_bit = BITS_IN_WORD - ADR_BITLONG(m_nBitLength);
    if (r_bit == BITS_IN_WORD) r_bit = 0;
    m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> r_bit) << r_bit;
    return;
  }
  l_bit = BITS_IN_WORD - r_bit; i = OldLong;
  m_bVect[i-1] |= pV[0] >> r_bit;
  for (j=0; j < LongLen - 1; j++)
    { m_bVect[i++] = (pV[j] << l_bit) | (pV[j+1] >> r_bit); }
  if (j*BITS_IN_WORD + l_bit < nLen)
    m_bVect[m_nLongLength-1] |= pV[LongLen-1] << l_bit;
  if (j = ADR_BITLONG(m_nBitLength)) {
    i = BITS_IN_WORD - j;
    m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> i) << i;
  }
}

//****************************** Protected function ***********************************//
//--------------------- Conc2(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
void CuBV::Conc2(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
{
  int NewBitLen, NewLongLen, LongLen, OldLong, r_bit, l_bit, i, j;

  NewBitLen = nLen1 + nLen2;
  if (NewBitLen == 0) { Init(); return;}
  NewLongLen = LEN_LONG(NewBitLen);
  if (m_nAllocLength < NewLongLen)  { Empty(); AllocBuffer(NewLongLen); }
  else { m_nLongLength = NewLongLen;  m_bVect[m_nLongLength-1] = 0; }

  m_nBitLength = NewBitLen;
  OldLong = LEN_LONG(nLen1);
  memcpy(m_bVect, pV1, OldLong*sizeof(ptrdiff_t));
  r_bit = ADR_BITLONG(nLen1);
  LongLen = LEN_LONG(nLen2);
  if (r_bit == 0)  { memcpy(m_bVect + OldLong, pV2, LongLen*sizeof(ptrdiff_t)); return;}
  l_bit = BITS_IN_WORD - r_bit; i = OldLong;
  m_bVect[OldLong-1] |= pV2[0] >> r_bit;
  for (j=0; j < LongLen-1; j++)
    { m_bVect[i++] = (pV2[j] << l_bit) | (pV2[j+1] >> r_bit); }
  if (j*BITS_IN_WORD + l_bit < nLen2)
    m_bVect[m_nLongLength-1] |= pV2[LongLen-1] << l_bit;
  if (j=ADR_BITLONG(m_nBitLength)) {
    i = BITS_IN_WORD - j;
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
  if (nNewLenBit==0)  { memset(m_bVect,0,m_nLongLength*sizeof(ptrdiff_t)); return; }
  nNewLenLong = LEN_LONG(nNewLenBit);
  AdrBeg = BIT_LONG(nShift);
  l_bit = ADR_BITLONG(nShift);
  if (l_bit)    {
    r_bit = BITS_IN_WORD - l_bit;  j=0;
    for (i = AdrBeg; i < m_nLongLength-1; i++)
      m_bVect[j++] = (m_bVect[i] << l_bit) | (m_bVect[i+1] >> r_bit);
    if ((j*BITS_IN_WORD + nShift) < m_nBitLength)
      m_bVect[nNewLenLong-1] <<=  l_bit;
  }
  else   memcpy(m_bVect, m_bVect + AdrBeg, nNewLenLong*sizeof(ptrdiff_t));
  i= m_nLongLength - nNewLenLong;
  memset(m_bVect + nNewLenLong, 0, i*sizeof(ptrdiff_t));
}

//--------------------------------------------------------- RightShiftInPlace(int nShift)
void CuBV::RightShiftInPlace(int nShift)
{  // RightShift bool vector  -- assume that 'this' is a new CuBV object
  int i, j, l_bit, r_bit, AdrBeg;
  ptrdiff_t *work;
  if (nShift <= 0) return;
  ASSERT((m_nBitLength - nShift) >= 0);
  if ((m_nBitLength - nShift)==0)  
   { memset(m_bVect, 0, m_nLongLength*sizeof(ptrdiff_t)); return; }
  work = new ptrdiff_t[m_nLongLength];       // may throw an exception
  memset(work, 0, m_nLongLength*sizeof(ptrdiff_t));
  AdrBeg = BIT_LONG(nShift);
  r_bit = ADR_BITLONG(nShift);
  if (r_bit)    {
    l_bit = BITS_IN_WORD - r_bit;  j=0;
    work[AdrBeg] = (m_bVect[0] >> r_bit);
    for (i = AdrBeg + 1; i < m_nLongLength; i++)
      { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
  }
  else memcpy(work + AdrBeg, m_bVect, (m_nLongLength - AdrBeg)*sizeof(ptrdiff_t));
  if (i=ADR_BITLONG(m_nBitLength))
    work[m_nLongLength-1] = (work[m_nLongLength-1] >> (BITS_IN_WORD-i)) << (BITS_IN_WORD-i);
  memcpy(m_bVect, work, m_nLongLength*sizeof(ptrdiff_t));
  delete [] work;
}

//--------------------------------------------- DizInPlace(const ptrdiff_t* pV, int nLen)
void CuBV::DizInPlace(const ptrdiff_t* pV, int nLen)
{
  ASSERT(nLen >= 0);  ASSERT(nLen == m_nBitLength);
  for (int i = 0; i < m_nLongLength; i++) m_bVect[i] |= pV[i];
}

//--------------------------------------------- ConInPlace(const ptrdiff_t* pV, int nLen)
void CuBV::ConInPlace(const ptrdiff_t* pV, int nLen)
{
  ASSERT(nLen >= 0); ASSERT(nLen == m_nBitLength);
  for (int i=0; i < m_nLongLength; i++) m_bVect[i] &= pV[i];
}

//-------------------------------------------- Add2InPlace(const ptrdiff_t* pV, int nLen)
void CuBV::Add2InPlace(const ptrdiff_t* pV, int nLen)
{
  ASSERT(nLen >= 0); ASSERT(nLen == m_nBitLength);
  for (int i=0; i < m_nLongLength; i++) m_bVect[i] ^= pV[i];
}

//------------------------------------------ ConNotInPlace(const ptrdiff_t* pV, int nLen)
void CuBV::ConNotInPlace(const ptrdiff_t* pV, int nLen)
{
  int i;
  ASSERT(nLen >= 0);  ASSERT(nLen == m_nBitLength);
  for (i=0; i < m_nLongLength; i++) m_bVect[i] &= ~pV[i];
  i = ADR_BITLONG(nLen);
  if (i)  m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> (BITS_IN_WORD-i)) << (BITS_IN_WORD-i);
}
//*************************************************************************************//

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Logic operations ////////////////////////////////////

//****************************** Protected function ***********************************//
//---------------------------------- LeftShiftVect(const ptrdiff_t* pV, int nLen, int nShift)
void CuBV::LeftShiftVect(const ptrdiff_t* pV, int nLen, int nShift)
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
  if (nShift==0)      { memcpy(m_bVect, pV, nSrcLong*sizeof(ptrdiff_t)); return; }
  AdrBeg = BIT_LONG(nShift);
  l_bit = ADR_BITLONG(nShift);
  if (l_bit)    {
    r_bit = BITS_IN_WORD - l_bit;  j=0;
    for (i = AdrBeg; i < nSrcLong-1; i++)
      m_bVect[j++] = (pV[i] << l_bit) | (pV[i+1] >> r_bit);
    if ((j*BITS_IN_WORD + nShift) < m_nBitLength)
      m_bVect[nNewLenLong-1] = pV[nSrcLong - 1] << l_bit;
  }
  else   memcpy(m_bVect, pV + AdrBeg, nNewLenLong*sizeof(ptrdiff_t));
  i = nSrcLong - nNewLenLong;
  memset(m_bVect + nNewLenLong, 0, i*sizeof(ptrdiff_t));
}

//-------------------------- RightShiftVect(const ptrdiff_t* pV, int nLen, int nShift)
void CuBV::RightShiftVect(const ptrdiff_t* pV, int nLen, int nShift)
{
  int i, j, l_bit, r_bit, nSrcLong, AdrBeg;
  ASSERT(nShift >= 0);  ASSERT((nLen - nShift) >= 0);
  nSrcLong = LEN_LONG(nLen);
  AllocBuffer(nSrcLong);
  m_nBitLength = nLen;
  if ((nLen - nShift) == 0) return;  //?????
  if (nShift==0)      { memcpy(m_bVect, pV, nSrcLong*sizeof(ptrdiff_t)); return; }
  AdrBeg = BIT_LONG(nShift);
  r_bit = ADR_BITLONG(nShift);
  if (r_bit)    {
    l_bit = BITS_IN_WORD - r_bit;  j=0;
    m_bVect[AdrBeg] = (pV[0] >> r_bit);
    for (i = AdrBeg + 1; i < nSrcLong; i++)
     { m_bVect[i] = (pV[j] << l_bit) | (pV[j+1] >> r_bit); j++; }
  }
  else memcpy(m_bVect + AdrBeg, pV, (nSrcLong - AdrBeg)*sizeof(ptrdiff_t));
  if (i=ADR_BITLONG(m_nBitLength))
    m_bVect[nSrcLong-1] = (m_bVect[nSrcLong-1] >> (BITS_IN_WORD - i)) << (BITS_IN_WORD - i);
}

//---------------------- Diz(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
void CuBV::Diz(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2); ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] | pV2[i];
}

//---------------------- Con(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
void CuBV::Con(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2);  ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] & pV2[i];
}

//--------------------- Add2(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
void CuBV::Add2(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2); ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] ^ pV2[i];
}

//------------------- ConNot(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
void CuBV::ConNot(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2)
{
  int i, LenLong;
  ASSERT(nLen1 == nLen2);  ASSERT(nLen1 >= 0);
  LenLong = LEN_LONG(nLen1);
  AllocBuffer(LenLong);
  m_nBitLength = nLen1;
  for (i=0; i < LenLong; i++) m_bVect[i] = pV1[i] & ~pV2[i];
}

//---------------------------------------------------- Not(const ptrdiff_t* pV, int nLen)
void CuBV::Not(const ptrdiff_t* pV, int nLen)
{
  int i, LenLong;
  ASSERT(nLen >= 0);
  LenLong = LEN_LONG(nLen);
  AllocBuffer(LenLong);
  m_nBitLength = nLen;
  for (i=0; i < LenLong; i++) m_bVect[i] = ~pV[i];
  i = ADR_BITLONG(nLen);
  if (i)  m_bVect[LenLong-1] = (m_bVect[LenLong-1] >> (BITS_IN_WORD-i)) << (BITS_IN_WORD-i);
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

//-------------------------------- operator |(const CuBV& bv, const ptrdiff_t* pV)
CuBV operator |(const CuBV& bv, const ptrdiff_t* pV)
{
  CuBV s;
  s.Diz(bv.m_bVect, bv.m_nBitLength, pV, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator |(const CuBV& bv, const ptrdiff_t* pV)
CuBV operator |(const ptrdiff_t* pV, const CuBV& bv)
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

//-------------------------------- operator &(const ptrdiff_t* pV, const CuBV& bv)
CuBV operator &(const ptrdiff_t* pV, const CuBV& bv)
{
  CuBV s;
  s.Con(pV, bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator &(const CuBV& bv, const ptrdiff_t* pV)
CuBV operator &(const CuBV& bv, const ptrdiff_t* pV)
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

//-------------------------------- operator ^(const ptrdiff_t* pV, const CuBV& bv)
CuBV operator ^(const ptrdiff_t* pV, const CuBV& bv)
{
  CuBV s;
  s.Add2(pV, bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator ^(const CuBV& bv, const ptrdiff_t* pV)
CuBV operator ^(const CuBV& bv, const ptrdiff_t* pV)
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

//-------------------------------- operator -(const ptrdiff_t* pV, const CuBV& bv)
CuBV operator -(const ptrdiff_t* pV, const CuBV& bv)
{
  CuBV s;
  s.ConNot(pV, bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------- operator -(const CuBV& bv, const ptrdiff_t* pV)
CuBV operator -(const CuBV& bv, const ptrdiff_t* pV)
{
  CuBV s;
  s.ConNot(bv.m_bVect, bv.m_nBitLength, pV, bv.m_nBitLength);
  return s;
}

//-------------------------------------------------- operator ~(const CuBV& bv)
CuBV operator ~(const CuBV& bv)
{ CuBV s; s.Not(bv.m_bVect, bv.m_nBitLength); return s; }

//-------------------------------------------------- Invert(const ptrdiff_t* pV)
void CuBV:: Invert(const ptrdiff_t* pV)
{ int i;
  for (i=0; i < m_nLongLength; i++) m_bVect[i] = ~pV[i];
  i = ADR_BITLONG(m_nBitLength);
  if (i)  m_bVect[m_nLongLength-1] = (m_bVect[m_nLongLength-1] >> (BITS_IN_WORD-i)) << (BITS_IN_WORD-i);
}

//------------------------------------------------------------- LoopLeftShift(int nShift)
void CuBV::LoopLeftShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CuBV object
  int i, j, l_bit, r_bit, nNewLenLong, nNewLenBit, nEndLenLong;
  ptrdiff_t *work;
  if (m_nLongLength==0 || nShift==0) return;
  ASSERT(nShift > 0);
  nNewLenBit = m_nBitLength - nShift;
  
  ASSERT(nNewLenBit > 0);
  nNewLenLong = LEN_LONG(nNewLenBit);
  
  work = new ptrdiff_t[m_nLongLength+1];       // may throw an exception
  memset(work, 0, m_nLongLength*sizeof(ptrdiff_t));
  i = BIT_LONG(nShift);
  l_bit = ADR_BITLONG(nShift);
  if (l_bit)    {
    r_bit = BITS_IN_WORD - l_bit;
    memcpy(work, m_bVect + i, (nNewLenLong + 1)*sizeof(ptrdiff_t));
    for (i=0; i<nNewLenLong; i++)
      work[i] = (work[i] << l_bit) | (work[i+1] >> r_bit);
    work[i] <<= l_bit;
  }
  else   memcpy(work, m_bVect + i, nNewLenLong*sizeof(ptrdiff_t));
  nEndLenLong = m_nLongLength - nNewLenLong;
  j = BIT_LONG(nNewLenBit);
  r_bit = ADR_BITLONG(nNewLenBit);
  if (r_bit == 0 )    { memcpy(work+j, m_bVect, nEndLenLong*sizeof(ptrdiff_t)); }
  else {
    l_bit = BITS_IN_WORD - r_bit;
    work[j] = (work[j] >> l_bit) << l_bit;
    work[j++] |= (m_bVect[0] >> r_bit);
    for (i=0; i<nEndLenLong; i++)
      work[j++] = (m_bVect[i] << l_bit) | (m_bVect[i+1] >> r_bit);
    j = BITS_IN_WORD - ADR_BITLONG(m_nBitLength);
    work[m_nLongLength-1] = (work[m_nLongLength-1] >> j)<<j;
  }
  memcpy(m_bVect, work, m_nLongLength*sizeof(ptrdiff_t));
  delete [] work;
}

//------------------------------------------------------------ LoopRightShift(int nShift)
void CuBV::LoopRightShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CuBV object
  int i, j, l_bit, r_bit, AdrBeg, AdrEnd, BitEnd;
  ptrdiff_t *work;
  if (nShift == 0) return;
  ASSERT(nShift > 0); ASSERT((m_nBitLength - nShift) >= 0);

  if ((m_nBitLength - nShift)==0)  
    { memset(m_bVect, 0, m_nLongLength*sizeof(ptrdiff_t)); return; }
  work = new ptrdiff_t[m_nLongLength];       // may throw an exception
  memset(work, 0, m_nLongLength*sizeof(ptrdiff_t));
  AdrBeg = BIT_LONG(nShift);
  AdrEnd = BIT_LONG(m_nBitLength - nShift);
  BitEnd = ADR_BITLONG(m_nBitLength);
  if (BitEnd < nShift) {
    l_bit = ADR_BITLONG(m_nBitLength - nShift);
    r_bit = BITS_IN_WORD - l_bit;     j = AdrEnd;
    for (i=0; i < AdrBeg + 1; i++)
      { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
  }
  else { l_bit = BitEnd - nShift; work[0] = m_bVect[AdrEnd] << l_bit; }
  r_bit = ADR_BITLONG(nShift); l_bit = BITS_IN_WORD - r_bit;
  work[AdrBeg] = (work[AdrBeg] >> l_bit) << l_bit;
  if (r_bit)    {
    j=0;
    work[AdrBeg] |= (m_bVect[0] >> r_bit);
    for (i=AdrBeg+1; i<m_nLongLength; i++)
      { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
  }
  else memcpy(work + AdrBeg, m_bVect, (m_nLongLength - AdrBeg)*sizeof(ptrdiff_t));
  if (i = ADR_BITLONG(m_nBitLength))
    work[m_nLongLength-1] = (work[m_nLongLength-1] >> (BITS_IN_WORD-i)) << (BITS_IN_WORD-i);
  memcpy(m_bVect, work, m_nLongLength*sizeof(ptrdiff_t));
  delete [] work;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// Operation of weighting,finding and casing ///////////////////////
//---------------------------------------------------------------------------- CountBit()
int CuBV::CountBit() const
{
  BYTE* pB = (BYTE*)m_bVect;
  int one=0, j, w = m_nLongLength<<3; 
  for (j=0; j < w; j++) one += TabC[pB[j]];
  return one;
}

//--------------------------------------------------------------- LeftOneLong(int nNext)
int CuBV::LeftOneLong(int nNext) const
{
  int i, j, pos = 0;
  ptrdiff_t val;
  ASSERT((m_nBitLength - nNext) > 0);
  if (nNext == -1) { i=0; j=0; val = m_bVect[i]; }
  else {
    i = BIT_LONG(nNext);  j=ADR_BITLONG(nNext); j++; 
    if (j == BITS_IN_WORD)  {
      j = 0;  i++; 
      if (i == m_nLongLength)  return (-1);
      val = m_bVect[i];
    }
    else
      val = m_bVect[i] << j;
  }
  if (val)  {
m0: if (val & 0xff00000000000000)  {
m3:   while (!(val & OB4[pos++]));
      return (nNext + pos);
    }
    else {
      pos +=8;
      if (val & 0x00ff000000000000) goto m3;
      pos +=8;
      if (val & 0x0000ff0000000000) goto m3;
      pos +=8;
      if (val & 0x000000ff00000000) goto m3;
      pos +=8;
      if (val & 0x00000000ff000000) goto m3;
      pos +=8;
      if (val & 0x0000000000ff0000) goto m3;
      pos +=8;
      if (val & 0x000000000000ff00) goto m3;
      pos +=8;
      goto m3;
    }
  }
  for (i++; i < m_nLongLength; i++)
    if (m_bVect[i])  {
      val = m_bVect[i];
      nNext = (i<<6) - 1;
      goto m0;
    }
  return (-1);
}

//-------------------------------------------------------------------- LeftOne(int nNext)
int CuBV::LeftOne(int nNext) const
{
  int offset, bit, pos = 0;
  ptrdiff_t val;
  ASSERT((m_nBitLength - nNext) > 0);
  if (m_nLongLength==0) return -1;
 
  if (nNext == -1) { offset=0; bit=0; val = m_bVect[0];}
  else {
    offset = BIT_LONG(nNext);  
    bit = ADR_BITLONG(nNext) + 1; 
    if (bit == BITS_IN_WORD)  
      { bit = 0;  offset++; val = m_bVect[offset]; }
    else 
      val = m_bVect[offset] << bit;
    if (offset == m_nLongLength)  return (-1);
  }
  if (val!=0) {
m0: if (val & 0xffffffff00000000)
      if (val & 0xffff000000000000)
        if (val & 0xff00000000000000)
          if (val & 0xf000000000000000)
            if (val & 0xc000000000000000)
              if (val & 0x8000000000000000)   return ((offset<<6)+bit);
              else                            return ((offset<<6)+bit+1);
          else
              if (val & 0x2000000000000000)   return ((offset<<6)+bit+2);
              else                            return ((offset<<6)+bit+3);
        else
            if (val & 0x0c00000000000000)
              if (val & 0x0800000000000000)   return ((offset<<6)+bit+4);
              else                            return ((offset<<6)+bit+5);
          else
              if (val & 0x0200000000000000)   return ((offset<<6)+bit+6);
              else                            return ((offset<<6)+bit+7);
      else
          if (val & 0x00f0000000000000)
            if (val & 0x00c0000000000000)
              if (val & 0x0080000000000000)   return ((offset<<6)+bit+8);
              else                            return ((offset<<6)+bit+9);
          else
              if (val & 0x0020000000000000)   return ((offset<<6)+bit+10);
              else                            return ((offset<<6)+bit+11);
        else
            if (val & 0x000c000000000000)
              if (val & 0x0008000000000000)   return ((offset<<6)+bit+12);
              else                            return ((offset<<6)+bit+13);
          else
              if (val & 0x0002000000000000)   return ((offset<<6)+bit+14);
              else                            return ((offset<<6)+bit+15);
    else
        if (val & 0x0000ff0000000000)
          if (val & 0x0000f00000000000)
            if (val & 0x0000c00000000000)
              if (val & 0x0000800000000000)   return ((offset<<6)+bit+16);
              else                            return ((offset<<6)+bit+17);
          else
              if (val & 0x0000200000000000)   return ((offset<<6)+bit+18);
              else                            return ((offset<<6)+bit+19);
        else
            if (val & 0x00000c0000000000) 
              if (val & 0x0000080000000000)   return ((offset<<6)+bit+20);
              else                            return ((offset<<6)+bit+21);
          else
              if (val & 0x0000020000000000)   return ((offset<<6)+bit+22);
              else                            return ((offset<<6)+bit+23);
      else 
          if (val & 0x000000f000000000)
            if (val & 0x000000c000000000)
              if (val & 0x0000008000000000)   return ((offset<<6)+bit+24);
              else                            return ((offset<<6)+bit+25);
          else
              if (val & 0x0000002000000000)   return ((offset<<6)+bit+26);
              else                            return ((offset<<6)+bit+27);
        else 
            if (val & 0x0000000c00000000) 
              if (val & 0x0000000800000000)   return ((offset<<6)+bit+28);
              else                            return ((offset<<6)+bit+29);
          else
              if (val & 0x0000000200000000)   return ((offset<<6)+bit+30);
              else                            return ((offset<<6)+bit+31);
    else
      if (val & 0x00000000ffff0000)
        if (val & 0x00000000ff000000)
          if (val & 0x00000000f0000000)
            if (val & 0x00000000c0000000)
              if (val & 0x0000000080000000)   return ((offset<<6)+bit+32);
              else                            return ((offset<<6)+bit+33);
            else
              if (val & 0x0000000020000000)   return ((offset<<6)+bit+34);
              else                            return ((offset<<6)+bit+35);
          else
            if (val & 0x000000000c000000)
              if (val & 0x0000000008000000)   return ((offset<<6)+bit+36);
              else                            return ((offset<<6)+bit+37);
            else
              if (val & 0x0000000002000000)   return ((offset<<6)+bit+38);
              else                            return ((offset<<6)+bit+39);
        else
          if (val & 0x0000000000f00000)
            if (val & 0x0000000000c00000)
              if (val & 0x0000000000800000)   return ((offset<<6)+bit+40);
              else                            return ((offset<<6)+bit+41);
            else
              if (val & 0x0000000000200000)   return ((offset<<6)+bit+42);
              else                            return ((offset<<6)+bit+43);
          else
            if (val & 0x00000000000c0000)
              if (val & 0x0000000000080000)   return ((offset<<6)+bit+44);
              else                            return ((offset<<6)+bit+45);
            else
              if (val & 0x0000000000020000)   return ((offset<<6)+bit+46);
              else                            return ((offset<<6)+bit+47);
      else
        if (val & 0x000000000000ff00)
          if (val & 0x000000000000f000)
            if (val & 0x000000000000c000)
              if (val & 0x0000000000008000)   return ((offset<<6)+bit+48);
              else                            return ((offset<<6)+bit+49);
            else
              if (val & 0x0000000000002000)   return ((offset<<6)+bit+50);
              else                            return ((offset<<6)+bit+51);
          else
            if (val & 0x0000000000000c00) 
              if (val & 0x0000000000000800)   return ((offset<<6)+bit+52);
              else                            return ((offset<<6)+bit+53);
            else
              if (val & 0x0000000000000200)   return ((offset<<6)+bit+54);
              else                            return ((offset<<6)+bit+55);
        else 
          if (val & 0x00000000000000f0)
            if (val & 0x00000000000000c0)
              if (val & 0x0000000000000080)   return ((offset<<6)+bit+56);
              else                            return ((offset<<6)+bit+57);
            else
              if (val & 0x0000000000000020)   return ((offset<<6)+bit+58);
              else                            return ((offset<<6)+bit+59);
          else 
            if (val & 0x000000000000000c) 
              if (val & 0x0000000000000008)   return ((offset<<6)+bit+60);
              else                            return ((offset<<6)+bit+61);
            else
              if (val & 0x0000000000000002)   return ((offset<<6)+bit+62);
              else                            return ((offset<<6) + 63);
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
  int i, j, pos = BITS_IN_WORD - 1;
  ptrdiff_t val;
  ASSERT((m_nBitLength - nPrev) >= 0);
  if (m_nLongLength==0) return -1;
  if (nPrev == -1) { i = m_nLongLength - 1; j=0;  val = m_bVect[i];}
  else {
    i = BIT_LONG(nPrev);  
    j = BITS_IN_WORD - ADR_BITLONG(nPrev); 
    if (j == BITS_IN_WORD)  { j = 0;  i--; val = m_bVect[i]; }
    else val = (m_bVect[i] >> j) << j;
  }
  if (val)  {
m0: if (val & 0x00000000000000ff)  {
m3:    while (!(val & OB4[pos--]));
      return ((i*BITS_IN_WORD)+pos+1);
    }
    else {
      pos -= 8;
      if (val & 0x000000000000ff00)  goto m3;
      pos -= 8;
      if (val & 0x0000000000ff0000)  goto m3;
      pos -= 8;
      if (val & 0x00000000ff000000)  goto m3;
      pos -= 8;
      if (val & 0x000000ff00000000)  goto m3;
      pos -= 8;
      if (val & 0x0000ff0000000000)  goto m3;
      pos -= 8;
      if (val & 0x00ff000000000000)  goto m3;
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
  int offset, j, pos = BITS_IN_WORD - 1;
  ptrdiff_t val;
  if (m_nLongLength==0) return -1;
  ASSERT((m_nBitLength - nPrev) >= 0);
  if (nPrev == -1) { offset = m_nLongLength - 1; val = m_bVect[offset];}
  else {
    offset = BIT_LONG(nPrev);  
    j = BITS_IN_WORD - ADR_BITLONG(nPrev); 
    if (j == BITS_IN_WORD)  { j = 0;  offset--; val = m_bVect[offset]; }
    else val = (m_bVect[offset] >> j) << j;
  }
  if (val!=0) {
m0: if (val & 0x00000000ffffffff)
      if (val & 0x000000000000ffff)
        if (val & 0x00000000000000ff)
          if (val & 0x000000000000000f)  
            if (val & 0x0000000000000003) 
              if (val & 0x0000000000000001) { val&=0xfffffffffffffffe; return ((offset<<6)+63); }
              else                          { val&=0xfffffffffffffffc; return ((offset<<6)+62); }
          else
              if (val & 0x0000000000000004) { val&=0xfffffffffffffff8; return ((offset<<6)+61); }
              else                          { val&=0xfffffffffffffff0; return ((offset<<6)+60); }
        else
            if (val & 0x0000000000000030) 
              if (val & 0x0000000000000010) { val&=0xffffffffffffffe0; return ((offset<<6)+59); }
              else                          { val&=0xffffffffffffffc0; return ((offset<<6)+58); }
          else
              if (val & 0x0000000000000040) { val&=0xffffffffffffff80; return ((offset<<6)+57); }
              else                          { val&=0xffffffffffffff00; return ((offset<<6)+56); }
      else
          if (val & 0x0000000000000f00)  
            if (val & 0x0000000000000300) 
              if (val & 0x0000000000000100) { val&=0xfffffffffffffe00; return ((offset<<6)+55); }
              else                          { val&=0xfffffffffffffc00; return ((offset<<6)+54); }
          else
              if (val & 0x0000000000000400) { val&=0xfffffffffffff800; return ((offset<<6)+53); }
              else                          { val&=0xfffffffffffff000; return ((offset<<6)+52); }
        else
            if (val & 0x0000000000003000) 
              if (val & 0x0000000000001000) { val&=0xffffffffffffe000; return ((offset<<6)+51); }
              else                          { val&=0xffffffffffffc000; return ((offset<<6)+50); }
          else
              if (val & 0x0000000000004000) { val&=0xffffffffffff8000; return ((offset<<6)+49); }
              else                          { val&=0xffffffffffff0000; return ((offset<<6)+48); }
      else
        if (val & 0x0000000000ff0000)
          if (val & 0x00000000000f0000)  
            if (val & 0x0000000000030000) 
              if (val & 0x0000000000010000) { val&=0xfffffffffffe0000; return ((offset<<6)+47); }
              else                          { val&=0xfffffffffffc0000; return ((offset<<6)+46); }
            else
              if (val & 0x0000000000040000) { val&=0xfffffffffff80000; return ((offset<<6)+45); }
              else                          { val&=0xfffffffffff00000; return ((offset<<6)+44); }
          else
            if (val & 0x0000000000300000) 
              if (val & 0x0000000000100000) { val&=0xffffffffffe00000; return ((offset<<6)+43); }
              else                          { val&=0xffffffffffc00000; return ((offset<<6)+42); }
            else
              if (val & 0x0000000000400000) { val&=0xffffffffff800000; return ((offset<<6)+41); }
              else                          { val&=0xffffffffff000000; return ((offset<<6)+40); }
        else
          if (val & 0x000000000f000000)  
            if (val & 0x0000000003000000) 
              if (val & 0x0000000001000000) { val&=0xfffffffffe000000; return ((offset<<6)+39); }
              else                          { val&=0xfffffffffc000000; return ((offset<<6)+38); }
            else
              if (val & 0x0000000004000000) { val&=0xfffffffff8000000; return ((offset<<6)+37); }
              else                          { val&=0xfffffffff0000000; return ((offset<<6)+36); }
          else
            if (val & 0x0000000030000000) 
              if (val & 0x0000000010000000) { val&=0xffffffffe0000000; return ((offset<<6)+35); }
              else                          { val&=0xffffffffc0000000; return ((offset<<6)+34); }
            else
              if (val & 0x0000000040000000) { val&=0xffffffff80000000; return ((offset<<6)+33); }
              else                          { val&=0xffffffff00000000; return ((offset<<6)+32); }
    else
      if (val & 0x0000ffff00000000)
        if (val & 0x000000ff00000000)
          if (val & 0x0000000f00000000)  
            if (val & 0x0000000300000000) 
              if (val & 0x0000000100000000) { val&=0xfffffffe00000000; return ((offset<<6)+31); }
              else                          { val&=0xfffffffc00000000; return ((offset<<6)+30); }
            else
              if (val & 0x0000000400000000) { val&=0xfffffff800000000; return ((offset<<6)+29); }
              else                          { val&=0xfffffff000000000; return ((offset<<6)+28); }
          else
            if (val & 0x0000003000000000) 
              if (val & 0x0000001000000000) { val&=0xffffffe000000000; return ((offset<<6)+27); }
              else                          { val&=0xffffffc000000000; return ((offset<<6)+26); }
            else
              if (val & 0x0000004000000000) { val&=0xffffff8000000000; return ((offset<<6)+25); }
              else                          { val&=0xffffff0000000000; return ((offset<<6)+24); }
        else
          if (val & 0x00000f0000000000)
            if (val & 0x0000030000000000) 
              if (val & 0x0000010000000000) { val&=0xfffffe0000000000; return ((offset<<6)+23); }
              else                          { val&=0xfffffc0000000000; return ((offset<<6)+22); }
            else
              if (val & 0x0000040000000000) { val&=0xfffff80000000000; return ((offset<<6)+21); }
              else                          { val&=0xfffff00000000000; return ((offset<<6)+20); }
          else
            if (val & 0x0000300000000000) 
              if (val & 0x0000100000000000) { val&=0xffffe00000000000; return ((offset<<6)+19); }
              else                          { val&=0xffffc00000000000; return ((offset<<6)+18); }
            else
              if (val & 0x0000400000000000) { val&=0xffff800000000000; return ((offset<<6)+17); }
              else                          { val&=0xffff000000000000; return ((offset<<6)+16); }
    else
        if (val & 0x00ff000000000000)
          if (val & 0x000f000000000000)  
            if (val & 0x0003000000000000) 
              if (val & 0x0001000000000000) { val&=0xfffe000000000000; return ((offset<<6)+15); }
              else                          { val&=0xfffc000000000000; return ((offset<<6)+14); }
          else
              if (val & 0x0004000000000000) { val&=0xfff8000000000000; return ((offset<<6)+13); }
              else                          { val&=0xfff0000000000000; return ((offset<<6)+12); }
        else
            if (val & 0x0030000000000000) 
              if (val & 0x0010000000000000) { val&=0xffe0000000000000; return ((offset<<6)+11); }
              else                          { val&=0xffc0000000000000; return ((offset<<6)+10); }
          else
              if (val & 0x0040000000000000) { val&=0xff80000000000000; return ((offset<<6)+9); }
              else                          { val&=0xff00000000000000; return ((offset<<6)+8); }
      else
          if (val & 0x0f00000000000000)  
            if (val & 0x0300000000000000) 
              if (val & 0x0100000000000000) { val&=0xfe00000000000000; return ((offset<<6)+7); }
              else                          { val&=0xfc00000000000000; return ((offset<<6)+6); }
          else
              if (val & 0x0400000000000000) { val&=0xf800000000000000; return ((offset<<6)+5); }
              else                          { val&=0xf000000000000000; return ((offset<<6)+4); }
        else
            if (val & 0x3000000000000000) 
              if (val & 0x1000000000000000) { val&=0xe000000000000000; return ((offset<<6)+3); }
              else                          { val&=0xc000000000000000; return ((offset<<6)+2); }
          else
              if (val & 0x4000000000000000) { val&=0x8000000000000000; return ((offset<<6)+1); }
              else                          { val&=0; return (offset<<6); }
  }
  for (offset--; offset >= 0; offset--)
    if(m_bVect[offset])  { val = m_bVect[offset]; goto m0; }
  return (-1);
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Comparing operations ///////////////////////////////////////

//****************************** Protected function ***********************************//
//--------------------------------------------- Equality(const ptrdiff_t* pV, int nLen) const
BOOL CuBV::Equality(const ptrdiff_t* pV, int nLen) const
{
  if (pV == NULL && m_bVect) return FALSE;
  if (m_nBitLength != nLen)  return FALSE;
  for (int i=0; i < m_nLongLength; i++)
    if (m_bVect[i] != pV[i]) return FALSE;
  return TRUE;
}

//-------------------------------------- Pogl(const ptrdiff_t* pV, int nLen, BOOL Dist) const
BOOL CuBV::Pogl(const ptrdiff_t* pV, int nLen, BOOL Dist) const
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

//------------------------------------- PoglEq(const ptrdiff_t* pV, int nLen, BOOL Dist) const
BOOL CuBV::PoglEq(const ptrdiff_t* pV, int nLen, BOOL Dist) const
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
  int i; ptrdiff_t LastLong;
  if (m_nLongLength == 0) return FALSE;
  for (i=0; i < m_nLongLength-1; i++)
    { LastLong = ~m_bVect[i]; if (LastLong) return FALSE; }
  i = BITS_IN_WORD - ADR_BITLONG(m_nBitLength);
  if (i != BITS_IN_WORD) LastLong = (~m_bVect[m_nLongLength-1] >> i) << i;
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
  ar.Write(bv.m_bVect, bv.m_nLongLength*sizeof(ptrdiff_t));
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
    if (ar.Read(bv.m_bVect, bv.m_nLongLength*sizeof(ptrdiff_t)) != bv.m_nLongLength*sizeof(ptrdiff_t))
      AfxThrowArchiveException(CArchiveException::endOfFile);
  }
  return ar;
}
#endif //not _LINUX

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Advanced dump of memmory /////////////////////////////////
//---------------------------------------------------------- GetBuffer(int nMinBufLength)

ptrdiff_t* CuBV::GetBuffer(int nMinBufLength)
{
  ASSERT(nMinBufLength >= 0);
  if (nMinBufLength > m_nBitLength) {
    ptrdiff_t* pbtOldData = m_bVect;
    int nOldLen = m_nLongLength, nOldBit = m_nBitLength;
    AllocBuffer(LEN_LONG(nMinBufLength));
    memcpy(m_bVect, pbtOldData, nOldLen*sizeof(ptrdiff_t));
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
    nNewLength = BITS_IN_WORD - nNewLength;
    m_bVect[nNewLong - 1] = (m_bVect[nNewLong - 1] >> nNewLength) << nNewLength;
  }
}

//--------------------------------------------------------- GetBufferSetLength(int nNewLength)
ptrdiff_t* CuBV::SetSize(int nNewLength, int nNewAllocLength/*-1*/)
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
    i = BITS_IN_WORD - i;
    m_bVect[m_nLongLength-1]=(m_bVect[m_nLongLength-1] >> i) << i;
  }
  if (nOldLong > m_nLongLength)
    memset(m_bVect+m_nLongLength, 0, (nOldLong - m_nLongLength)*sizeof(ptrdiff_t));
  return m_bVect;
}


/////////////////////////////////////////////////////////////////////////////////////////
//***************************************************************************************
//-------------------------------------------------------------------
void CuBV::AssignDiz(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ptrdiff_t[nLen];       // may throw an exception
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
void CuBV::AssignDiz(int nBitLength, int Num, ptrdiff_t* v1, ...)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ptrdiff_t[nLen];       // may throw an exception
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

  ptrdiff_t * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ptrdiff_t* );
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
void CuBV::AssignCon(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ptrdiff_t[nLen];       // may throw an exception
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
void CuBV::AssignCon(int nBitLength, int Num, ptrdiff_t* v1, ...)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ptrdiff_t[nLen];       // may throw an exception
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

  ptrdiff_t* v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ptrdiff_t* );
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
void CuBV::AssignXor(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ptrdiff_t[nLen];       // may throw an exception
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
void CuBV::AssignXor(int nBitLength, int Num, ptrdiff_t* v1, ...)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ptrdiff_t[nLen];       // may throw an exception
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

  ptrdiff_t* v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ptrdiff_t* );
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
void CuBV::AssignDif(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2)
{
  int nLen = LEN_LONG(nBitLength);
  if (m_nLongLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new ptrdiff_t[nLen];       // may throw an exception
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
