/////////////////////////////////////////////////////////////////
// Ver.2.1.2     22.01.2008
//               Добавлены функции конвертации данных базового класса булевых векторов и матриц
// Ver.2.1.1     22.02.2007
//               В функции Add можно не указать второго операнда
//               и тогда к текущей добавятся все строки матрицы
//               Новая функция RemoveAllRows();
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1     11.02.2005
//              Change type 'unsigned int' on ULONG
// Ver.1.0     08.02.2005
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// Short Boolean vector
#ifndef _SBOOLEAN_
#define _SBOOLEAN_

#include <string.h>
#include <stdlib.h>

#ifdef _JOINT
#include "ShortBool.h"
#include "BaseBool.h"
#include "archive.h"
#else
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include "../Common/archive.h"
#endif

#ifdef _LINUX
#undef _DEBUG
class CBV;
class CBM;
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define TRUE 1
#define FALSE 0

#define ASSERT assert
#include <assert.h>

#ifndef FO_
#define FO_
#define FSTD(_x) friend _x
#define STD(_x) _x
#endif

#else
#include <afxwin.h>         // MFC core and standard components

#ifndef FO_
#define FO_
#define FSTD(_x) friend _x AFXAPI 
#define STD(_x) _x AFXAPI 
#endif

#endif

#define SIZE_SHORTBOOL 64
#define SIZE_BYTE 8

const BYTE sTabC[256] = {
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};  


const size_t sOB[64]=
{
	0x8000000000000000, 0x4000000000000000, 0x2000000000000000, 0x1000000000000000,
	0x800000000000000,  0x400000000000000,  0x200000000000000,  0x100000000000000,
	0x80000000000000,   0x40000000000000,   0x20000000000000,   0x10000000000000,
	0x8000000000000,    0x4000000000000,    0x2000000000000,    0x1000000000000,
	0x800000000000,     0x400000000000,     0x200000000000,     0x100000000000,
	0x80000000000,      0x40000000000,      0x20000000000,      0x10000000000,
	0x8000000000,       0x4000000000,       0x2000000000,       0x1000000000,
	0x800000000,        0x400000000,        0x200000000,        0x100000000,
	0x80000000,         0x40000000,         0x20000000,         0x10000000, 
	0x8000000,          0x4000000,          0x2000000,          0x1000000, 
	0x800000,           0x400000,           0x200000,           0x100000, 
	0x80000,            0x40000,            0x20000,            0x10000, 
	0x8000,             0x4000,             0x2000,             0x1000, 
	0x800,              0x400,              0x200,              0x100, 
	0x80,               0x40,               0x20,               0x10, 
	0x8,                0x4,                0x2,                0x1
};


inline __int8 COUNTLONG(size_t u)
{ 
	BYTE* pB= (BYTE*)&u;
	return (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]] + sTabC[pB[4]] + sTabC[pB[5]] + sTabC[pB[6]] + sTabC[pB[7]]); 
}


class CsBV
{
public:
  friend class CBV;
//******************************* Constructors\Destructor *******************************
  CsBV();
  CsBV(const CsBV& bvSrc);
  CsBV(__int8 nLength, BOOL Fl);
  CsBV(const size_t UintVal, __int8 nLenBit);
  CsBV(const char* pch);
  ~CsBV();

//  void ToShort(CBV &bv);
  //void ToShort(CBV bv);
  //void FromShort(CBV &bv);
//*********************** Functions for getting class's parametrs ***********************
  __int8 GetBitLength() const;
//*************************************** Reading ***************************************
  operator size_t () const;
  BOOL GetBitAt(__int8 nIndex) const;
#ifndef _LINUX
  CString BitChar(char One = '1',char Zero = '0');
#else
  char* BitChar(char One = '1',char Zero='0');
#endif
  CsBV Extract(__int8 nFirst, __int8 nCount);

//*************************************** Writing ***************************************
  void SetBitAt(__int8 nIndex, BOOL bit);
//*********************** Operators and functions of assignment *************************
  const CsBV& operator = (const CsBV& bvSrc);
  const CsBV& operator = (const size_t pbt);
  const CsBV& operator = (const char* pch);
  void Zero();
  void One();
//************************** Operators of advanced assignment ***************************
  const CsBV& operator |= (const CsBV& bv1);
  const CsBV& operator |= (const size_t Vect);
  const CsBV& operator &= (const CsBV& bv1);
  const CsBV& operator &= (const size_t Vect);
  const CsBV& operator ^= (const CsBV& bv1);
  const CsBV& operator ^= (const size_t Vect);
  const CsBV& operator -= (const CsBV& bv1);
  const CsBV& operator -= (const size_t Vect);
  const CsBV& operator <<= (__int8 nShift);
  const CsBV& operator >>= (__int8 nShift);
//********************************** Logic operations ***********************************
  FSTD(CsBV) operator | (const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator | (const CsBV& bv1,const size_t Vect);
  FSTD(CsBV) operator | (const size_t Vect,const CsBV& bv2);
  FSTD(CsBV) operator & (const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator & (const size_t Vect,const CsBV& bv2);
  FSTD(CsBV) operator & (const CsBV& bv1,const size_t Vect);
  FSTD(CsBV) operator ^ (const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator ^ (const size_t Vect,const CsBV& bv2);
  FSTD(CsBV) operator ^ (const CsBV& bv1,const size_t Vect);
  FSTD(CsBV) operator - (const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator - (const size_t Vect,const CsBV& bv2);
  FSTD(CsBV) operator - (const CsBV& bv1,const size_t Vect);
  FSTD(CsBV) operator ~ (const CsBV& bv2);
  void Invert(const size_t Vect);
  FSTD(CsBV) operator << (const CsBV& bv1, __int8 nShift);
  FSTD(CsBV) operator >> (const CsBV& bv1, __int8 nShift);
  void LoopLeftShift(__int8 nShift);
  void LoopRightShift(__int8 nShift);
//******************** Operations of weighting, finding and casing **********************
  __int8  CountBit() const;
  __int8 LeftOne(__int8 nNext = -1) const;
  __int8 RightOne(__int8 nNext = -1) const;
//**************************** Operations of concatinations *****************************
  void Concat(const CsBV& bv);
  void Concat(const size_t Vect, __int8 nLength);
  void Concat(const CsBV& bv1, const CsBV& bv2);
  void Concat(BOOL Bit = FALSE);
//******************************* Compareing operations *********************************
  BOOL IsEmpty() const;
  BOOL IsZero() const;
  BOOL IsOne() const;
  FSTD(BOOL) operator == (const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator == (const CsBV& bv1, const size_t Vect);
  FSTD(BOOL) operator == (const size_t Vect, const CsBV& bv2);
  FSTD(BOOL) operator != (const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator != (const CsBV& bv1, const size_t Vect);
  FSTD(BOOL) operator != (const size_t Vect, const CsBV& bv2);
  FSTD(BOOL) operator >  (const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator >  (const CsBV& bv1, const size_t Vect);
  FSTD(BOOL) operator >  (const size_t Vect, const CsBV& bv2);
  FSTD(BOOL) operator <  (const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator <  (const CsBV& bv1, const size_t Vect);
  FSTD(BOOL) operator <  (const size_t Vect, const CsBV& bv2);
  FSTD(BOOL) operator >= (const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator >= (const CsBV& bv1, const size_t Vect);
  FSTD(BOOL) operator >= (const size_t Vect, const CsBV& bv2);
  FSTD(BOOL) operator <= (const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator <= (const CsBV& bv1, const size_t Vect);
  FSTD(BOOL) operator <= (const size_t Vect, const CsBV& bv2);
#ifndef _LINUX
//****************************** Input/Output operations ********************************
#ifdef _DEBUG
  friend CDumpContext& AFXAPI operator << (CDumpContext& dc, const CsBV& bv);
#endif
  friend CArchive& AFXAPI operator << (CArchive& ar, const CsBV& bv);
  friend CArchive& AFXAPI operator >> (CArchive& ar, CsBV& bv);
#endif

//***************************** Advanced access to memory *******************************
  void Empty();
  void SetSize(__int8 nNewLength);
protected:
  size_t m_bVect;
  __int8 m_nBitLength;
//******************************** Protected functions **********************************
  void CharBit(__int8 nLenByte, const char* pch);
  void LeftShiftVect(const size_t Vect, __int8 Len, __int8 nShift);
  void LeftShiftInPlace(__int8 nShift);
  void RightShiftVect(const size_t Vect, __int8 Len, __int8 nShift);
  void RightShiftInPlace(__int8 nShift);
  void Diz(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2);
  void DizInPlace(const size_t Vect, __int8 Len);
  void Con(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2);
  void ConInPlace(const size_t Vect1, __int8 Len);
  void Add2(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2);
  void Add2InPlace(const size_t Vect, __int8 Len);
  void Not(const size_t Vect, __int8 Len1);
  void ConNot(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2);
  void ConNotInPlace(const size_t Vect, __int8 Len1);
  void Conc(const size_t Vect, __int8 Len);
  void Conc2(const size_t Vect1, __int8 Len1, const size_t Vect2, __int8 Len2);
  BOOL Equality(const size_t Vect, __int8 Len) const;
  BOOL Pogl(const size_t Vect, __int8 Len, BOOL Dist) const;
  BOOL PoglEq(const size_t Vect, __int8 Len, BOOL Dist) const;
  static size_t SafeStrlen(const char* pch);
};

////////////////////////////////////////////////////////////////////////////////////////
// Inlines for CsBV class
////////////////////////////////////////////////////////////////////////////////////////
//                           CsBV class
////////////////////////////////////////////////////////////////////////////////////////
inline __int8 CsBV::GetBitLength() const  { return m_nBitLength; }

inline void CsBV::Empty() { m_bVect = m_nBitLength = 0; }
inline void CsBV::Zero()  { m_bVect = 0; }
inline BOOL CsBV::IsEmpty() const { return m_nBitLength == 0; }

inline CsBV::operator size_t() const { return m_bVect; }

inline BOOL CsBV::GetBitAt(__int8 nIndex) const
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < m_nBitLength);
	return ((m_bVect & sOB[nIndex]) != 0);
}

inline void CsBV::SetBitAt(__int8 nIndex,  BOOL bit)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < m_nBitLength);
	if (bit)
		m_bVect |= sOB[nIndex];
	else
		m_bVect &= ~sOB[nIndex];
}

inline const CsBV& CsBV::operator = (const CsBV& bvSrc)
{
	m_nBitLength = bvSrc.m_nBitLength;
	m_bVect = bvSrc.m_bVect;
	return *this;
}

inline const CsBV& CsBV::operator = (const char* pch)
{
	m_nBitLength = (__int8)SafeStrlen(pch);
	if (m_nBitLength > 0)
		CharBit(m_nBitLength, pch);
	return *this; 
}

inline const CsBV& CsBV::operator = (const size_t IntVal)
{
	if (m_nBitLength > 0)
		m_bVect = IntVal >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	return *this;
}

inline void CsBV::DizInPlace(const size_t Vect1, __int8 Len1)
{
	ASSERT(Len1 == m_nBitLength);
	m_bVect |= Vect1;
}

inline void CsBV::ConInPlace(const size_t Vect1, __int8 Len1)
{
	ASSERT(Len1 == m_nBitLength);
	m_bVect &= Vect1;
}

inline void CsBV::Add2InPlace(const size_t Vect1, __int8 Len1)
{
	ASSERT(Len1 == m_nBitLength);
	m_bVect ^= Vect1;
}

inline void CsBV::ConNotInPlace(const size_t Vect1, __int8 Len1)
{
	ASSERT(Len1 == m_nBitLength);
	m_bVect &= ~Vect1;
}

inline void CsBV:: Invert(const size_t Vect)
{
	m_bVect = ~Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
}

inline size_t CsBV::SafeStrlen(const char* pch)
{
	return (pch == NULL || strlen((char*)pch) == 0) ? 0 : strlen((char*)pch);
}





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Limited on width Boolean matrix
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


#ifndef _LINUX
class CsBM : public CObject
{
   DECLARE_SERIAL(CsBM)
#else
class CsBM 
{
#endif

public:
   friend class CsBV;
   friend class CBM;

//******************************* Constructors\Destructor *******************************
   CsBM();
   CsBM(const CsBM& bm, BOOL Fl = TRUE );
   CsBM(unsigned int nRow, __int8 nColumn, BOOL Fl = FALSE);
   CsBM(const size_t pbt, unsigned int nRow, __int8 nColumn);
   CsBM(const char* pch);
#ifndef _LINUX
   CsBM(const CStringArray& StrAr);
#else
   CsBM(const vector <string>& StrAr);
#endif
   ~CsBM();
   //void ToShort(CBM &Bm);
   //void FromShort(CBM &Bm);
//*********************** Functions for getting class's parametrs ***********************
   __int8 GetCountC() const;
   unsigned int GetCountR() const;
   unsigned int GetUpperBound() const;

//********************************* Working with memory *********************************
   void SetSize(unsigned int nRow, __int8 nColumn, __int8 nGrowBy = -1);
   void RemoveAll();
   void RemoveAllRows();
   void FreeExtra();

//********************************** Reading the data ***********************************

   BOOL GetBitAt(unsigned int nRow, __int8 nColumn) const;
   BOOL GetBitAt(unsigned int nRow, __int8 nColumn, size_t mask) const;
   size_t GetRow(unsigned int nIndex) const;
   CsBV GetRowBv(unsigned int nRow) const;
   CsBV GetRowBv(unsigned int nIndex, size_t mask) const;
   CsBV GetColumnBv(__int8 nColomn) const;
   CsBV GetColumnBv(__int8 nColumn, size_t mask) const;
   size_t operator[](unsigned int nIndex) const;
#ifndef _LINUX
   CString BitChar(char One = '1',char Zero = '0');
   void BitChar(CStringArray & StrRes, char One = '1',char Zero = '0');
#else
   void BitChar(vector <string> & StrRes, char One = '1',char Zero = '0');
   char* BitChar(char One = '1',char Zero = '0');
#endif
//********************************** Select of matrix ***********************************
   CsBM ExtractRows(unsigned int nFirst, unsigned int nCount) const;
   CsBM ExtractColumns(__int8 nFirst, __int8 nCount) const;
   CsBM Extract(unsigned int nFirstRow, __int8 nFirstColumn, unsigned int nCountRow, __int8 nCountColumn) const;
   CsBM Minor(const CsBV& Rows, const CsBV& Columns) const;
   CsBM Trans() const;

//********************************** Writing the data ***********************************
   void SetBitAt(unsigned int nRow, __int8 nColumn, BOOL bit);
   void SetRow(unsigned int nRow, const size_t newRow);
   void SetRow(unsigned int nRow, const CsBV& newRow);
   void SetRow(unsigned int nRow, const CsBM& bm, unsigned int nbmRow);

//********************************** Working with row ***********************************
   void SetRowGrow(unsigned int nIndex, const size_t newRow);
   void SetRowGrow(unsigned int nIndex, const CsBV& newRow);
   void SetRowGrow(unsigned int nRow, const CsBM& bm, unsigned int nbmRow);
   unsigned int Add(const size_t newRow);
   unsigned int Add(const CsBV& bv);
   unsigned int Add(const CsBM& bm, unsigned int nbmRow = -1);
   unsigned int Add(BOOL bit = FALSE, unsigned int nCount = 1);
   void InsertRow(unsigned int nRow, const size_t newRow, unsigned int nCount = 1);
   void InsertRow(unsigned int nRow, const CsBV& newRow, unsigned int nCount = 1);
   void InsertRow(unsigned int nRow, unsigned int nStartIndex, const CsBM& NewBM, unsigned int nCount = 1);
   void RemoveRow(unsigned int nIndex, unsigned int nCount = 1);
   void ExchangeRow(unsigned int nRow1, unsigned int nRow2, unsigned int nCount = 1);
   void SortDecr(unsigned int nBeg = 0, unsigned int number = -1);
   void SortIncr(unsigned int nBeg = 0, unsigned int number = -1);
   __int8 AddCol(__int8 numCol=1);

//*********************** Operators and functions of assignment *************************
   const CsBM& operator = (const CsBM& bm);
   const CsBM& operator = (const char* pch);
#ifndef _LINUX
   const CsBM& operator  =(const CStringArray& StrAr);
#else
   const CsBM& operator =(const vector <string>& StrAr);
#endif
   void Zero(unsigned int nRow = -1);
   void One(unsigned int nRow = -1);

//**************************** Operations of concatinations *****************************
   void Concat(const CsBM& bm);
   void Concat(const CsBM& bm1, const CsBM& bm2);

//************************** Operators of advanced assignment ***************************
   const CsBM& operator |= (const CsBM& bm);
   const CsBM& operator |= (const CsBV& bv);
   const CsBM& operator |= (const size_t pt);
   const CsBM& operator &= (const CsBM& bm);
   const CsBM& operator &= (const CsBV& bv);
   const CsBM& operator &= (const size_t pt);
   const CsBM& operator ^= (const CsBM& bm);
   const CsBM& operator ^= (const CsBV& bv);
   const CsBM& operator ^= (const size_t pt);
   const CsBM& operator -= (const CsBV& bv);
   const CsBM& operator -= (const size_t pt);

//********************************** Logic operations ***********************************
   FSTD(CsBM) operator | (const CsBM& bm1,const CsBM& bm2);
   FSTD(CsBM) operator | (const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator | (const CsBM& bm, const size_t pt);
   CsBV Disjunction();
   FSTD(CsBM) operator & (const CsBM& bm1,const CsBM& bm2);
   FSTD(CsBM) operator & (const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator & (const CsBM& bm, const size_t pt);
   CsBV Conjunction();
   FSTD(CsBM) operator ^ (const CsBM& bm1,const CsBM& bm2);
   FSTD(CsBM) operator ^ (const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator ^ (const CsBM& bm, const size_t pt);
   CsBV BitwiseExclusive();
   FSTD(CsBM) operator - (const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator - (const CsBM& bm, const size_t pt);
   FSTD(CsBM) operator ~ (const CsBM& bm);

//******************** Operations of weighting, finding and casing **********************
   __int8 CountBit(unsigned int nRow = -1) const;
   __int8 CountBit(const size_t mask, unsigned int nRow = -1) const;
   __int8 LeftOne(unsigned int nRow, __int8 nNext = -1) const;
   __int8 LeftOne(unsigned int nRow, __int8 nNext, size_t mask) const;
   __int8 RightOne(unsigned int nRow, __int8 nPrev = -1) const;
   __int8 RightOne(unsigned int nRow, __int8 nPrev, size_t mask) const;
   unsigned int FindRow (BOOL Empty, unsigned int nFRow = -1) const;
   unsigned int FindRow (BOOL Empty, size_t mask, unsigned int nFRow = -1) const;
   unsigned int FindRow (const CsBV& bv, unsigned int nFRow = -1) const;
   unsigned int FindRow (const size_t pt, unsigned int nFRow = -1) const;

//******************************* Compareing operations *********************************
   BOOL IsEmpty() const;
   BOOL IsZero(unsigned int nRow = -1) const;
   BOOL IsZero(const size_t mask, unsigned int nRow) const;
   BOOL IsOne(unsigned int nRow = -1) const;
   BOOL IsOne(const size_t mask, unsigned int nRow) const;
   FSTD(BOOL) operator == (const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator != (const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator >  (const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator <  (const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator >= (const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator <= (const CsBM& bm1,const CsBM& bm2);
   __int8 CoverRow(unsigned int nRow1, unsigned int nRow2, const size_t mask);
   __int8 CoverRow(unsigned int nRow1, unsigned int nRow2);

#ifndef _LINUX
//****************************** Input/Output operations ********************************
    void Serialize(CArchive&);
#ifdef _DEBUG
    void Dump(CDumpContext&) const;
    void AssertValid() const;
#endif
#endif
    void Serialize(CArch&);

protected:
   size_t* m_pData;
   unsigned int m_nSize;
   unsigned int m_nMaxSize;
   __int8 m_nGrowBy;
   __int8 m_nBitLength;

//******************************** Protected functions **********************************
    void Init();
    void AllocMatrix(unsigned int nRow, __int8 nColumn);
#ifndef _LINUX
    void CharBit(const CString s);
    void CharBit(const CStringArray& s);
#endif
    void CharBit(const vector <string>& s);
    void CharBit(char* s);
};


////////////////////////////////////////////////////////////////////////////
// Boolean matrix

inline unsigned int CsBM::GetCountR() const		{ return m_nSize; }

inline __int8 CsBM::GetCountC() const			{ return m_nBitLength; }

inline unsigned int CsBM::GetUpperBound() const { return m_nSize - 1; }

inline void CsBM::RemoveAll() { SetSize(0,0); }

inline void CsBM::RemoveAllRows() { SetSize(0, m_nBitLength); }

inline size_t CsBM::GetRow(unsigned int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return m_pData[nIndex];
}

inline void CsBM::SetRow(unsigned int nIndex,const size_t newRow)
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize); 
	m_pData[nIndex] = newRow;
}

inline unsigned int CsBM::Add(const size_t newRow)
{
	SetRowGrow(m_nSize, newRow);
	return (m_nSize - 1);
}

inline unsigned int CsBM::Add(const CsBV& bv)
{
	SetRowGrow(m_nSize, bv);
	return (m_nSize - 1);
}

inline BOOL CsBM::IsEmpty() const
{ return (m_pData == NULL && m_nSize == 0); }

inline size_t CsBM::operator [](unsigned int nIndex) const
{ return GetRow(nIndex); }

inline BOOL CsBM::GetBitAt(unsigned int nRow, __int8 nColumn) const
{
	ASSERT(nColumn >= 0);
	ASSERT(nRow >= 0);
	ASSERT(nColumn < m_nBitLength);
	ASSERT(nRow < m_nSize);
	return ((m_pData[nRow] & sOB[nColumn]) != 0);
}

inline BOOL CsBM::GetBitAt(unsigned int nRow, __int8 nColumn, size_t mask) const
{
	ASSERT(nColumn >= 0);
	ASSERT(nRow >= 0);
	ASSERT(nColumn < m_nBitLength);
	ASSERT(nRow < m_nSize);
	return ((m_pData[nRow] & mask & sOB[nColumn]) != 0);
}

inline void CsBM::SetBitAt(unsigned int nRow, __int8 nColumn,  BOOL bit)
{
	ASSERT(nColumn >= 0);
	ASSERT(nRow >= 0);
	ASSERT(nColumn < m_nBitLength);
	ASSERT(nRow < m_nSize);
	if (bit)
		m_pData[nRow] |= sOB[nColumn];
	else
		m_pData[nRow] &=~sOB[nColumn];
}

#ifndef _LINUX
#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif
#endif
