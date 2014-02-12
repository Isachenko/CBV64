/////////////////////////////////////////////////////////////////
// Ver.2.1.2    28.03.2008
//              SetOnes и SetZeros - исправлены для реализации в другой интерпретации
//              в виде функций AddOnes и AddZeros
// Ver.2.1.1     22.02.2007
//               RemoveAllRows - сохраняет число столбцов матрицы
//               Add для CBM перестала быть inline, допускает добавление
//               всей матрицы целиком при -1, принятом по умолчанию.
//               поправка в конструкторах m_nAllocLength
//               AssignCopy - добавил чистку пространства от Len до AllocLen
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.2.0.4     19.05.2006
// Ver.2.0.3     05.04.2006
// Ver.2.0.2     26.03.2006
// Ver.2.0.1     21.03.2006
// Ver.2.0.0     11.03.2006   Russian == English
// Ver.1.0.0     13.02.2005
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
#ifndef _ULONGTERN_
#define _ULONGTERN_

#include "../Common/BaseBool.h"
#include "../ComBool/UlongBool.h"
#include "../ComBool/UlongTern.h"
#include <string>

/////////////////////////////////////////////////////////////////////////////
// Ternary vector

class CuTV
{
public:
   friend class CuBV;
   friend class CuBM;

//******************************* Constructors\Destructor *******************************
   CuTV();
   CuTV(const CuTV& tvSrc);
   CuTV(const CuBV& bv1Src, const CuBV& bv0Src);
   CuTV(int nLenBit, char value = 'n');  //  1, 0, -, + - тр.вектор в кодировке 
                                        //  10,01,00,11,'n'-без инициализ-ии
   CuTV(const ULONG* pbt1, const ULONG* pbt0, int nLenBit);
   CuTV(const char* pch);
   ~CuTV();

//*********************** Functions for getting class's parametrs ***********************
   int GetLongLength() const;
   int GetBitLength() const;
   int GetAllocLength() const;

//*********************** Functions for generation **************************************
  CuTV GenRtv ( int nCol);
  CuTV GenRtvMid (int nCol, int nRang);
  CuTV GenRtvFix (int nCol, int nRang);

//*************************************** Data Reading **********************************
   char GetBitAt(int nIndex) const;
   CuTV Extract(int nFirst, int nCount);
#ifndef _LINUX
   CString BitChar(char One = '1', char Zero='0', 
                   char Dont='-', char Rest='+', int Max=0);
#else
   string BitChar(char One = '1', char Zero='0', 
                   char Dont='-', char Rest='+', int Max=0);
#endif

   CuBV GetOnes() const;
   CuBV GetZeros() const;
   CuBV GetDefs() const;
   CuBV GetUnDefs() const;
   ULONG* GetOnesB() const;
   ULONG* GetZerosB() const;

//*************************************** Data Writing **********************************
   void SetBitAt(int nIndex, char symb);
   void SetVect(CuBV bv1Src, CuBV bv0Src);
   void SetOnes(const CuBV& bv);
   void SetZeros(const CuBV& bv);
   void AddOnes(const CuBV& bv);
   void AddZeros(const CuBV& bv);
   void SetUnDefs(const CuBV& bv);
   void SetOnes(const ULONG* bv);
   void SetZeros(const ULONG* bv);
   void AddOnes(const ULONG* bv);
   void AddZeros(const ULONG* bv);

//*********************** Operators and functions of assignment *************************
   const CuTV& operator=(const CuTV& tvSrc);
   const CuTV& operator=(const CuBV& bvSrc);
   const CuTV& operator=(const char* pch);
   void Clear(char symb = '-');
   void Empty();

//*********************** Operators and functions of expanded assignment ****************
   const CuTV& operator <<=(int nShift);
   const CuTV& operator >>=(int nShift);
   const CuTV& operator &=(const ULONG* maska);
   const CuTV& operator &=(const CuBV& maska);
   const CuTV& operator &=(const CuTV& tv);

//********************************** Logic operations ***********************************
   friend CuTV operator<<(const CuTV& tv, int nShift);
   friend CuTV operator>>(const CuTV& tv, int nShift);
   friend CuTV operator&=(const CuTV& tv, const ULONG* maska);
   friend CuTV operator&=(const CuTV& tv, const CuBV& maska);
   friend CuTV operator&=(const CuTV& tv1, const CuTV& tv2);
   void LoopLeftShift(int nShift);
   void LoopRightShift(int nShift);
   void InvertDefs();

//******************** Logic Operations of weighting, finding and casing ****************

   int  CountDefs() const;       // число 1 и 0
   int  CountUnDefs() const;       // число 1 и 0
   int  CountOnes() const;       // число 1
   int  CountZeros() const;      // число и 0
   int LeftUnDef(int nNext = -1) const;
   int LeftDef(int nNext = -1) const;
   int LeftOne(int nNext = -1) const;
   int LeftZero(int nNext = -1) const;
   int RightUnDef(int nNext = -1) const;
   int RightDef(int nNext = -1) const;
   int RightOne(int nNext = -1) const;
   int RightZero(int nNext = -1) const;

//**************************** Operations of concatinations *****************************
   void Concat(const CuTV& tv);
   void Concat(const CuTV& tv1, const CuTV& tv2);
   void Concat(char symb = '-');

//******************************* Compareing operations *********************************
   BOOL IsEmpty() const;
   BOOL IsCorrect() const;  // кодировка 11 не корректна
   BOOL IsBool() const;     // вектор булев ?
   BOOL IsTrivial() const;  // все '-'
   BOOL IsOne() const;
   BOOL IsZero() const;
   BOOL ExistZero() const;
   BOOL ExistOne() const;
   BOOL IsOrthog(const CuTV& tv) const;
   BOOL IsOrthog(const CuBV& bv) const;
   BOOL IsNeighb(const CuTV& tv) const; //соседство
   BOOL IsNeighb(const CuBV& bv) const; //соседство
   BOOL IsAdjac(const CuTV& tv) const;  //смежность
   BOOL IsAdjac(const CuBV& bv) const;  //смежность
   BOOL IsCover(const CuTV& tv) const;  // 1 поглощает 2
   BOOL IsCover(const CuBV& bv) const;  // 1 поглощает 2
   BOOL IsIntersec(const CuTV& tv) const;//пересекаютс
   friend BOOL operator==(const CuTV& tv1, const CuTV& tv2);
   friend BOOL operator==(const CuTV& tv1, const CuBV& bv2);
   friend BOOL operator==(const CuBV& bv1, const CuTV& tv2);
   friend BOOL operator!=(const CuTV& tv1, const CuTV& tv2);
   friend BOOL operator!=(const CuTV& tv1, const CuBV& bv2);
   friend BOOL operator!=(const CuBV& bv1, const CuTV& tv2);
   friend BOOL operator >(const CuTV& tv1, const CuTV& tv2);
   friend BOOL operator <(const CuTV& tv1, const CuTV& tv2);
   friend BOOL operator>=(const CuTV& tv1, const CuTV& tv2);
   friend BOOL operator<=(const CuTV& tv1, const CuTV& tv2);

//******************************* Operations on intervals *******************************

   int NumNeighb(const CuTV& tv) const;  //номер компоненты соседства
   int NumNeighb(const CuBV& bv) const;  //номер компоненты соседства
   int NumAdjac(const CuTV& tv) const;   //номер компоненты смежности
   int NumAdjac(const CuBV& bv) const;   //номер компоненты смежности
   CuBV Orthogon(const CuTV& tv) const;
   CuBV Orthogon(const CuBV& bv) const;
   CuTV Intersec(const CuTV& tv) const;  //CuTV = Empty,если не пересекаютс
   CuTV Concens(const CuTV& tv) const;   //рез-т обобщ.склеивани
   CuTV Concens(const CuBV& tv) const;   //рез-т обобщ.склеивани
   CuTV MinCover(const CuTV& tv) const;
   CuTV MinCover(const CuBV& tv) const;
   CuBM CuTVtoCuBM() const;               //перевод тр.вектора в мн-во булевых

//**************************** Operations on TERNARY VECTOR PARTS ***********************
//********************************** Logic operations ***********************************
   void OrInPlace(BOOL Part, const CuBV& bv);
   void AndInPlace(BOOL Part, const CuBV& bv);
   void XorInPlace(BOOL Part, const CuBV& bv);
   void SubInPlace(BOOL Part, const CuBV& bv);
   void InvertBitsInPlace(BOOL Part);
   CuBV  InvertBits(BOOL Part) const;

#ifndef _LINUX
//****************************** Input/Output operations ********************************
#ifdef _DEBUG
   friend CDumpContext& operator<<(CDumpContext& dc, const CuTV& tv);
#endif
   friend CArchive& operator<<(CArchive& ar, const CuTV& tv);
   friend CArchive& operator>>(CArchive& ar, CuTV& tv);
#endif

//***************************** Advanced access to memory *******************************
   ULONG* GetBuffer(int nMinBufLength);
   void ReleaseBuffer(int nNewLength = -1);
   ULONG* SetSize(int nNewLength, int nNewAllocLength = -1);

//***************************************************************************************
   ULONG* m_bVect1;
   ULONG* m_bVect0;

   protected:
   int m_nBitLength;
   int m_nLongLength;
   int m_nAllocLength;

//******************************** Protected functions **********************************
   void AllocBuffer(int nLen);
   void AllocCopy(CuTV& dest, int nCopyLen, int nCopyIndex, int nExtraLen);
   void AssignChar(int nLenBit, const char* pch);
   void AssignCopy(int nLenBit, int nLenLong, const ULONG* pbtSrcDat1,
                   const ULONG* pbtSrcDat0);
   void CharBit(int nLenLong, const char* pch);
   void Conc(const ULONG* SrcVect1, const ULONG* SrcVect0, int SrcBitLen);
   void Conc2(const ULONG* Vect11, const ULONG* Vect10, int SrcBitLen1,
              const ULONG* Vect21, const ULONG* Vect20, int SrcBitLen2);
   BOOL Equality(const ULONG* Vect1, const ULONG* Vect0, int BitLen) const;
   BOOL EqualBool(const ULONG* Vect, int BitLen) const;
   void Extr(const ULONG* SrcVect1, const ULONG* SrcVect0,
             int SrcBitLen, int nFirst, int nCount);
   void LeftShiftVect(const ULONG* SrcVect1, const ULONG* SrcVect0,
             int nSrcLen, int nShift);
   BOOL Pogl(const ULONG* Vect1, const ULONG* Vect0, int BitLen) const;
   void RightShiftVect(const ULONG* SrcVect1, const ULONG* SrcVect0,
             int nSrcLen, int nShift);
   void Init();
};

////////////////////////////////////////////////////////////////////////////////////////
//                 Inlines for CuTV class
////////////////////////////////////////////////////////////////////////////////////////
//********************** Functions for getting class's parametrs ***********************
inline int CuTV::GetLongLength() const  { return m_nLongLength; }
inline int CuTV::GetBitLength() const   { return m_nBitLength; }
inline int CuTV::GetAllocLength() const { return m_nAllocLength; }

//************************************** Data Reading **********************************
inline CuBV CuTV::GetOnes() const { CuBV bv(m_bVect1, m_nBitLength); return bv; }
inline CuBV CuTV::GetZeros() const { CuBV bv(m_bVect0, m_nBitLength); return bv; }
inline ULONG* CuTV::GetOnesB() const { return ( ULONG*)m_bVect1; }
inline ULONG* CuTV::GetZerosB() const { return ( ULONG*)m_bVect0; }

//********************** Operators and functions of assignment *************************
inline const CuTV& CuTV::operator =(const CuTV& tv)
{ AssignCopy(tv.m_nBitLength, tv.m_nLongLength, tv.m_bVect1, tv.m_bVect0);
  return *this;
}
inline const CuTV& CuTV::operator =(const CuBV& bv)
{ CuBV inv=~bv;
  AssignCopy(bv.GetBitLength(), bv.GetLongLength(), (ULONG*)bv, (ULONG*)inv);
  return *this;
}
inline const CuTV& CuTV::operator =(const char* pch)
{ AssignChar(strlen((char*)pch), pch); return *this; }

inline void CuTV::Empty()  { if (m_bVect1 != NULL) delete [] m_bVect1; Init(); }

//****************************** Compareing operations *********************************
inline BOOL CuTV::IsEmpty() const  { return m_nLongLength == 0; }

inline void CuTV::Init()
{ m_nBitLength = m_nLongLength = m_nAllocLength = 0;  m_bVect1 = m_bVect0 = NULL; }

//*************************** Operations of concatinations *****************************
inline void CuTV::Concat(const CuTV& tv)
{  Conc(tv.m_bVect1, tv.m_bVect0, tv.m_nBitLength); return; }

inline void CuTV::Concat(const CuTV& tv1,const CuTV& tv2)
{   Conc2(tv1.m_bVect1, tv1.m_bVect0, tv1.m_nBitLength, 
          tv2.m_bVect1, tv2.m_bVect0, tv2.m_nBitLength); return; }



/////////////////////////////////////////////////////////////////////////////
// Ternary matrix

#ifndef _LINUX
class CuTM : public CObject
{
   DECLARE_SERIAL(CuTM)
#else
class CuTM
   {
#endif

public:
   friend class CuTV;
   friend class CuBV;
   friend class CuBM;

//******************************* Constructors\Destructor *******************************
   CuTM();
   CuTM(const CuTM& tm, BOOL Fl = TRUE );
   CuTM(const CuBM& bm1, const CuBM& bm2, BOOL Fl = TRUE);
   CuTM(int nRow, int nColumn, char symb = '-');
   CuTM(const char* pch);
#ifndef _LINUX
   CuTM(const CStringArray& StrAr);
#endif
   ~CuTM();

//*********************** Functions for getting class's parametrs ***********************
   int GetLongLength() const;
   int GetAllocLength() const;
   int GetCountC() const;
   int GetCountR() const;
   int GetUpperBound() const;

//*********************** Functions for generation ****************************
  CuTM GenRtm (int nRowl, int nCol);
  CuTM GenRtmMid (int nRowl, int nCol, int nRang);
  CuTM GenRtmFix (int nRowl, int nCol, int nRang);

//********************************* Working with memory *********************************
   void SetSize(int nRow, int nColumn, int nGrowBy = 10, int nColGrow=0);
   void RemoveAll();
   void RemoveAllRows();
   void FreeExtra();

//********************************** Data Reading ***************************************
   char GetBitAt(int nRow, int nColumn) const;
   CuTV GetRowTv(int nRow) const;
   CuTV GetRowTv(int nRow,ULONG* maska) const;
   CuTV GetRowTv(int nRow, CuBV& maska) const;
   CuTV GetColumnTv(int nColumn) const;
   CuTV GetColumnTv(int nColumn,ULONG* maska) const;
   CuTV GetColumnTv(int nColumn, CuBV& maska) const;
#ifndef _LINUX
   CString BitChar(char One = '1', char Zero = '0', 
                   char Dont = '-', char Rest = '+', BOOL WithNum = FALSE);
   void BitChar(CStringArray & StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE,
                char One = '1', char Zero = '0', char Dont = '-', char Rest = '+');
#else
   string BitChar(char One = '1', char Zero = '0', 
                   char Dont = '-', char Rest = '+', BOOL WithNum = FALSE);
#endif
   CuBM GetDefs() const;
   CuBM GetUnDefs() const;
   CuBM GetOnes() const;
   CuBM GetZeros() const;
   CuBV GetRowUnDef(int nRow) const;
   CuBV GetRowDef(int nRow) const;
   CuBV GetRowOne(int nRow) const;
   CuBV GetRowZero(int nRow) const;
   CuBV GetRowUnDef(int nRow, ULONG* maska) const;
   CuBV GetRowDef(int nRow, ULONG* maska) const;
   CuBV GetRowOne(int nRow, ULONG* maska) const;
   CuBV GetRowZero(int nRow, ULONG* maska) const;
   ULONG* GetLongZero(int nRow) const;
   ULONG* GetLongOne(int nRow) const;

   void Param(int* aPar, int nMaxLen);

//********************************** Select of matrix ***********************************
   CuTM ExtractRows(int nFirst, int nCount) const;
   CuTM ExtractColumns(int nFirst, int nCount) const;
   CuTM Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const;
   CuTM ExtractColumns(const CuBV& Columns) const;
   CuTM Minor(const CuBV& Rows, const CuBV& Columns) const;
   CuTM Trans() const;

//********************************** Data Writing ***************************************
   void SetBitAt(int nRow, int nColumn, char symb='-');
   void SetRowTv(int nRow, const CuTV& newRow);
   void SetRowTv(int nRow, const CuTM& tm, int ntmRow);
   void SetPartAt(BOOL Part, const CuBM& bm);
   void SetPartAt(BOOL Part, const CuBV& bv, int nRow);
   void SetRowZero(int nRow, const ULONG* newRow);
   void SetRowOne(int nRow, const ULONG* newRow);

//**************************** Working with rows (changing their number) *****************
   int Add(const CuTV& tv);
   int Add(const CuTM& tm, int ntmRow=-1);
   int Add(char symb = '-', int nCount = 1);
   void SetRowGrow(int nRow, const CuTV& newRow);
   void SetRowGrow(int nRow, const CuTM& tm, int ntmRow);
   void InsertRow(int nRow, const CuTV& newRow, int nCount = 1);  //несколько копий
   void InsertRow(int nRow, int nStartRow, const CuTM& tm, int nCount = 1);
   void RemoveRow(int nRow, int nCount = 1);
   void ExchangeRow(int nRow1, int nRow2, int nCount = 1);
   int AddCol(int numCol = 1);

   void SortDecr(int nBeg = 0, int number = -1);
   void SortIncr(int nBeg = 0, int number = -1);

//*********************** Operators and functions of assignment *************************
   const CuTM& operator =(const CuTM& tm);
   const CuTM& operator =(const char* pch);
#ifndef _LINUX
   const CuTM& operator =(const CStringArray& StrAr);
#endif
   void Clear(char symb = '-', int nRow = -1);

//********************************** Logic operations ***********************************
   void InvertDefs(int nRow = -1); //инвертировать опр.значения строк или одной стр.

//**************************** Operations of concatinations *****************************
   void Concat(const CuTM& tm);
   void Concat(const CuTM& tm1, const CuTM& tm2);

//******************** Operations of weighting, finding and casing **********************
   int CountUnDefs(int nRow = -1) const;
   int CountDefs(int nRow = -1) const;
   int CountOnes(int nRow = -1) const;
   int CountZeros(int nRow = -1) const;

   int CountUnDefs(const ULONG* mask, int nRow = -1) const;
   int CountDefs(const ULONG* mask, int nRow = -1) const;
   int CountOnes(const ULONG* mask, int nRow = -1) const;
   int CountZeros(const ULONG* mask, int nRow = -1) const;

   int LeftUnDef(int nRow, int nNext = -1) const;
   int LeftDef(int nRow, int nNext = -1) const;
   int LeftOne(int nRow, int nNext = -1) const;
   int LeftZero(int nRow, int nNext = -1) const;
   int LeftUnDef(int nRow, int nNext, ULONG* mask) const;
   int LeftDef(int nRow, int nNext, ULONG* mask) const;
   int LeftOne(int nRow, int nNext, ULONG* mask) const;
   int LeftZero(int nRow, int nNext, ULONG* mask) const;

   int RightUnDef(int nRow, int nPrev = -1) const;
   int RightDef(int nRow, int nPrev = -1) const;
   int RightOne(int nRow, int nPrev = -1) const;
   int RightZero(int nRow, int nPrev = -1) const;
   int RightUnDef(int nRow, int nPrev, ULONG* mask) const;
   int RightDef(int nRow, int nPrev, ULONG* mask) const;
   int RightOne(int nRow, int nPrev, ULONG* mask) const;
   int RightZero(int nRow, int nPrev, ULONG* mask) const;

   int FindUnDefR (int nRow = -1) const; // все '-'
   int FindDefR (int nRow = -1) const;   // нет '-'
   int FindOneR (int nRow = -1) const;   // все '1'
   int FindZeroR (int nRow = -1) const;  // все '0'
   int FindUnDefR (ULONG* mask, int nFRow = -1) const;
   int FindDefR (ULONG* mask, int nFRow = -1) const;   // нет '-'
   int FindOneR (ULONG* mask, int nFRow = -1) const;
   int FindZeroR (ULONG* mask, int nFRow = -1) const;
   int FindR (const CuTV& tv, int nFRow = -1) const;
   int FindR (const CuBV& bv, int nFRow = -1) const;

//******************************* Matrix compareing operations *************************
   BOOL IsCorrect(int nRow = -1) const;  //new кодировка 11 не корректна
   BOOL IsBool(int nRow = -1) const;     //new матрица булева ?
   BOOL IsBool(const ULONG* mask, int nRow = -1)  const;
   BOOL IsTrivial(int nRow = -1) const;  //new все '-'
   BOOL IsTrivial(const ULONG* mask, int nRow = -1) const;
   BOOL IsOne(int nRow = -1) const;
   BOOL IsOne(const ULONG* mask, int nRow) const;
   BOOL IsZero(int nRow = -1) const;
   BOOL IsZero(const ULONG* mask, int nRow = -1) const;
   BOOL ExistOne(int nRow/*=-1*/) const;
   BOOL ExistOne(const ULONG* mask, int nRow/*=-1*/) const;
   BOOL ExistZero(int nRow/*=-1*/) const;
   BOOL ExistZero(const ULONG* mask, int nRow/*=-1*/) const;
   BOOL IsEmpty() const;
   friend BOOL operator==(const CuTM& tm1, const CuTM& tm2);
   friend BOOL operator!=(const CuTM& tm1, const CuTM& tm2);

//***************************** Row compareing operations *******************************

   BOOL AreEqual(int nRow1, int nRow2) const;
   BOOL AreEqual(const ULONG* mask, int nRow1, int nRow2) const;
   BOOL AreEqual(int nRow1, const CuTV& tv) const;
   BOOL AreEqual(const ULONG* mask, int nRow1, const CuTV& tv) const;

   BOOL AreNotEqual(int nRow1, int nRow2) const;
   BOOL AreNotEqual(const ULONG* mask, int nRow1, int nRow2) const;
   BOOL AreNotEqual(int nRow1, const CuTV& tv) const;
   BOOL AreNotEqual(const ULONG* mask, int nRow1, const CuTV& tv) const;

   BOOL AreOrthog(int nRow1, int nRow2) const;
   BOOL AreOrthog(const ULONG* mask, int nRow1, int nRow2) const;
   BOOL AreOrthog(int nRow1, const CuTV& tv) const;
   BOOL AreOrthog(const ULONG* mask, int nRow1, const CuTV& tv) const;
   BOOL AreOrthog(int nRow1, const CuBV& bv) const;
   BOOL AreOrthog(const ULONG* mask, int nRow1, const CuBV& bv) const;

   BOOL AreNeighb(int nRow1, int nRow2) const;
   BOOL AreNeighb(const ULONG* mask, int nRow1, int nRow2) const;
   BOOL AreNeighb(int nRow1, const CuTV& tv) const;
   BOOL AreNeighb(const ULONG* mask, int nRow1, const CuTV& tv) const;

   BOOL AreConcens(int nRow1, int nRow2) const;
   BOOL AreConcens(const ULONG* mask, int nRow1, int nRow2) const;
   BOOL AreConcens(int nRow1, const CuTV& tv) const;
   BOOL AreConcens(const ULONG* mask, int nRow1, const CuTV& tv) const;

   BOOL IsCover(int nRow1, int nRow2) const;
   BOOL IsCover(const ULONG* mask, int nRow1, int nRow2) const;
   BOOL IsCover(int nRow1, const CuTV& tv) const;
   BOOL IsCover(const ULONG* mask, int nRow1, const CuTV& tv) const;
   BOOL IsCover(int nRow1, const CuBV& bv) const;
   BOOL IsCover(const ULONG* mask, int nRow1, const CuBV& bv) const;

   BOOL IsCovered(int nRow1, const CuTV& tv) const;
   BOOL IsCovered(const ULONG* mask, int nRow1, const CuTV& tv) const;

//******************************* Row interval operations  ******************************
   CuBV Orthogon(int nRow1, int nRow2) const;
   CuBV Orthogon(const ULONG* mask, int nRow1, int nRow2) const;
   CuBV Orthogon(int nRow1, const CuTV& tv) const;
   CuBV Orthogon(const ULONG* mask, int nRow1, const CuTV& tv) const;
   CuBV Orthogon(int nRow1, const CuBV& bv) const;
   CuBV Orthogon(const ULONG* mask, int nRow1, const CuBV& bv) const;

   CuTV Intersec(int nRow1, int nRow2) const;
   CuTV Intersec(const ULONG* mask, int nRow1, int nRow2) const;
   CuTV Intersec(int nRow1, const CuTV& tv) const;
   CuTV Intersec(const ULONG* mask, int nRow1, const CuTV& tv) const;

   CuTV Concens(int nRow1, int nRow2) const;
   CuTV Concens(const ULONG* mask, int nRow1, int nRow2) const;
   CuTV Concens(int nRow1, const CuTV& tv) const;
   CuTV Concens(const ULONG* mask, int nRow1, const CuTV& tv) const;
   CuTV Concens(int nRow1, const CuBV& bv) const;
   CuTV Concens(const ULONG* mask, int nRow1, const CuBV& bv) const;

   CuTV MinCover(int nRow1, int nRow2) const;
   CuTV MinCover(const ULONG* mask, int nRow1, int nRow2) const;
   CuTV MinCover(int nRow1, const CuTV& tv) const;
   CuTV MinCover(const ULONG* mask, int nRow1, const CuTV& tv) const;
   CuTV MinCover(int nRow1, const CuBV& bv) const;
   CuTV MinCover(const ULONG* mask, int nRow1, const CuBV& bv) const;
   CuBM CuTMtoCuBM(void) const; // раскрыть интервалы

//**************************** Operations on TERNARY VECTOR PARTS ***********************
//****************************** Row Logic operations ***********************************
   void AndInPlace(BOOL Part, const CuBV& bv, int Row);
   void OrInPlace(BOOL Part, const CuBV& bv, int Row);
   void XorInPlace(BOOL Part, const CuBV& bv, int Row);
   void SubInPlace(BOOL Part, const CuBV& bv, int Row);
   void InvertBitsInPlace(BOOL Part, int Row);
   void InvertBitsInPlace(BOOL Part);

#ifndef _LINUX
//****************************** Input/Output operations ********************************
    void Serialize(CArchive& ar);
#ifdef _DEBUG
    void Dump(CDumpContext&) const;
    void AssertValid() const;
#endif

#endif

protected:
   ULONG** m_pData;
   int m_nSize;
   int m_nMaxSize;
   int m_nGrowBy;
   int m_nBitLength;
   int m_nLongLength;
   int m_nAllocLength;

//******************************** Protected functions **********************************
    void Init();
    void SafeDelete();
    void AllocMatrix(int nRow, int nColumn);
#ifndef _LINUX
    void CharBit(const CString s);
    void CharBit(const CStringArray& s);
#endif
    void CharBit(const char* s);
};


////////////////////////////////////////////////////////////////////////////////////////
//                 Inlines for CuTM class
////////////////////////////////////////////////////////////////////////////////////////
//***************** Functions for getting class's parametrs **********
inline int CuTM::GetLongLength() const   { return m_nLongLength; }
inline int CuTM::GetAllocLength() const  { return m_nAllocLength; }
inline int CuTM::GetCountC() const       { return m_nBitLength; }
inline int CuTM::GetCountR() const       { return m_nSize; }
inline int CuTM::GetUpperBound() const   { return m_nMaxSize; }
//********************************* Working with memory *********************************
inline void CuTM::RemoveAll()  { SetSize(0,0); }
inline void CuTM::RemoveAllRows()  { SetSize(0,m_nBitLength); }
//********************************** Data Reading ***************************************
inline ULONG* CuTM::GetLongZero(int nRow) const
{ return ( ULONG*)&m_pData[nRow][m_nAllocLength]; }
inline ULONG* CuTM::GetLongOne(int nRow) const
{ return ( ULONG*)&m_pData[nRow]; }
//**************************** Working with rows (changing their number) *****************
inline int CuTM::Add(const CuTV& tv)
{ SetRowGrow(m_nSize, tv); return (m_nSize - 1); }
//******************************* Matrix compareing operations *************************
inline BOOL CuTM::IsEmpty() const   { return ((m_pData==NULL) & (m_nSize==0)); }
inline int CuTM::CountUnDefs(int nRow/*=-1*/) const
{ int i=CountDefs(nRow);
  return ((nRow=-1)?((m_nSize*m_nBitLength)-i):(m_nBitLength-i));
}
//----------------- Init() --------------------------------------------------------------
inline void CuTM::Init()
{ m_pData = NULL;  m_nGrowBy = 10;
  m_nSize = m_nMaxSize =  m_nBitLength = m_nLongLength = m_nAllocLength = 0;
}
/////////////////////////////////////////////////////////////////////////////
// ------------------------------------------ End of declaration of CuTM class
/////////////////////////////////////////////////////////////////////////////

#ifndef _LINUX
#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif

#endif     //_TERNARY_
