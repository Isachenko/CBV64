/////////////////////////////////////////////////////////////////
// Ver.2.1.1     22.02.2007
//               ¬ функции Add можно не указать второго операнда
//               и тогда к текущей добав€тс€ все строки матрицы
//               ѕоправки по конструкторам в св€зи с m_nAllocLength
// Ver.2.1.0     03.07.2006
//               ѕереход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.2.0.5     19.06.2006
//               - поправки BitChar 
// Ver.2.0.3     05.04.2006
//               - поправки BitChar (WithClear,...)
// Ver.2.0.2     26.03.2006
//               - построение символьного представлени€
// Ver.2.0.1     21.03.2006
//               - √енераторы случайных перестроены на возврат *this
// Ver.2.0.0     10.03.2006   Russian == English
//               - ƒобавлены генераторы случайных
//               - ƒобавлены функции записи логических операций на месте
// Ver.1.0     12.02.2005
/////////////////////////////////////////////////////////////////


#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../ComBool/UlongBool.h"

#ifndef _LINUX
IMPLEMENT_SERIAL(CuBM, CObject, 0)
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW

#endif

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// √енераци€ псевдослучайной <nRow>*<nCol> булевой матрицы //
// с равноверо€тным распределением нулей и единиц          //                                      //
/////////////////////////////////////////////////////////////

CuBM CuBM::GenRbm(int nRow, int nCol)
{
  int i;
  CuBV bv;
  SetSize (nRow, nCol); 
  for (i=0; i<nRow; i++) SetRow (i, bv.GenRbv(nCol));
  return *this;
}

/////////////////////////////////////////////////////////////
// √енераци€ псевдослучайной <nRow>*<nCol> булевой матрицы //
// со средним числом nRang едириц в каждой строке          //
// (с дисперсией 1/32) (при nRang>=nCol все элементы матрицы равны 1)                                     //
/////////////////////////////////////////////////////////////

CuBM CuBM::GenRbmMid (int nRow, int nCol, int nRang)
{
  int i;
  CuBV bv;
  SetSize (nRow, nCol); 
  for (i=0; i<nRow; i++) SetRow (i, bv.GenRbvMid(nCol, nRang));
  return *this;
}

/////////////////////////////////////////////////////////////
// √енераци€ псевдослучайной <nRow>*<nCol> булевой матрицы //
// с фиксированным числом <nRang> единиц в каждой строке 
// (при nRang>=nCol все элементы матрицы равны 1)
/////////////////////////////////////////////////////////////

CuBM CuBM::GenRbmFix (int nRow, int nCol, int nRang)
{ 
  int i;
  CuBV bv;
  SetSize (nRow, nCol); 
  for (i=0; i<nRow; i++) SetRow (i, bv.GenRbvFix(nCol, nRang));
  return *this;
}

///*******************************************************// 
/////////////////////////////////////////////////////////////
// √енерирование псевдослучайной <nRow>*<nCol> булевой матрицы 
// с линейно независимыми строками и равноверо€тным распреде-
// лением нулей и единиц.
/////////////////////////////////////////////////////////////

CuBM CuBM::RbmUndRow(int nRow, int nCol)
{
  RemoveAll();
  if(nRow>nCol)
  {
//    TRACE("RbmUndRow: Cтрок больше, чем столбцов!");
    return *this;
  }
  int i, j, k;
  SetSize(nRow, nCol); 
  CuBM BmC(nRow, nCol); 
  CuBV bv, bvH((BYTE)0,nCol);
  for (i=0; i<nRow; i++)
  {
    SetRow(i, bv=bv.GenRbv(nCol));
    for(j=0; j<i; j++)
      if(!(bv&BmC.GetRowBv(j,bvH)).IsZero())
        bv^=BmC.GetRow(j);
    if(bv.IsZero()) {i--; continue;}
    BmC.SetRow(i,bv);
    bvH.SetBitAt(k=bv.LeftOne(-1),1);
    for(j=0; j<i; j++)
      if(BmC.GetBitAt(j,k)) BmC.SetRow(j,BmC.GetRow(j)^bv);
  }
  return *this;
}
/////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Construction/Destruction ///////////////////////////////////////

//****************************** Protected function ***********************************//

//---------------------------------------------------- AllocMatrix(int nRow, int nColumn)
void CuBM::AllocMatrix(int nRow, int nColumn)
{ 
  ASSERT((nRow > 0) && (nColumn > 0));

  m_pData = new ptrdiff_t*[nRow];
  int i, nLong = LEN_LONG(nColumn);
  for (i=0; i < nRow; i++) {
    m_pData[i] = (ptrdiff_t*) new ptrdiff_t[nLong];
    memset(m_pData[i], 0, nLong*sizeof(ptrdiff_t));  // zero fill
  }
  m_nSize = m_nMaxSize = nRow;
  m_nBitLength = nColumn;
  m_nAllocLength = m_nLongLength = nLong;
}

//-------------------------------------------------------------------------- SafeDelete()
void CuBM::SafeDelete()
{
  for (int i=0; i < m_nSize; i++) {
    if (m_pData[i] != NULL) delete [] m_pData[i]; }
  if(m_nMaxSize > 0) delete [] m_pData;
  m_pData = NULL;
}

//******************************* Constructors\Destructor *******************************

//--------------------------------------------------------------------------------- CuBM()
CuBM::CuBM() { Init(); }

//----------------------------------------------- CuBM(const CuBM& bm, BOOL Fl/* = TRUE */)
CuBM::CuBM(const CuBM& bm, BOOL Fl/* = TRUE */)
{
  int i, j;
  if (bm.m_nSize == 0 && bm.m_nBitLength==0) { Init(); return; }
  if (Fl)  {
    AllocMatrix(bm.m_nSize, bm.m_nBitLength);
    for (i=0; i < m_nSize; i++)
      memcpy(m_pData[i], bm.m_pData[i], m_nLongLength*sizeof(ptrdiff_t));
  }
  else {
    AllocMatrix(bm.m_nBitLength, bm.m_nSize);
    for (j = 0; j < m_nBitLength; j++)
      for (i = 0; i < m_nSize; i++) 
        SetBitAt(i, j, bm.GetBitAt(j, i));
  }
  m_nGrowBy = bm.m_nGrowBy;
}

//----------------------------------------- CuBM(int nRow, int nColumn,BOOL Fl/*= FALSE*/)
CuBM::CuBM(int nRow, int nColumn, BOOL Fl /*= FALSE*/)
{
  ASSERT((nRow >= 0) && (nColumn >= 0));
  int i, nLong = LEN_LONG(nColumn), w = S_4 - ADR_BITLONG(nColumn);

  if (nRow == 0 )  {
    if (nColumn==0) Init();
    else {
      m_nSize = m_nMaxSize = 0; m_nAllocLength = m_nLongLength = nLong;
      m_nBitLength = nColumn; m_nGrowBy = 10; m_pData = NULL;
    }
    return;
  }
  AllocMatrix(nRow, nColumn);
  if (Fl)
    for (i=0; i < nRow; i++) {
      memset(m_pData[i], BYTE_1, nLong*sizeof(ptrdiff_t));  /* one's fill*/
      if (w != S_4) m_pData[i][nLong-1] = (m_pData[i][nLong-1] >> w) << w;
    }
  m_nGrowBy = 10;
}

//-------------------------------------------- CuBM(const ULONG* pV, int nRow, int nColumn)
CuBM::CuBM(const ptrdiff_t* pV, int nRow, int nColumn)
{
  if (nRow==0 && nColumn==0) { Init(); return; }
  ASSERT((nRow > 0) && (nColumn > 0));
  int i, nLong = LEN_LONG(nColumn), w = S_4 - ADR_BITLONG(nColumn);
  AllocMatrix(nRow, nColumn);
  for (i=0; i<nRow; i++) {
    memcpy(m_pData[i], pV, nLong*sizeof(ptrdiff_t));  /* one's fill*/
    if (w != S_4) m_pData[i][nLong-1] = (m_pData[i][nLong-1] >> w) << w;
  }
  m_nGrowBy = 10;
}

//------------------------------------------------------------------- CuBM(const char pch)
CuBM::CuBM(const char* pch)
{
#ifndef _LINUX
  CString s(pch);  Init();  if (s.GetLength()) CharBit(s); 
#else
  char* s = strdup(pch); Init(); if (s[0]!=0)  CharBit(s); free(s);
#endif
}

#ifndef _LINUX
//------------------------------------------------------------------- CuBM(CStringArray StrAr)
CuBM::CuBM(const CStringArray& StrAr)
{  Init();  CharBit(StrAr); }
#endif

//-------------------------------------------------------------------------------- ~CuBM()
CuBM::~CuBM() { SafeDelete(); }

/////////////////////////////////////////////////////////////////////////////////////////


//********************************* Working with memory *********************************
//-------------------------------- SetSize(int nRow, int nColumn, int nGrowBy /* = -1 */)
void CuBM::SetSize(int nRow, int nColumn, int nGrowBy /* = -1 */, int nColGrow/*=0*/)
{
  int i, nLong, nmaxRow, nLongmax, w;
#ifdef _DEBUG
  ASSERT_VALID(this);  ASSERT((nRow >= 0) && (nColumn >= 0));
#endif
  if (nGrowBy != -1) m_nGrowBy = nGrowBy;  // set new size
  nmaxRow = nRow + m_nGrowBy; nLongmax = LEN_LONG(nColumn + nColGrow);
  nLong = LEN_LONG(nColumn);
  //------------------ shrink to nothing
  if (nRow == 0) {
    SafeDelete(); m_nBitLength = nColumn;       //Init();
    m_nSize = m_nMaxSize=0; m_nLongLength = nLong;
    m_nAllocLength = nLongmax;
    return;
  }
  //------------------ create one with exact size
  if (m_pData == NULL) { // create one with exact size
    ASSERT((long)nmaxRow*sizeof(ptrdiff_t*) <= ULONG_MAX);  // no overflow
    //AllocMatrix(nRow, nColumn);
    m_pData = (ptrdiff_t**) new ptrdiff_t*[nmaxRow];
    for (i=0; i < nRow; i++) {
      m_pData[i] = (ptrdiff_t*) new ptrdiff_t[nLongmax];
      memset(m_pData[i], 0, nLongmax*sizeof(ptrdiff_t));           // zero fill
    }
    m_nSize = nRow; m_nMaxSize = nmaxRow;
    m_nBitLength = nColumn;  m_nAllocLength =nLongmax;
    m_nLongLength=nLong;
    return;
  }
  //------------------ Set columns size
  if (nLong > m_nAllocLength) {
    ULONG* pV;
    for (i=0; i < m_nSize; i++) {
      pV = (ptrdiff_t*) new ptrdiff_t[nLongmax];
      memset(pV, 0, nLongmax*sizeof(ptrdiff_t));  // zero fill
      if (m_pData[i] != NULL) {
        memcpy(pV, m_pData[i], m_nLongLength*sizeof(ptrdiff_t)); // copy new data from old
        delete [] m_pData[i];
      }
      m_pData[i] = pV;
    }
    m_nAllocLength = nLongmax;
  }
  else {                //when(nLong < m_nAllocLength)
    int nost;
    if (nColumn < m_nBitLength) { w = S_4 - ADR_BITLONG(nColumn);  nost=nLong; }
    else   { w = S_4 - ADR_BITLONG(m_nBitLength);  nost=m_nLongLength; }
    if (w == S_4) w = 0;
    for (i=0; i < m_nSize; i++) {
      memset(m_pData[i] + nost, 0, (m_nAllocLength-nost)*sizeof(ptrdiff_t));
      m_pData[i][nost-1] = (m_pData[i][nost-1] >> w) << w;
    }
  }  
  m_nBitLength = nColumn; m_nLongLength = nLong;
  //------------------ Set rows size
  if (nRow <= m_nMaxSize) {
    if (nRow > m_nSize) {    // it fits // initialize the new elements
      for (i = m_nSize; i < nRow; i++) {
        m_pData[i] = (ptrdiff_t*) new ptrdiff_t[m_nAllocLength];
        memset(m_pData[i], 0, m_nAllocLength*sizeof(ptrdiff_t));
      }
    }
    else 
      for (i = m_nSize-1; i >= nRow; i--) 
        if (m_pData[i] != NULL) { delete [] m_pData[i];  m_pData[i] = NULL; }
    m_nSize = nRow;
    return;
  }
  //-------------- Otherwise grow array
  ASSERT(nRow > m_nSize);
  ASSERT((long)nmaxRow*sizeof(ptrdiff_t*) <= ULLONG_MAX);  // no overflow
  ptrdiff_t** pNewData = (ptrdiff_t**) new ptrdiff_t*[nmaxRow];
  memcpy(pNewData, m_pData, m_nSize*sizeof(ptrdiff_t*)); // copy new data from old
  for (i = m_nSize; i < nRow; i++) {        // construct remaining elements
    pNewData[i] = (ptrdiff_t*) new ptrdiff_t[m_nAllocLength];
    memset(pNewData[i], 0, m_nAllocLength*sizeof(ptrdiff_t));
  }
  for (i = nRow; i < nmaxRow; i++) pNewData[i] = NULL;
 // get rid of old stuff (note: no destructors called)
  delete [] m_pData;  m_pData = pNewData;
  m_nSize = nRow;  m_nMaxSize = nmaxRow;
}


//---------------------------------------------------------------------  FreeExtra()
void CuBM::FreeExtra()
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ptrdiff_t* pV;

  if (m_nLongLength < m_nAllocLength) { // Free row memory
    for (int i=0; i < m_nSize; i++) {
      pV = (ptrdiff_t*) new ptrdiff_t[m_nLongLength];
      memcpy(pV, m_pData[i], m_nLongLength*sizeof(ptrdiff_t)); // copy new data from old
      if (m_pData[i] != NULL) delete [] m_pData[i];
      m_pData[i] = pV;
    }
    m_nAllocLength = m_nLongLength;
  }

  if (m_nSize != m_nMaxSize) { // shrink to desired size
    ASSERT((long)m_nSize*sizeof(ptrdiff_t*) <= ULLONG_MAX);  // no overflow
    ptrdiff_t** pNewData = NULL;
    if (m_nSize != 0) {
      pNewData = (ptrdiff_t**) new ptrdiff_t[m_nSize];
      memcpy(pNewData, m_pData, m_nSize*sizeof(ptrdiff_t*));       // copy new data from old
    }
    // get rid of old stuff (note: no destructors called)
    delete [] m_pData;
    m_pData = pNewData;
    m_nMaxSize = m_nSize;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _LINUX
//********************************** Reading the data ***********************************
//----------------------------------------------- BitChar(char One,char Zero)
void CuBM::BitChar(CStringArray & StrRes, BOOL WithClear, BOOL WithNum, 
                   char One,char Zero)
{ 
  int i,j,pos = 0;
  CString ch;
  if (WithClear) StrRes.RemoveAll();
  if (m_nSize>9999)     { ch = "%5d: ";}
  else if (m_nSize>999) { ch = "%4d: ";}
  else if (m_nSize>99)  { ch = "%3d: ";}
  else                  { ch = "%2d: ";}

  for (j=0;j<m_nSize;j++) {
    CString s;
    if(WithNum == TRUE) s.Format((const char*)ch,j);
    for (i=0;i<m_nBitLength;i++)
      if (GetBitAt(j,i)) s+=One;
      else               s+=Zero;
    StrRes.Add(s);
  }
}

//----------------------------------------------------------- BitChar(char One,char Zero)
CString CuBM::BitChar(char One, char Zero, BOOL WithNum)
{
  int i,j,pos = 0;
  CString ch, s;
  if(WithNum == TRUE) {
    if (m_nSize>9999)     { ch = "%5d: "; i=9;}
    else if (m_nSize>999) { ch = "%4d: "; i=8;}
    else if (m_nSize>99)  { ch = "%3d: "; i=7;}
    else                  { ch = "%2d: "; i=6;}
  }
  else i = 2;

  CString res('\0',(m_nBitLength+i) * m_nSize);
  for (j=0;j<m_nSize;j++) {
    if(WithNum == TRUE) {
      s.Format((const char*)ch,j);
      for (i=0;i<m_nSize;i++) 
        res.SetAt(pos++,s[i]);
    }
   for (i=0;i<m_nBitLength;i++)
     if (GetBitAt(j,i)) res.SetAt(pos++,One);
     else               res.SetAt(pos++,Zero);
   res.SetAt(pos++,'\n');  //res.SetAt(pos++,'\r'); 
  }
  return res;
}
#else
//----------------------------------------------------------- BitChar(char One,char Zero)
char* CuBM::BitChar(char One, char Zero, BOOL WithNum)
{
  int i,j,pos = 0;
  char ch[20], s[20];
  if(WithNum == TRUE) {
    if (m_nSize>9999)     { strcpy(ch,"%5d: "); i=9;}
    else if (m_nSize>999) { strcpy(ch,"%4d: "); i=8;}
    else if (m_nSize>99)  { strcpy(ch,"%3d: "); i=7;}
    else                  { strcpy(ch,"%2d: "); i=6;}
  }
  else i = 2;
  char* res;
  res = (char*)malloc((m_nBitLength+i) * m_nSize+1);
  res[(m_nBitLength+i) * m_nSize] = 0;
  for (j=0;j<m_nSize;j++) {
    if(WithNum == TRUE) {
      sprintf(s,(const char*)ch,j);
      for (i=0;i<m_nSize;i++) res[pos++] = s[i];
    }
    for (i=0;i<m_nBitLength;i++)
      if (GetBitAt(j,i)) res[pos++] = One;
      else               res[pos++] = Zero;
    res[pos++] = '\n';  //res[pos++] = '\r'; 
  }
  return res;
}
#endif

//---------------------------------------------------------------GetRowBv(int nIndex)
CuBV CuBM::GetRowBv(int nIndex) const
{
  ASSERT(nIndex >= 0 && nIndex < m_nSize);
  CuBV bv(m_pData[nIndex], m_nBitLength);
  return bv;
}
 
//---------------------------------------------------------------GetRowBv(int nIndex)
CuBV CuBM::GetRowBv(int nIndex, ULONG* mask) const
{ 
  ASSERT(nIndex >= 0 && nIndex < m_nSize);
  CuBV bv(m_pData[nIndex], m_nBitLength);
  bv &= mask;
  return bv;
}

//---------------------------------------------------------------GetColumnBv(int nColumn)
CuBV CuBM::GetColumnBv(int nColumn) const
{
  ASSERT((nColumn >= 0) && (nColumn < m_nBitLength));
  CuBV bv(m_nSize, FALSE);
  for (int i = 0; i < m_nSize; i++) bv.SetBitAt(i, GetBitAt(i,nColumn));
  return bv;
}

//---------------------------------------------------------------GetColumnBv(int nColumn)
CuBV CuBM::GetColumnBv(int nColumn, ULONG* mask) const
{
  ASSERT((nColumn >= 0) && (nColumn < m_nBitLength));
  CuBV bv(m_nSize, FALSE);
  for (int i = 0; i < m_nSize; i++) bv.SetBitAt(i, GetBitAt(i,nColumn));
  bv &= mask;
  return bv;
}

//------------------------------------------------ExtractRows(int nFirst, int nCount)
CuBM CuBM::ExtractRows(int nFirst, int nCount) const
{
  ASSERT((nFirst >= 0) && ((nFirst + nCount) <= m_nSize) &&(nCount > 0));
  CuBM bm(nCount, m_nBitLength);
  for (int i = 0; i < nCount; i++) bm.SetRow(i, GetRow(nFirst + i));
  return bm;
}

//------------------------------------------------ExtractColumns(int nFirst, int nCount)
CuBM CuBM::ExtractColumns(int nFirst, int nCount) const
{
  ASSERT((nFirst >= 0) && (nCount > 0) && ((nFirst + nCount) <= m_nBitLength));
  CuBM bm(m_nSize, nCount);
  CuBV bv(m_nBitLength, FALSE);
  CuBV bv1;
 
  for (int i = 0; i < m_nSize; i++)  { 
    bv = m_pData[i]; 
    bv1 = bv.Extract(nFirst, nCount);
    bm.SetRow(i, bv1);
  }
  return bm;
}

//------------- Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn)
CuBM CuBM::Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const
{
  ASSERT(nFirstRow >= 0); ASSERT(nFirstColumn >= 0);
  ASSERT((nFirstColumn + nCountColumn) <= m_nBitLength);
  ASSERT((nFirstRow + nCountRow) <= m_nSize);
  ASSERT(nCountColumn > 0);  ASSERT(nCountRow > 0);
  CuBM bm(nCountRow, nCountColumn); 
  CuBV bv(m_nBitLength, FALSE);
  CuBV bv1;
 
  for (int i = nFirstRow; i < nFirstRow + nCountRow; i++)  { 
    bv = m_pData[i];
    bv1 = bv.Extract(nFirstColumn, nCountColumn);
    bm.SetRow(i, bv1);
  }
  return bm;
}

//-------------------------------------------- Minor(const CuBV& Rows, const CuBV& Columns)
CuBM CuBM::Minor(const CuBV& Rows, const CuBV& Columns) const
{
  int i, j, LeftOneRow = -1, LeftOneColumn = -1;
  int NewRowLen, NewColumnLen;
  NewRowLen = Rows.CountBit();
  NewColumnLen = Columns.CountBit();
  CuBM bm(NewRowLen, NewColumnLen);
  for (i=0; i < NewRowLen; i++)  {
    LeftOneRow = Rows.LeftOne(LeftOneRow);
    for (j=0; j < NewColumnLen; j++)  {
      LeftOneColumn = Columns.LeftOne(LeftOneColumn);
      bm.SetBitAt(i, j, GetBitAt(LeftOneRow, LeftOneColumn));
    }
    LeftOneColumn = -1;
  }
  return bm;
}

//------------------------------------------------------------------------- Trans() const
CuBM CuBM::Trans() const { CuBM bm(*this, FALSE);  return bm; }

/////////////////////////////////////////////////////////////////////////////////////////

//********************************* Working with data ***********************************
//-------------------------------------------------------SetRow(int nRow, const CuBV& bv)
void CuBM::SetRow(int nRow, const CuBV& bv)
{
  ASSERT(nRow >= 0 && nRow < m_nSize);  ASSERT(m_nBitLength == bv.GetBitLength());
  memcpy(m_pData[nRow], bv, m_nLongLength*sizeof(ptrdiff_t));
}

//--------------------------------------------SetRow(int nRow, const CuBM& bm, int nbmRow)
void CuBM::SetRow(int nRow, const CuBM& bm, int nbmRow)
{ 
  ASSERT(nRow >= 0 && nRow < m_nSize);  ASSERT(m_nBitLength == bm.GetCountC());
  memcpy(m_pData[nRow], bm.m_pData[nbmRow], m_nLongLength*sizeof(ptrdiff_t));
}

/////////////////////////////////////////////////////////////////////////////////////////

//********************************** Working with row ***********************************
//----------------------------------------------SetRowGrow(int nRow, const ULONG* newRow)
void CuBM::SetRowGrow(int nRow, const ULONG* newRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0);
#endif
  if (nRow >= m_nSize)
    SetSize(nRow + 1, m_nBitLength, m_nGrowBy, m_nAllocLength*S_8 - m_nBitLength);
  memcpy(m_pData[nRow], newRow, m_nLongLength*sizeof(ptrdiff_t));
}

//----------------------------------------------SetRowGrow(int nRow, const CuBV& bv)
void CuBM::SetRowGrow(int nRow, const CuBV& bv)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0);
  if (m_nSize > 0)  ASSERT(m_nBitLength == bv.GetBitLength());
#endif
  if (nRow >= m_nSize) 
    SetSize(nRow + 1, bv.GetBitLength(), m_nGrowBy, 
                      bv.GetAllocLength()*S_8 - m_nBitLength);
  memcpy(m_pData[nRow], bv, m_nLongLength*sizeof(ptrdiff_t));
}

//---------------------------------------------SetRowGrow(int nRow, const CuBM& bm, int nbmRow)
void CuBM::SetRowGrow(int nRow, const CuBM& bm, int nbmRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0 && nbmRow < bm.m_nSize);
  if (m_nSize > 0) ASSERT(m_nBitLength == bm.m_nBitLength);
#endif
  if (nRow >= m_nSize)
    SetSize(nRow+1, m_nBitLength, m_nGrowBy, m_nAllocLength*S_8 - m_nBitLength);
  memcpy(m_pData[nRow], bm.m_pData[nbmRow], m_nLongLength*sizeof(ptrdiff_t));
}

//-------------------------------------------- Add -------------------------
int CuBM::Add(BOOL bit/*=FALSE*/, int nCount/*=1*/)
{
  int i, first = m_nSize;
  if(m_nSize + nCount < m_nMaxSize) {
    m_nSize += nCount;
    for(i = first; i < m_nSize; i++) {
      m_pData[i] = (ptrdiff_t*) new ptrdiff_t[m_nAllocLength];
      memset(m_pData[i], 0, m_nAllocLength*sizeof(ptrdiff_t));
    }
  }
  else 
    SetSize(m_nSize + nCount, m_nBitLength, m_nGrowBy, m_nAllocLength*S_8 - m_nBitLength);
  if (bit == TRUE) {
    ULONG maska=((i = ADR_BITLONG(m_nBitLength)) > 0 ) ?
                (LONG_1 << (S_8 - i)) : LONG_1;
    for (; first < m_nSize; first++) {
      memset(m_pData[first], BYTE_1, (m_nLongLength-1)*sizeof(ptrdiff_t));
      m_pData[first][m_nLongLength-1] = maska;
    }
  }
  return m_nSize-1;
}

//-----------------------------------Add(const CuBM& bm, int nbmRow/* =-1*/)
int CuBM::Add(const CuBM& bm, int nbmRow/* =-1*/)
{ int ind = m_nSize;
  if (nbmRow>=0)  SetRowGrow(ind, bm, nbmRow); 
  else InsertRow(ind, 0, bm, bm.GetCountR());
  return (ind); 
}

//-----------------------------------InsertRow(int nRow, const ULONG* newRow, int nCount)
void CuBM::InsertRow(int nRow, const ULONG* newRow, int nCount /*=1*/)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT((nRow >= 0) && (nCount > 0));
#endif
  ptrdiff_t* pV;
  if (nRow >= m_nSize) {   // adding after the end of the array
    SetSize(nRow + nCount, m_nBitLength, m_nGrowBy, m_nAllocLength*S_8 - m_nBitLength);
  }
  else {                   // inserting in the middle of the array
    SetSize(m_nSize + nCount, m_nBitLength, m_nGrowBy, m_nAllocLength*S_8 - m_nBitLength);
    for (int i=0; i < nCount; i++) {
      pV = m_pData[m_nSize-1];
      memmove(&m_pData[nRow+1], &m_pData[nRow],(m_nSize - nRow-1)*sizeof(ptrdiff_t*));
      m_pData[nRow] = pV;
    }
  }
  // insert new value in the gap
  for (int i = nRow; i < nRow+nCount; i++)
    memcpy(m_pData[i], newRow, m_nLongLength*sizeof(ptrdiff_t));
}

//---------------------------------InsertRow(int nRow, const CuBV& newRow, int nCount)
void CuBM::InsertRow(int nRow, const CuBV& newRow, int nCount)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT((newRow != NULL) && (nRow >= 0));
  ASSERT(newRow.GetLongLength() == m_nLongLength);
#endif
  InsertRow(nRow, (const ptrdiff_t*)newRow, nCount);
}

//---------------------InsertRow(int nRow, int nStartRow, const CuBM& pNewBM, int nCount)
void CuBM::InsertRow(int nRow, int nStartRow, const CuBM& NewBM, int nCount)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT((nRow >= 0) && (nStartRow >= 0)); 
  ASSERT((nStartRow + nCount <= NewBM.m_nSize) && 
         (m_nBitLength == NewBM.m_nBitLength));
#endif
  if (NewBM.m_nSize > 0)    {
    InsertRow(nRow, NewBM.m_pData[nStartRow], nCount);
    for (int i = 1; i < nCount; i++)
      SetRow(nRow + i, NewBM.m_pData[nStartRow + i]);
  }
}

//-----------------------------------------RemoveRow(int nIndex, int nCount)
void CuBM::RemoveRow(int nIndex, int nCount /* = 1 */)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT((nIndex >= 0) && (nCount >= 0));
  ASSERT(nIndex + nCount <= m_nSize);
#endif
  int nMoveCount = m_nSize - (nIndex + nCount);     // just remove a range
  for (int i=0; i < nCount; i++) 
    if (m_pData[i+nIndex] != NULL) delete [] m_pData[i + nIndex];
  if (nMoveCount) {
      memcpy(&m_pData[nIndex], &m_pData[nIndex + nCount], nMoveCount*sizeof(ptrdiff_t*));
  }
  m_nSize -= nCount;
}

//----------------------------------------ExchangeRow(int nRow1, int nRow2, int nCount))
void CuBM::ExchangeRow(int nRow1, int nRow2, int nCount/*=1*/)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT((nCount > 0) && (nRow1 + nCount <= m_nSize) && 
         (nRow2 + nCount <= m_nSize));
#endif
  ptrdiff_t *pV;
  while(nCount >0) {
    pV = m_pData[nRow1]; m_pData[nRow1++] = m_pData[nRow2];
    m_pData[nRow2++] = pV; nCount--; 
  }
}

//----------------- SortDecr ------------------------------------------------------------
void CuBM::SortDecr(int nBeg/*=0*/, int number/*=-1*/)
{
  ASSERT(number == -1 || number > 1);
  BOOL bNotDone = TRUE;
  int pos, posNext, nEnd = (number < 0)? m_nSize : (nBeg + number);
  ASSERT(nEnd <= m_nSize);
  while (bNotDone) {
    bNotDone = FALSE;
    for(pos = nBeg, posNext = nBeg+1; pos < nEnd-1; pos++, posNext++) {
      if(CountBit(pos) < CountBit(posNext)) {
        ExchangeRow(pos, posNext);  bNotDone = TRUE;
      }
    }
  }
}

//----------------- SortIncr ------------------------------------------------------------
void CuBM::SortIncr(int nBeg/*=0*/, int number/*=-1*/)
{
  ASSERT(number == -1 || number > 1);
  BOOL bNotDone = TRUE;
  int pos, posNext, nEnd = (number < 0)? m_nSize : (nBeg + number);
  ASSERT(nEnd <=m_nSize);
  while (bNotDone) {
    bNotDone = FALSE;
    for(pos = nBeg, posNext = nBeg+1; pos < nEnd-1; pos++, posNext++) {
      if(CountBit(pos) > CountBit(posNext)) {
        ExchangeRow(pos,posNext);  bNotDone = TRUE;
      }
    }
  }
}

//----------------- AddCol -----------------------------------------------------------
int CuBM::AddCol(int numCol/*=1*/) //numCol columns adding to the matrix
                                  //return the number of new last column
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(numCol > 0);
#endif
  int i, nLong;
  m_nBitLength += numCol;
  nLong = LEN_LONG(m_nBitLength);
  //------------------ Set columns size
  if (nLong > m_nAllocLength) {
    ptrdiff_t * pV;
    for (i=0; i < m_nSize; i++) {
      pV = (ptrdiff_t*) new ptrdiff_t[nLong];
      memset(pV, 0, nLong*sizeof(ptrdiff_t));  // zero fill
      memcpy(pV, m_pData[i], m_nLongLength*sizeof(ptrdiff_t)); // copy new data from old
      delete [] m_pData[i];
      m_pData[i] = pV; 
    }
    m_nAllocLength = nLong;
  }
  m_nLongLength = nLong;
  return (m_nBitLength - 1);
}

/////////////////////////////////////////////////////////////////////////////////////////
//*********************** Operators and functions of assignment *************************
//------------------------------------------------------ operator =(const CuBM& bm) const
const CuBM& CuBM::operator =(const CuBM& bm)
{
  int i, w = bm.m_nSize;
  SetSize(w, bm.m_nBitLength, m_nGrowBy);
  for (i=0; i < w; i++) SetRow(i, bm.GetRow(i));
  return *this;
}

//------------------------------------------------------ operator =(const char* pch)
//const CuBM& CuBM::operator =(const char* pch)
const CuBM& CuBM::operator =(char* pch)
{ 
 CharBit(pch); 
 return *this; }

#ifndef _LINUX
//------------------------------------------------------ operator =(CStringArray StrAr)
const CuBM& CuBM::operator =(const CStringArray& StrAr)
{ CharBit(StrAr);  return *this; }
#endif

//----------------------------------------------------------------------- One(int nRow)
void CuBM::One(int nRow)
{
  int i, first, last;
  ASSERT (nRow>=-1);
  if (nRow == -1) { first=0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  i = S_8 - ADR_BITLONG(m_nBitLength);
  for (; first <= last; first++) {
    memset(m_pData[first], BYTE_1, m_nLongLength*sizeof(ptrdiff_t));
    if (i != S_8)
      m_pData[first][m_nLongLength-1] = (m_pData[first][m_nLongLength-1] >> i) << i;
  }
}

//----------------------------------------------------------------------- Zero(int nRow)
void CuBM::Zero(int nRow)
{
  int first, last;
  ASSERT (nRow>=-1);
  if (nRow == -1) { first=0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  for (; first <= last; first++)
    memset(m_pData[first], 0, m_nLongLength*sizeof(ptrdiff_t));
}

/////////////////////////////////////////////////////////////////////////////////////////

//**************************** Operations of concatinations *****************************
//----------------------------------------------------------------- Concat(const CuBM& bm)
void CuBM::Concat(const CuBM& bm)
{
  ASSERT(m_nSize == bm.m_nSize);
  int i, j, OldLen = BIT_LONG(m_nBitLength), l_bit, r_bit;
  l_bit = ADR_BITLONG(m_nBitLength);  r_bit = S_4 - l_bit;
  SetSize(m_nSize, m_nBitLength + bm.m_nBitLength, m_nGrowBy);
  for (i=0; i < m_nSize; i++)  {
    m_pData[i][OldLen] |= (bm.m_pData[i][0] >> l_bit);
    for (j = 1; j < bm.m_nLongLength; j++)
      m_pData[i][j + OldLen] = (bm.m_pData[i][j-1] << r_bit) | (bm.m_pData[i][j] >> l_bit);
    m_pData[i][j + OldLen] |=  (bm.m_pData[i][j-1] << r_bit); 
  }
}

//------------------------------------------------- Concat(const CuBM& bm1, const CuBM& bm2)
void CuBM::Concat(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT(bm1.m_nSize == bm2.m_nSize);
  SetSize(bm1.m_nSize, bm1.m_nBitLength + bm2.m_nBitLength, m_nGrowBy);
  int i, j, OldLen = BIT_LONG(bm1.m_nBitLength), l_bit, r_bit;
  l_bit = ADR_BITLONG(bm1.m_nBitLength);  r_bit = S_4 - l_bit;
  for (i=0; i < m_nSize; i++)
    memcpy(m_pData[i], bm1.m_pData[i], bm1.m_nLongLength*sizeof(ULONG));
  for (i=0; i < m_nSize; i++)  {
    m_pData[i][OldLen] |= (bm2.m_pData[i][0] >> l_bit);
    for (j = 1; j < bm2.m_nLongLength; j++)
      m_pData[i][j + OldLen] = (bm2.m_pData[i][j-1] << r_bit) | (bm2.m_pData[i][j] >> l_bit);
    m_pData[i][j + OldLen] |= bm2.m_pData[i][j-1] << r_bit;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////

//************************** Operators of advanced assignment ***************************
//------------------------------------------------------- operator|=(const CuBM& bm) 
const CuBM& CuBM::operator|=(const CuBM& bm)
{
  ASSERT((m_nSize == bm.m_nSize) && (m_nBitLength == bm.m_nBitLength));
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] |= bm.m_pData[i][j];
  return *this;
}

//------------------------------------------------------- operator|=(const CuBV& bv) 
const CuBM& CuBM::operator|=(const CuBV& bv)
{
  ASSERT(m_nBitLength == bv.GetBitLength());
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] |= bv.GetLongAt(j);
  return *this;
}

//------------------------------------------------------- operator|=(const ULONG * pb)
const CuBM& CuBM::operator|=(const ULONG* pV)
{
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] |= pV[j];
  return *this;
}

//------------------------------------------------------- operator&=(const CuBM& bm)
const CuBM& CuBM::operator&=(const CuBM& bm)
{
  ASSERT((m_nSize == bm.m_nSize) && (m_nBitLength == bm.m_nBitLength));
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] &= bm.m_pData[i][j];
  return *this;
}

//------------------------------------------------------- operator&=(const CuBV& bv)
const CuBM& CuBM::operator&=(const CuBV& bv)
{
  ASSERT(m_nBitLength == bv.GetBitLength());
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] &= bv.GetLongAt(j);
  return *this;
}

//------------------------------------------------------- operator&=(const ULONG *pb)
const CuBM& CuBM::operator&=(const ULONG *pV)
{
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] &= pV[j];
  return *this;
}

//------------------------------------------------------- operator^=(const CuBM& bm)
const CuBM& CuBM::operator^=(const CuBM& bm)
{
  ASSERT((m_nSize == bm.m_nSize) && (m_nBitLength == bm.m_nBitLength));
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] ^= bm.m_pData[i][j];
  return *this;
}

//------------------------------------------------------- operator^=(const CuBV& bv)
const CuBM& CuBM::operator^=(const CuBV& bv)
{
  ASSERT(m_nBitLength == bv.GetBitLength());
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] ^= bv.GetLongAt(j);
  return *this;
}

//------------------------------------------------------- operator^=(const ULONG *pb)
const CuBM& CuBM::operator^=(const ULONG *pV)
{                                              // may be new '1' on the end in last long
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] ^= pV[j];
  return *this;
}

//------------------------------------------------------- operator-=(const CuBV& bv)
const CuBM& CuBM::operator-=(const CuBV& bv)
{
  ASSERT(m_nBitLength == bv.GetBitLength());
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] -= bv.GetLongAt(j);
  return *this;
}

//------------------------------------------------------- operator-=(const ULONG *pb)
const CuBM& CuBM::operator-=(const ULONG *pV)
{
  for (int i=0; i < m_nSize; i++)
    for (int j=0; j < m_nLongLength; j++)
      m_pData[i][j] -= pV[j];
  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

//********************************** Logic operations ***********************************
//--------------------------------------- operator|(const CuBM& bm1, const CuBM& bm2)
CuBM operator|(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT((bm1.m_nSize == bm2.m_nSize) && 
         (bm1.m_nBitLength == bm2.m_nBitLength));
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] | bm2.m_pData[i][j];
 return bm;
}

//--------------------------------------- operator|(const CuBM& bm1, const CuBV& bv2)
CuBM operator|(const CuBM& bm1, const CuBV& bv)
{
  ASSERT(bm1.m_nBitLength == bv.GetBitLength());
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
   for (int j=0; j < bm1.m_nLongLength; j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] | bv.GetLongAt(j);
  return bm;
}

//--------------------------------------- operator|(const CuBM& bm1, const ULONG *pb)
CuBM operator|(const CuBM& bm1, const ULONG *pV)
{
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] | pV[j];
  return bm;
}

//------------------------------------------------------------------------- Disjunction()
CuBV CuBM::Disjunction()
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CuBV bv(m_nBitLength, FALSE);
  ULONG val;
  for (int i=0; i < m_nLongLength; i++)  {
    val = 0;
    for (int j=0; j < m_nSize; j++) val |= m_pData[j][i];
    bv.SetLongAt(i, val);
  }
  return bv;
}

//--------------------------------------- operator&(const CuBM& bm1, const CuBM& bm2)
CuBM operator&(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT((bm1.m_nSize == bm2.m_nSize) && (bm1.m_nBitLength == bm2.m_nBitLength));
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] & bm2.m_pData[i][j];
  return bm;
}

//--------------------------------------- operator&(const CuBM& bm1, const CuBV& bv2)
CuBM operator&(const CuBM& bm1, const CuBV& bv)
{
  ASSERT(bm1.m_nBitLength == bv.GetBitLength());
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] & bv.GetLongAt(j);
  return bm;
}

//--------------------------------------- operator&(const CuBM& bm1, const ULONG *pb)
CuBM operator&(const CuBM& bm1, const ULONG *pV)
{
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
   for (int j=0; j < bm1.m_nLongLength; j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] & pV[j];
  return bm;
}

//------------------------------------------------------------------------- Conjunction()
CuBV CuBM::Conjunction()
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CuBV bv(m_nBitLength, FALSE);
  ULONG val;
  for (int i=0; i < m_nLongLength; i++)  {
    val = LONG_1;
    for (int j=0; j < m_nSize; j++) val &= m_pData[j][i];
    bv.SetLongAt(i, val);
  }
  return bv;
}

//--------------------------------------- operator^(const CuBM& bm1, const CuBM& bm2)
CuBM operator^(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT((bm1.m_nSize == bm2.m_nSize) && (bm1.m_nBitLength == bm2.m_nBitLength));
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] ^ bm2.m_pData[i][j];
  return bm;
}

//--------------------------------------- operator^(const CuBM& bm1, const CuBV& bv2)
CuBM operator^(const CuBM& bm1, const CuBV& bv)
{
  ASSERT(bm1.m_nBitLength == bv.GetBitLength());
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] ^ bv.GetLongAt(j);
  return bm;
}

//--------------------------------------- operator^(const CuBM& bm1, const ULONG *pb)
CuBM operator^(const CuBM& bm1, const ULONG *pV)
{
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] ^ pV[j];
  return bm;
}

//------------------------------------------------------------------------- BitwiseExclusive()
CuBV CuBM::BitwiseExclusive()
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CuBV bv(m_nBitLength, FALSE);
  ULONG val;
  for (int i=0; i < m_nLongLength; i++)  {
    val = 0;
    for (int j=0; j < m_nSize; j++) val ^= m_pData[j][i];
    bv.SetLongAt(i, val);
  }
  return bv;
}

//--------------------------------------- operator-(const CuBM& bm1, const CuBV& bv2)
CuBM operator-(const CuBM& bm1, const CuBV& bv)
{
  ASSERT(bm1.m_nBitLength == bv.GetBitLength());
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] - bv.GetLongAt(j);  //?????? 
  return bm;
}

//--------------------------------------- operator-(const CuBM& bm1, const ULONG *pb)
CuBM operator-(const CuBM& bm1, const ULONG *pV)
{
  CuBM bm(bm1.m_nSize, bm1.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)
    for (int j=0; j < bm1.m_nLongLength; j++)
      bm.m_pData[i][j] = bm1.m_pData[i][j] - pV[j];  //?????? 
  return bm;
}

//------------------------------------------------------- operator~(const CuBM& bm)
CuBM operator~(const CuBM& bm)
{
#ifdef _DEBUG
  ASSERT_VALID(&bm);
#endif
  int j;
  CuBM bm1(bm.m_nSize, bm.m_nBitLength);
  for (int i=0; i < bm1.m_nSize; i++)   {
    for (j=0; j < bm1.m_nLongLength; j++)
      bm1.m_pData[i][j] = ~bm.m_pData[i][j];
    if ((j = ADR_BITLONG(bm1.m_nBitLength)) > 0 ) {
      j = S_4 - j;
    bm1.m_pData[i][bm1.m_nLongLength - 1] = (bm1.m_pData[i][bm1.m_nLongLength - 1] >> j) << j;
    }
  }
  return bm1;
}

/////////////////////////////////////////////////////////////////////////////////////////

//******************** Operations of weighting, finding and casing **********************
//-------------------------------------------------------------------- CountBit(int nRow)
int CuBM::CountBit(int nRow) const
{
  int j, one=0, first, last, len;
  ASSERT (nRow >= -1);
  if (nRow == -1) { first = 0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  len = m_nLongLength<<2;
  for (; first <= last; first++) {
    BYTE * pB = (BYTE *)m_pData[first];
    for (j=0; j < len; j++) 
      one += TabC[pB[j]];
  }
  return one;
}

//-------------------------------------------------------------------- CountBit(int nRow)
int CuBM::CountBit(const ULONG* mask, int nRow) const 
{
  int j, one=0, first, last, len;
  ASSERT (nRow >= -1);
  if (nRow == -1) { first = 0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  len = m_nLongLength<<2;
  BYTE * pMask =(BYTE *)mask; 
  for (; first <= last; first++) {
    BYTE * pB = (BYTE *)m_pData[first];
    for (j=0; j < len; j++) 
      one += TabC[pB[j]&pMask[j]];
  }
  return one;

}

//---------------------------------------------------------- LeftOne(int nRow, int nNext)
int CuBM::LeftOne(int nRow, int nNext) const
{
  int i, j, pos = 0;
  ULONG val;
  ASSERT((m_nBitLength - nNext) >= 0);
  if (nNext == -1) { i=0;  j=0; }
  else { i = BIT_LONG(nNext);  j = ADR_BITLONG(nNext);  j++; }
  if (j == S_4)  { j=0;  i++; }
  if (i == m_nLongLength)  return (-1);
  val = m_pData[nRow][i] << j;
  if (val)  {
    while (!(val & OB4[pos++]));
    return (nNext + pos);
  }
  for (j = i+1; j < m_nLongLength; j++)
    if (m_pData[nRow][j])  {
      while (!(m_pData[nRow][j] & OB4[pos++]));
      return ((j*S_4) + pos - 1);
    }
  return (-1);
}


//----------------------------------------------- LeftOne(int nRow, int nNext,ULONG* mask)
int CuBM::LeftOne(int nRow, int nNext, ULONG* mask) const
{ 
  int i, j, pos = 0;
  ULONG val;
  ASSERT((m_nBitLength - nNext) >= 0);
  if (nNext == -1) { i=0;  j=0; }
  else { i = BIT_LONG(nNext);  j = ADR_BITLONG(nNext);  j++; }
  if (j == S_4)  { j=0;  i++; }
  if (i == m_nLongLength)  return (-1);
  val = (m_pData[nRow][i] & mask[i]) << j;
  if (val)  {
    while (!(val & OB4[pos++]));
    return (nNext + pos);
  }
  for (j = i+1; j < m_nLongLength; j++)
    if (m_pData[nRow][j] & mask[j])  {
      while (!(m_pData[nRow][j] & mask[j] & OB4[pos++]));
      return ((j*S_4) + pos - 1);
    }
  return (-1);
}

//------------------------------------------------------------------- RightOne(int nPrev)
int CuBM::RightOne(int nRow, int nPrev) const
{
  int i, j, pos = S_4 - 1;
  ULONG val;
  ASSERT((m_nBitLength - nPrev) >= 0);
  if (nPrev == -1) { i = m_nLongLength - 1;  j=0; }
  else { i = BIT_LONG(nPrev);  j = S_4 - ADR_BITLONG(nPrev); }
  if (j==S_4)  { j=0;  i--; }
  val = (m_pData[nRow][i] >> j) << j;
  if (val)  {
    while (!(val & OB4[pos--]));
    return ((i*S_4) + pos + 1);
  }
  for (j = i-1; j >= 0; j--)
    if (m_pData[nRow][j])  {
      while (!(m_pData[nRow][j] & OB4[pos--]));
      return ((j*S_4) + pos + 1);
    }
  return (-1);
}

//------------------------------------------------------------------- RightOne(int nPrev)
int CuBM::RightOne(int nRow, int nPrev, ULONG* mask) const
{
  int i, j, pos = S_4 - 1;
  ULONG val;
  ASSERT((m_nBitLength - nPrev) >= 0);
  if (nPrev == -1) { i = m_nLongLength - 1;  j=0; }
  else { i = BIT_LONG(nPrev);  j=S_4 - ADR_BITLONG(nPrev); }
  if (j == S_4)  { j=0;  i--; }
  val = ((m_pData[nRow][i] & mask[i]) >> j) << j;
  if (val)  {
    while (!(val & OB4[pos--]));
    return ((i*S_4) + pos + 1);
  }
  for (j = i-1; j >= 0; j--)
    if (m_pData[nRow][j] & mask[j])  {
      while (!(m_pData[nRow][j] & mask[j] & OB4[pos--]));
      return ((j*S_4) + pos + 1);
    }
  return (-1);
}

//------------------------------------------------------- FindRow (BOOL Empty, int nFRow)
int CuBM::FindRow (BOOL Empty, int nFRow) const
{
  ULONG work; 
  if (m_nSize == 0) return -1;
  ASSERT(nFRow < m_nSize-1);
  nFRow++; 
  for (int i = nFRow; i <= m_nSize-1; i++)  {
    work=0;
    for (int j=0; j < m_nLongLength; j++)   work |= m_pData[i][j];
    if (work == 0 && Empty == 1) return i;
    if (work != 0 && Empty == 0) return i;
  }   
  return -1;
}

//------------------------------------------ FindRow (BOOL Empty, ULONG* mask, int nFRow)
int CuBM::FindRow (BOOL Empty, ULONG* mask, int nFRow) const
{
  ULONG work; 
  if (m_nSize == 0) return -1;
  ASSERT(nFRow < m_nSize-1);
  nFRow++; 
  for (int i = nFRow; i < m_nSize; i++)  {
    work=0;
    for (int j=0; j < m_nLongLength; j++)   work |= m_pData[i][j] & mask[j];
    if (work == 0 && Empty == 1) return i;
    if (work != 0 && Empty == 0) return i;
  }   
  return -1;
}

//------------------------------------------------- FindRow (const CuBV& bv, int nFRow)
int CuBM::FindRow (const CuBV& bv, int nFRow) const
{
  if (m_nSize == 0) return -1;       //new 24.01.00    
  ASSERT(nFRow < m_nSize-1);
  nFRow++; 
  for (int i = nFRow; i < m_nSize; i++)
    if (memcmp(bv, m_pData[i], m_nLongLength*sizeof(ULONG)) == 0) return i;
  return -1;
}

//------------------------------------------------- FindRow (const ULONG* pt, int nFRow)
int CuBM::FindRow (const ULONG* pV, int nFRow) const
{
  if (m_nSize == 0) return -1;       //new 24.01.00    
  ASSERT(nFRow < m_nSize-1);
  nFRow++; 
  for (int i = nFRow; i < m_nSize; i++)
    if (memcmp(pV, m_pData[i], m_nLongLength*sizeof(ULONG)) == 0) return i;
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////

//******************************* Compareing operations *********************************
//-----------------------------------------------------------IsZero(int nRow)
BOOL CuBM::IsZero(int nRow) const
{
  int i, first, last;
  ASSERT (nRow >= -1);
  if (m_nLongLength == 0) return FALSE;
  if (nRow == -1) { first = 0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  for (; first <= last; first++)
    for (i=0; i < m_nLongLength; i++)
      if (m_pData[first][i] != 0) return FALSE;
  return TRUE;
}

//-----------------------------------------------------------IsZero(int nRow)
BOOL CuBM::IsZero(const ULONG* mask, int nRow) const
{
  int i, first, last;
  ASSERT (nRow >= -1);
  if (m_nLongLength == 0) return FALSE;
  if (nRow == -1) { first = 0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  for (; first <= last; first++)
    for (i=0; i < m_nLongLength; i++)
      if ((m_pData[first][i] & mask[i]) != 0) return FALSE;
  return TRUE;
}

//-----------------------------------------------------------IsOne(int nRow)
BOOL CuBM::IsOne(int nRow) const
{
  int i, first, last;
  ASSERT (nRow>=-1);
  if (m_nLongLength == 0) return FALSE;
  ULONG b1, b2;
  if (nRow == -1) { first = 0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  for (; first <= last; first++) {
    for (i=0; i < m_nLongLength-1; i++)
      if (m_pData[first][i] != LONG_1) return FALSE;
    b1 = ~(m_pData[first][m_nLongLength-1]) + 1;
    b2 = OB4[ADR_BITLONG(m_nBitLength-1)];
    if (b1 != b2)       return FALSE;
  }
  return TRUE;
}

//-----------------------------------------------------------IsOne(int nRow)
BOOL CuBM::IsOne(const ULONG* mask, int nRow) const

{
  int i, first, last;
  ASSERT (nRow>=-1);
  if (m_nLongLength == 0) return FALSE;
  ULONG b1, b2;
  if (nRow == -1) { first = 0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  for (; first <= last; first++) {
    for (i=0; i < m_nLongLength-1; i++)
      if ((m_pData[first][i] & mask[i]) != LONG_1) return FALSE;
    b1 = ~(m_pData[first][m_nLongLength-1]) & (mask[m_nLongLength-1] + 1);
    b2 = OB4[ADR_BITLONG(m_nBitLength-1)];
    if (b1 != b2)       return FALSE;
  }
  return TRUE;
}

//-------------------------------------- operator==(const CuBM& bm1, const CuBM& bm2)
BOOL operator==(const CuBM& bm1, const CuBM& bm2)
{
  if ((bm1.m_nSize != bm2.m_nSize) || (bm1.m_nBitLength != bm2.m_nBitLength)) return FALSE;
  for (int i=0; i < bm1.m_nSize; i++) 
    for (int j=0; j < bm1.m_nLongLength; j++)
      if (bm1.m_pData[i][j] != bm2.m_pData[i][j]) return FALSE;
  return TRUE;
}

//-------------------------------------- operator!=(const CuBM& bm1, const CuBM& bm2)
BOOL operator!=(const CuBM& bm1, const CuBM& bm2)
{
  if ((bm1.m_nSize != bm2.m_nSize) || (bm1.m_nBitLength != bm2.m_nBitLength)) return TRUE;
  for (int i=0; i < bm1.m_nSize; i++)
   for (int j=0; j < bm1.m_nLongLength; j++)
     if (bm1.m_pData[i][j] != bm2.m_pData[i][j]) return TRUE;
  return FALSE;
}

//--------------------------------------- operator>(const CuBM& bm1, const CuBM& bm2)
BOOL operator>(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT((bm1.m_nSize == bm2.m_nSize) && 
         (bm1.m_nBitLength == bm2.m_nBitLength));
  int i, j;
  for (i=0; i < bm1.m_nSize; i++) {
    for (j=0; j < bm1.m_nLongLength; j++)
      if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) return FALSE;
    for (j=0; j < bm1.m_nLongLength; j++)
      if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
  }
  return FALSE;
}

//--------------------------------------- operator<(const CuBM& bm1, const CuBM& bm2)
BOOL operator<(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT((bm1.m_nSize == bm2.m_nSize) && (bm1.m_nBitLength == bm2.m_nBitLength));
  int i, j;
  for (i=0; i < bm1.m_nSize; i++) {
    for (j=0; j < bm1.m_nLongLength; j++)
      if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) return FALSE;
    for (j=0; j < bm1.m_nLongLength; j++)
      if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
  }
  return FALSE;
}

//-------------------------------------- operator>=(const CuBM& bm1, const CuBM& bm2)
BOOL operator>=(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT((bm1.m_nSize == bm2.m_nSize) && (bm1.m_nBitLength == bm2.m_nBitLength));
  int i, j;
  for (i=0; i < bm1.m_nSize; i++)
    for (j=0; j < bm1.m_nLongLength; j++)
      if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) return FALSE;
  return TRUE;
}

//-------------------------------------- operator<=(const CuBM& bm1, const CuBM& bm2)
BOOL operator<=(const CuBM& bm1, const CuBM& bm2)
{
  ASSERT((bm1.m_nSize == bm2.m_nSize) && (bm1.m_nBitLength == bm2.m_nBitLength));
  int i, j;
  for (i=0; i < bm1.m_nSize; i++)
    for (j=0; j < bm1.m_nLongLength; j++)
      if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) return FALSE;
  return TRUE;
}

//--------------------------------------------------CoverRow(int nRow1, int nRow2)
int CuBM::CoverRow(int nRow1, int nRow2)
{
  int pi, pj, k;
  ULONG work, worki, workj;

  for (k = pi = pj = 0; k < m_nLongLength; k++) {
    worki = GetLongAt(nRow1, k); workj = GetLongAt(nRow2, k);
    work = worki & workj;
    if (work == worki) pi++;
    if (work == workj) pj++;
  }
  if (pi == m_nLongLength) return 1;       /* i-а€ строка поглощаетс€ j-ой */
  if (pj == m_nLongLength) return -1;      /* i-а€ строка поглощает j-ую */
  return 0;                     
}

//---------------------------------CoverRow(int nRow1, int nRow2, const ULONG * mask)
int CuBM::CoverRow(int nRow1, int nRow2, const ULONG* mask)
{
  int pi, pj, k;
  ULONG work, worki, workj;

  for (k = pi = pj = 0; k < m_nLongLength; k++) { 
    worki = GetLongAt(nRow1, k) & mask[k];  workj = GetLongAt(nRow2, k) & mask[k];
    work = worki & workj;
    if (work == worki) pi++;
    if (work == workj) pj++;
  }
  if (pi == m_nLongLength) return 1;       /* i-а€ строка поглощаетс€ j-ой */
  if (pj == m_nLongLength) return -1;      /* i-а€ строка поглощает j-ую */
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////


#ifndef _LINUX
/////////////////////////////////////////////////////////////////////////////
// Serialization

//---------------------------------------------------------------Serialize(CArchive& ar)
void CuBM::Serialize(CArchive& ar)
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i, j;
  if (ar.IsStoring()) {
    ar << m_nSize;
    ar << m_nBitLength;
    ar << m_nLongLength; 
    for (i=0; i < m_nSize; i++)
      for (j=0; j<m_nLongLength; j++)
        ar << m_pData[i][j];
  }
  else {
    ar >> m_nSize;
    ar >> m_nBitLength;
    ar >> m_nLongLength;
    SetSize(m_nSize, m_nBitLength, m_nGrowBy);
    for (i=0; i < m_nSize; i++)
      for (j=0; j < m_nLongLength; j++)
        ar >> m_pData[i][j];
  }
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

//-----------------------------------------------------------Dump(CDumpContext& dc) const
#ifdef _DEBUG
void CuBM::Dump(CDumpContext& dc) const
{
  ASSERT_VALID(this);
#define MAKESTRING(x) #x
  AFX_DUMP1(dc, "a " MAKESTRING(CuBM) " with ", m_nSize);
  AFX_DUMP0(dc, " elements");
#undef MAKESTRING
  if (dc.GetDepth() > 0) {
    AFX_DUMP0(dc, "\n");
    for (int i = 0; i < m_nSize; i++) {
      AFX_DUMP1(dc, "\n\t[", i);
      AFX_DUMP1(dc, "] = ",NULL);
    }
  }
}

//-----------------------------------------------------------AssertValid() const
void CuBM::AssertValid() const
{
  if (m_pData == NULL) {
    ASSERT(m_nSize >= 0);
    ASSERT(m_nMaxSize >= 0);
    ASSERT(m_nBitLength >= 0);
    ASSERT(m_nLongLength >= 0);
    ASSERT(m_nAllocLength >= 0);
  }
  else {
    ASSERT(m_nSize >= 0);
    ASSERT(m_nMaxSize >= 0);
    ASSERT(m_nSize <= m_nMaxSize);
    ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize*sizeof(ULONG*)));
    ASSERT(m_nBitLength >= 0);
    ASSERT(m_nLongLength == LEN_LONG(m_nBitLength));
    ASSERT(m_nAllocLength >= 0);
    ASSERT(m_nGrowBy > 0);
  }
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// Operation of weighting,finding and casing ///////////////////////
//----------------------------------------------------------------------- CharBit(CString s)
void CuBM::CharBit(const CString s)
{
  int j = 0, i = 0, max = 0;
  CString w = s;
  if (w.GetLength() <= 2) { Init();  return; }
  if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
  while (!w.IsEmpty()) {
    j = w.Find('\n');
    if (j > max + 1) max = j - 1;
    w = w.Right(w.GetLength() - j-1);
    i++;
  }
  if (m_pData == NULL) AllocMatrix(i, max);
  else { SetSize(i, max, m_nGrowBy);  Zero(); }
  w = s; j = 0;
  if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
  while (!w.IsEmpty()) {
    max = w.Find('\n');
    for(i=0; i < max-1; i++)
      if (w[i] == '1') SetBitAt(j, i,TRUE);
      else             ASSERT(w[i] == '0'|| w[i] == '.' || w[i] == '-');
    w = w.Right(w.GetLength() - max-1);
    j++;
  }
}

//----------------------------------------------------------------------- CharBit(CStringArray s)
void CuBM::CharBit(const CStringArray& s)
{
  int k, i, max = 0;
  if (s.GetSize()==0) { Init();  return; }

  for (k=0; k < s.GetSize(); k++) {
    i = s[k].GetLength();
    if (i > max) max = i;    // max - length of row
  }  
  if (m_pData == NULL) AllocMatrix(s.GetSize(), max);
  else { SetSize(s.GetSize(), max, m_nGrowBy);  Zero(); }
  for (k=0; k < s.GetSize(); k++) {
    max = s[k].GetLength();
    CString w(s[k]);
    for(i=0; i < max; i++)
      if (w[i] == '1') SetBitAt(k, i,TRUE);
      else             ASSERT(w[i] == '0' || w[i] == '.' || w[i] == '-');
  }
}
#endif

//----------------------------------------------------------------------- CharBit(CString s)
void CuBM::CharBit(char* s)
{
  int j = 0, i = 0, max = 0, len = strlen(s);
  char* w, *pdest;
  w = (char*)malloc(len+2);
#ifndef _MSVC9
  strcpy(w,s);
#else
  strcpy_s(w, len+2, s);
#endif
  if (len <= 2) { Init();  return; }
  if (w[len-2] != '\r')
#ifndef _MSVC9
    strcat(w,"\r\n");
#else
    strcat_s(w, len+2, "\r\n");
#endif
  while (w[0] !='\0') {
    pdest = strchr(w,'\n');
    j = (int)(pdest - w + 1);
    if (j > max + 1) max = j - 1;
#ifndef _MSVC9
    strcpy(w, pdest+1);
#else
    strcpy_s(w, len+2, pdest+1);
#endif
    i++;
  }
  if (m_pData == NULL) AllocMatrix(i, max);
  else { SetSize(i, max, m_nGrowBy);  Zero(); }
#ifndef _MSVC9
    strcpy(w,s);
#else
    strcpy_s(w, len+2, s);
#endif
  j = 0;
  if (w[len-2] != '\r') 
#ifndef _MSVC9
    strcat(w,"\r\n");
#else
    strcat_s(w, len+2, "\r\n");
#endif
  while (w[0]!='\0') {
    pdest = strchr(w,'\n');
    max = (int)(pdest - w + 1);
    for(i=0;i<max-1;i++)
      if (w[i] == '1') SetBitAt(j,i,TRUE);
      else             ASSERT(w[i] == '0'|| w[i] == '.' || w[i] == '-');    //new 11.02
#ifndef _MSVC9
    strcpy(w, pdest+1);
#else
    strcpy_s(w, len+2, pdest+1);
#endif
    j++;
  }
  free(w);
}

//**************************************************************************************

//-------------------------------------------------------------------
void CuBM::SetRowDiz(int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] |= v1[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowDiz(int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] = v1[i] | v2[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowDiz(const ULONG* mask, int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) 
    m_pData[nRow][i] = (m_pData[nRow][i] | v1[i]) & mask[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowDiz(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] = (v1[i] | v2[i]) & mask[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowDiz(int nRow, int Num, ULONG* v1, ...)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);

  ULONG * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)     m_pData[nRow][i] |= v[j][i];     
  }
}

//-------------------------------------------------------------------
void CuBM::SetRowDiz(const ULONG* mask, int nRow, int Num, ULONG* v1, ...)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);

  ULONG * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)  m_pData[nRow][i] |= v[j][i];     
    m_pData[nRow][i] &= mask[i];
  }
}


//-------------------------------------------------------------------
void CuBM::SetRowCon(int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] &= v1[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowCon(int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] = v1[i] & v2[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowCon(const ULONG* mask, int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) 
    m_pData[nRow][i] &= v1[i] & mask[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowCon(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] = v1[i] & v2[i] & mask[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowCon(int nRow, int Num, ULONG* v1, ...)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);

  ULONG * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)    m_pData[nRow][i] &= v[j][i];     
  }
}

//-------------------------------------------------------------------
void CuBM::SetRowCon(const ULONG* mask, int nRow, int Num, ULONG* v1, ...)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);

  ULONG * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_pData[nRow][i] = v[0][i] & mask[i];
    for (int j=1; j< Num; j++) m_pData[nRow][i] &= v[j][i];     
  }
}


//-------------------------------------------------------------------
void CuBM::SetRowXor(int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] ^= v1[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowXor(int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] = v1[i] ^ v2[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowXor(const ULONG* mask, int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) 
    m_pData[nRow][i] = (m_pData[nRow][i] ^ v1[i]) & mask[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowXor(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] = (v1[i] ^ v2[i]) & mask[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowXor(int nRow, int Num, ULONG* v1, ...)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);

  ULONG * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)    m_pData[nRow][i] ^= v[j][i];
  }
}

//-------------------------------------------------------------------
void CuBM::SetRowXor(const ULONG* mask, int nRow, int Num, ULONG* v1, ...)
{
  ASSERT(nRow > 0);
  ASSERT(nRow < m_nSize);

  ULONG * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, ULONG* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nLongLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++) m_pData[nRow][i] ^= v[j][i];   
    m_pData[nRow][i] &= mask[i];
  }
}

//-------------------------------------------------------------------
void CuBM::SetRowDif(int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] &= ~v1[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowDif(int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) m_pData[nRow][i] = v1[i] & ~v2[i];
}

//-------------------------------------------------------------------
void CuBM::SetRowDif(const ULONG* mask, int nRow, const ULONG* v1)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) 
    m_pData[nRow][i] &= (~v1[i] & mask[i]);
}

//-------------------------------------------------------------------
void CuBM::SetRowDif(const ULONG* mask, int nRow, const ULONG* v1, const ULONG* v2)
{
  ASSERT(nRow > 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nLongLength; i++) 
    m_pData[nRow][i] = (v1[i] & ~v2[i]) & mask[i];
}

