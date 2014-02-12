/////////////////////////////////////////////////////////////////
// Ver.2.1.1     22.02.2007
//               Add для CBM перестала быть inline, допускает добавление
//               всей матрицы целиком при -1, принятом по умолчанию.
//               поправка в конструкторах m_nAllocLength
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

// Ver.1.2.4     19.05.2006
//             - поправка BitChar - более точный захват памяти под результат
// Ver.1.2.3     05.04.2006
//             - поправки BitChar (WithClear,...)
// Ver.1.2.2     26.03.2006
//             - построение символьного представления
// Ver.1.2.1     21.03.2006
//             Генераторы случайных перестроены на возврат *this
//             FindRow - будет возвращать -1 для пустых матриц
// Ver.1.2.0     11.03.2006   Russian == English
//               - Добавлены генераторы случайных
// Ver.1.1.0     18.02.2005
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// NOTE: we allocate an array of 'm_nMaxSize' elements, but only
//  the current size 'm_nSize' contains properly constructed objects.

#include <string>
#include <limits.h>
#include "../Common/BaseBool.h"
#include "../ComBool/UlongBool.h"
#include "../ComBool/UlongTern.h"
#include <stdio.h>
using namespace std;

#ifndef _LINUX
IMPLEMENT_SERIAL(CuTM, CObject, 0)
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////
// Генерация псевдослучайной <nRow>*<nCol> троичной матрицы //
// с равномерным распределением значений '-', '0' и '1'     //
// по элементам c вероятностями p(-)=1/2 и p(0)=p(1)=1/4.   //
//////////////////////////////////////////////////////////////

CuTM CuTM::GenRtm(int nRow, int nCol)
{
  int i;
  CuTV tv;
  SetSize (nRow, nCol);
  for (i=0; i<nRow; i++) SetRowTv (i, tv.GenRtv (nCol));
  return *this;
}

////////////////////////////////////////////////////////////////
// Генерация псевдослучайной троичной <nRow>*<nCol> матрицы   //
// со средним числом <nRang> определенных компонент в строке  //
// (дисперсия 1/32), принимающих значение'0'или'1'c вероятн.1/2 
// (при nRang>=nCol все элементы матрицы имеют определенное значение) 
////////////////////////////////////////////////////////////////

CuTM CuTM::GenRtmMid (int nRow, int nCol, int nRang)
{
  int i;
  CuTV tv;
  SetSize (nRow, nCol);
  for (i=0; i<nRow; i++) SetRowTv (i, tv.GenRtvMid (nCol, nRang));
  return *this;
}

///////////////////////////////////////////////////.//////////
// Генерация псевдослучайной троичной <nRow>*<nCol> матрицы //
// с фиксированным числом <nRang> определенных компонент    // 
// в строке, принимающих знач.'0'или'1'c вероятностью 1/2   
// (при nRang>=nCol все элементы матрицы имеют определенное значение) 
//////////////////////////////////////////////////////////////

CuTM CuTM::GenRtmFix(int nRow, int nCol, int nRang)
{
  int i;
  CuTV tv;
  SetSize (nRow, nCol);
  for (i=0; i<nRow; i++) SetRowTv (i, tv.GenRtvFix(nCol, nRang));
  return *this;
}

//******************************* Constructors\Destructor *******************************

//----------------- CuTM() ---------------------------------------------------------------
CuTM::CuTM() { Init(); }

//----------------- CuTM(const CuTM& tm, BOOL Fl/* = TRUE */) -----------------------------
CuTM::CuTM(const CuTM& tm, BOOL Fl/* = TRUE */)
{int i, j;
 if(tm.m_nSize == 0) {
   m_nSize= m_nMaxSize= 0; m_nAllocLength=m_nLongLength= tm.m_nLongLength;
   m_nBitLength= tm.m_nBitLength; m_nGrowBy = tm.m_nGrowBy; m_pData = NULL;
   return;
 }  
 if (Fl)  { //copy tm in our matrix
   AllocMatrix(tm.m_nSize, tm.m_nBitLength);
   for (i=0; i < m_nSize; i++) {
     memcpy(m_pData[i], tm.m_pData[i], m_nLongLength*sizeof(ULONG));
     memcpy(m_pData[i]+m_nLongLength, tm.m_pData[i]+tm.m_nAllocLength, 
            m_nLongLength*sizeof(ULONG));
   }
 }
 else {    //copy tm in our matrix after transponir
   AllocMatrix(tm.m_nBitLength, tm.m_nSize);
   for (j=0; j < m_nBitLength; j++)
     for (i=0; i < m_nSize; i++) SetBitAt(i, j, tm.GetBitAt(j, i));
 }
 m_nGrowBy = tm.m_nGrowBy;
}

//----------------- CuTM(const CuBM& bm1, const CuBM& bm2,BOOL Fl) --------------------------
CuTM::CuTM(const CuBM& bm1, const CuBM& bm2,BOOL Fl/* = TRUE */)
{int i, j;
 BOOL a1, a0;
 ASSERT(bm1.GetCountR() == bm2.GetCountR() &&
     bm1.GetCountC() == bm2.GetCountC());
 if (Fl)  { //copy tm in our matrix
   AllocMatrix(bm1.GetCountR(), bm1.GetCountC());
   for (i=0; i < m_nSize; i++) {
     memcpy(m_pData[i], bm1.GetRow(i), m_nLongLength*sizeof(ULONG));
     memcpy(m_pData[i]+m_nLongLength, bm2.GetRow(i), m_nLongLength*sizeof(ULONG));
   }
 }
 else {    //copy tm in our matrix after transponir
   AllocMatrix(bm1.GetCountR(), bm1.GetCountC());
   for (j=0; j < m_nBitLength; j++)
     for (i=0; i < m_nSize; i++) {
       a1 = bm1.GetBitAt(j, i); a0 = bm2.GetBitAt(j, i);
       if(a1 && !a0) SetBitAt(i, j,'1');
       if(!a1 && a0) SetBitAt(i, j,'0');
       if(a1 && a0) SetBitAt(i, j,'+');
     }  
 } 
   
  m_nGrowBy = 10;  
}

//----------------- CuTM(int nRow, int nColumn, char symb='-') -----------------------------
CuTM::CuTM(int nRow, int nColumn, char symb/*='-'*/)
{  ASSERT(nRow >= 0 && nColumn >= 0);
 int i, nLong = LEN_LONG(nColumn) * sizeof(ULONG),
  w = S_4-ADR_BITLONG(nColumn);
  if (nRow == 0) {                            //01.02.2002
    if ( nColumn == 0) Init();
    else {
      m_nSize= m_nMaxSize= 0; m_nAllocLength = m_nLongLength = nLong;
      m_nBitLength= nColumn; m_nGrowBy = 10; m_pData = NULL;
    }
    return;
  }
  AllocMatrix(nRow, nColumn);
  if(symb == '1' || symb == '+') 
    for (i=0; i < nRow; i++) {          ///////изменения 17.12.99г.
      memset(m_pData[i], BYTE_1, nLong*sizeof(ULONG));/* one's fill*/
      if (w != S_4)  m_pData[i][nLong-1] = (m_pData[i][nLong-1]>>w)<<w;
    }
  if(symb == '0' || symb == '+') 
    for (i=0; i < nRow; i++) {
      memset(m_pData[i]+nLong, BYTE_1, nLong*sizeof(ULONG));/* zero's fill*/  
      if (w != S_4) 
        m_pData[i][(nLong<<1)-1] = (m_pData[i][(nLong<<1)-1]>>w)<<w;
    }
  m_nGrowBy = 10;  
}
   
//----------------- CuTM(const char* pch) ------------------------------------------------
CuTM::CuTM(const char* pch)
{
#ifndef _LINUX
  CString s(pch);
#else
  string s(pch);
#endif
  Init();

#ifndef _LINUX
  if (s.GetLength()) 
    CharBit(s);
#else
  if (s.size()) 
    CharBit(s.c_str());
#endif
}

#ifndef _LINUX
//----------------- CuTM(CStringArray StrAr) ---------------------------------------------
CuTM::CuTM(const CStringArray& StrAr)
{ Init(); CharBit(StrAr); }
#endif

//----------------- ~CuTM() --------------------------------------------------------------
CuTM::~CuTM()
{ // ASSERT_VALID(this);
  SafeDelete(); }

/////////////////////////////////////////////////////////////////////////////////////////
//***************** Working with memory *************************************************
//----------------- SetSize -------------------------------------------------------------
void CuTM::SetSize(int nRow, int nColumn, int nGrowBy/*=-1*/, int nColGrow/*=0*/)
{ int i, nLong, nmaxRow, nLongmax, w;
 // ASSERT_VALID(this);
 ASSERT(nRow >= 0 && nColumn >= 0);
 if (nGrowBy != -1) m_nGrowBy = nGrowBy;  // set new size
 nmaxRow= nRow+m_nGrowBy; nLongmax=LEN_LONG(nColumn+nColGrow);
 nLong = LEN_LONG(nColumn);
 if (nRow == 0) { 
   SafeDelete(); m_nBitLength= nColumn;       //Init();
   m_nSize= m_nMaxSize=0;   m_nLongLength= nLong;
   m_nAllocLength= nLongmax;
   return;
 }
 //------------------ create one with exact size
 if (m_pData == NULL) { // create one with exact size
   ASSERT((long)nmaxRow * sizeof(ULONG*) <= ULONG_MAX); // no overflow
  m_pData = (ULONG**) new ULONG[nmaxRow * sizeof(ULONG*)];
  for (i=0; i < nRow; i++) {
    m_pData[i] = (ULONG*) new ULONG[nLongmax<<1];
    memset(m_pData[i], 0, (nLongmax<<1)*sizeof(ULONG)); }
  m_nSize = nRow; m_nMaxSize = nmaxRow;
  m_nBitLength = nColumn; m_nAllocLength = nLongmax;
  m_nLongLength= nLong;
  return;
 }
 //------------------ Set columns size
 if (nLong > m_nAllocLength) {
   ULONG * ptr;
   for (i=0; i < m_nSize; i++) {
     ptr= (ULONG*) new ULONG[nLongmax<<1];
     memset(ptr, 0, (nLongmax<<1)*sizeof(ULONG));
     if (m_pData[i]!=NULL) {
       memcpy(ptr, m_pData[i], m_nLongLength*sizeof(ULONG));   // copy new data from the old
       memcpy(ptr+nLongmax, m_pData[i]+m_nAllocLength, m_nLongLength*sizeof(ULONG));
       delete [] (ULONG*)m_pData[i]; 
     }
     m_pData[i] = ptr;
   }
  m_nAllocLength= nLongmax;
 }
 else {               //when(nLong < m_nAllocLength)  new 022.12.99!!!!!!!!!!
   int nost;
   if (nColumn < m_nBitLength) {
   w = S_4-ADR_BITLONG(nColumn);  nost = nLong;   }
   else   {
   w = S_4-ADR_BITLONG(m_nBitLength); nost = m_nLongLength;   }
   if (w==8) w = 0;                     //new 02.02.99!!!!!!!!!!
     for (i=0; i < m_nSize; i++) {
       memset(m_pData[i]+nost, 0,(m_nAllocLength-nost)*sizeof(ULONG));  //new 22.12.99 m_nLongLength-nLong
       memset(m_pData[i]+m_nAllocLength+nost, 0,(m_nAllocLength-nost)*sizeof(ULONG));
       m_pData[i][nost-1] = (m_pData[i][nost-1]>>w)<<w;    //nLong
       m_pData[i][m_nAllocLength+nost-1] = (m_pData[i][m_nAllocLength+nost-1]>>w)<<w; //nLong
     }
  }
 m_nBitLength = nColumn; m_nLongLength = nLong;
//------------------ Set rows size
 if (nRow <= m_nMaxSize) {
   if (nRow > m_nSize) { 
     for (i= m_nSize; i < nRow; i++) {
       m_pData[i] = (ULONG*) new ULONG[m_nAllocLength<<1];
       memset(m_pData[i], 0, (m_nAllocLength<<1)*sizeof(ULONG)); }
   }
   else 
     for (i= m_nSize-1; i >= nRow; i--) 
       if (m_pData[i]!=NULL) {
       delete [] (ULONG*)m_pData[i]; m_pData[i] = NULL; }
   m_nSize = nRow;
   return;
 }
 //-------------- Otherwise grow array
                     //new 22.12.99
 ASSERT((long)nmaxRow * sizeof(ULONG*) <= ULONG_MAX);  // no overflow
 ULONG** pNewData = (ULONG**) new ULONG[(nmaxRow * sizeof(ULONG*))<<1];
 memcpy(pNewData, m_pData, m_nSize*sizeof(ULONG*)); // copy new data from old
 for (i= m_nSize; i < nRow; i++) {
   pNewData[i] = (ULONG*) new ULONG[m_nAllocLength<<1];
   memset(pNewData[i], 0, (m_nAllocLength<<1)*sizeof(ULONG));
 }
 for (i= nRow; i < nmaxRow; i++) pNewData[i] = NULL;
 delete [] (ULONG*)m_pData;  m_pData = pNewData;
 m_nSize = nRow;  m_nMaxSize = nmaxRow;
}

//----------------- FreeExtra -----------------------------------------------------------
void CuTM::FreeExtra()
{ // ASSERT_VALID(this);
  int i;
  ULONG * ptr;
  if (m_nLongLength < m_nAllocLength) { // Free row memory
    for (i=0; i < m_nSize; i++) {
      ptr= (ULONG*) new ULONG[m_nLongLength<<1];
      if (m_pData[i]!=NULL) {
        memcpy(ptr, m_pData[i], m_nLongLength*sizeof(ULONG)); // copy new data from old
        memcpy(ptr+m_nLongLength, m_pData[i]+m_nAllocLength, m_nLongLength*sizeof(ULONG));
        delete [] (ULONG*)m_pData[i];
      }    
      m_pData[i] = ptr;
   }
   m_nAllocLength = m_nLongLength;
  }
  if (m_nSize != m_nMaxSize) { // shrink to desired size
#ifdef SIZE_T_MAX
    ASSERT((long)m_nSize * sizeof(ULONG*) <= SIZE_T_MAX);  // no overflow
#endif
    ULONG** pNewData = NULL;
    if (m_nSize != 0) {
      pNewData = (ULONG**) new ULONG[m_nSize * sizeof(ULONG*)];
      memcpy(pNewData, m_pData, m_nSize*sizeof(ULONG*));       // copy new data from old
    }
    // get rid of old stuff (note: no destructors called)
    delete [] (ULONG*)m_pData;
    m_pData = pNewData;
    m_nMaxSize = m_nSize;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////// Data Reading //////////////////////////////////////////////////////
//----------------- GetBitAt ------------------------------------------------------------
char CuTM::GetBitAt(int nRow, int nColumn) const
{ ASSERT(nColumn >= 0 && nColumn < m_nBitLength && nRow >= 0 && nRow < m_nSize);
 int a1, a0;
 a1 = m_pData[nRow][BIT_LONG(nColumn)] & OB4[ADR_BITLONG(nColumn)];
 a0 = m_pData[nRow][m_nAllocLength+BIT_LONG(nColumn)] & OB4[ADR_BITLONG(nColumn)];
 if(a1 != 0 && a0 == 0)  return '1';
 if(a0 != 0 && a1 == 0)  return '0';
 if(a0 == 0 && a1 == 0)  return '-';
 else return '+';
}

//----------------- GetRowTv ------------------------------------------------------------
CuTV CuTM::GetRowTv(int nRow) const
{ ASSERT(nRow >= 0 && nRow < m_nSize);
 CuTV tv(m_pData[nRow], m_pData[nRow]+m_nAllocLength, m_nBitLength);
 return tv;
}
 
//----------------- GetRowTv ------------------------------------------------------------
CuTV CuTM::GetRowTv(int nRow,ULONG* maska) const
{ ASSERT(nRow >= 0 && nRow < m_nSize);
  CuTV tv(GetRowTv(nRow));
  tv&= maska;
  return tv;
}

//----------------- GetRowTv ------------------------------------------------------------
CuTV CuTM::GetRowTv(int nRow,CuBV& maska) const
{ ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == maska.GetBitLength());
  CuTV tv(GetRowTv(nRow));
  tv&= maska;
  return tv;
}

//----------------- GetColumnTv ---------------------------------------------------------
CuTV CuTM::GetColumnTv(int nColumn) const
{ASSERT(nColumn >= 0 && nColumn < m_nBitLength);
 CuTV tv(m_nSize);
 int i;
 for (i=0; i < m_nSize; i++) tv.SetBitAt(i, GetBitAt(i, nColumn));
 return tv;
}

//----------------- GetColumnTv ---------------------------------------------------------
CuTV CuTM::GetColumnTv(int nColumn,ULONG* maska) const
{ASSERT(nColumn >= 0 && nColumn < m_nBitLength);
 CuTV tv(m_nSize);
 int i;
 for (i=0; i < m_nSize; i++) tv.SetBitAt(i, GetBitAt(i, nColumn));
 tv&= maska;
 return tv;
}  

//----------------- GetColumnTv ---------------------------------------------------------
CuTV CuTM::GetColumnTv(int nColumn,CuBV& maska) const
{ASSERT(nColumn >= 0 && nColumn < m_nBitLength && m_nSize == maska.GetBitLength());
 CuTV tv(m_nSize);
 int i;
 for (i=0; i < m_nSize; i++) tv.SetBitAt(i, GetBitAt(i, nColumn));
 tv&= maska;
 return tv;
}

#ifndef _LINUX
//----------------- BitChar ---------------------------------------------------
CString CuTM::BitChar(char One,char Zero,char Dont,char Rest,BOOL WithNum)
{
  int i,j,pos = 0;
  CString ch, s;
  if(WithNum) {
    if (m_nSize>9999)     { ch = "%5d: "; i=9; }
    else if (m_nSize>999) { ch = "%4d: "; i=8; }
    else if (m_nSize>99)  { ch = "%3d: "; i=7; }
    else                  { ch = "%2d: "; i=6; }
  }
  else i=2;
  CString res('\0',(m_nBitLength+i) * m_nSize);
  for (j=0;j<m_nSize;j++) {
    if(WithNum) {
      s.Format((const char*)ch,j);
      for (i=0;i<m_nSize;i++) res.SetAt(pos++,s[i]);
    }
  for (i=0;i<m_nBitLength;i++)
    switch (GetBitAt(j,i)) {
      case '1': res.SetAt(pos++,One); break;
      case '0': res.SetAt(pos++,Zero); break;
      case '-': res.SetAt(pos++,Dont); break;
      default:  res.SetAt(pos++,Rest);
    }
    res.SetAt(pos++,'\n');  //res.SetAt(pos++,'\r'); 
  }
  return res;
}

//------------------------------------------------- BitChar(char One,char Zero)
void CuTM::BitChar(CStringArray & StrRes, BOOL WithClear, BOOL WithNum,
                   char One, char Zero, char Dont, char Rest)
{ int i,j;
  if (WithClear) StrRes.RemoveAll();
  CString ch,s;
  if (m_nSize>9999)     { ch = "%5d: ";}
  else if (m_nSize>999) { ch = "%4d: ";}
  else if (m_nSize>99)  { ch = "%3d: ";}
  else                  { ch = "%2d: ";}

  for (j=0;j<m_nSize;j++) {
    if(WithNum) s.Format((const char*)ch,j);
    else        s.Empty();
    for (i=0;i<m_nBitLength;i++)
      switch (GetBitAt(j,i)) {
        case '1': s+=One; break;
        case '0': s+=Zero; break;
        case '-': s+=Dont; break;
        default:  s+=Rest;
      }  
    StrRes.Add(s);
  }
}
#else
//----------------- BitChar ---------------------------------------------------
string CuTM::BitChar(char One,char Zero,char Dont,char Rest,BOOL WithNum)
{
  int i,j,pos = 0;
  string ch, s;
  char buf[120];
  if(WithNum) {
    if (m_nSize>9999)     { ch = "%5d: "; i=9; }
    else if (m_nSize>999) { ch = "%4d: "; i=8; }
    else if (m_nSize>99)  { ch = "%3d: "; i=7; }
    else                  { ch = "%2d: "; i=6; }
  }
  else i=2;
  string res;
  res.resize('\0',(m_nBitLength+i) * m_nSize);
  for (j=0;j<m_nSize;j++) {
    if(WithNum) {
      sprintf(buf,ch.c_str(),j);
      for (i=0;i<m_nSize;i++) res[pos++] = buf[i];
    }
    for (i=0;i<m_nBitLength;i++)
      switch (GetBitAt(j,i)) {
      case '1': res[pos++]=One; break;
      case '0': res[pos++]=Zero; break;
      case '-': res[pos++]=Dont; break;
      default:  res[pos++]=Rest;
    }
    res[pos++] ='\n';// res.SetAt(pos++,'\n');
  }
  return res;
}
#endif

//--------------------------- GetUnDefs -------------------------------------------------
CuBM CuTM::GetUnDefs() const
{// // ASSERT_VALID(this);
 int i, j;
 ULONG maska;
 CuBM bmTag(m_nSize, m_nBitLength);
 maska=((j=ADR_BITLONG(m_nBitLength)) == 0)? LONG_1:(~(LONG_1 >> j));
 for (i=0; i < m_nSize; i++) {
   for (j=0; j < m_nLongLength-1; j++) 
     bmTag.SetLongAt(i, j,~(m_pData[i][j]|m_pData[i][m_nAllocLength+j]));
   bmTag.SetLongAt(i, j,(~(m_pData[i][j]|m_pData[i][m_nAllocLength+j]))&maska);
 }
 return  bmTag;
}   

//--------------------------- GetDefs ---------------------------------------------------
CuBM CuTM::GetDefs() const
{// // ASSERT_VALID(this);
 int i, j;
 CuBM bmTag(m_nSize, m_nBitLength);
 for (i=0; i < m_nSize; i++) 
   for (j=0; j < m_nLongLength; j++) 
     bmTag.SetLongAt(i, j, m_pData[i][j]|m_pData[i][m_nAllocLength+j]);
 return  bmTag;
}   

//--------------------------- GetOnes ---------------------------------------------------
CuBM CuTM::GetOnes() const
{// // ASSERT_VALID(this);
 int i, j;
 CuBM bmTag(m_nSize, m_nBitLength);
 for (i=0; i < m_nSize; i++) 
   for (j=0; j < m_nLongLength; j++) 
     bmTag.SetLongAt(i, j, m_pData[i][j]);
 return  bmTag;
}   

//--------------------------- GetZeros --------------------------------------------------
CuBM CuTM::GetZeros() const
{// // ASSERT_VALID(this);
 int i, j;
 CuBM bmTag(m_nSize, m_nBitLength);
 for (i=0; i < m_nSize; i++) 
   for (j=0; j < m_nLongLength; j++) 
     bmTag.SetLongAt(i, j, m_pData[i][m_nAllocLength+j]);
 return  bmTag;
}   

//--------------------------- GetRowUnDef -----------------------------------------------
CuBV CuTM::GetRowUnDef(int nRow) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow);
 int i;
 ULONG maska;
 maska=((i=ADR_BITLONG(m_nBitLength)) == 0)? LONG_1:(~(LONG_1 >> i));
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength-1; i++) 
   bvTag.SetLongAt(i,~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]));
 bvTag.SetLongAt(i,(~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))&maska);
 return  bvTag;
}   

//--------------------------- GetRowUnDef -----------------------------------------------
CuBV CuTM::GetRowUnDef(int nRow,ULONG* maska) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow && nRow >= 0);
 int i;
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength-1; i++) 
   bvTag.SetLongAt(i,(~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))&maska[i]);
 return  bvTag;
} 

//--------------------------- GetRowDef -------------------------------------------------
CuBV CuTM::GetRowDef(int nRow) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow);
 int i;
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength; i++) 
   bvTag.SetLongAt(i, m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]);
 return  bvTag;
}   

//--------------------------- GetRowDef -------------------------------------------------
CuBV CuTM::GetRowDef(int nRow,ULONG* maska) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow);
 int i;
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength; i++) 
   bvTag.SetLongAt(i,(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])&maska[i]);
 return  bvTag;
}   

//--------------------------- GetRowOne -------------------------------------------------
CuBV CuTM::GetRowOne(int nRow) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow);
 int i;
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength; i++) 
   bvTag.SetLongAt(i, m_pData[nRow][i]);
 return  bvTag;
}   

//--------------------------- GetRowOne -------------------------------------------------
CuBV CuTM::GetRowOne(int nRow,ULONG* maska) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow);
 int i;
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength; i++) 
   bvTag.SetLongAt(i,(m_pData[nRow][i])&maska[i]);
 return  bvTag;
}   

//--------------------------- GetRowZero ------------------------------------------------
CuBV CuTM::GetRowZero(int nRow) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow);
 int i;
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength; i++) 
   bvTag.SetLongAt(i, m_pData[nRow][m_nAllocLength+i]);
 return  bvTag;
}   

//--------------------------- GetRowZero ------------------------------------------------
CuBV CuTM::GetRowZero(int nRow,ULONG* maska) const
{// // ASSERT_VALID(this);
 ASSERT(m_nSize >= nRow);
 int i;
 CuBV bvTag(m_nBitLength, FALSE);
 for (i=0; i < m_nLongLength; i++) 
   bvTag.SetLongAt(i,(m_pData[nRow][m_nAllocLength+i])&maska[i]);
 return  bvTag;
}   

void CuTM::Param(int* aPar, int nMaxLen)
// return the number of long rows - rows with weght more than nMaxLen,
// the average rows weght and the average long rows weght
{int nAll=0, nRang, nL, nLarAll=0;
 aPar[0]=0;
 for(nL=0; nL < m_nSize; nL++) {
   nRang=CountDefs(nL);
   nAll+= nRang;
   if(nRang > nMaxLen) {
   aPar[0]++; nLarAll+= nRang;
   }
 }
 aPar[1]= nAll/m_nSize;
 aPar[2]= nLarAll/aPar[0];
}

 
/////////////////////////////////////////////////////////////////////////////////////////
//***************** Select of matrix ****************************************************
//----------------- ExtractRows ---------------------------------------------------------
CuTM CuTM::ExtractRows(int nFirst, int nCount) const
{ASSERT(nFirst >= 0 && (nFirst + nCount) <= m_nSize && nCount > 0);
 CuTM tm(nCount, m_nBitLength);
 int i;
 for (i=0; i < nCount; i++) tm.SetRowTv(i, GetRowTv(nFirst + i));
 return tm;
}

//----------------- ExtractColumns ------------------------------------------------------
CuTM CuTM::ExtractColumns(int nFirst, int nCount) const
{ASSERT(nFirst >= 0 && (nFirst + nCount) <= m_nBitLength && nCount > 0);
 CuTM tm(m_nSize, nCount);
 int j, i,k, l_bit, r_bit, nSrcLong,AdrBeg;
 ULONG ch;
 k = S_4 - ADR_BITLONG(nCount);
 ch=(LONG_1 >> k) << k;
 AdrBeg = BIT_LONG(nFirst);
 l_bit = ADR_BITLONG(nFirst);
 r_bit = S_4 - l_bit;
 nSrcLong = BIT_LONG(nFirst + nCount);
 for (j=0; j < m_nSize; j++)  {
   if (AdrBeg - nSrcLong)  {
     for (i=AdrBeg; i <= nSrcLong; i++)  {
       tm.m_pData[j][i-AdrBeg]=( m_pData[j][i]<<l_bit) | (m_pData[j][i+1]>>r_bit);
       tm.m_pData[j][i-AdrBeg+tm.m_nAllocLength]=( m_pData[j][i+m_nAllocLength]<<l_bit)
       |(m_pData[j][i+1+m_nAllocLength]>>r_bit);       
     }
     if (k != S_4) {
       tm.m_pData[j][nSrcLong-AdrBeg] &= ch;     
       tm.m_pData[j][nSrcLong-AdrBeg+tm.m_nAllocLength] &= ch;
     }  
   }
   else  {
     tm.m_pData[j][0]=(m_pData[j][AdrBeg]<<l_bit) | (m_pData[j][AdrBeg+1]>>r_bit);
     tm.m_pData[j][tm.m_nAllocLength]=(m_pData[j][AdrBeg+m_nAllocLength]<<l_bit) 
                 | (m_pData[j][AdrBeg+1+m_nAllocLength]>>r_bit);
     if (k != S_4) {
       tm.m_pData[j][0] &= ch;
       tm.m_pData[j][tm.m_nAllocLength] &= ch;
     }  
   }
 }
 return tm;
}

//----------------- Extract --------------------------------------------------------------
CuTM CuTM::Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const
{ASSERT(nFirstRow >= 0 && nFirstColumn >= 0 && nCountColumn > 0 && nCountRow > 0);
 ASSERT(((nFirstColumn + nCountColumn) <= m_nBitLength) && (nFirstRow + nCountRow <= m_nSize));
 CuTM tm(nCountRow, nCountColumn);
 int j, i,k, l_bit, r_bit, nSrcLong,AdrBeg;
 ULONG ch;
 k = S_4 - ADR_BITLONG(nCountColumn);
 ch=(LONG_1 >> k) << k;
 AdrBeg = BIT_LONG(nFirstColumn);
 l_bit = ADR_BITLONG(nFirstColumn);
 r_bit = S_4 - l_bit;
 nSrcLong = BIT_LONG(nFirstColumn + nCountColumn);
 for (j= nFirstRow; j < nFirstRow+nCountRow; j++)  {
   if (AdrBeg - nSrcLong)  {
     for (i=AdrBeg; i <= nSrcLong; i++)  {
       tm.m_pData[j-nFirstRow][i-AdrBeg] = (m_pData[j][i] << l_bit) |
       (m_pData[j][i+1] >> r_bit);
       tm.m_pData[j-nFirstRow][i-AdrBeg+tm.m_nAllocLength] = 
       (m_pData[j][i+m_nAllocLength] << l_bit)|(m_pData[j][i+1+m_nAllocLength] >> r_bit);
     }
     if (k != S_4) {
       tm.m_pData[j-nFirstRow][nSrcLong-AdrBeg] &= ch;
       tm.m_pData[j-nFirstRow][nSrcLong-AdrBeg+tm.m_nAllocLength] &= ch;
     }  
   }
   else  {
     tm.m_pData[j-nFirstRow][0]=(m_pData[j][AdrBeg]<<l_bit)|(m_pData[j][AdrBeg+1]>>r_bit);
     tm.m_pData[j-nFirstRow][tm.m_nAllocLength]=((m_pData[j][AdrBeg]<<l_bit)+m_nAllocLength)
       |(m_pData[j][AdrBeg+1+m_nAllocLength]>>r_bit);
     if (k != S_4) {
       tm.m_pData[j-nFirstRow][0] &= ch;
       tm.m_pData[j-nFirstRow][tm.m_nAllocLength] &= ch;
     }  
   }
  }
 return tm;
}

//----------------- ExtractColumns ------------------------------------------------------
CuTM CuTM::ExtractColumns(const CuBV& Columns) const
{ASSERT(Columns.GetBitLength() > 0);
 ASSERT(m_nBitLength >= Columns.GetBitLength());
 int i, j, LeftOneColumn;
 int NewColumnLen = Columns.CountBit();
 CuTM tm(m_nSize,NewColumnLen);
 for(i=0; i < m_nSize; i++)  {
   LeftOneColumn = -1;
   for (j=0; j < NewColumnLen; j++)  {
     LeftOneColumn = Columns.LeftOne(LeftOneColumn);
     tm.SetBitAt(i, j, GetBitAt(i,LeftOneColumn));
   }
 }
 return tm;
}

//----------------- Minor ---------------------------------------------------------------
CuTM CuTM::Minor(const CuBV& Rows, const CuBV& Columns) const
{ASSERT(Columns.GetBitLength() > 0 && Rows.GetBitLength() >= 0);
 ASSERT(m_nBitLength >= Columns.GetBitLength() && m_nSize >= Rows.GetBitLength());
 int i, j,LeftOneRow = -1,LeftOneColumn = -1;
 int  NewRowLen,NewColumnLen;
 NewRowLen = Rows.CountBit();
 NewColumnLen = Columns.CountBit();
 CuTM tm(NewRowLen,NewColumnLen);
 for (i=0; i < NewRowLen; i++)  {
   LeftOneRow = Rows.LeftOne(LeftOneRow);
   for (j=0; j < NewColumnLen; j++)  {
     LeftOneColumn = Columns.LeftOne(LeftOneColumn);
     tm.SetBitAt(i, j, GetBitAt(LeftOneRow, LeftOneColumn));
   }
   LeftOneColumn = -1;
 }
 return tm;
}

//----------------- Trans ---------------------------------------------------------------
CuTM CuTM::Trans() const
{// // ASSERT_VALID(this); 
 CuTM tm(*this,FALSE);
 return tm;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Data Writing ////////////////////////////////////////////////////////
//----------------- SetBitAt ------------------------------------------------------------
void CuTM::SetBitAt(int nRow, int nColumn, char symb/*='-'*/)
{ASSERT(nRow >= 0 && nRow < m_nSize); 
 ASSERT(nColumn >= 0 && nColumn <= m_nBitLength);
 int i=BIT_LONG(nColumn), j=ADR_BITLONG(nColumn);
 switch (symb) {
     case '1': m_pData[nRow][i] |=OB4[j]; m_pData[nRow][m_nAllocLength+i]&=~OB4[j]; break;
     case '0': m_pData[nRow][m_nAllocLength+i] |=OB4[j]; m_pData[nRow][i]&=~OB4[j]; break;
     case '-': m_pData[nRow][i]&=~OB4[j]; m_pData[nRow][m_nAllocLength+i]&=~OB4[j]; break;
     default:  m_pData[nRow][i] |=OB4[j]; m_pData[nRow][m_nAllocLength+i] |=OB4[j];
    } 
}

//----------------- SetRowTv --------------------------------------------------------------
void CuTM::SetRowTv(int nRow, const CuTV& tv)
{ ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == tv.GetBitLength());
  memcpy(m_pData[nRow],(const ULONG *)tv.GetOnes(), tv.GetLongLength()*sizeof(ULONG));
  memcpy(&m_pData[nRow][m_nAllocLength],(const ULONG *)tv.GetZeros(),
    tv.GetLongLength()*sizeof(ULONG));
}

//----------------- SetRowTv --------------------------------------------------------------
void CuTM::SetRowTv(int nRow, const CuTM& tm, int ntmRow)
{ ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == tm.GetCountC());
  memcpy(m_pData[nRow], tm.m_pData[ntmRow], m_nLongLength*sizeof(ULONG));
  memcpy(&m_pData[nRow][m_nAllocLength],&tm.m_pData[ntmRow][m_nAllocLength],
         m_nLongLength*sizeof(ULONG));
}

//--------------------------- SetPartAt -------------------------------------------------
void CuTM::SetPartAt(BOOL Part, const CuBM& bm)
{// // ASSERT_VALID(this);
 ASSERT(m_nBitLength == bm.GetCountC() && m_nSize == bm.GetCountR());
 int i, j;
 if(Part)
   for (i=0; i < m_nSize; i++) 
     for (j=0; j < m_nLongLength; j++) 
       m_pData[i][j]= bm.GetLongAt(i, j);
 else 
   for (i=0; i < m_nSize; i++) 
     for (j=0; j < m_nLongLength; j++) 
       m_pData[i][m_nAllocLength+j]= bm.GetLongAt(i, j);
 return;
}   

//--------------------------- SetPartAt -------------------------------------------------
void CuTM::SetPartAt(BOOL Part, const CuBV& bv, int Row)
{// // ASSERT_VALID(this);
 ASSERT(m_nBitLength == bv.GetBitLength() && m_nSize >= Row);
 int i;
 if(Part)
   for (i=0; i < m_nLongLength; i++) 
     m_pData[Row][i]= bv.GetLongAt(i);
 else 
   for (i=0; i < m_nLongLength; i++) 
     m_pData[Row][m_nAllocLength+i]= bv.GetLongAt(i);
 return;
}   

//--------------------------- SetRowZero -------------------------------------------------
void CuTM::SetRowZero(int nRow, const ULONG* newRow)
{// // ASSERT_VALID(this);
 for (int i=0; i < m_nLongLength; i++) 
     m_pData[nRow][m_nAllocLength+i]= newRow[i];
 return;
}   

//--------------------------- SetRowOne --------------------------------------------------
void CuTM::SetRowOne(int nRow, const ULONG* newRow)
{// // ASSERT_VALID(this);
 for (int i=0; i < m_nLongLength; i++) 
   m_pData[nRow][i]= newRow[i];
 return;
}   
/////////////////////////////////////////////////////////////////////////////////////////
//***************** Working with rows ****************************************************
//----------------- Add -----------------------------------------------------------------
int CuTM::Add(char symb/*='-'*/, int nCount/*=1*/)
{int i, first = m_nSize;
 if(m_nSize + nCount < m_nAllocLength) {
   m_nSize+= nCount;
   for(i=first; i < m_nSize; i++) {
     m_pData[i] = (ULONG*) new ULONG[m_nAllocLength<<1];
     memset(m_pData[i], 0, (m_nAllocLength<<1)*sizeof(ULONG));
   }
 }
 else 
   SetSize(m_nSize+nCount, m_nBitLength, m_nGrowBy, m_nAllocLength*S_4-m_nBitLength);
 if(symb != '-') {
   ULONG maska=((i = ADR_BITLONG(m_nBitLength)) > 0 )? (LONG_1 <<(S_4 - i)):LONG_1;
   for (; first < m_nSize; first++) {
     switch (symb) {
       case '1': 
         memset(m_pData[first]+m_nAllocLength, 0, m_nLongLength*sizeof(ULONG));
         memset(m_pData[first], BYTE_1, (m_nLongLength-1)*sizeof(ULONG));
         m_pData[first][m_nLongLength-1]= maska; break;
       case '0': 
         memset(m_pData[first]+m_nAllocLength, BYTE_1, (m_nLongLength-1)*sizeof(ULONG));
         m_pData[first][m_nAllocLength+m_nLongLength-1] = maska;  
         memset(m_pData[first], 0, m_nLongLength*sizeof(ULONG)); break; 
       case '+': 
         memset(m_pData[first]+m_nAllocLength, BYTE_1, (m_nLongLength-1)*sizeof(ULONG));
         m_pData[first][m_nAllocLength+m_nLongLength-1]= maska;  
         memset(m_pData[first], BYTE_1, (m_nLongLength-1)*sizeof(ULONG));
         m_pData[first][m_nLongLength-1]= maska;
     }
   }
 }
 return m_nSize-1;
} 

int CuTM::Add(const CuTM& tm, int ntmRow/*=-1*/)
{ 
  int ind = m_nSize;
  if (ntmRow>=0)  SetRowGrow(ind, tm, ntmRow); 
  else InsertRow(ind, 0, tm, tm.GetCountR());
  return (ind); 
}

//----------------- SetRowGrow ----------------------------------------------------------
void CuTM::SetRowGrow(int nRow, const CuTV& newRow)
{// // ASSERT_VALID(this);  //change nRow with new vector
 ASSERT(nRow >= 0);
 if (m_nSize > 0)
   ASSERT(m_nBitLength == newRow.GetBitLength());
 if (nRow >= m_nSize)
    SetSize(nRow+1, newRow.GetBitLength(), m_nGrowBy, m_nAllocLength*S_4-m_nBitLength);
 memcpy(m_pData[nRow],(const ULONG *)newRow.GetOnes(), m_nLongLength*sizeof(ULONG));
 memcpy(m_pData[nRow]+m_nAllocLength,(const ULONG *)newRow.GetZeros(), m_nLongLength*sizeof(ULONG));
}

//----------------- SetRowGrow ----------------------------------------------------------
void CuTM::SetRowGrow(int nRow, const CuTM& tm, int ntmRow)
{ // // ASSERT_VALID(this); //change nRow with ntmRow from tm
  ASSERT(nRow >= 0 && ntmRow < tm.m_nSize);
  if (m_nSize > 0) 
    ASSERT(m_nBitLength == tm.m_nBitLength);  //11.02.99
  if (nRow >= m_nSize) 
    SetSize(nRow+1, tm.m_nBitLength, m_nGrowBy, m_nAllocLength*S_4-m_nBitLength);
  memcpy(m_pData[nRow], tm.m_pData[ntmRow], m_nLongLength*sizeof(ULONG));
  memcpy(m_pData[nRow]+m_nAllocLength, tm.m_pData[ntmRow]+tm.m_nAllocLength, 
         m_nLongLength*sizeof(ULONG));
}

//----------------- InsertRow -----------------------------------------------------------
void CuTM::InsertRow(int nRow, const CuTV& newRow, int nCount/*=1*/)
//Insert nCount copies of vector newRow before nRow row of TM
{ // // ASSERT_VALID(this);        //insert nCount vector copies beginning from nRow
  ASSERT(nRow >= 0 && newRow.GetBitLength() == m_nBitLength);
  ULONG *ptr;
  int i;
  if (nRow >= m_nSize) {     // adding after the end of the array
    SetSize(nRow + nCount, m_nBitLength, m_nGrowBy, m_nAllocLength*S_4-m_nBitLength);
                             // grow so nIndex is valid
  }
  else {                     // inserting in the middle of the array
    int out = m_nSize-nRow, begin= m_nSize-1;
    SetSize(m_nSize + nCount, m_nBitLength, m_nGrowBy, m_nAllocLength*S_4-m_nBitLength);
                            // grow it to new size
    for (i=1; i <= out; i++, begin--) {
      ptr= m_pData[begin]; 
      m_pData[begin] = m_pData[m_nSize-i];
//      memmove(&m_pData[begin],&m_pData[m_nSize-i],sizeof(ULONG*)); // Rom
      m_pData[m_nSize-i]=ptr;
    }
  }
// insert new value in the gap
  ASSERT(nRow + nCount <= m_nSize);
  for(i= nRow; i<nRow+nCount; i++) {
    memcpy(m_pData[i], newRow.GetOnes(), m_nLongLength*sizeof(ULONG));
    memcpy(m_pData[i]+m_nAllocLength, newRow.GetZeros(), m_nLongLength*sizeof(ULONG));
  }
}

//----------------- InsertRow -----------------------------------------------------------
void CuTM::InsertRow(int nRow, int nStartRow, const CuTM& tm, int nCount)
{// // ASSERT_VALID(this);
//  ASSERT(tm.IsKindOf(RUNTIME_CLASS(CuTM)));
  ASSERT(nRow >= 0 && nStartRow >= 0 && m_nBitLength == tm.m_nBitLength);
  if (tm.m_nSize > 0) {
    InsertRow(nRow, tm.GetRowTv(nStartRow), nCount);
    for (int i = 1; i < nCount; i++)
      SetRowTv(nRow + i, tm, nStartRow+i);
  }
}

//----------------- RemoveRow -----------------------------------------------------------
void CuTM::RemoveRow(int nRow, int nCount /* = 1 */)
{ // // ASSERT_VALID(this);
  ASSERT(nRow >= 0 && nCount >= 0 && nRow + nCount <= m_nSize);
  int nMoveCount = m_nSize - (nRow + nCount);     // just remove a range
  for (int i=0; i < nCount; i++) 
    if (m_pData[i+nRow]!=NULL)
      delete [] (ULONG*)m_pData[i+nRow];
  if (nMoveCount) {
      memcpy(&m_pData[nRow],&m_pData[nRow + nCount], nMoveCount*sizeof(ULONG*));
    }  
    m_nSize -= nCount;
}

//----------------- ExchangeRow ---------------------------------------------------------
void CuTM::ExchangeRow(int nRow1, int nRow2, int nCount/*=1*/)
{ASSERT(nCount>0 && nRow1+nCount<= m_nSize && nRow2+nCount<= m_nSize);
 ULONG *ptr;
 while(nCount >0) {
   ptr= m_pData[nRow1]; m_pData[nRow1++]= m_pData[nRow2];
   m_pData[nRow2++]=ptr; nCount--; 
 }
 return;
}

//----------------- AddCol -----------------------------------------------------------
int CuTM::AddCol(int numCol/*=1*/)
//numCol columns adding to the matrix
//return the number of new last column
{ int i, nLong;
 // // ASSERT_VALID(this);
 ASSERT(numCol > 0);
 m_nBitLength+= numCol;
 nLong = LEN_LONG(m_nBitLength) * sizeof(ULONG);
 //------------------ Set columns size
 if (nLong > m_nAllocLength) {
   ULONG * ptr;
   for (i=0; i < m_nSize; i++) {
     ptr= (ULONG*) new ULONG[nLong<<1];
     memset(ptr, 0, (nLong<<1)*sizeof(ULONG));  // zero fill
     memcpy(ptr, m_pData[i], m_nLongLength*sizeof(ULONG)); // copy new data from old
     memcpy(ptr+nLong, m_pData[i]+m_nAllocLength, m_nLongLength*sizeof(ULONG));
     delete [] (ULONG*)m_pData[i];
     m_pData[i] = ptr; 
   }
   m_nAllocLength= nLong;
 }
 m_nLongLength = nLong;
 return m_nBitLength-1;
}
 

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Operators and functions of assignment ///////////////////////////////
//----------------- Concat --------------------------------------------------------------
//----------------- operator =(const CuTM&) ----------------------------------------------
const CuTM& CuTM::operator =(const CuTM& tm)
{int i;
 SetSize(tm.m_nSize, tm.m_nBitLength, tm.m_nGrowBy);
 for (i=0; i < tm.m_nSize; i++) SetRowTv(i, tm.GetRowTv(i));
 return *this;
}

//----------------- operator =(const char* ) --------------------------------------------
const CuTM& CuTM::operator =(const char* pch)
{CharBit(pch);
 return *this;
}
#ifndef _LINUX
//----------------- operator =(const CStringArray) --------------------------------------
const CuTM& CuTM::operator =(const CStringArray& StrAr)
{ CharBit(StrAr);
 return *this;
}
#endif

//----------------- Clear ---------------------------------------------------------------
void CuTM::Clear(char symb/*='-'*/, int nRow/*=-1*/)
{// // ASSERT_VALID(this);
 ASSERT (nRow>=-1);
 int i,k,first =0,last = m_nSize-1;
 ULONG maska; 
 if (nRow != -1) first =last = nRow;
 maska=((i = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1<<(S_4 - i)):LONG_1;
 for (k=first; k <= last; k++) {
   memset(m_pData[k], BYTE_1, (m_nLongLength-1)*sizeof(ULONG));
   m_pData[k][m_nLongLength-1]= maska;
   memset(m_pData[k]+m_nAllocLength, BYTE_1, (m_nLongLength-1)*sizeof(ULONG));
   m_pData[k][m_nAllocLength+m_nLongLength-1]= maska;  
   switch (symb) {
     case '1': memset(m_pData[k]+m_nAllocLength, 0, m_nLongLength*sizeof(ULONG)); break;
     case '0': memset(m_pData[k], 0, m_nLongLength*sizeof(ULONG)); break; 
     case '-': memset(m_pData[k], 0, m_nLongLength*sizeof(ULONG));
               memset(m_pData[k]+m_nAllocLength, 0, m_nLongLength*sizeof(ULONG));break;
   }
 } 
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Logic operations ////////////////////////////////////////////////////
//----------------- InvertDefs ----------------------------------------------------------
void CuTM::InvertDefs(int nRow/*=-1*/)//инвертировать опр.значения строк или одной стр.
{// // ASSERT_VALID(this); 
 int j;
 ULONG a ; //, maska;
// maska=((j = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1<<(S_4 - j)):LONG_1;
 if(nRow >=0) 
   for(j=0; j < m_nLongLength; j++) {
     a= m_pData[nRow][j];
     m_pData[nRow][j]= m_pData[nRow][j+m_nAllocLength];
     m_pData[nRow][j+m_nAllocLength]=a;
   }
 else
  for(nRow++; nRow < m_nSize; nRow++) 
   for(j=0; j < m_nLongLength; j++) {
     a= m_pData[nRow][j];
     m_pData[nRow][j]= m_pData[nRow][j+m_nAllocLength];
     m_pData[nRow][j+m_nAllocLength]=a;
   }
}    

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Operations of concatinations ////////////////////////////////////////
//----------------- Concat --------------------------------------------------------------
void CuTM::Concat(const CuTM& tm)
{// ASSERT_VALID(this);
 ASSERT(m_nSize == tm.m_nSize);
 int i, j, OldLen = BIT_LONG(m_nBitLength),
 l_bit = ADR_BITLONG(m_nBitLength), r_bit = S_4 - l_bit;
 SetSize(m_nSize, m_nBitLength + tm.m_nBitLength);
 for (i=0; i < m_nSize; i++)  {
   m_pData[i][OldLen] |= (tm.m_pData[i][0] >> l_bit);
   m_pData[i][OldLen+m_nAllocLength] |= (tm.m_pData[i][tm.m_nAllocLength] >> l_bit);
   for (j=1; j < tm.m_nLongLength; j++) {
     m_pData[i][j+OldLen] =
     (tm.m_pData[i][j-1] << r_bit) | (tm.m_pData[i][j] >> l_bit);
     m_pData[i][j+OldLen+m_nAllocLength]=(tm.m_pData[i][tm.m_nAllocLength+j-1]
     << r_bit) | (tm.m_pData[i][j+tm.m_nAllocLength] >> l_bit);
   }
   m_pData[i][j+OldLen+m_nAllocLength] |= tm.m_pData[i][tm.m_nAllocLength+j-1]<<r_bit;
   m_pData[i][j+OldLen] |= tm.m_pData[i][j-1]<<r_bit;
 }
}

//----------------- Concat --------------------------------------------------------------
void CuTM::Concat(const CuTM& tm1, const CuTM& tm2)
{ASSERT(tm1.m_nSize == tm2.m_nSize);
 SetSize(tm1.m_nSize, tm1.m_nBitLength + tm2.m_nBitLength);
 int i, j,OldLen = BIT_LONG(tm1.m_nBitLength),
 l_bit = ADR_BITLONG(tm1.m_nBitLength), r_bit = S_4 - l_bit;
 for (i=0; i < m_nSize; i++) {
   memcpy(m_pData[i], tm1.m_pData[i], tm1.m_nLongLength*sizeof(ULONG));
   memcpy(m_pData[i]+m_nAllocLength, tm1.m_pData[i]+tm1.m_nAllocLength, tm1.m_nLongLength*sizeof(ULONG));
 }
 for (i=0; i < m_nSize; i++)  {
   m_pData[i][OldLen] |= (tm2.m_pData[i][tm2.m_nAllocLength] >> l_bit);
   m_pData[i][OldLen+m_nAllocLength] |= (tm2.m_pData[i][tm2.m_nAllocLength] >> l_bit);
   for (j=1; j < tm2.m_nLongLength; j++) {
     m_pData[i][j+OldLen] = (tm2.m_pData[i][j-1]<<r_bit)|(tm2.m_pData[i][j]>>l_bit);
     m_pData[i][j+OldLen+m_nAllocLength] =
     (tm2.m_pData[i][tm2.m_nAllocLength+j-1]<<r_bit)|(tm2.m_pData[i][tm2.m_nAllocLength+j]>>l_bit);
   }
   m_pData[i][j+OldLen] |= (tm2.m_pData[i][j-1] << r_bit);
   m_pData[i][j+OldLen+m_nAllocLength] |= (tm2.m_pData[i][tm2.m_nAllocLength+j-1] << r_bit);
 }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Operations of weighting, finding and casing /////////////////////////
//----------------- CountDef ------------------------------------------------------------
int CuTM::CountDefs(int nRow/*=-1*/) const
{
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int j,k,one=0,first,last;
  ULONG maska;
  BYTE* pm= (BYTE*)&maska;
  if (nRow==-1) { first =0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  for (k=first; k <= last; k++)
    for (j=0; j < m_nLongLength; j++) {
      maska = m_pData[k][j] | m_pData[k][m_nAllocLength+j];
      one += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//    one+=COUNTBIT();
   }
 return one;
}

//----------------- CountOne ------------------------------------------------------------
int CuTM::CountOnes(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1);
  int j, k, one=0, first, last, w;
  if (nRow==-1) { first =0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  w = m_nLongLength<<2;
  for (k=first; k <= last; k++) {
    BYTE* pm = (BYTE*)m_pData[k];
    for (j=0; j < w; j++) 
      one+=TabC[pm[j]]; // COUNTBIT(m_pData[k][j]);
  }
  return one;
}

//----------------- CountZero -----------------------------------------------------------
int CuTM::CountZeros(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1);
  int j, k, one=0, first, last, w;
  if (nRow==-1) { first =0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  w = (m_nLongLength + m_nAllocLength)<<2;
  for (k=first; k <= last; k++) {
    BYTE* pm = (BYTE*)m_pData[k];
    for (j=m_nAllocLength<<2; j < w; j++) 
      one+=TabC[pm[j]]; //COUNTBIT(m_pData[k][m_nAllocLength+j]);
  }
  return one;
}

//----------------- CountUndef ----------------------------------------------------------
int CuTM::CountUnDefs(const ULONG* mask, int nRow) const
{ 
  int j, k, one=0, first, last;
  ASSERT (nRow>=-1);
  ULONG maska;
  BYTE* pm= (BYTE*)&maska;
  if (nRow==-1) { first =0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }

  for (k=first; k <=last; k++)
    for (j=0; j < m_nLongLength; j++) {
      maska = (~(m_pData[k][j] | m_pData[k][m_nAllocLength+j])) & mask[j];
      one += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//    one+=(int)COUNTBIT();
    }
  return one;
}

//----------------- CountDef ------------------------------------------------------------
int CuTM::CountDefs(const ULONG* mask, int nRow) const
{ return m_nBitLength - CountUnDefs(); }

//----------------- CountOne ------------------------------------------------------------
int CuTM::CountOnes(const ULONG* mask, int nRow/*=-1*/)  const
{ 
  int j,k,one=0,first,last;
  ASSERT (nRow>=-1);
  ULONG maska;
  BYTE* pm= (BYTE*)&maska;
  if (nRow==-1) { first =0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }

  for (k=first; k <=last; k++)
    for (j=0; j < m_nLongLength; j++) {
      maska = m_pData[k][j] & mask[j];
      one += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//    one+=(int)COUNTBIT(m_pData[k][j]&mask[j]);
    }
  return one;
}

//----------------- CountZero ----------------------------------------------------------
int CuTM::CountZeros(const ULONG* mask, int nRow/*=-1*/)  const
{ 
  int j,k,one=0,first,last;
  ASSERT (nRow>=-1);
  ULONG maska;
  BYTE* pm= (BYTE*)&maska;
  if (nRow==-1) { first =0; last = m_nSize-1; }
  else { first = nRow; last = nRow; }
  for (k=first; k <=last; k++)
    for (j=0; j < m_nLongLength; j++) {
      maska = m_pData[k][m_nAllocLength+j] & mask[j];
      one += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//    one+=(int)COUNTBIT(m_pData[k][m_nAllocLength+j]&mask[j]);
    }
  return one;
}

//----------------- LeftUnDef -----------------------------------------------------------
int CuTM::LeftUnDef(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);  //испр. 28.04.99
 int i, j=0, pos = 0;
 if(m_nBitLength <= (nNext+1)) return (-1);
 ULONG ch, ch_last;
 i=ADR_BITLONG(m_nBitLength);
 ch_last =((~(m_pData[nRow][m_nLongLength-1] |
              m_pData[nRow][m_nAllocLength+m_nLongLength-1]))>>(S_4-i))<<(S_4-i);
 i=0;
 if (nNext >= 0) {
   i=BIT_LONG(nNext); j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 ch=(i == (m_nLongLength-1))? ch_last : 
                              (~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]));
 ch <<= j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j < m_nLongLength-1; j++)
   if ((ch=~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 if (j < m_nLongLength) 
   if(ch_last) {
     while (!(ch_last & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- LeftDef -------------------------------------------------------------
int CuTM::LeftDef(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0, j=0, pos = 0;
 ULONG ch;
 if (nNext >= 0) {
   i=BIT_LONG(nNext); j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 if (i == m_nLongLength)  return (-1);
 ch = (m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]) << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j < m_nLongLength; j++)
   if ((ch= m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- LeftOne -------------------------------------------------------------
int CuTM::LeftOne(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0, j=0, pos = 0;
 ULONG ch;
 if (nNext >= 0) {
   i=BIT_LONG(nNext); j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 if (i == m_nLongLength)  return (-1);
 ch = m_pData[nRow][i] << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j < m_nLongLength; j++)
   if ((ch= m_pData[nRow][j])) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- LeftZero ------------------------------------------------------------
int CuTM::LeftZero(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0, j=0, pos = 0;
 ULONG ch;
 if (nNext >= 0) {
   i=BIT_LONG(nNext); j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 if (i == m_nLongLength)  return (-1);
 ch = m_pData[nRow][m_nAllocLength+i] << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j < m_nLongLength; j++)
   if ((ch= m_pData[nRow][m_nAllocLength+j])) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- LeftUndef -----------------------------------------------------------
int CuTM::LeftUnDef(int nRow, int nNext, ULONG* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0, j=0, pos = 0;
 ULONG ch;
 if (nNext >= 0) {
   i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 if (i == m_nLongLength)  return (-1);
 ch = ((~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))& mask[i]) << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j<m_nLongLength; j++)
   if ((ch=(~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))&mask[j])) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- LeftDef -------------------------------------------------------------
int CuTM::LeftDef(int nRow, int nNext, ULONG* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0, j=0, pos = 0;
 ULONG ch;
 if (nNext >= 0) {
   i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 if (i == m_nLongLength)  return (-1);
 ch = ((m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])&mask[i]) << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j<m_nLongLength; j++)
   if ((ch=(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])&mask[j])) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- LeftOne -------------------------------------------------------------
int CuTM::LeftOne(int nRow, int nNext, ULONG* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0, j=0, pos = 0;
 ULONG ch;
 if (nNext >= 0) {
   i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 if (i == m_nLongLength)  return (-1);
 ch = (m_pData[nRow][i]& mask[i]) << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j<m_nLongLength; j++)
   if ((ch= m_pData[nRow][j]&mask[j])) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- LeftZero ------------------------------------------------------------
int CuTM::LeftZero(int nRow, int nNext, ULONG* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0, j=0, pos = 0;
 ULONG ch;
 if (nNext >= 0) {
   i=BIT_LONG(nNext);  j=ADR_BITLONG(nNext);
   if (++j==S_4) {j=0; i++; }
 }
 if (i == m_nLongLength)  return (-1);
 ch = (m_pData[nRow][m_nAllocLength+i]&mask[i]) << j;
 if (ch)  {
   while (!(ch & OB4[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j<m_nLongLength; j++)
   if ((ch= m_pData[nRow][m_nAllocLength+j]&mask[j])) {
     while (!(ch & OB4[pos++]));
     return ((j*S_4)+pos-1);
   }
 return (-1);
}

//----------------- RightUnDef ----------------------------------------------------------
int CuTM::RightUnDef(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])) >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j>=0; j--)
   if((ch=(~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))))  {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//----------------- RightDef ------------------------------------------------------------
int CuTM::RightDef(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j>=0; j--)
   if((ch= m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))  {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//----------------- RightOne ------------------------------------------------------------
int CuTM::RightOne(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = (m_pData[nRow][i] >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j>=0; j--)
   if((ch= m_pData[nRow][j]))  {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}


//----------------- RightZero ----------------------------------------------------------
int CuTM::RightZero(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = (m_pData[nRow][m_nAllocLength+i] >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j>=0; j--)
   if((ch= m_pData[nRow][m_nAllocLength+j]))  {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//----------------- RightUnDef ----------------------------------------------------------
int CuTM::RightUnDef(int nRow, int nPrev/*=-1*/,ULONG* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))& mask[i] >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j >= 0; j--)
   if((ch=(~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))& mask[i])) {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//----------------- RightDef ----------------------------------------------------------
int CuTM::RightDef(int nRow, int nPrev/*=-1*/,ULONG* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = (((m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])& mask[i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j >= 0; j--)
   if((ch=(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])& mask[i])) {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//----------------- RightOne ------------------------------------------------------------
int CuTM::RightOne(int nRow, int nPrev/*=-1*/,ULONG* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((m_pData[nRow][i]& mask[i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j >= 0; j--)
   if((ch= m_pData[nRow][j]& mask[i])) {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//----------------- RightZero -----------------------------------------------------------
int CuTM::RightZero(int nRow, int nPrev/*=-1*/,ULONG* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i= m_nLongLength-1, j=0, pos = 7;
 ULONG ch;
 if (nPrev >= 0) {
   i=BIT_LONG(nPrev); j=S_4 - ADR_BITLONG(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((m_pData[nRow][m_nAllocLength+i]& mask[i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB4[pos--]));
   return ((i*S_4)+pos+1);
 }
 for (j=i-1; j >= 0; j--)
   if((ch= m_pData[nRow][m_nAllocLength+j]& mask[i])) {
     while (!(ch & OB4[pos--]));
     return ((j*S_4)+pos+1);
   }
 return (-1);
}

//----------------- FindUndefRow --------------------------------------------------------
int CuTM::FindUnDefR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 ULONG work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=work=0; j < m_nLongLength; j++) 
      if((work |= m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]) !=0)
        break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindDefRow ----------------------------------------------------------
int CuTM::FindDefR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0; j < m_nLongLength; j++) 
      if((m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]) !=0)
        return nRow;
 }    
 return -1;
}                               

//----------------- FindOneRow ----------------------------------------------------------
int CuTM::FindOneR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 ULONG work, maska;
 maska=((j = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1<<(S_4 - j)):LONG_1;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work= m_pData[nRow][m_nLongLength-1]^maska;
        j < m_nLongLength-1; j++) 
      if((work |= m_pData[nRow][j]^LONG_1) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindZeroRow ---------------------------------------------------------
int CuTM::FindZeroR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 ULONG work, maska;
 maska=((j = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1<<(S_4 - j)):LONG_1;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work= m_pData[nRow][m_nAllocLength+m_nLongLength-1]^maska;
        j < m_nLongLength-1; j++) 
      if((work |= m_pData[nRow][m_nAllocLength+j]^LONG_1) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindUndefRow --------------------------------------------------------
int CuTM::FindUnDefR(ULONG* mask, int nRow/*=-1*/) const
{int j;
 ULONG work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=work=0; j < m_nLongLength; j++) 
     if((work|=(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])&mask[j]) !=0)
        break;
   if (work == 0) return nRow;
 }   
 return -1;
}                               

//----------------- FindDefRow --------------------------------------------------------
int CuTM::FindDefR (ULONG* mask, int nRow/*=-1*/) const
{int j;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0; j < m_nLongLength; j++) 
     if(((m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])&mask[j]) !=0)
       return nRow;
 }   
 return -1;
}

//----------------- FindOneRow ----------------------------------------------------------
int CuTM::FindOneR (ULONG* mask, int nRow/*=-1*/) const
{int j;
 ULONG work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work= m_pData[nRow][m_nLongLength-1]^mask[m_nLongLength-1];
        j < m_nLongLength-1; j++) 
      if((work |= m_pData[nRow][j]^mask[j]) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindZeroRow ---------------------------------------------------------
int CuTM::FindZeroR (ULONG* mask, int nRow/*=-1*/) const
{int j;
 ULONG work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work= m_pData[nRow][m_nAllocLength+m_nLongLength-1]^mask[m_nLongLength-1];
        j < m_nLongLength-1; j++) 
      if((work |= m_pData[nRow][m_nAllocLength+j]^mask[j]) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                                                        

//----------------- FindRow -------------------------------------------------------------
int CuTM::FindR (const CuTV& tv, int nRow) const
{
  if (IsEmpty()) return -1;     // 21.03.2006
  ASSERT(m_nBitLength == tv.GetBitLength());
  for (nRow++; nRow < m_nSize; nRow++)  
    if((memcmp(tv.GetOnes(), m_pData[nRow], m_nLongLength*sizeof(ULONG)) == 0) &&
       (memcmp(tv.GetZeros(), m_pData[nRow]+m_nAllocLength, 
               m_nLongLength*sizeof(ULONG))== 0)) return nRow;
  return -1;
}

//----------------- FindRow -------------------------------------------------------------
int CuTM::FindR (const CuBV& bv, int nRow) const
{
  if (IsEmpty()) return -1;     // 21.03.2006
  ASSERT(m_nBitLength == bv.GetBitLength());
  for (nRow++; nRow < m_nSize; nRow++)  
    if((memcmp((ULONG*)bv, m_pData[nRow], m_nLongLength*sizeof(ULONG)) == 0) &&
       (memcmp((ULONG*)~bv, m_pData[nRow]+m_nAllocLength, 
               m_nLongLength*sizeof(ULONG)) == 0)) return nRow;
  return -1;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Matrix Compareing and sorting operations ///////////////////////////////////////
//----------------- IsCorrect -----------------------------------------------------------
BOOL CuTM::IsCorrect(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++)
   for (i=0; i < m_nLongLength; i++)
     if((m_pData[first][i]&m_pData[first][m_nAllocLength+i]) != 0) return FALSE;
 return TRUE;
}

//----------------- IsBool --------------------------------------------------------------
BOOL CuTM::IsBool(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 ULONG maska;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1<<(S_4 - i)):LONG_1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) {
   for (i=0; i < m_nLongLength-1; i++)
     if(((m_pData[first][i]|m_pData[first][m_nAllocLength+i])^LONG_1) != 0)
       return FALSE;
   if(((m_pData[first][m_nLongLength-1] |
        m_pData[first][m_nAllocLength+m_nLongLength-1])^maska) != 0) return FALSE;
 }
 return TRUE;
}

//----------------- IsBool --------------------------------------------------------------
BOOL CuTM::IsBool(const ULONG* mask, int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if(((m_pData[first][i]|m_pData[first][m_nAllocLength+i])^mask[i]) != 0)
       return FALSE;
 return TRUE;
}


//----------------- IsTrivial -----------------------------------------------------------
BOOL CuTM::IsTrivial(int nRow/*=-1*/) const  //all components are '-'
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if((m_pData[first][i]|m_pData[first][m_nAllocLength+i]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- IsTrivial -----------------------------------------------------------
BOOL CuTM::IsTrivial(const ULONG* mask, int nRow/*=-1*/) const  //all components are '-'
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if(((m_pData[first][i]|m_pData[first][m_nAllocLength+i])&mask[i]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- IsOne ---------------------------------------------------------------
BOOL CuTM::IsOne(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 ULONG maska;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1<<(S_4 - i)):LONG_1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) {
   for (i=0; i < m_nLongLength-1; i++)
     if((m_pData[first][i]^LONG_1) != 0)
       return FALSE;
   if((m_pData[first][m_nLongLength-1]^maska) != 0) return FALSE;
 }
 return TRUE;
}

//----------------- IsOne ---------------------------------------------------------------
BOOL CuTM::IsOne(const ULONG* mask, int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if(((m_pData[first][i]&mask[i])^mask[i]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- IsZero --------------------------------------------------------------
BOOL CuTM::IsZero(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 ULONG maska;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BITLONG(m_nBitLength)) > 0 )?(LONG_1<<(S_4 - i)):LONG_1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) {
   for (i=0; i < m_nLongLength-1; i++)
     if((m_pData[first][m_nAllocLength+i]^LONG_1) != 0)
       return FALSE;
   if((m_pData[first][m_nAllocLength+m_nLongLength-1]^maska) != 0)
     return FALSE;
 }
 return TRUE;
}

//----------------- IsZero --------------------------------------------------------------
BOOL CuTM::IsZero(const ULONG* mask, int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (m_nLongLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++)
   for (i=0; i < m_nLongLength; i++)
     if(((m_pData[first][m_nAllocLength+i]&mask[i])^mask[i]) != 0)
       return FALSE;
 return TRUE;
}

//--------------------------- ExistOne --------------------------------------------------
BOOL CuTM::ExistOne(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if(m_pData[first][i] != 0) return TRUE;
 return FALSE;
}

//--------------------------- ExistOne --------------------------------------------------
BOOL CuTM::ExistOne(const ULONG* mask, int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if((m_pData[first][i]&mask[i])!= 0) return TRUE;
 return FALSE;
}

//--------------------------- ExistZero -------------------------------------------------
BOOL CuTM::ExistZero(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if(m_pData[first][m_nAllocLength+i] != 0) return TRUE;
 return FALSE;
}

//--------------------------- ExistZero --------------------------------------------------
BOOL CuTM::ExistZero(const ULONG* mask, int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i, first =0,last = m_nSize-1;
 if (nRow != -1) first =last = nRow;
 for (; first <= last; first++) 
   for (i=0; i < m_nLongLength; i++)
     if((m_pData[first][m_nAllocLength+i]&mask[i])!= 0) return TRUE;
 return FALSE;
}

//----------------- SortDecr ------------------------------------------------------------
void CuTM::SortDecr(int nBeg/*=0*/, int number/*=-1*/)
{ASSERT(number == -1 || number > 1);
 BOOL bNotDone = TRUE;
 int pos, posNext, nEnd=(number < 0)?m_nSize:(nBeg+number);
 ASSERT(nEnd <= m_nSize);
 while (bNotDone) {
   bNotDone = FALSE;
   for(pos= nBeg, posNext = nBeg+1; pos < nEnd-1;pos++, posNext++)
    if(CountDefs(pos) < CountDefs(posNext)) {
      ExchangeRow(pos, posNext);
      bNotDone = TRUE;
    }
 }
}

//----------------- SortIncr ------------------------------------------------------------
void CuTM::SortIncr(int nBeg/*=0*/, int number/*=-1*/)
{ASSERT(number == -1 || number > 1);
 BOOL bNotDone = TRUE;
 int pos, posNext, nEnd=(number < 0)? m_nSize : (nBeg + number);
 ASSERT(nEnd <= m_nSize);
 while (bNotDone) {
   bNotDone = FALSE;
   for(pos= nBeg, posNext = nBeg+1; pos < nEnd-1; pos++, posNext++)
     if(CountDefs(pos) > CountDefs(posNext)) {
       ExchangeRow(pos, posNext);
       bNotDone = TRUE;
     }
 }
}

//----------------- operator== ----------------------------------------------------------
BOOL operator==(const CuTM& tm1, const CuTM& tm2)
{ if ((tm1.m_nSize != tm2.m_nSize) || (tm1.m_nBitLength != tm2.m_nBitLength))
   return FALSE;
 for (int i=0; i < tm1.m_nSize; i++)
   for (int j=0; j < tm1.m_nLongLength; j++) 
     if ((tm1.m_pData[i][j] != tm2.m_pData[i][j]) ||
         (tm1.m_pData[i][tm1.m_nAllocLength+j] != 
          tm2.m_pData[i][tm2.m_nAllocLength+j])) 
       return FALSE;
 return TRUE;
}

//----------------- operator!= ----------------------------------------------------------
BOOL operator!=(const CuTM& tm1, const CuTM& tm2)
{if ((tm1.m_nSize != tm2.m_nSize) || (tm1.m_nBitLength != tm2.m_nBitLength))
   return TRUE;
 for (int i=0; i < tm1.m_nSize; i++)
   for (int j=0; j < tm1.m_nLongLength; j++)
     if ((tm1.m_pData[i][j] != tm2.m_pData[i][j]) ||
         (tm1.m_pData[i][tm1.m_nAllocLength+j] != 
          tm2.m_pData[i][tm2.m_nAllocLength+j])) 
     return TRUE;
 return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Row compareing operations ///////////////////////////////////////////
//----------------- AreEqual ------------------------------------------------------------
BOOL CuTM::AreEqual(int nRow1, int nRow2) const
{// ASSERT_VALID(this);
 for (int j=0; j < m_nLongLength; j++)
     if (((m_pData[nRow1][j] != m_pData[nRow2][j]) ||
          (m_pData[nRow1][m_nAllocLength+j] != m_pData[nRow2][m_nAllocLength+j])))
       return FALSE;
 return TRUE;
}

//----------------- AreEqual ------------------------------------------------------------
BOOL CuTM::AreEqual(const ULONG* mask, int nRow1, int nRow2) const
{// ASSERT_VALID(this);
 for (int j=0; j < m_nLongLength; j++)
     if ((((m_pData[nRow1][m_nAllocLength+j]^m_pData[nRow2][m_nAllocLength+j])|
     (m_pData[nRow1][j]^m_pData[nRow2][j]))&mask[j]) != 0)
       return FALSE;
 return TRUE;
}
//----------------- AreEqual ------------------------------------------------------------
BOOL CuTM::AreEqual(int nRow, const CuTV& tv) const
{// ASSERT_VALID(this);
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 for (int j=0; j < m_nLongLength; j++)
     if (((m_pData[nRow][j] != tv.m_bVect1[j]) ||
          (m_pData[nRow][m_nAllocLength+j] != tv.m_bVect0[j])))
       return FALSE;
 return TRUE;
}

//----------------- AreEqual ------------------------------------------------------------
BOOL CuTM::AreEqual(const ULONG* mask, int nRow, const CuTV& tv) const
{// ASSERT_VALID(this);
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 for (int j=0; j < m_nLongLength; j++)
  if ((((m_pData[nRow][m_nAllocLength+j]^tv.m_bVect0[j])|
     (m_pData[nRow][j]^tv.m_bVect1[j]))&mask[j]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- AreNotEqual ------------------------------------------------------------
BOOL CuTM::AreNotEqual(int nRow1, int nRow2) const
{// ASSERT_VALID(this);
  for (int j=0; j < m_nLongLength; j++)
    if (((m_pData[nRow1][j] != m_pData[nRow2][j]) ||
         (m_pData[nRow1][m_nAllocLength+j] != m_pData[nRow2][m_nAllocLength+j])))
    return TRUE;
  return FALSE;
}

//----------------- AreNotEqual ------------------------------------------------------------
BOOL CuTM::AreNotEqual(const ULONG* mask, int nRow1, int nRow2) const
{
  // ASSERT_VALID(this);
  for (int j=0; j < m_nLongLength; j++)
    if ((((m_pData[nRow1][m_nAllocLength+j]^m_pData[nRow2][m_nAllocLength+j])|
          (m_pData[nRow1][j]^m_pData[nRow2][j]))&mask[j]) != 0)
      return TRUE;
  return FALSE;
}
//----------------- AreNotEqual ------------------------------------------------------------
BOOL CuTM::AreNotEqual(int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++)
    if (((m_pData[nRow][j] != tv.m_bVect1[j]) ||
         (m_pData[nRow][m_nAllocLength+j] != tv.m_bVect0[j])))
      return TRUE;
  return FALSE;
}

//----------------- AreNotEqual ------------------------------------------------------------
BOOL CuTM::AreNotEqual(const ULONG* mask, int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++)
    if ((((m_pData[nRow][m_nAllocLength+j]^tv.m_bVect0[j])|
          (m_pData[nRow][j]^tv.m_bVect1[j]))&mask[j]) != 0)
     return TRUE;
  return FALSE;
}

//----------------- AreOrthog -----------------------------------------------------------
BOOL CuTM::AreOrthog(int nRow1, int nRow2) const
{
  // ASSERT_VALID(this);
  for (int j=0; j < m_nLongLength; j++)
    if ((m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j] ||
         m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]))
      return TRUE;
  return FALSE;
}

//----------------- AreOrthog -----------------------------------------------------------
BOOL CuTM::AreOrthog(const ULONG* mask, int nRow1, int nRow2) const
{
  // ASSERT_VALID(this);
  for (int j=0; j < m_nLongLength; j++)
    if((((m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j])|
         (m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]))&mask[j]) !=0)
      return TRUE;
  return FALSE;
}

//----------------- AreOrthog -----------------------------------------------------------
BOOL CuTM::AreOrthog(int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++)
    if (((m_pData[nRow][j]&tv.m_bVect0[j]) ||
         (m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j])))
      return TRUE;
  return FALSE;
}

//----------------- AreOrthog -----------------------------------------------------------
BOOL CuTM::AreOrthog(const ULONG* mask, int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++)
    if ((((m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j])|
          (m_pData[nRow][j]&tv.m_bVect0[j]))&mask[j]) != 0)
      return TRUE;
  return FALSE;
}

//----------------- AreOrthog -----------------------------------------------------------
BOOL CuTM::AreOrthog(int nRow, const CuBV& bv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++)
    if (((m_pData[nRow][j] & (~bv.GetLongAt(j))) ||
         (m_pData[nRow][m_nAllocLength+j] & bv.GetLongAt(j))))
      return TRUE;
  return FALSE;
}

//----------------- AreOrthog -----------------------------------------------------------
BOOL CuTM::AreOrthog(const ULONG* mask, int nRow, const CuBV& bv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++)
    if ((((m_pData[nRow][m_nAllocLength+j] & bv.GetLongAt(j))|
          (m_pData[nRow][j] & (~bv.GetLongAt(j)))) & mask[j]) != 0)
      return TRUE;
  return FALSE;
}

//----------------- AreNeighb -----------------------------------------------------------
BOOL CuTM::AreNeighb(int nRow1, int nRow2) const
{
  // ASSERT_VALID(this);
  int j, i;
  ULONG  mas1, mas0;
  BYTE* pm = (BYTE*)&mas1;
  for (i=j=0; j < m_nLongLength; j++) {
    if(((mas0 = m_pData[nRow1][m_nAllocLength+j] ^ m_pData[nRow2][m_nAllocLength+j])^
        (mas1 = m_pData[nRow1][j]^m_pData[nRow2][j]))) return FALSE;
    i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//     COUNTBIT(mas1);
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreNeighb -----------------------------------------------------------
BOOL CuTM::AreNeighb(const ULONG* mask, int nRow1, int nRow2) const
{
  // ASSERT_VALID(this);
  int j, i;
  ULONG  mas1, mas0;
  BYTE* pm = (BYTE*)&mas1;
  for (j=i=0; j < m_nLongLength; j++) {
    if((((mas0 = m_pData[nRow1][m_nAllocLength+j] ^ m_pData[nRow2][m_nAllocLength+j])^
         (mas1 = m_pData[nRow1][j] ^ m_pData[nRow2][j])) & mask[j]) !=0) return FALSE;
    mas1 &=mask[j];
    i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//   i+=COUNTBIT(mas1&mask[j]);
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreNeighb -----------------------------------------------------------
BOOL CuTM::AreNeighb(int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  int i, j;
  ULONG  mas1, mas0;
  BYTE* pm = (BYTE*)&mas1;
  for (i=j=0; j < m_nLongLength; j++) {
    if(((mas0 = m_pData[nRow][m_nAllocLength+j] ^ tv.m_bVect0[j])^
        (mas1 = m_pData[nRow][j] ^ tv.m_bVect1[j]))) return FALSE;
    i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//   i+=COUNTBIT(mas1);
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreNeighb -----------------------------------------------------------
BOOL CuTM::AreNeighb(const ULONG* mask, int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  int i, j;
  ULONG  mas1, mas0;
  BYTE* pm = (BYTE*)&mas1;
  for (j=i=0; j < m_nLongLength; j++) {
    if((((mas0 = m_pData[nRow][m_nAllocLength+j] ^ tv.m_bVect0[j]) ^
         (mas1 = m_pData[nRow][j] ^ tv.m_bVect1[j])) & mask[j]) !=0) return FALSE;
    mas1 &=mask[j];
    i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//   i+=COUNTBIT(mas1&mask[j]);
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ----------------------------------------------------------
BOOL CuTM::AreConcens(int nRow1, int nRow2) const
{
  // ASSERT_VALID(this);
  int j, i;
  ULONG  mas;
  BYTE* pm = (BYTE*)&mas;
  for (i=j=0; j < m_nLongLength; j++) {
    if((mas=(m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]) |
       (m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j]))!= 0)
      i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//     i+=COUNTBIT(mas);
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ----------------------------------------------------------
BOOL CuTM::AreConcens(const ULONG* mask, int nRow1, int nRow2) const
{
  // ASSERT_VALID(this);
  int j, i;
  ULONG  mas;
  BYTE* pm = (BYTE*)&mas;
  for (j=i=0; j < m_nLongLength; j++) {
    if(((mas=(m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]) |
      (m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j]))&mask[j]) != 0) {
      mas &= mask[j];
      i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//    i+=COUNTBIT(mas&mask[j]);
    }
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ----------------------------------------------------------
BOOL CuTM::AreConcens(int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  int i, j;
  ULONG  mas;
  BYTE* pm = (BYTE*)&mas;
  for (i=j=0; j < m_nLongLength; j++) {
    if((mas=(m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]) |
       (m_pData[nRow][j]&tv.m_bVect0[j]))!= 0)
      i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//    i+=COUNTBIT(mas);
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ----------------------------------------------------------
BOOL CuTM::AreConcens(const ULONG* mask, int nRow, const CuTV& tv) const
{
  // ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  int i, j;
  ULONG  mas;
  BYTE* pm = (BYTE*)&mas;
  for (j=i=0; j < m_nLongLength; j++) {
    if(((mas=(m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]) |
             (m_pData[nRow][j]&tv.m_bVect0[j]))&mask[j]) != 0) {
      mas &= mask[j];
      i += TabC[pm[0]] + TabC[pm[1]] + TabC[pm[2]] + TabC[pm[3]];
//     i+=COUNTBIT(mas&mask[j]);
    }
  }
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- IsCover -------------------------------------------------------------
BOOL CuTM::IsCover(int nRow1, int nRow2) const
{// ASSERT_VALID(this);
  for (int j=0; j < m_nLongLength; j++) { 
    if((((m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][m_nAllocLength+j])^
          m_pData[nRow1][m_nAllocLength+j]) ==0) &&
       (((m_pData[nRow1][j]&m_pData[nRow2][j])^m_pData[nRow1][j]) == 0)) continue;
      return FALSE;
  }
  return TRUE;
}

//----------------- IsCover -------------------------------------------------------------
BOOL CuTM::IsCover(const ULONG* mask, int nRow1, int nRow2) const
{// ASSERT_VALID(this);
  for (int j=0; j < m_nLongLength; j++) 
    if((((m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][m_nAllocLength+j])^
          m_pData[nRow1][m_nAllocLength+j])|
        ((m_pData[nRow1][j]&m_pData[nRow2][j])^m_pData[nRow1][j])&mask[j]) !=0)
      return FALSE;
  return TRUE;
}

//----------------- IsCover -------------------------------------------------------------
BOOL CuTM::IsCover(int nRow, const CuTV& tv) const
{// ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++) {
    if((((m_pData[nRow][m_nAllocLength+j] & tv.m_bVect0[j])^
          m_pData[nRow][m_nAllocLength+j]) ==0) &&
       (((m_pData[nRow][j]&tv.m_bVect1[j])^m_pData[nRow][j]) == 0)) continue;
      return FALSE; 
  }
  return TRUE;
}

//----------------- IsCover -------------------------------------------------------------
BOOL CuTM::IsCover(const ULONG* mask, int nRow, const CuTV& tv) const
{// ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
int i, j;
  for (j=i=0; j < m_nLongLength; j++) 
    if((((m_pData[nRow][m_nAllocLength+j]&tv.m_bVect0[j])^
          m_pData[nRow][m_nAllocLength+j])|
        ((m_pData[nRow][j]&tv.m_bVect1[j])^m_pData[nRow][j])&mask[j]) !=0)
     return FALSE;
  return TRUE;
}

//----------------- IsCover -------------------------------------------------------------
BOOL CuTM::IsCover(int nRow, const CuBV& bv) const
{// ASSERT_VALID(this);
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  ULONG part;
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    if((((m_pData[nRow][m_nAllocLength+j]&(~part)) ^ 
          m_pData[nRow][m_nAllocLength+j]) ==0) ||
       (((m_pData[nRow][j]&part)^m_pData[nRow][j]) == 0)) continue;
      return FALSE;
  }
  return TRUE;
}

//----------------- IsCover -------------------------------------------------------------
BOOL CuTM::IsCover(const ULONG* mask, int nRow, const CuBV& bv) const
{// ASSERT_VALID(this);
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  ULONG  part;
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    if(((((m_pData[nRow][m_nAllocLength+j] & (~part))^
           m_pData[nRow][m_nAllocLength+j])|
         ((m_pData[nRow][j]&part) ^ m_pData[nRow][j])) & mask[j]) !=0) 
      return FALSE;
  }
  return TRUE;
}

//----------------- IsCovered -----------------------------------------------------------
BOOL CuTM::IsCovered(int nRow, const CuTV& tv) const
{// ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++) {
    if((((m_pData[nRow][m_nAllocLength+j] & tv.m_bVect0[j]) ^ tv.m_bVect0[j]) ==0)&&
        (((m_pData[nRow][j] & tv.m_bVect1[j]) ^ tv.m_bVect1[j]) == 0)) continue;
     return FALSE;
  }
  return TRUE;
}

//----------------- IsCovered -----------------------------------------------------------
BOOL CuTM::IsCovered(const ULONG* mask, int nRow, const CuTV& tv) const
{// ASSERT_VALID(this);
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  for (int j=0; j < m_nLongLength; j++) 
    if((((m_pData[nRow][m_nAllocLength+j] & tv.m_bVect0[j]) ^ tv.m_bVect0[j])|
     ((m_pData[nRow][j] & tv.m_bVect1[j]) ^ tv.m_bVect1[j]) & mask[j]) !=0) 
      return FALSE;
  return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////// Row interval operations /////////////////////////////////////////////
//--------------------------- Orthogon --------------------------------------------------
CuBV CuTM::Orthogon(int nRow1, int nRow2) const
{// ASSERT_VALID(this);
  CuBV bvTag(m_nBitLength, FALSE);
  for (int j=0; j < m_nLongLength; j++)
    bvTag.SetLongAt(j,((m_pData[nRow1][j] & m_pData[nRow2][m_nAllocLength+j]|
                      (m_pData[nRow1][m_nAllocLength+j] & m_pData[nRow2][j]))));
  return bvTag;
}

//--------------------------- Orthogon --------------------------------------------------
CuBV CuTM::Orthogon(const ULONG* mask, int nRow1, int nRow2) const
{// ASSERT_VALID(this);
  CuBV bvTag(m_nBitLength, FALSE);
  for (int j=0; j < m_nLongLength; j++)
    bvTag.SetLongAt(j,((m_pData[nRow1][j] & m_pData[nRow2][m_nAllocLength+j]|
          (m_pData[nRow1][m_nAllocLength+j] & m_pData[nRow2][j])) & mask[j]));
  return bvTag;
}

//--------------------------- Orthogon --------------------------------------------------
CuBV CuTM::Orthogon(int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuBV bvTag(m_nBitLength, FALSE);
  for (int j=0; j < m_nLongLength; j++)
    bvTag.SetLongAt(j,((m_pData[nRow][j] & tv.m_bVect0[j]|
                       (m_pData[nRow][m_nAllocLength+j] & tv.m_bVect1[j]))));
  return bvTag;
}

//--------------------------- Orthogon --------------------------------------------------
CuBV CuTM::Orthogon(const ULONG* mask, int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuBV bvTag(m_nBitLength, FALSE);
  for (int j=0; j < m_nLongLength; j++)
    bvTag.SetLongAt(j,((m_pData[nRow][j] & tv.m_bVect0[j]|
          (m_pData[nRow][m_nAllocLength+j] & tv.m_bVect1[j])) & mask[j]));
  return bvTag;
}

//--------------------------- Orthogon --------------------------------------------------
CuBV CuTM::Orthogon(int nRow, const CuBV& bv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG part;
  CuBV bvTag(m_nBitLength, FALSE);
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    bvTag.SetLongAt(j,((m_pData[nRow][j] & (~part))|
          (m_pData[nRow][m_nAllocLength+j] & part)));
  }
  return bvTag;
}

//--------------------------- Orthogon --------------------------------------------------
CuBV CuTM::Orthogon(const ULONG* mask, int nRow, const CuBV& bv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG part;
  CuBV bvTag(m_nBitLength, FALSE);
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    bvTag.SetLongAt(j,((m_pData[nRow][j] & (~part))|
          (m_pData[nRow][m_nAllocLength+j] & part)) & mask[j]);
  }
  return bvTag;
}

//--------------------------- Intersec --------------------------------------------------
CuTV CuTM::Intersec(int nRow1, int nRow2) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = m_pData[nRow1][j] | m_pData[nRow2][j];
    tvTag.m_bVect0[j] = m_pData[nRow1][m_nAllocLength+j]|
          m_pData[nRow2][m_nAllocLength+j];
  }
  return tvTag;
}

//--------------------------- Intersec --------------------------------------------------
CuTV CuTM::Intersec(const ULONG* mask, int nRow1, int nRow2) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = (m_pData[nRow1][j] | m_pData[nRow2][j]) & mask[j];
    tvTag.m_bVect0[j] = (m_pData[nRow1][m_nAllocLength+j]|
          m_pData[nRow2][m_nAllocLength+j]) & mask[j];
  }
  return tvTag;
}

//--------------------------- Intersec --------------------------------------------------
CuTV CuTM::Intersec(int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = m_pData[nRow][j] | tv.m_bVect1[j];
    tvTag.m_bVect0[j] = m_pData[nRow][m_nAllocLength+j] | tv.m_bVect0[j];
  }
  return tvTag;
}

//--------------------------- Intersec --------------------------------------------------
CuTV CuTM::Intersec(const ULONG* mask, int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = (m_pData[nRow][j] | tv.m_bVect1[j])&mask[j];
    tvTag.m_bVect0[j] = (m_pData[nRow][m_nAllocLength+j] | tv.m_bVect0[j]) & mask[j];
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTM::Concens(int nRow1, int nRow2) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG maska;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    maska = (m_pData[nRow1][j] & m_pData[nRow2][m_nAllocLength+j])|
            (m_pData[nRow1][m_nAllocLength+j] & m_pData[nRow2][j]);
    tvTag.m_bVect1[j] = (m_pData[nRow1][j] | m_pData[nRow2][j]) ^ maska;
    tvTag.m_bVect0[j] = (m_pData[nRow1][m_nAllocLength+j]|
                         m_pData[nRow2][m_nAllocLength+j]) ^ maska;
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTM::Concens(const ULONG* mask, int nRow1, int nRow2) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG maska;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    maska = (m_pData[nRow1][j] & m_pData[nRow2][m_nAllocLength+j])|
            (m_pData[nRow1][m_nAllocLength+j] & m_pData[nRow2][j]);
    tvTag.m_bVect1[j] = ((m_pData[nRow1][j] | m_pData[nRow2][j]) ^ maska) & mask[j];
    tvTag.m_bVect0[j] = ((m_pData[nRow1][m_nAllocLength+j]|
                          m_pData[nRow2][m_nAllocLength+j]) ^ maska) & mask[j];
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTM::Concens(int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG maska;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    maska = (m_pData[nRow][j] & tv.m_bVect0[j])|
            (m_pData[nRow][m_nAllocLength+j] & tv.m_bVect1[j]);
    tvTag.m_bVect1[j] = (m_pData[nRow][j] | tv.m_bVect1[j]) ^ maska;
    tvTag.m_bVect0[j] = (m_pData[nRow][m_nAllocLength+j] | tv.m_bVect0[j]) ^ maska;
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTM::Concens(const ULONG* mask, int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG maska;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    maska = (m_pData[nRow][j] & tv.m_bVect0[j])|
            (m_pData[nRow][m_nAllocLength+j] & tv.m_bVect1[j]);
    tvTag.m_bVect1[j] = ((m_pData[nRow][j] | tv.m_bVect1[j]) ^ maska) & mask[j];
    tvTag.m_bVect0[j] = ((m_pData[nRow][m_nAllocLength+j] | tv.m_bVect0[j]) ^ maska) & mask[j];
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTM::Concens(const ULONG* mask, int nRow, const CuBV& bv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG maska, part;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    maska = (~((m_pData[nRow][j] & (~part)|
               (m_pData[nRow][m_nAllocLength+j] & part)))) & mask[j];
    tvTag.m_bVect1[j] = part & maska;
    tvTag.m_bVect0[j] = (~part) & maska;
  }
  return tvTag;
}

//--------------------------- Concens ---------------------------------------------------
CuTV CuTM::Concens(int nRow, const CuBV& bv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG maska, part;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    maska = ~((m_pData[nRow][j] & (~part))|
              (m_pData[nRow][m_nAllocLength+j] & part));
    tvTag.m_bVect1[j] = part & maska;
    tvTag.m_bVect0[j] = (~part) & maska;
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTM::MinCover(int nRow1, int nRow2) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = m_pData[nRow1][j] & m_pData[nRow2][j];
    tvTag.m_bVect0[j] = m_pData[nRow1][m_nAllocLength+j]&
                        m_pData[nRow2][m_nAllocLength+j];
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTM::MinCover(const ULONG* mask, int nRow1, int nRow2) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = m_pData[nRow1][j] & m_pData[nRow2][j] & mask[j];
    tvTag.m_bVect0[j] = m_pData[nRow1][m_nAllocLength+j]&
                        m_pData[nRow2][m_nAllocLength+j] & mask[j];
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTM::MinCover(int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = m_pData[nRow][j] & tv.m_bVect1[j];
    tvTag.m_bVect0[j] = m_pData[nRow][m_nAllocLength+j] & tv.m_bVect0[j];
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTM::MinCover(const ULONG* mask, int nRow, const CuTV& tv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    tvTag.m_bVect1[j] = m_pData[nRow][j] & tv.m_bVect1[j] & mask[j];
    tvTag.m_bVect0[j] = m_pData[nRow][m_nAllocLength+j]&
                        tv.m_bVect0[j] & mask[j];
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTM::MinCover(int nRow, const CuBV& bv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG part;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    tvTag.m_bVect1[j] = m_pData[nRow][j] & part;
    tvTag.m_bVect0[j] = m_pData[nRow][m_nAllocLength+j] & (~part);
  }
  return tvTag;
}

//--------------------------- MinCover --------------------------------------------------
CuTV CuTM::MinCover(const ULONG* mask, int nRow, const CuBV& bv) const    //мн-во ортогон.компонент
{// ASSERT_VALID(this);
  ULONG part;
  CuTV tvTag(m_nBitLength);
  for (int j=0; j < m_nLongLength; j++) {
    part = bv.GetLongAt(j);
    tvTag.m_bVect1[j] = m_pData[nRow][j] & part & mask[j];
    tvTag.m_bVect0[j] = m_pData[nRow][m_nAllocLength+j] & (~part) & mask[j];
  }
  return tvTag;
}

//----------------- CuTMtoCuBM ------------------------------------------------------------ 
CuBM CuTM::CuTMtoCuBM(void) const    // раскрыть интервалы
{// ASSERT_VALID(this);
  CuBM bm(m_nBitLength, 0);
  CuBM bmTag(m_nSize, m_nBitLength);
  CuTV tv(m_nBitLength);
  for(int i=0; i < m_nSize; i++) {
    tv=GetRowTv(i); bm= tv.CuTVtoCuBM();
    bmTag.Add(bm,-1);
  }
  return bmTag;
}

//////////////////// Operations on TERNARY VECTOR PARTS /////////////////////////////////
/////////////////////////// Row Logic operations ////////////////////////////////////////
//--------------------------- AndInPlace ------------------------------------------------
void CuTM::AndInPlace(BOOL Part, const CuBV& bv, int Row)
{// ASSERT_VALID(this);
 ASSERT(m_nBitLength == bv.GetBitLength());
  int i;
  if(Part)
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][i]&= bv.GetLongAt(i);
  else
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][m_nAllocLength+i]&= bv.GetLongAt(i);
  return;
}

//--------------------------- OrInPlace -------------------------------------------------
void CuTM::OrInPlace(BOOL Part, const CuBV& bv, int Row)
{// ASSERT_VALID(this);
  ASSERT(m_nBitLength == bv.GetBitLength());
  int i;
  if(Part)
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][i]|= bv.GetLongAt(i);
  else
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][m_nAllocLength+i]|= bv.GetLongAt(i);
  return;
}

//--------------------------- XorInPlace ------------------------------------------------
void CuTM::XorInPlace(BOOL Part, const CuBV& bv, int Row)
{// ASSERT_VALID(this);
 ASSERT(m_nBitLength == bv.GetBitLength());
   int i;
  if(Part)
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][i]^= bv.GetLongAt(i);
  else
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][m_nAllocLength+i]^= bv.GetLongAt(i);
  return;
}

//--------------------------- SubInPlace ------------------------------------------------
void CuTM::SubInPlace(BOOL Part, const CuBV& bv, int Row)
{// ASSERT_VALID(this);
  ASSERT(m_nBitLength == bv.GetBitLength());
  int i;
  if(Part)
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][i]&=~bv.GetLongAt(i);
  else
    for (i=0; i < m_nLongLength; i++)
      m_pData[Row][m_nAllocLength+i]&=~bv.GetLongAt(i);
  return;
}

//--------------------------- InvertBitsInPlace -------------------------------------------
void CuTM::InvertBitsInPlace(BOOL Part)
{ ASSERT(m_nBitLength != 0);
  ULONG maska;
  int i, j;
  maska = ((i = ADR_BITLONG(m_nBitLength)) == 0)? LONG_1 : (~(LONG_1>>i));
  if(Part)
    for (i=0; i < m_nSize; i++) {
      for (j=0; j < m_nLongLength; j++) m_pData[i][j]=~m_pData[i][j];
      m_pData[i][m_nAllocLength-1]&= maska;
    }
  else
    for (i=0; i < m_nSize; i++) {
      for (j=0; j < m_nLongLength; j++)
         m_pData[i][j]=~m_pData[i][m_nAllocLength+j];
      m_pData[i][m_nAllocLength+m_nLongLength-1]&= maska;
    }
  return;
}

//------------------------ InvertBitsInPlace -------------------------------------------
void CuTM::InvertBitsInPlace(BOOL Part, int Row)
{ ASSERT(m_nBitLength != 0);
  int j;
  ULONG maska;
  maska = ((j = ADR_BITLONG(m_nBitLength)) == 0)? LONG_1 : (~(LONG_1>>j));
  if(Part) {
    for (j=0; j < m_nLongLength; j++) m_pData[Row][j]=~m_pData[Row][j];
    m_pData[Row][m_nAllocLength-1]&= maska;
  }
  else {
    for (j=0; j < m_nLongLength; j++)
       m_pData[Row][j]=~m_pData[Row][m_nLongLength+j];
    m_pData[Row][m_nAllocLength+m_nLongLength-1]&= maska;
  }
  return;
}

#if 0

//------------------------------------- operator>(const CuTM& bm1, const CuTM& bm2)
BOOL operator>(const CuTM& bm1, const CuTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i, j;
 for (i=0; i<bm1.m_nSize; i++) {
   for (j=0; j<bm1.m_nBitLength; j++)
     if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) return FALSE;
   for (j=0; j<bm1.m_nBitLength; j++)
     if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
 }
 return FALSE;
}

//--------------------------------------- operator<(const CuTM& bm1, const CuTM& bm2)
BOOL operator<(const CuTM& bm1, const CuTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i, j;
 for (i=0; i<bm1.m_nSize; i++) {
   for (j=0; j<bm1.m_nBitLength; j++)
     if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) return FALSE;
   for (j=0; j<bm1.m_nBitLength; j++)
     if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
 }
 return FALSE;
}

//--------------------------------------  operator>=(const CuTM& bm1, const CuTM& bm2)
BOOL  operator>=(const CuTM& bm1, const CuTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i, j;
 for (i=0; i<bm1.m_nSize; i++)
   for (j=0; j<bm1.m_nBitLength; j++)
     if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) return FALSE;
 return TRUE;
}

//--------------------------------------  operator<=(const CuTM& bm1, const CuTM& bm2)
BOOL  operator<=(const CuTM& bm1, const CuTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i, j;
 for (i=0; i<bm1.m_nSize; i++)
   for (j=0; j<bm1.m_nBitLength; j++)
     if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) return FALSE;
 return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// Serialization

//---------------------------------------------------------------Serialize(CArchive& ar)
void CuTM::Serialize(CArchive& ar)
{ // ASSERT_VALID(this);
  int i, j;
  
  if (ar.IsStoring()) {
    ar << m_nSize;
    ar << m_nBitLength;
    ar << m_nLongLength; 
    for (i=0; i < m_nSize; i++)  
      for (j=0; j < m_nLongLength; j++) {
        ar << m_pData[i][j]; ar << m_pData[i][m_nAllocLength+j];
    }
  }
  else {
    ar >> m_nSize;
    ar >> m_nBitLength;
    ar >> m_nLongLength;
    SetSize(m_nSize, m_nBitLength);
    for (i=0; i<m_nSize; i++)  
      for (j=0; j<m_nLongLength; j++) {
        ar >> m_pData[i][j];  ar >> m_pData[i][m_nAllocLength+j];
    }
  }
}

#endif
/////////////////////////////////////////////////////////////////////////////
// Diagnostics

//-----------------------------------------------------------Dump(CDumpContext& dc) const
#ifdef _DEBUG
void CuTM::Dump(CDumpContext& dc) const
{ // ASSERT_VALID(this);
#define MAKESTRING(x) #x
    AFX_DUMP1(dc, "a " MAKESTRING(CuTM) " with ", m_nSize);
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
void CuTM::AssertValid() const
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
    ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(ULONG*)));
    ASSERT(m_nBitLength >= 0);
    ASSERT(m_nLongLength == LEN_LONG(m_nBitLength));
    ASSERT(m_nAllocLength >= 0);
    ASSERT(m_nGrowBy >= 0);
  }
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////////////////
//********************* Protected functions **********************************************//
//---------------------- AllocMatrix------------------------------------------------------
void CuTM::AllocMatrix(int nRow, int nColumn)
{ ASSERT(nRow >= 0 && nColumn >= 0);
  m_pData = (ULONG**) new ULONG[nRow * sizeof(ULONG*)];
  int i, nLong= LEN_LONG(nColumn);
  for (i=0; i < nRow; i++) {
    m_pData[i] = (ULONG*) new ULONG[nLong<<1];
    memset(m_pData[i], 0, (nLong<<1)*sizeof(ULONG));                   // zero fill
  }
  m_nSize = m_nMaxSize = nRow;
  m_nBitLength = nColumn;
  m_nAllocLength = m_nLongLength = nLong;
}

#ifndef _LINUX
//----------------- CharBit -------------------------------------------------------------
void CuTM::CharBit(const CString s)
{int j = 0, i = 0, max = 0;
 CString w = s;
 if (w.GetLength() <= 2) { Init(); return; }
 if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
 while (!w.IsEmpty()) {
   j = w.Find('\n');
   if (j > max + 1) max = j - 1;
   w = w.Right(w.GetLength() - j-1);
   i++;
 }
 if (m_pData == NULL) AllocMatrix(i, max);
 else { SetSize(i, max, m_nGrowBy); Clear(); }
 w = s; j = 0;
 if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
 while (!w.IsEmpty()) {
   max = w.Find('\n');
   for(i=0; i < max-1; i++) {
     ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
     if(w[i] != '-') SetBitAt(j, i,w[i]);
   } 
   w = w.Right(w.GetLength() - max-1);
   j++;
 }
}    

//----------------- CharBit -------------------------------------------------------------
void CuTM::CharBit(const CStringArray& s)   //new 11.02
{int k, i, max = 0;
 if (s.GetSize()==0) { Init(); return;}
 for (k=0; k < s.GetSize(); k++) {
   i = s[k].GetLength();
   if (i > max) max = i;    // max - length of row
 }  
 if (m_pData == NULL) AllocMatrix(s.GetSize(), max);
 else { SetSize(s.GetSize(), max, m_nGrowBy); Clear(); }
 for (k=0; k < s.GetSize(); k++) { 
   max = s[k].GetLength();
   CString w(s[k]);
   for(i=0; i < max; i++) {
     ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
     if (w[i] != '-') SetBitAt(k, i,w[i]);
   }
 }
}
#else
//----------------- CharBit -------------------------------------------------------------
void CuTM::CharBit(const  char * s)
{ int j = 0, i = 0, max = 0;
  string w = s;
  if (w.size() <= 2) { Init(); return; }
  if (w[w.size()-2] != '\r') w = w + "\r\n";
  while (!w.empty()) {
    j = w.find('\n');
    if (j > max + 1) max = j - 1;
    w.erase(0,j+1);
    i++;
  }
  if (m_pData == NULL) AllocMatrix(i, max);
  else { SetSize(i, max, m_nGrowBy); Clear(); }
  w = s; j = 0;
  if (w[w.size()-2] != '\r') w = w + "\r\n";
  while (!w.empty()) {
    max = w.find('\n');
    for(i=0; i < max-1; i++) {
      ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
      if(w[i] != '-') SetBitAt(j, i,w[i]);
    } 
    w.erase(max+1); //  = w.Right(w.GetLength() - max-1);
    j++;
  }
}    

#endif

//----------------- SafeDelete() --------------------------------------------------------
void CuTM::SafeDelete()
{ int i;
  for (i=0; i < m_nSize; i++) { 
    if (m_pData[i]!=NULL) delete [] (ULONG*)m_pData[i]; }
  if(m_nMaxSize > 0) delete [] (ULONG*)m_pData;
  m_pData=NULL;
}

