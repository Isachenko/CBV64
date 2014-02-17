/////////////////////////////////////////////////////////////////
// Ver.2.1.1     22.02.2007
//               В функции Add можно не указать второго операнда
//               и тогда к текущей добавятся все строки матрицы
//               Новая функция RemoveAllRows();
//               AssignCopy - добавил чистку пространства от Len до AllocLen
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

///////////////////////////////////////////////////////////////
//  Boolean vector
#ifndef _ULONGBOOLEAN_
#define _ULONGBOOLEAN_

#include "../Common/BaseBool.h"
#include <string>
using namespace std;

#define BITS_IN_BYTE 8
#define BITS_IN_WORD (sizeof(ptrdiff_t) * BITS_IN_BYTE)

class CuBV
{
public:
//******************************* Constructors\Destructor *******************************
  CuBV();
  CuBV(const CuBV& bv);
  CuBV(int nLen, BOOL Fl = FALSE);
  CuBV(const ptrdiff_t* pV, int nLenBit);
  CuBV(const char* pch);
  ~CuBV() { SafeDelete(m_bVect); };   // free any attached data

//*********************** Functions for getting class's parametrs ***********************
  int GetLongLength() const;   //inline
  int GetBitLength() const;    //inline
  int GetAllocLength() const;  //inline

//*********************** Functions for generation **************************************
  CuBV GenRbv (int nCol);
  CuBV GenRbvMid(int nCol, int nRang);
  CuBV GenRbvFix (int nCol, int nRang);

//*************************************** Reading ***************************************
  operator ptrdiff_t*() const;                   //inline
  ptrdiff_t operator[](int nIndex) const;        //inline
  ptrdiff_t  GetLongAt(int nIndex) const;        //inline
  BOOL GetBitAt(int nIndex) const;           //inline
#ifndef _LINUX
  CString BitChar(char One = '1', char Zero='0', int Max=0) const;
#else
  string BitChar(char One = '1', char Zero='0', int Max=0) const;
#endif
  CuBV Extract(int nFirst, int nCount);

//*************************************** Writing ***************************************
  void SetLongAt(int nIndex, ptrdiff_t value);   //inline
  void SetBitAt(int nIndex, BOOL bit);       //inline

//*********************** Operators and functions of assignment *************************
  const CuBV& operator=(const CuBV& bvSrc);    //inline
  const CuBV& operator=(const ptrdiff_t* pV);     //inline
  const CuBV& operator=(const char* pch);     //inline
  void Zero();                               //inline
  void One();

//************************** Operators of advanced assignment ***************************
  const CuBV& operator |=(const CuBV& bv1);   //inline
  const CuBV& operator |=(const ptrdiff_t* pV);  //inline
  const CuBV& operator &=(const CuBV& bv1);   //inline
  const CuBV& operator &=(const ptrdiff_t* pV);  //inline
  const CuBV& operator ^=(const CuBV& bv1);   //inline
  const CuBV& operator ^=(const ptrdiff_t* pV);  //inline
  const CuBV& operator -=(const CuBV& bv1);   //inline
  const CuBV& operator -=(const ptrdiff_t* pV);  //inline
  const CuBV& operator <<=(int nShift);      //inline
  const CuBV& operator >>=(int nShift);      //inline

//********************************** Logic operations ***********************************
  friend CuBV operator|(const CuBV& bv1, const CuBV& bv2);
  friend CuBV operator|(const CuBV& bv1, const ptrdiff_t* pV);
  friend CuBV operator|(const ptrdiff_t* pV, const CuBV& bv2);
  friend CuBV operator&(const CuBV& bv1, const CuBV& bv2);
  friend CuBV operator&(const ptrdiff_t* pV, const CuBV& bv2);
  friend CuBV operator&(const CuBV& bv1, const ptrdiff_t* pV);
  friend CuBV operator^(const CuBV& bv1, const CuBV& bv2);
  friend CuBV operator^(const ptrdiff_t* pV, const CuBV& bv2);
  friend CuBV operator^(const CuBV& bv1, const ptrdiff_t* pV);
  friend CuBV operator-(const CuBV& bv1, const CuBV& bv2);
  friend CuBV operator-(const ptrdiff_t* pV, const CuBV& bv2);
  friend CuBV operator-(const CuBV& bv1, const ptrdiff_t* pV);
  friend CuBV operator~(const CuBV& bv2);
  void Invert(const ptrdiff_t* pV);
  friend CuBV operator<<(const CuBV& bv1, int nShift);
  friend CuBV operator>>(const CuBV& bv1, int nShift);
  void LoopLeftShift(int nShift);
  void LoopRightShift(int nShift);

//******************** Operations of weighting, finding and casing **********************
  int  CountBit() const;
  int LeftOne(int nNext = -1) const;
  int LeftOneLong(int nNext = -1) const;
  int RightOne(int nNext = -1) const;

//**************************** Operations of concatinations *****************************
  void Concat(const CuBV& bv);                  //inline
  void Concat(const CuBV& bv1, const CuBV& bv2); //inline
  void Concat(const ptrdiff_t* pV, int nLen);      //inline
  void Concat(BOOL Bit = FALSE);

//******************************* Compareing operations *********************************
  BOOL IsEmpty() const;         //inline
  BOOL IsZero() const;
  BOOL IsOne() const;
  friend BOOL operator==(const CuBV& bv1, const CuBV& bv2);    //inline
  friend BOOL operator==(const CuBV& bv1, const ptrdiff_t* pV);   //inline
  friend BOOL operator==(const ptrdiff_t* pV, const CuBV& bv2);   //inline
  friend BOOL operator!=(const CuBV& bv1, const CuBV& bv2);    //inline
  friend BOOL operator!=(const CuBV& bv1, const ptrdiff_t* pV);   //inline
  friend BOOL operator!=(const ptrdiff_t* pV, const CuBV& bv2);   //inline
  friend BOOL operator>(const CuBV& bv1, const CuBV& bv2);     //inline
  friend BOOL operator>(const CuBV& bv1, const ptrdiff_t* pV);    //inline
  friend BOOL operator>(const ptrdiff_t* pV, const CuBV& bv2);    //inline
  friend BOOL operator<(const CuBV& bv1, const CuBV& bv2);     //inline
  friend BOOL operator<(const CuBV& bv1, const ptrdiff_t* pV);    //inline
  friend BOOL operator<(const ptrdiff_t* pV, const CuBV& bv2);    //inline
  friend BOOL operator>=(const CuBV& bv1, const CuBV& bv2);    //inline
  friend BOOL operator>=(const CuBV& bv1, const ptrdiff_t* pV);   //inline
  friend BOOL operator>=(const ptrdiff_t* pV, const CuBV& bv2);   //inline
  friend BOOL operator<=(const CuBV& bv1, const CuBV& bv2);    //inline
  friend BOOL operator<=(const CuBV& bv1, const ptrdiff_t* pV);   //inline
  friend BOOL operator<=(const ptrdiff_t* pV, const CuBV& bv2);   //inline

#ifndef _LINUX
//****************************** Input/Output operations ********************************
#ifdef _DEBUG
  friend CDumpContext& operator<<(CDumpContext& dc,const CuBV& bv);
#endif
  friend CArchive& operator<<(CArchive& ar, const CuBV& bv);
  friend CArchive& operator>>(CArchive& ar, CuBV& bv);
#endif

//***************************** Advanced access to memory *******************************
  void Empty();             //inline
  ptrdiff_t* GetBuffer(int nMinBufLength);
  void ReleaseBuffer(int nNewLength = -1);
  ptrdiff_t* SetSize(int nNewLength, int nNewAllocLength = -1);

  void AssignDiz(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2);
  void AssignDiz(int nBitLength, int Num, ptrdiff_t* v1, ...);
  void AssignCon(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2);
  void AssignCon(int nBitLength, int Num, ptrdiff_t* v1, ...);
  void AssignXor(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2);
  void AssignXor(int nBitLength, int Num, ptrdiff_t* v1, ...);
  void AssignDif(int nBitLength, const ptrdiff_t* v1, const ptrdiff_t* v2);

protected:
    ptrdiff_t* m_bVect;
    int m_nBitLength;
    int m_nLongLength;
    int m_nAllocLength;

//******************************** Protected functions **********************************
  void AllocCopy(CuBV& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
  void AllocBuffer(int nLenLong);
  void CharBit(const char* pch);
  void AssignChar(int nLen, const char* pch);
  void AssignCopy(int nLen, int nLenLong, const ptrdiff_t* pV);

  void LeftShiftVect(const ptrdiff_t* pV, int nLen, int nShift);
  void LeftShiftInPlace(int nShift);
  void RightShiftVect(const ptrdiff_t* pV, int nLen, int nShift);
  void RightShiftInPlace(int nShift);

  void Diz(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2);
  void DizInPlace(const ptrdiff_t* pV, int nLen);
  void Con(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2);
  void ConInPlace(const ptrdiff_t* pV, int nLen);
  void Add2(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2);
  void Add2InPlace(const ptrdiff_t* pV, int nLen);
  void Not(const ptrdiff_t* pV, int nLen);
  void ConNot(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2);
  void ConNotInPlace(const ptrdiff_t* pV, int nLen);

  void Extr(const ptrdiff_t* pV, int nLen, int nFirst, int nCount);

  void Conc2(const ptrdiff_t* pV1, int nLen1, const ptrdiff_t* pV2, int nLen2);

  BOOL Equality(const ptrdiff_t* pV, int nLen) const;
  BOOL Pogl(const ptrdiff_t* pV, int nLen, BOOL Dist) const;
  BOOL PoglEq(const ptrdiff_t* pV, int nLen, BOOL Dist) const;
  static void SafeDelete(ptrdiff_t* pV);             //inline
  static int SafeStrlen(const char* pch);        //inline
  void Init();                                   //inline
};

extern CuBV GetRandVu();


/////////////////////////////////////////////////////////////////////////////
// Boolean matrix
#ifndef _LINUX
class CuBM : public CObject
{
    DECLARE_SERIAL(CuBM)
#else
class CuBM
{
#endif
public:
    friend class CuBV;
//******************************* Constructors\Destructor *******************************
    CuBM();
    CuBM(const CuBM& bm, BOOL Fl = TRUE );
    CuBM(int nRow, int nColumn, BOOL Fl = FALSE);
    CuBM(const ptrdiff_t* pV, int nRow, int nColumn);
    CuBM(const char* pch);
#ifndef _LINUX
    CuBM(const CStringArray& StrAr);
#endif
    ~CuBM();

//*********************** Functions for getting class's parametrs ***********************
    int GetLongLength() const;    //inline
    int GetCountC() const;        //inline
    int GetAllocLength() const;   //inline
    int GetCountR() const;        //inline
    int GetUpperBound() const;    //inline

//*********************** Functions for generation **************************************
  CuBM GenRbm(int nRow, int nCol);
  CuBM GenRbmMid(int nRowl, int nCol, int nRang);
  CuBM GenRbmFix(int nRowl, int nCol, int nRang);
  CuBM RbmUndRow(int nRow, int nCol);

//********************************* Working with memory *********************************
    void SetSize(int nRow, int nColumn, int nGrowBy = -1,int nColGrow=0);
    void RemoveAll();             //inline
    void RemoveAllRows();         //inline
    void FreeExtra();

//********************************** Reading the data ***********************************

    BOOL GetBitAt(int nRow, int nColumn) const;                  //inline
    BOOL GetBitAt(int nRow, int nColumn, ULONG* mask) const;     //inline
    ULONG GetLongAt(int nRow, int nIndex) const;                 //inline
    ULONG GetLongAt(int nRow, int nIndex, ULONG* mask) const;    //inline
    ULONG* GetRow(int nIndex) const;                             //inline
    CuBV GetRowBv(int nRow) const;
    CuBV GetRowBv(int nIndex, ptrdiff_t* mask) const;
    CuBV GetColumnBv(int nColomn) const;
    CuBV GetColumnBv(int nColumn, ptrdiff_t* mask) const;
    ULONG* operator[](int nIndex) const;                         //inline
#ifndef _LINUX
    CString BitChar(char One = '1',char Zero = '0',BOOL WithNum=FALSE);
    void BitChar(CStringArray& StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE,
                  char One = '1',char Zero='0');
#else
    char* BitChar(char One = '1',char Zero = '0',BOOL WithNum=FALSE);
#endif
//********************************** Select of matrix ***********************************
    CuBM ExtractRows(int nFirst, int nCount) const;
    CuBM ExtractColumns(int nFirst, int nCount) const;
    CuBM Extract(int nFirstR, int nFirstC, int nCountR, int nCountC) const;
    CuBM Minor(const CuBV& Rows, const CuBV& Columns) const;
    CuBM Trans() const;

//********************************** Writing the data ***********************************
    void SetBitAt(int nRow, int nColumn, BOOL bit);              //inline
    void SetLongAt(int nRow, int nIndex, ULONG val);             //inline
    void SetRow(int nRow, const ULONG* newRow);                  //inline
    void SetRow(int nRow, const CuBV& newRow);
    void SetRow(int nRow, const CuBM& bm, int nbmRow);

//********************************** Working with row ***********************************
    void SetRowGrow(int nIndex, const ptrdiff_t* newRow);
    void SetRowGrow(int nIndex, const CuBV& newRow);
    void SetRowGrow(int nRow, const CuBM& bm, int nbmRow);
    int Add(const ptrdiff_t* newRow);                                //inline
    int Add(const CuBV& bv);                                      //inline
    int Add(const CuBM& bm, int nbmRow);                          //inline
    int Add(BOOL bit = FALSE, int nCount = 1);
    void InsertRow(int nRow, const ULONG* newRow, int nCount = 1);
    void InsertRow(int nRow, const CuBV& newRow, int nCount = 1);
    void InsertRow(int nRow, int nStartIndex, const CuBM& NewBM, int nCount = 1);
    void RemoveRow(int nIndex, int nCount = 1);
    void ExchangeRow(int nRow1, int nRow2, int nCount = 1);
    void SortDecr(int nBeg=0, int number = -1);
    void SortIncr(int nBeg=0, int number = -1);
    int AddCol(int numCol = 1);

//*********************** Operators and functions of assignment *************************
    const CuBM& operator =(const CuBM& bm);
//    const CuBM& operator =(const char* pch);
    const CuBM& operator =(char* pch);
#ifndef _LINUX
    const CuBM& operator =(const CStringArray& StrAr);
#endif
    void Zero(int nRow = -1);
    void One(int nRow = -1);

//**************************** Operations of concatinations *****************************
    void Concat(const CuBM& bm);
    void Concat(const CuBM& bm1, const CuBM& bm2);

//************************** Operators of advanced assignment ***************************
    const CuBM& operator|=(const CuBM& bm);
    const CuBM& operator|=(const CuBV& bv);
    const CuBM& operator|=(const ptrdiff_t* pV);
    const CuBM& operator&=(const CuBM& bm);
    const CuBM& operator&=(const CuBV& bv);
    const CuBM& operator&=(const ptrdiff_t* pV);
    const CuBM& operator^=(const CuBM& bm);
    const CuBM& operator^=(const CuBV& bv);
    const CuBM& operator^=(const ptrdiff_t* pV);
    const CuBM& operator-=(const CuBV& bv);
    const CuBM& operator-=(const ptrdiff_t* pV);

//********************************** Logic operations ***********************************
    friend CuBM operator|(const CuBM& bm1, const CuBM& bm2);
    friend CuBM operator|(const CuBM& bm1, const CuBV& bv2);
    friend CuBM operator|(const CuBM& bm,  const ptrdiff_t* pV);
    CuBV Disjunction();
    friend CuBM operator&(const CuBM& bm1, const CuBM& bm2);
    friend CuBM operator&(const CuBM& bm1, const CuBV& bv2);
    friend CuBM operator&(const CuBM& bm, const ptrdiff_t* pV);
    CuBV Conjunction();
    friend CuBM operator^(const CuBM& bm1, const CuBM& bm2);
    friend CuBM operator^(const CuBM& bm1, const CuBV& bv2);
    friend CuBM operator^(const CuBM& bm, const ptrdiff_t* pV);
    CuBV BitwiseExclusive();
    friend CuBM operator-(const CuBM& bm1, const CuBV& bv2);
    friend CuBM operator-(const CuBM& bm, const ptrdiff_t* pV);
    friend CuBM operator~(const CuBM& bm);

//******************** Operations of weighting, finding and casing **********************
    int CountBit(int nRow = -1) const;
    int CountBit(const ULONG* mask, int nRow = -1) const;
    int LeftOne(int nRow, int nNext = -1) const;
    int LeftOne(int nRow, int nNext, ULONG* mask) const;
    int RightOne(int nRow, int nPrev = -1) const;
    int RightOne(int nRow, int nPrev, ULONG* mask) const;
    int FindRow (BOOL Empty, int nFRow = -1) const;
    int FindRow (BOOL Empty, ULONG* mask, int nFRow = -1) const;
    int FindRow (const CuBV& bv, int nFRow = -1) const;
    int FindRow(const ULONG* pt, int nFRow = -1) const;

//******************************* Compareing operations *********************************
    BOOL IsEmpty() const;                                    //inline
    BOOL IsZero(int nRow = -1) const;
    BOOL IsZero(const ULONG* mask, int nRow) const;
    BOOL IsOne(int nRow = -1) const;
    BOOL IsOne(const ULONG* mask, int nRow) const;
    friend BOOL operator==(const CuBM& bm1, const CuBM& bm2);
    friend BOOL operator!=(const CuBM& bm1, const CuBM& bm2);
    friend BOOL operator>(const CuBM& bm1, const CuBM& bm2);
    friend BOOL operator<(const CuBM& bm1, const CuBM& bm2);
    friend BOOL operator>=(const CuBM& bm1, const CuBM& bm2);
    friend BOOL operator<=(const CuBM& bm1, const CuBM& bm2);
    int CoverRow(int nRow1, int nRow2, const ULONG* mask);
    int CoverRow(int nRow1, int nRow2);

#ifndef _LINUX
//****************************** Input/Output operations ********************************
     void Serialize(CArchive&);
#ifdef _DEBUG
     void Dump(CDumpContext&) const;
     void AssertValid() const;
#endif
#endif

  void SetRowDiz(int nRow, const ULONG* v1);
  void SetRowDiz(int nRow, const ULONG* v1, const ULONG* v2);
  void SetRowDiz(const ULONG* mask, int nRow, const ULONG* v1);
  void SetRowDiz(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2);
  void SetRowDiz(int nRow, int Num, ULONG* v1, ...);
  void SetRowDiz(const ULONG* mask, int nRow, int Num, ULONG* v1, ...);

  void SetRowCon(int nRow, const ULONG* v1);
  void SetRowCon(int nRow, const ULONG* v1, const ULONG* v2);
  void SetRowCon(const ULONG* mask, int nRow, const ULONG* v1);
  void SetRowCon(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2);
  void SetRowCon(int nRow, int Num, ULONG* v1, ...);
  void SetRowCon(const ULONG* mask, int nRow, int Num, ULONG* v1, ...);

  void SetRowXor(int nRow, const ULONG* v1);
  void SetRowXor(int nRow, const ULONG* v1, const ULONG* v2);
  void SetRowXor(const ULONG* mask, int nRow, const ULONG* v1);
  void SetRowXor(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2);
  void SetRowXor(int nRow, int Num, ULONG* v1, ...);
  void SetRowXor(const ULONG* mask, int nRow, int Num, ULONG* v1, ...);

  void SetRowDif(int nRow, const ULONG* v1);
  void SetRowDif(int nRow, const ULONG* v1, const ULONG* v2);
  void SetRowDif(const ULONG* mask, int nRow, const ULONG* v1);
  void SetRowDif(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2);


protected:
    ptrdiff_t** m_pData;
    int m_nSize;
    int m_nMaxSize;
    int m_nGrowBy;
    int m_nBitLength;
    int m_nLongLength;
    int m_nAllocLength;

//******************************** Protected functions **********************************
  void Init();                               //inline
     void SafeDelete();
  void AllocMatrix(int nRow, int nColumn);
#ifndef _LINUX
  void CharBit(const CString s);
  void CharBit(const CStringArray& s);
#endif
  void CharBit(char* s);
};


////////////////////////////////////////////////////////////////////////////////////////
//                 Inlines for CuBV class
////////////////////////////////////////////////////////////////////////////////////////


inline int CuBV::GetLongLength() const { return m_nLongLength; }
inline int CuBV::GetBitLength() const { return m_nBitLength; }
inline int CuBV::GetAllocLength() const { return m_nAllocLength; }
inline CuBV::operator ptrdiff_t*() const { return (ptrdiff_t*)m_bVect; }

inline ptrdiff_t CuBV::operator[](int nIndex) const
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nLongLength);
  return m_bVect[nIndex]; 
}

inline ptrdiff_t CuBV::GetLongAt(int nIndex) const 
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nLongLength);
  return m_bVect[nIndex]; 
}

inline BOOL CuBV::GetBitAt(int nIndex) const 
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  return ((m_bVect[BIT_LONG(nIndex)] & OB4[ADR_BITLONG(nIndex)])!=0);
}

inline void CuBV::SetLongAt(int nIndex, ptrdiff_t val)
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nLongLength);
  m_bVect[nIndex] = val;
}

inline void CuBV::SetBitAt(int nIndex, BOOL bit)
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  if (bit) m_bVect[BIT_LONG(nIndex)] |=OB4[ADR_BITLONG(nIndex)];
  else     m_bVect[BIT_LONG(nIndex)] &=~OB4[ADR_BITLONG(nIndex)];
}

inline BOOL CuBV::IsEmpty() const { return m_nLongLength == 0; }

inline const CuBV& CuBV::operator =(const CuBV& bv)
{ AssignCopy(bv.m_nBitLength, bv.m_nLongLength, bv.m_bVect); return *this; }

inline const CuBV& CuBV::operator =(const char* pch)
{ AssignChar(SafeStrlen(pch), pch); return *this; }

inline const CuBV& CuBV::operator =(const ptrdiff_t* pV)
{ ASSERT(m_nBitLength > 0);
  AssignCopy(m_nBitLength, m_nLongLength, pV);  return *this;
}

inline void CuBV::Zero()
{ ASSERT(m_nLongLength >= 0);
  memset(m_bVect, 0, m_nLongLength*sizeof(ptrdiff_t)); 
}

inline void CuBV::Concat(const CuBV& bv) 
{ Concat(bv.m_bVect, bv.m_nBitLength); }

inline void CuBV::Concat(const CuBV& bv1, const CuBV& bv2)
{ Conc2(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength); }

inline const CuBV& CuBV::operator |=(const CuBV& bv)
{ DizInPlace(bv.m_bVect, bv.m_nBitLength); return *this;}

inline const CuBV& CuBV::operator |=(const ptrdiff_t* pV)
{ DizInPlace(pV, m_nBitLength); return *this;}

inline const CuBV& CuBV::operator &=(const CuBV& bv)
{ ConInPlace(bv.m_bVect, bv.m_nBitLength); return *this;}

inline const CuBV& CuBV::operator &=(const ptrdiff_t* pV)
{ ConInPlace(pV, m_nBitLength); return *this;}

inline const CuBV& CuBV::operator ^=(const CuBV& bv)
{ Add2InPlace(bv.m_bVect, bv.m_nBitLength);    return *this;}

inline const CuBV& CuBV::operator ^=(const ptrdiff_t* pV)
{ Add2InPlace(pV, m_nBitLength); return *this;}

inline const CuBV& CuBV::operator -=(const CuBV& bv)
{ ConNotInPlace(bv.m_bVect, bv.m_nBitLength); return *this;}

inline const CuBV& CuBV::operator -=(const ptrdiff_t* pV)
{ ConNotInPlace(pV,m_nBitLength); return *this;}

inline const CuBV&  CuBV::operator <<=(int nShift)
{ LeftShiftInPlace(nShift);  return *this; }

inline const CuBV& CuBV::operator >>=(int nShift)
{ RightShiftInPlace(nShift); return *this;}

inline void CuBV::Empty() { SafeDelete(m_bVect); Init(); }

inline BOOL operator ==(const CuBV& bv1, const CuBV& bv2)
{ return bv1.Equality(bv2.m_bVect, bv2.m_nBitLength); }

inline BOOL operator ==(const CuBV& bv, const ptrdiff_t* pV)
{ return bv.Equality(pV, bv.m_nBitLength); }

inline BOOL operator ==(const ptrdiff_t* pV, const CuBV& bv)
{ return bv.Equality(pV, bv.m_nBitLength); }

inline BOOL operator !=(const CuBV& bv1, const CuBV& bv2)
{ return !(bv1.Equality(bv2.m_bVect, bv2.m_nBitLength)); }

inline BOOL operator !=(const CuBV& bv, const ptrdiff_t* pV)
{ return !(bv.Equality(pV, bv.m_nBitLength)); }

inline BOOL operator !=(const ptrdiff_t* pV, const CuBV& bv)
{ return !(bv.Equality(pV, bv.m_nBitLength)); }

inline BOOL operator >(const CuBV& bv1, const CuBV& bv2)
{ return (bv1.Pogl(bv2.m_bVect, bv2.m_nBitLength, TRUE)); }

inline BOOL operator >(const CuBV& bv, const ptrdiff_t* pV)
{ return (bv.Pogl(pV, bv.m_nBitLength, TRUE)); }

inline BOOL operator >(const ptrdiff_t* pV, const CuBV& bv)
{ return (bv.Pogl(pV, bv.m_nBitLength, FALSE)); }

inline BOOL operator <(const CuBV& bv1, const CuBV& bv2)
{ return (bv2.Pogl(bv1.m_bVect, bv1.m_nBitLength, TRUE)); }

inline BOOL operator <(const CuBV& bv, const ptrdiff_t* pV)
{ return (bv.Pogl(pV, bv.m_nBitLength, FALSE)); }

inline BOOL operator <(const ptrdiff_t* pV, const CuBV& bv)
{ return (bv.Pogl(pV, bv.m_nBitLength, TRUE)); }

inline BOOL operator>=(const CuBV& bv1, const CuBV& bv2)
{ return (bv1.PoglEq(bv2.m_bVect, bv2.m_nBitLength,TRUE)); }

inline BOOL operator>=(const CuBV& bv, const ptrdiff_t* pV)
{ return (bv.PoglEq(pV, bv.m_nBitLength, TRUE)); }

inline BOOL operator>=(const ptrdiff_t* pV, const CuBV& bv)
{ return (bv.PoglEq(pV, bv.m_nBitLength, FALSE)); }

inline BOOL operator <=(const CuBV& bv1, const CuBV& bv2)
{ return (bv2.PoglEq(bv1.m_bVect, bv1.m_nBitLength, TRUE)); }

inline BOOL operator <=(const CuBV& bv, const ptrdiff_t* pV)
{ return (bv.PoglEq(pV, bv.m_nBitLength, FALSE)); }

inline BOOL operator<=(const ptrdiff_t* pV, const CuBV& bv)
{ return (bv.PoglEq(pV, bv.m_nBitLength, TRUE)); }



//----------------------------------- (protect)
inline void CuBV::Init()
{ m_nBitLength = m_nLongLength = m_nAllocLength = 0;  m_bVect = NULL; }

inline void CuBV::SafeDelete(ptrdiff_t* pV)
{ if (pV != NULL) delete [] pV; }

inline int CuBV::SafeStrlen(const char* pch)
{ return (pch == NULL) ? 0 : strlen((char*)pch); }

/////////////////////////////////////////////////////////////////////////////
// ------------------------------------------ End of declaration of CuBV class
/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////
//                 Inlines for CuBM class
////////////////////////////////////////////////////////////////////////////////////////

inline int CuBM::GetLongLength() const { return m_nLongLength; }
inline int CuBM::GetCountC() const { return m_nBitLength; }
inline int CuBM::GetAllocLength() const { return m_nAllocLength; }

inline int CuBM::GetCountR() const     { return m_nSize; }
inline int CuBM::GetUpperBound() const { return m_nSize - 1; }
inline void CuBM::RemoveAll()          { SetSize(0,0); }
inline void CuBM::RemoveAllRows()      { SetSize(0,m_nBitLength); }

inline BOOL CuBM::IsEmpty() const { return (m_pData == NULL && m_nSize == 0); }
inline ULONG* CuBM::operator [](int nIndex) const {return GetRow(nIndex); }

inline ULONG* CuBM::GetRow(int nIndex) const
{ ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }

inline void CuBM::SetRow(int nIndex, const ULONG* newRow)
{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
  memcpy(m_pData[nIndex], newRow, m_nLongLength*sizeof(ULONG));
}

inline int CuBM::Add(const ULONG* newRow)
{ SetRowGrow(m_nSize, newRow); return (m_nSize - 1); }

inline int CuBM::Add(const CuBV& bv)
{ SetRowGrow(m_nSize, bv); return (m_nSize - 1); }

inline ULONG CuBM::GetLongAt(int nRow, int nIndex) const
{ ASSERT ((nIndex >= 0) && (nRow >= 0));
  ASSERT ((nIndex < m_nLongLength) && (nRow < m_nSize));
  return m_pData[nRow][nIndex];
}

inline ULONG CuBM::GetLongAt(int nRow, int nIndex, ULONG* mask) const
{ ASSERT ((nIndex >= 0) && (nRow >= 0));
  ASSERT ((nIndex < m_nLongLength) && (nRow < m_nSize));
  return (m_pData[nRow][nIndex] & mask[nIndex]);
}

inline BOOL CuBM::GetBitAt(int nRow, int nColumn) const
{ ASSERT ((nColumn >= 0) && (nRow >= 0));
  ASSERT ((nColumn < m_nBitLength) && (nRow < m_nSize));
  return ((m_pData[nRow][BIT_LONG(nColumn)] & OB4[ADR_BITLONG(nColumn)])!=0);
}

inline BOOL CuBM::GetBitAt(int nRow, int nColumn, ULONG* mask) const
{ ASSERT ((nColumn >= 0) && (nRow >= 0));
  ASSERT ((nColumn < m_nBitLength) && (nRow < m_nSize));
  return ((m_pData[nRow][BIT_LONG(nColumn)] &
           mask[BIT_LONG(nColumn)] & OB4[ADR_BITLONG(nColumn)])!=0);
}

inline void CuBM::SetLongAt(int nRow, int nIndex, ULONG val)
{ ASSERT ((nIndex >= 0) && (nRow >= 0));
  ASSERT ((nIndex < m_nLongLength) && (nRow < m_nSize));
  m_pData[nRow][nIndex] = val;
}

inline void CuBM::SetBitAt(int nRow,int nColumn, BOOL bit)
{ ASSERT ((nColumn >= 0) && (nRow >= 0));
  ASSERT ((nColumn < m_nBitLength) && (nRow < m_nSize));
  if (bit) m_pData[nRow][BIT_LONG(nColumn)] |=OB4[ADR_BITLONG(nColumn)];
  else     m_pData[nRow][BIT_LONG(nColumn)] &=~OB4[ADR_BITLONG(nColumn)];
}

inline void CuBM::Init()
{ 
  m_pData = NULL; m_nGrowBy = 10;
  m_nSize = m_nMaxSize =  m_nBitLength = m_nLongLength = m_nAllocLength = 0;
}

/////////////////////////////////////////////////////////////////////////////
// ------------------------------------------ End of declaration of CuBM class
/////////////////////////////////////////////////////////////////////////////
#ifndef _LINUX
#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif

#endif     //_BOOLEAN_
