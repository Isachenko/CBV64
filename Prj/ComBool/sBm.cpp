/////////////////////////////////////////////////////////////////
// Ver.2.1.2     22.01.2008
//               ��������� ������� ����������� ������ �������� ������ ������� ������
// Ver.2.1.1     22.02.2007
//               � ������� Add ����� �� ������� ������� ��������
//               � ����� � ������� ��������� ��� ������ �������
// Ver.2.1.0     07.07.2006
//               ������� � LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1        11.02.2005
//                Change type 'unsigned int' on ULONG
// Ver.1.0        08.02.2005
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#ifdef _JOINT
#include "ShortBool.h"
#include "BaseBool.h"
#include "archive.h"
#else
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include "../Common/archive.h"
#endif

//#define SIZE_T_MAX  UINT_MAX            /* max size for a size_t */

class CBV;
class CBM;
#ifndef _LINUX
IMPLEMENT_SERIAL(CsBM, CObject, 0)
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif //_DEBUG

#else  //_LINUX 
#undef _DEBUG

#endif //_LINUX 
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
/////////////////////// Construction/Destruction ////////////////////
//*************** Protected function ******************************//
//-------------------------------------------------------------------
void CsBM::Init()
{
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nBitLength = 0;
	m_nGrowBy = 10;
}

//-------------------------------------------------------------------
void CsBM::AllocMatrix(unsigned int nRow, __int8 nColumn)
{
	ASSERT(nRow > 0);
	ASSERT(nColumn > 0 && nColumn <= SIZE_SHORTBOOL);
	m_pData = new size_t[nRow];
	for (unsigned int i = 0; i < nRow; i++)
		m_pData[i] = 0;
	m_nSize = m_nMaxSize = nRow;
	m_nBitLength = nColumn;
}

//****************************** Constructors\Destructor *******************************
//-------------------------------------------------------------------
CsBM::CsBM() { Init(); }

//-------------------------------------------------------------------
CsBM::CsBM(const CsBM& bm, BOOL Fl/* = TRUE */)
{
	if (bm.m_nSize == 0 && bm.m_nBitLength == 0)
	{
		Init();
		return;
	}
	if (Fl)
	{
		AllocMatrix(bm.m_nSize, bm.m_nBitLength);
		memcpy(m_pData, bm.m_pData, bm.m_nSize * sizeof(size_t));
	}
	else
	{
		ASSERT(bm.m_nSize <= SIZE_SHORTBOOL);
		AllocMatrix(bm.m_nBitLength, bm.m_nSize);
		for (__int8 j = 0; j < m_nBitLength; j++)
			for (unsigned int i = 0; i < m_nSize; i++)
				SetBitAt(i,(__int8)j, bm.GetBitAt(j, i));
	}
	m_nGrowBy = bm.m_nGrowBy;
}

//-------------------------------------------------------------------
CsBM::CsBM(unsigned int nRow, __int8 nColumn, BOOL Fl /*= FALSE*/)
{
	ASSERT(nRow >= 0);
	ASSERT(nColumn >= 0 && nColumn <= SIZE_SHORTBOOL);

	if (nRow == 0)
	{
		if (nColumn == 0)
			Init();
		else
		{            
			m_nSize = m_nMaxSize = 0;
			m_nBitLength = nColumn;
			m_nGrowBy = 10;
			m_pData = NULL;
		}
		return;
	}
	AllocMatrix(nRow, nColumn);
	if (Fl)
		for (unsigned int i = 0; i < nRow; i++)
			m_pData[i] = 0xffffffffffffffff >> (SIZE_SHORTBOOL - nColumn) << (SIZE_SHORTBOOL - nColumn);  /* one's fill*/
	m_nGrowBy = 10;  
}

//-------------------------------------------------------------------
CsBM::CsBM(const size_t Vect, unsigned int nRow, __int8 nColumn)
{
	if (nRow == 0 && nColumn == 0)
	{
		Init();
		return;
	}
	ASSERT(nRow > 0);
	ASSERT(nColumn > 0 && nColumn <= SIZE_SHORTBOOL);
	size_t V = Vect >> (SIZE_SHORTBOOL - nColumn) << (SIZE_SHORTBOOL - nColumn);
	AllocMatrix(nRow, nColumn);
	for (unsigned int i = 0; i < nRow; i++)
		m_pData[i] = V;  /* one's fill*/
	m_nGrowBy = 10;
}

//-------------------------------------------------------------------
CsBM::CsBM(const char* pch)
{
#ifndef _LINUX
	CString s(pch);
	Init();
	if (s.GetLength())
		CharBit(s);
#else
	char * s;
	s = strdup(pch);
	Init();
	if (s[0]!=0) CharBit(s);
	free(s);
#endif
}

#ifndef _LINUX
//-------------------------------------------------------------------
CsBM::CsBM(const CStringArray& StrAr)
{
	Init();
	CharBit(StrAr);
}
#else
CsBM::CsBM(const vector <string>& StrAr)
{ Init(); CharBit(StrAr); }
#endif

//-------------------------------------------------------------------
CsBM::~CsBM()
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_pData != NULL)
  {
	  delete [] m_pData;
	  m_pData = NULL;
  }
}

//-------------------------------------------------------------------
// ��������������� Bm �� ������������ ������������� 
// � "���������������"(BmS)  

/*
void CsBM::ToShort(CBM &Bm)
{
	ASSERT(Bm.GetCountC() <= SIZE_SHORTBOOL);
	CsBV sbv;
	RemoveAll();
	for (int i = 0; i < Bm.GetCountR(); i++)
	{
		sbv.ToShort(Bm.GetRowBv(i));
		Add(sbv);
	}
	return;
}
*/

//-------------------------------------------------------------------
// ��������������� "��������������u��" ������������� 
// ������� ������� (BmS) � ����������e (Bm)  

/*
void CsBM::FromShort(CBM &Bm)
{
	Bm.SetSize(0, m_nBitLength);
	CBV bv;
	CsBV bvS;
	for (unsigned int i = 0; i < m_nSize; i++)
	{
		bvS = GetRowBv(i);
		bvS.FromShort(bv);
		Bm.Add(bv);
	}
	return;
}

*/

////////////////////////////////////////////////////////////////////////////////////////
//******************************** Working with memory *********************************
//------------------------------------------ SetSize(int nRow, int nColumn, int nGrowBy)
void CsBM::SetSize(unsigned int nRow, __int8 nColumn, __int8 nGrowBy /* = -1 */)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nRow >= 0);
	ASSERT(nColumn >= 0 && nColumn <= SIZE_SHORTBOOL);
#endif

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size
	unsigned int nmaxRow = nRow + m_nGrowBy;
	//------------------ shrink to nothing
	if (nRow == 0)
	{
		if (m_pData != NULL)
		{
			delete [] m_pData;
			m_pData = NULL;
		}
		m_nBitLength = nColumn;
		m_nSize = m_nMaxSize = 0;
		return;
	}
	//------------------ create one with exact size
	if (m_pData == NULL)
	{ // create one with exact size
#ifdef SIZE_T_MAX
		ASSERT(nmaxRow * sizeof(size_t) <= SIZE_T_MAX);  // no overflow
#endif
		m_pData = new size_t[nmaxRow];
		for (unsigned int i = 0; i < nRow; i++)
			m_pData[i] = 0;
		m_nSize = nRow;
		m_nMaxSize = nmaxRow;
		m_nBitLength = nColumn;
		return;
	}
	m_nBitLength = nColumn;

	//------------------ Set rows size
	if (nRow <= m_nMaxSize)
	{
		if (nRow > m_nSize)     // it fits // initialize the new elements
			for (unsigned int i = m_nSize; i < nRow; i++)
				m_pData[i] = 0;
		m_nSize = nRow;
		return;
	}

#ifdef SIZE_T_MAX
	ASSERT(nmaxRow * sizeof(size_t) <= SIZE_T_MAX);  // no overflow
#endif
	size_t* pNewData = new size_t[nmaxRow];
	memcpy(pNewData, m_pData, m_nSize * sizeof(size_t)); // copy new data from old
	ASSERT(nRow > m_nSize);  // construct remaining elements
	for (unsigned int i = m_nSize; i < nRow; i++)
		pNewData[i] = 0;
	for (unsigned int i = nRow; i < nmaxRow; i++)
		pNewData[i] = 0;
	if (m_pData != NULL)
		delete [] m_pData;
	m_pData = pNewData;
	m_nSize = nRow;
	m_nMaxSize = nmaxRow;
}

//-------------------------------------------------------------------------  FreeExtra()
void CsBM::FreeExtra()
{
#ifdef _DEBUG
	ASSERT_VALID(this);
#endif
	if (m_nSize != m_nMaxSize)
	{ // shrink to desired size
		size_t* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = new size_t[m_nSize];
			memcpy(pNewData, m_pData, m_nSize * sizeof(size_t)); // copy new data from old
		}
		// get rid of old stuff (note: no destructors called)
		delete [] m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//********************************* Reading the data ***********************************
#ifndef _LINUX
//------------------------------------BitChar(CStringArray & StrRes, char One,char Zero)
void CsBM::BitChar(CStringArray & StrRes, char One, char Zero)
{
	StrRes.RemoveAll();
	for (unsigned int j = 0; j < m_nSize; j++)
	{
		CString s;
		for (__int8 i = 0; i < m_nBitLength; i++)
			if (GetBitAt(j, i))
				s += One;
			else
				s += Zero;
		StrRes.Add(s);
	}
}

//----------------------------------------------------------BitChar(char One, char Zero)
CString CsBM::BitChar(char One, char Zero)
{
	unsigned int pos = 0;
	CString s('\0', (m_nBitLength + 2) * m_nSize);
	for (unsigned int j = 0; j < m_nSize; j++)
	{
		for (__int8 i = 0; i < m_nBitLength; i++)
			if (GetBitAt(j, i))
				s.SetAt(pos++, One);
			else
				s.SetAt(pos++, Zero);
		s.SetAt(pos++, '\n');
	}
	return s;
}
#else
//------------------------------------BitChar(CStringArray & StrRes, char One,char Zero)
void CsBM::BitChar(vector <string>& StrRes, char One, char Zero)
{ 
  int i,j,pos = 0;
  StrRes.clear();
  for (j = 0; j < m_nSize; j++) {
    string s;
    for (i = 0; i < m_nBitLength; i++)
      if (GetBitAt(j, i)) s += One;
      else               s += Zero;
      StrRes.push_back(s);
  }
}

//----------------------------------------------------------BitChar(char One, char Zero)
char* CsBM::BitChar(char One, char Zero)
{ 
  int i,j,pos = 0;
  char* s;
  s = (char*)malloc((m_nBitLength+2)*m_nSize + 1);
  for (j = 0; j < m_nSize; j++) {
    for (i = 0; i < m_nBitLength; i++)
      if (GetBitAt(j, i)) s[pos++] = One;
      else               s[pos++] = Zero;
   // s[pos++] = '\r';
    s[pos++] = '\n';
  }
  s[pos] = '\0';
  return s;
}
#endif
//------------------------------------------------------------------GetRowBv(int nIndex)
CsBV CsBM::GetRowBv(unsigned int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	CsBV bv(m_pData[nIndex], m_nBitLength);
	return bv;
}
 
//-----------------------------------------------GetRowBv(int nIndex, ULONG mask)
CsBV CsBM::GetRowBv(unsigned int nIndex, size_t mask) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	CsBV bv(m_pData[nIndex], m_nBitLength);
	bv &= mask;
	return bv;
}

//--------------------------------------------------------------GetColumnBv(int nColumn)
CsBV CsBM::GetColumnBv(__int8 nColumn) const
{
	ASSERT(nColumn >= 0);
	ASSERT(m_nSize <= SIZE_SHORTBOOL);
	ASSERT(nColumn < m_nBitLength);

	__int8 ns = m_nSize % SIZE_SHORTBOOL;
	CsBV bv(ns, FALSE);
	for (__int8 i = 0; i < ns; i++)
		bv.SetBitAt(i, GetBitAt(i, nColumn));
	return bv;
}
  
//--------------------------------------------------------------GetColumnBv(int nColumn)
CsBV CsBM::GetColumnBv(__int8 nColumn, size_t mask) const
{
	ASSERT(nColumn >= 0);
	ASSERT(m_nSize <= SIZE_SHORTBOOL);
	ASSERT(nColumn < m_nBitLength);

	__int8 ns = m_nSize % SIZE_SHORTBOOL;
	CsBV bv(ns, FALSE);
	for (__int8 i = 0; i < ns; i++)
		bv.SetBitAt(i, GetBitAt(i, nColumn));
	bv &= mask;
	return bv;
}

  
//--------------------------------------------------ExtractRows(int nFirst, int nCount)
CsBM CsBM::ExtractRows(unsigned int nFirst, unsigned int nCount) const
{
	ASSERT(nFirst >= 0);
	ASSERT((nFirst + nCount) <= m_nSize);
	ASSERT(nCount > 0);

	CsBM bm(nCount, m_nBitLength);
	for (unsigned int i = 0; i < nCount; i++)
		bm.SetRow(i, GetRow(nFirst + i));
	return bm;
}

//------------------------------------------------ExtractColumns(int nFirst, int nCount)
CsBM CsBM::ExtractColumns(__int8 nFirst, __int8 nCount) const
{
	ASSERT(nFirst >= 0);
	ASSERT(nCount > 0);
	ASSERT((nFirst + nCount) <= m_nBitLength);

	CsBM bm(m_nSize, nCount);
	CsBV bv(m_nBitLength, FALSE);
	CsBV bv1;

	for (unsigned int i = 0; i < m_nSize; i++)
	{ 
		bv = m_pData[i]; 
		bv1 = bv.Extract(nFirst, nCount);
		bm.SetRow(i, bv1);
	}
	return bm;
}

//-------------------------- Extract(int nFirstR, int nFirstC, int nCountR, int nCountC)
CsBM CsBM::Extract(unsigned int nFirstR, __int8 nFirstC, unsigned int nCountR, __int8 nCountC) const
{
	ASSERT(nFirstR >= 0);
	ASSERT(nFirstC >= 0);
	ASSERT((nFirstC + nCountC) <= m_nBitLength);
	ASSERT((nFirstR + nCountR) <= m_nSize);
	ASSERT(nCountC > 0);
	ASSERT(nCountR > 0);

	CsBM bm(nCountR, nCountC);
	CsBV bv(m_nBitLength, FALSE);
	CsBV bv1;
	for (unsigned int i = nFirstR; i < nFirstR + nCountR; i++)
	{ 
		bv = m_pData[i]; 
		bv1 = bv.Extract(nFirstC, nCountC);
		bm.SetRow(i, bv1);
	}
	return bm;
} 

//----------------------------------------- Minor(const CsBV& Rows, const CsBV& Columns)
CsBM CsBM::Minor(const CsBV& Rows, const CsBV& Columns) const
{
	__int8 LeftOneRow = -1, LeftOneColumn = -1;
	__int8 NewRowLen = Rows.CountBit();
	__int8 NewColumnLen = Columns.CountBit();
	ASSERT(NewColumnLen >= 0);
	CsBM bm(NewRowLen, NewColumnLen);
	for (__int8 i = 0; i < NewRowLen; i++)
	{
		LeftOneRow = Rows.LeftOne(LeftOneRow);
		for (__int8 j = 0; j < NewColumnLen; j++)
		{
			LeftOneColumn = Columns.LeftOne(LeftOneColumn);
			bm.SetBitAt(i, j, GetBitAt(LeftOneRow, LeftOneColumn));
		}
		LeftOneColumn = -1;
	}
	return bm;
}

//------------------------------------------------------------------------------ Trans()
CsBM CsBM::Trans() const
{
	CsBM bm(*this, FALSE);
	return bm;
}

////////////////////////////////////////////////////////////////////////////////////////
//******************************** Working with data ***********************************
//------------------------------------------------------SetRow(int nRow, const CsBV& bv)
void CsBM::SetRow(unsigned int nRow, const CsBV& bv)
{
	ASSERT(nRow >= 0 && nRow < m_nSize);
	ASSERT(m_nBitLength == bv.GetBitLength());
	m_pData[nRow] = (size_t)bv;
}

//------------------------------------------SetRow(int nRow, const CsBM& bm, int nbmRow)
void CsBM::SetRow(unsigned int nRow, const CsBM& bm, unsigned int nbmRow)
{
	ASSERT(nRow >= 0 && nRow < m_nSize);
	ASSERT(m_nBitLength == bm.GetCountC());
	m_pData[nRow] = bm.m_pData[nbmRow];
}

////////////////////////////////////////////////////////////////////////////////////////
//********************************* Working with row ***********************************
//---------------------------------------SetRowGrow(int nRow, const ULONG newRow)
void CsBM::SetRowGrow(unsigned int nRow, const size_t newRow)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nRow >= 0);
#endif
	if (nRow >= m_nSize)
		SetSize(nRow + 1, m_nBitLength, m_nGrowBy);
	m_pData[nRow] = newRow >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
}

//---------------------------------------------------SetRowGrow(int nRow, const CBV& bv)
void CsBM::SetRowGrow(unsigned int nRow, const CsBV& bv)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nRow >= 0);
	if (m_nSize > 0)
		ASSERT(m_nBitLength == bv.GetBitLength());
#endif
	if (nRow >= m_nSize) 
		SetSize(nRow + 1, bv.GetBitLength(), m_nGrowBy);
	m_pData[nRow] = (size_t)bv;
}

//--------------------------------------SetRowGrow(int nRow, const CsBM& bm, int nbmRow)
void CsBM::SetRowGrow(unsigned int nRow, const CsBM& bm, unsigned int nbmRow)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nRow >= 0 && nbmRow < bm.m_nSize);
	if (m_nSize > 0)
		ASSERT(m_nBitLength == bm.m_nBitLength);
#endif
	if (nRow >= m_nSize)
		SetSize(nRow + 1, m_nBitLength, m_nGrowBy);
	m_pData[nRow] = bm.m_pData[nbmRow];
}

//------------------------------------------------------------- Add(BOOL bit,int nCount)
unsigned int CsBM::Add(BOOL bit/*=FALSE*/, unsigned int nCount/*=1*/)
{
	unsigned int first = m_nSize;
	size_t mask = 0xffffffffffffffff >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	if(m_nSize + nCount < m_nMaxSize)
	{
		m_nSize += nCount;
		for(unsigned int i = first; i < m_nSize; i++)
			m_pData[i] = 0;
	}
	else 
		SetSize(m_nSize + nCount, m_nBitLength, m_nGrowBy);
	if (bit == TRUE) 
		for (; first < m_nSize; first++)
			m_pData[first] = mask;
	return m_nSize - 1;
} 

//-----------------------------Add(const CsBM& bm, int nbmRow)
unsigned int CsBM::Add(const CsBM& bm, unsigned int nbmRow/* =-1*/)   // 22.02.2007
{
	unsigned int i = m_nSize;
	if (nbmRow >= 0)
		SetRowGrow(i, bm, nbmRow); 
	else
		InsertRow(i, 0, bm, bm.GetCountR());
	return (i); 
}

//-----------------------------InsertRow(int nRow,const ULONG newRow, int nCount)
void CsBM::InsertRow(unsigned int nRow, const size_t newRow, unsigned int nCount /*=1*/)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nRow >= 0);       // will expand to meet need
	ASSERT(nCount > 0);      // zero or negative size not allowed
#endif

	if (nRow >= m_nSize)   // adding after the end of the array
		SetSize(nRow + nCount, m_nBitLength, m_nGrowBy);  // grow so nIndex is valid
	else
	{                   // inserting in the middle of the array
		SetSize(m_nSize + nCount, m_nBitLength, m_nGrowBy);  // grow it to new size 
		memmove(&m_pData[nRow + nCount], &m_pData[nRow], (m_nSize - nRow - 1) * sizeof(size_t));
	}
	// insert new value in the gap
	size_t r = newRow >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	for (unsigned int i = nRow; i < nRow + nCount; i++)
		m_pData[i] = r;
}


//-----------------------------------InsertRow(int nRow, const CsBV& newRow, int nCount)
void CsBM::InsertRow(unsigned int nRow, const CsBV& newRow, unsigned int nCount)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nRow >= 0);
	ASSERT(newRow.GetBitLength() == m_nBitLength);
#endif
	InsertRow(nRow, (size_t)newRow, nCount);
}

//----------------------InsertRow(int nRow,int nStartRow, const CsBM& NewBM, int nCount)
void CsBM::InsertRow(unsigned int nRow, unsigned int nStartRow, const CsBM& NewBM, unsigned int nCount)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nRow >= 0);
	ASSERT(m_nBitLength == NewBM.m_nBitLength);
	ASSERT(nStartRow >= 0);
	ASSERT(nStartRow + nCount <= NewBM.m_nSize);
#endif
	if (NewBM.m_nSize > 0)
	{
		InsertRow(nRow, NewBM.m_pData[nStartRow], nCount);
		for (unsigned int i = 1; i < nCount; i++)
			SetRow(nRow + i, NewBM.m_pData[nStartRow + i]);
	}
}

//-----------------------------------------------------RemoveRow(int nIndex, int nCount)
void CsBM::RemoveRow(unsigned int nIndex, unsigned int nCount /* = 1 */)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	ASSERT(nIndex + nCount <= m_nSize);
#endif
	unsigned int nMoveCount = m_nSize - (nIndex + nCount);     // just remove a range
	if (nMoveCount)
		memcpy(&m_pData[nIndex], &m_pData[nIndex + nCount], nMoveCount * sizeof(size_t));
	m_nSize -= nCount;
}

//------------------------------------------ExchangeRow(int nRow1,int nRow2,int nCount))
void CsBM::ExchangeRow(unsigned int nRow1, unsigned int nRow2, unsigned int nCount/*=1*/)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(nCount > 0 && nRow1 + nCount <= m_nSize && nRow2+nCount <= m_nSize);
#endif

	size_t Vect;
	while(nCount > 0)
	{
		Vect = m_pData[nRow1];
		m_pData[nRow1++] = m_pData[nRow2];
		m_pData[nRow2++] = Vect;
		nCount--; 
	}
	return;
}

//--------------------------------------------------------SortDecr(int nBeg, int number)
void CsBM::SortDecr(unsigned int nBeg/*=0*/, unsigned int number/*=-1*/)
{
	ASSERT(number == -1 || number > 1);

	BOOL bNotDone = TRUE;
	unsigned int pos, posNext, nEnd = (number < 0) ? m_nSize : (nBeg + number);
	ASSERT(nEnd <= m_nSize);
	while (bNotDone)
	{
		bNotDone = FALSE;
		for(pos = nBeg, posNext = nBeg + 1; pos < nEnd - 1; pos++, posNext++)
			if (CountBit(pos) < CountBit(posNext))
			{
				ExchangeRow(pos, posNext);
				bNotDone = TRUE;
			}
	}
}

//--------------------------------------------------------SortIncr(int nBeg, int number)
void CsBM::SortIncr(unsigned int nBeg/*=0*/, unsigned int number/*=-1*/)
{
	ASSERT(number == -1 || number > 1);

	BOOL bNotDone = TRUE;
	unsigned int pos, posNext, nEnd = (number < 0) ? m_nSize : (nBeg + number);
	ASSERT(nEnd <= m_nSize);
	while (bNotDone) {
		bNotDone = FALSE;
		for(pos = nBeg, posNext = nBeg + 1; pos < nEnd - 1; pos++, posNext++)
			if(CountBit(pos) > CountBit(posNext))
			{
				ExchangeRow(pos, posNext);
				bNotDone = TRUE;
			}
	}
}

//--------------------------------------------------------------------AddCol(int numCol)
__int8 CsBM::AddCol(__int8 numCol/*=1*/)
  //numCol columns adding to the matrix           return the number of new last column
{
#ifdef _DEBUG
	ASSERT_VALID(this);
	ASSERT(numCol > 0 && (m_nBitLength + numCol) <= SIZE_SHORTBOOL);
#endif
	m_nBitLength += numCol;
	return m_nBitLength - 1;
}

////////////////////////////////////////////////////////////////////////////////////////
//********************** Operators and functions of assignment *************************
//---------------------------------------------------------- operator =(const CsBM& bm0)
const CsBM& CsBM::operator = (const CsBM& bm)
{
	unsigned int i, w = bm.m_nSize;
	SetSize(w, bm.m_nBitLength, m_nGrowBy);
	for (i = 0; i < w; i++)
		SetRow(i, bm.GetRow(i));
	return *this;
}

//---------------------------------------------------------- operator =(const char* pch)
const CsBM& CsBM::operator = (const char* pch)
{
#ifndef _LINUX  
	CharBit(pch);
	return *this; 
#else
	char *ps;
	ps = strdup(pch);
	CharBit(ps); 
	free(ps);
	return *this; 
#endif
}

#ifndef _LINUX  
//------------------------------------------------------ operator =(CStringArray& StrAr)
const CsBM& CsBM::operator = (const CStringArray& StrAr)
{
	CharBit(StrAr);
	return *this;
}
#else
const CsBM& CsBM::operator =(const vector <string>& StrAr)
{ CharBit(StrAr); return *this; }
#endif

//------------------------------------------------------------------------ One(int nRow)
void CsBM::One(unsigned int nRow)
{
	unsigned int first, last;
	size_t mask = 0xffffffffffffffff >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	if (nRow == -1)
	{
		first = 0;
		last = m_nSize - 1;
	}
	else
	{
		first = nRow;
		last = nRow;
	}
	for (; first <= last; first++)
		m_pData[first] = mask; 
}

//----------------------------------------------------------------------- Zero(int nRow)
void CsBM::Zero(unsigned int nRow)
{
	unsigned int first, last;
	if (nRow == -1)
	{
		first = 0;
		last = m_nSize - 1;
	}
	else
	{
		first = nRow;
		last = nRow;
	}
	for (; first <= last; first++)
		m_pData[first] = 0; 
}


////////////////////////////////////////////////////////////////////////////////////////
//*************************** Operations of concatinations *****************************
//--------------------------------------------------------------- Concat(const CsBM& bm)
void CsBM::Concat(const CsBM& bm)
{
	ASSERT(m_nSize == bm.m_nSize);
	ASSERT(m_nBitLength + bm.m_nBitLength <= SIZE_SHORTBOOL);

	__int8 NewLen, OldLen;
	OldLen = SIZE_SHORTBOOL - bm.m_nBitLength;
	NewLen = m_nBitLength + bm.m_nBitLength;
	SetSize(m_nSize, NewLen, m_nGrowBy);
	NewLen = SIZE_SHORTBOOL - NewLen;
	for (unsigned int i = 0; i < m_nSize; i++) 
		m_pData[i] |= bm.m_pData[i] >> OldLen << NewLen;
}

//---------------------------------------------- Concat(const CsBM& bm1,const CsBM& bm2)
void CsBM::Concat(const CsBM& bm1, const CsBM& bm2)
{
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength + bm2.m_nBitLength <= SIZE_SHORTBOOL);

	__int8 NewLen, OldLen;
	NewLen = bm1.m_nBitLength + bm2.m_nBitLength;
	SetSize(bm1.m_nSize, NewLen, m_nGrowBy);
	OldLen = SIZE_SHORTBOOL - bm2.m_nBitLength;
	NewLen = SIZE_SHORTBOOL - NewLen;
	for (unsigned int i = 0; i < m_nSize; i++) 
		m_pData[i] = bm1.m_pData[i] | (bm2.m_pData[i] >> OldLen << NewLen);
}

////////////////////////////////////////////////////////////////////////////////////////
//************************* Operators of advanced assignment ***************************
//----------------------------------------------------------- operator|=(const CsBM& bm) 
const CsBM& CsBM::operator |= (const CsBM& bm)
{
	ASSERT(m_nSize == bm.m_nSize);
	ASSERT(m_nBitLength == bm.m_nBitLength);
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] |= bm.m_pData[i];
	return *this;
}

//----------------------------------------------------------- operator|=(const CsBV& bv) 
const CsBM& CsBM::operator |= (const CsBV& bv)
{
	ASSERT(m_nBitLength == bv.GetBitLength());
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] |= (size_t)bv;
	return *this;
}

//-------------------------------------------------- operator|=(const ULONG Vect)
const CsBM& CsBM::operator |= (const size_t Vect)
{
	size_t r = Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] |= r;
	return *this;
}

//----------------------------------------------------------- operator&=(const CsBM& bm)
const CsBM& CsBM::operator &= (const CsBM& bm)
{
	ASSERT(m_nSize == bm.m_nSize);
	ASSERT(m_nBitLength == bm.m_nBitLength);
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] &= bm.m_pData[i];
	return *this;
}

//----------------------------------------------------------- operator&=(const CsBV& bv)
const CsBM& CsBM::operator &= (const CsBV& bv)
{
	ASSERT(m_nBitLength == bv.GetBitLength());
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] &= (size_t)bv;
	return *this;
}

//-------------------------------------------------- operator&=(const ULONG Vect)
const CsBM& CsBM::operator &= (const size_t Vect)
{
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] &= Vect;
	return *this;
}

//----------------------------------------------------------- operator^=(const CsBM& bm)
const CsBM& CsBM::operator ^= (const CsBM& bm)
{
	ASSERT(m_nSize == bm.m_nSize);
	ASSERT(m_nBitLength == bm.m_nBitLength);
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] ^= bm.m_pData[i];
	return *this;
}

//----------------------------------------------------------- operator^=(const CsBV& bv)
const CsBM& CsBM::operator ^= (const CsBV& bv)
{
	ASSERT(m_nBitLength == bv.GetBitLength());
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] ^= (size_t)bv;
	return *this;
}

//-------------------------------------------------- operator^=(const ULONG Vect)
const CsBM& CsBM::operator ^= (const size_t Vect)
{
	size_t r = Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] ^= r;
	return *this;
}

//----------------------------------------------------------- operator-=(const CsBV& bv)
const CsBM& CsBM::operator -= (const CsBV& bv)
{
	ASSERT(m_nBitLength == bv.GetBitLength());
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] -= (size_t)bv;
	return *this;
}

//-------------------------------------------------- operator-=(const ULONG Vect)
const CsBM& CsBM::operator -= (const size_t Vect)
{
	size_t r = Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	for (unsigned int i = 0; i < m_nSize; i++)
		m_pData[i] -= r;
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////
//********************************* Logic operations ***********************************
//----------------------------------- operator|(const CsBM& bm1, const CsBM& bm2)
STD(CsBM) operator | (const CsBM& bm1, const CsBM& bm2)
{
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] | bm2.m_pData[i];
	return bm;
}

//----------------------------------- operator|(const CsBM& bm1, const CsBV& bv2)
STD(CsBM) operator | (const CsBM& bm1, const CsBV& bv2)
{
	ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] | (size_t)bv2;
	return bm;
}

//--------------------------- operator|(const CsBM& bm1, const ULONG Vect)
STD(CsBM) operator | (const CsBM& bm1, const size_t Vect)
{
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	size_t r = Vect >> (SIZE_SHORTBOOL - bm1.m_nBitLength) << (SIZE_SHORTBOOL - bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] | r;
	return bm;
}

//------------------------------------------------------------------------ Disjunction()
CsBV CsBM::Disjunction()
{
#ifdef _DEBUG
	ASSERT_VALID(this);
#endif
	CsBV bv(m_nBitLength, FALSE);
	for (unsigned int j = 0; j < m_nSize; j++)
		bv |= m_pData[j];
	return bv;
}

//------------------------------------ operator&(const CsBM& bm1,const CsBM& bm2)
STD(CsBM) operator & (const CsBM& bm1, const CsBM& bm2)
{
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] & bm2.m_pData[i];
	return bm;
}

//----------------------------------- operator&(const CsBM& bm1, const CsBV& bv2)
STD(CsBM) operator & (const CsBM& bm1, const CsBV& bv2)
{
	ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] & (size_t)bv2;
	return bm;
}

//--------------------------- operator&(const CsBM& bm1, const ULONG Vect)
STD(CsBM) operator & (const CsBM& bm1, const size_t Vect)
{ 
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] & Vect;
	return bm;
}

//------------------------------------------------------------------------ Conjunction()
CsBV CsBM::Conjunction()
{
#ifdef _DEBUG
	ASSERT_VALID(this);
#endif
	CsBV bv(m_nBitLength, TRUE);
	for (unsigned int j = 0; j < m_nSize; j++)
		bv &= m_pData[j];
	return bv;
}

//------------------------------------ operator^(const CsBM& bm1,const CsBM& bm2)
STD(CsBM) operator ^ (const CsBM& bm1, const CsBM& bm2)
{
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] ^ bm2.m_pData[i];
	return bm;
}

//----------------------------------- operator^(const CsBM& bm1, const CsBV& bv2)
STD(CsBM) operator ^ (const CsBM& bm1, const CsBV& bv2)
{
	ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] ^ (size_t)bv2;
	return bm;
}

//--------------------------- operator^(const CsBM& bm1, const ULONG Vect)
STD(CsBM) operator ^ (const CsBM& bm1, const size_t Vect)
{
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	size_t r = Vect >> (SIZE_SHORTBOOL - bm1.m_nBitLength) << (SIZE_SHORTBOOL - bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] ^ r;
	return bm;
}

//------------------------------------------------------------------- BitwiseExclusive()
CsBV CsBM::BitwiseExclusive()
{
#ifdef _DEBUG
	ASSERT_VALID(this);
#endif

	CsBV bv(m_nBitLength, FALSE);
	for (unsigned int j = 0; j < m_nSize; j++)
		bv ^= m_pData[j];
	return bv;
}

//----------------------------------- operator-(const CsBM& bm1, const CsBV& bv2)
STD(CsBM) operator - (const CsBM& bm1, const CsBV& bv2)
{
	ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] - (size_t)bv2;
	return bm;
}

//--------------------------- operator-(const CsBM& bm1, const ULONG Vect)
STD(CsBM) operator - (const CsBM& bm1, const size_t Vect)
{
	CsBM bm(bm1.m_nSize, bm1.m_nBitLength);
	size_t r = Vect >> (SIZE_SHORTBOOL - bm1.m_nBitLength) << (SIZE_SHORTBOOL - bm1.m_nBitLength);
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		bm.m_pData[i] = bm1.m_pData[i] - r;
	return bm;
}

//----------------------------------------------------- operator~(const CsBM& bm)
STD(CsBM) operator ~ (const CsBM& bm)
{
	CsBM bm1(bm.m_nSize, bm.m_nBitLength);
	size_t mask = 0xffffffffffffffff >> (SIZE_SHORTBOOL - bm.m_nBitLength) << (SIZE_SHORTBOOL - bm.m_nBitLength);
	for (unsigned int i = 0; i <bm1.m_nSize; i++)
		bm1.m_pData[i] = ~bm.m_pData[i] && mask;
	return bm1;
}

////////////////////////////////////////////////////////////////////////////////////////
//******************* Operations of weighting, finding and casing **********************
//------------------------------------------------------------------- CountBit(int nRow)
__int8 CsBM::CountBit(unsigned int nRow) const
{
	ASSERT (nRow >= -1);

	__int8 one = 0;
	if (nRow != -1)
	{
		BYTE* pB= (BYTE*)&m_pData[nRow];
		return (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]] + sTabC[pB[4]] + sTabC[pB[5]] + sTabC[pB[6]] + sTabC[pB[7]]); 
	}
	for (unsigned int k = 0; k < m_nSize; k++)
	{
		BYTE* pB= (BYTE*)&m_pData[k];
		one += sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]] + sTabC[pB[4]] + sTabC[pB[5]] + sTabC[pB[6]] + sTabC[pB[7]];
	}
	return one;
}

//------------------------------------------------------------------- CountBit(int nRow)
__int8 CsBM::CountBit(const size_t mask, unsigned int nRow) const 
{
	ASSERT (nRow >= -1);

	__int8 one = 0;
	size_t val;
	BYTE* pB= (BYTE*)&val;
	if (nRow != -1)
	{ 
		val = m_pData[nRow] & mask;
		return (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]] + sTabC[pB[4]] + sTabC[pB[5]] + sTabC[pB[6]] + sTabC[pB[7]]); 
	}
	for (unsigned int k = 0; k < m_nSize; k++) {
		val = m_pData[k] & mask;
		one += sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]] + sTabC[pB[4]] + sTabC[pB[5]] + sTabC[pB[6]] + sTabC[pB[7]];
	}
	return one;
}

//--------------------------------------------------------- LeftOne(int nRow, int nNext)
__int8 CsBM::LeftOne(unsigned int nRow, __int8 nNext) const
{
	ASSERT((nNext >= -1) && (m_nBitLength > nNext) );
	for (__int8 i = nNext + 1; i < m_nBitLength; i++)
		if (m_pData[nRow] & sOB[i])
			return i;
	return (-1);
}

//-------------------------------- LeftOne(int nRow, int nNext, const ULONG mask)
__int8 CsBM::LeftOne(unsigned int nRow, __int8 nNext, size_t mask) const
{
	ASSERT((nNext >= -1) && (m_nBitLength > nNext) );
	for (__int8 i = nNext + 1; i < m_nBitLength; i++)
		if (m_pData[nRow] & mask & sOB[i])
			return i;
	return (-1);
}

//-------------------------------------------------------- RightOne(int nRow, int nPrev)
__int8 CsBM::RightOne(unsigned int nRow, __int8 nPrev) const
{
	ASSERT((m_nBitLength >= nPrev) && (nPrev > 0));
	for (int i = nPrev - 1; i >= 0; i--)
		if (m_pData[nRow] & sOB[i])
			return i;
	return (-1);
}

//--------------------------------------RightOne(int nRow, int nPrev, ULONG mask)
__int8 CsBM::RightOne(unsigned int nRow, __int8 nPrev, size_t mask) const
{
	ASSERT((m_nBitLength >= nPrev) && (nPrev > 0));
	for (__int8 i = nPrev - 1; i >= 0; i--)
		if (m_pData[nRow] & mask & sOB[i])
			return i;
	return (-1);
}

//------------------------------------------------------ FindRow (BOOL Empty, int nFRow)
unsigned int CsBM::FindRow (BOOL Empty, unsigned int nFRow) const
{
	if (m_nSize == 0)
		return -1;

	ASSERT(nFRow < m_nSize - 1);

	for (unsigned int i = ++nFRow; i <= m_nSize - 1; i++)
	{
		if (m_pData[i] == 0 && Empty == TRUE)
			return i;
		if (m_pData[i] != 0 && Empty == FALSE)
			return i;
	}
	return -1;
}

//------------------------------------FindRow (BOOL Empty, ULONG mask, int nFRow)
unsigned int CsBM::FindRow (BOOL Empty, size_t mask, unsigned int nFRow) const
{
	if (m_nSize == 0)
		return -1;

	ASSERT(nFRow < m_nSize - 1);

	for (unsigned int i = ++nFRow; i <= m_nSize - 1; i++)
	{
		if ((m_pData[i] & mask) == 0 && Empty == TRUE)
			return i;
		if ((m_pData[i] & mask) != 0 && Empty == FALSE)
			return i;
	}
	return -1;
}

//---------------------------------------------------FindRow (const CsBV& bv, int nFRow)
unsigned int CsBM::FindRow (const CsBV& bv, unsigned int nFRow) const
{
	ASSERT(nFRow < m_nSize - 1);

	if (m_nSize == 0)
		return -1;
	for (unsigned int i = ++nFRow; i < m_nSize; i++)
		if ((size_t)bv == m_pData[i])
			return i;
	return -1;
}

//------------------------------------------FindRow (const ULONG Vect, int nFRow)
unsigned int CsBM::FindRow (const size_t Vect, unsigned int nFRow) const
{
	ASSERT(nFRow < m_nSize - 1);

	if (m_nSize == 0)
		return -1;
	size_t r = Vect >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	for (unsigned int i = ++nFRow; i < m_nSize; i++)
		if (r == m_pData[i])
			return i;
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////
//****************************** Comparing operations **********************************
//----------------------------------------------------------------------IsZero(int nRow)
BOOL CsBM::IsZero(unsigned int nRow) const
{
	if (m_nBitLength == 0)
		return FALSE;
	if (nRow != -1)
		return (m_pData[nRow] == 0);
	for (unsigned int k = 0; k < m_nSize; k++)
		if (m_pData[k] != 0)
			return FALSE;
	return TRUE;
}

//---------------------------------------------IsZero(const ULONG mask, int nRow)
BOOL CsBM::IsZero(const size_t mask, unsigned int nRow) const
{
	if (m_nBitLength == 0)
		return FALSE;
	if (nRow != -1)
		return ((m_pData[nRow] & mask) == 0);
	for (unsigned int k = 0; k < m_nSize; k++)
		if ((m_pData[k] & mask) != 0)
			return FALSE;
	return TRUE;
}

//-----------------------------------------------------------------------IsOne(int nRow)
BOOL CsBM::IsOne(unsigned int nRow) const
{ 
	ULONG mask = 0xffffffffffffffff >> (SIZE_SHORTBOOL - m_nBitLength) << (SIZE_SHORTBOOL - m_nBitLength);
	if (m_nBitLength == 0)
		return FALSE;
	if (nRow != -1)
		return (m_pData[nRow] == mask);
	for (unsigned int k = 0; k < m_nSize; k++)
		if (m_pData[k] != mask)
			return FALSE;
	return TRUE;
}

//--------------------------------------------------IsOne(const unsigned mask, int nRow)
BOOL CsBM::IsOne(const size_t mask, unsigned int nRow) const
{
	if (m_nBitLength == 0)
		return FALSE;
	if (nRow != -1)
		return ((m_pData[nRow] & mask) == mask);
	for (unsigned int k = 0; k < m_nSize; k++)
		if ((m_pData[k] & mask) != mask)
			return FALSE;
	return TRUE;
}

//----------------------------------- operator==(const CsBM& bm1,const CsBM& bm2)
STD(BOOL) operator == (const CsBM& bm1, const CsBM& bm2)
{
	if ((bm1.m_nSize != bm2.m_nSize) || (bm1.m_nBitLength != bm2.m_nBitLength))
		return FALSE;
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		if (bm1.m_pData[i] != bm2.m_pData[i])
			return FALSE;
	return TRUE;
}

//----------------------------------- operator!=(const CsBM& bm1,const CsBM& bm2)
STD(BOOL) operator != (const CsBM& bm1, const CsBM& bm2)
{
	if ((bm1.m_nSize != bm2.m_nSize) || (bm1.m_nBitLength != bm2.m_nBitLength)) return TRUE;
	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		if (bm1.m_pData[i] != bm2.m_pData[i])
			return TRUE;
	return FALSE;
}

//------------------------------------ operator>(const CsBM& bm1,const CsBM& bm2)
STD(BOOL) operator > (const CsBM& bm1, const CsBM& bm2)
{
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);

	for (unsigned int i = 0; i < bm1.m_nSize; i++)
	{
		if (bm2.m_pData[i] != (bm1.m_pData[i] & bm2.m_pData[i]))
			return FALSE;
		if (bm2.m_pData[i] != bm1.m_pData[i])
			return TRUE;
	}
	return FALSE;
}

//------------------------------------ operator<(const CsBM& bm1,const CsBM& bm2)
STD(BOOL) operator < (const CsBM& bm1, const CsBM& bm2)
{ 
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);

	for (unsigned int i = 0; i < bm1.m_nSize; i++)
	{
		if (bm1.m_pData[i] != (bm1.m_pData[i] & bm2.m_pData[i]))
			return FALSE;
		if (bm2.m_pData[i] != bm1.m_pData[i])
			return TRUE;
	}
	return FALSE;
}

//----------------------------------- operator>=(const CsBM& bm1,const CsBM& bm2)
STD(BOOL) operator >= (const CsBM& bm1, const CsBM& bm2)
{
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);

	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		if (bm2.m_pData[i] != (bm1.m_pData[i] & bm2.m_pData[i]))
			return FALSE;
	return FALSE;
}

//----------------------------------- operator<=(const CsBM& bm1,const CsBM& bm2)
STD(BOOL) operator <= (const CsBM& bm1, const CsBM& bm2)
{
	ASSERT(bm1.m_nSize == bm2.m_nSize);
	ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);

	for (unsigned int i = 0; i < bm1.m_nSize; i++)
		if (bm1.m_pData[i] != (bm1.m_pData[i] & bm2.m_pData[i]))
			return FALSE;
	return FALSE;
}

//--------------------------------------------------------CoverRow(int nRow1, int nRow2)
__int8 CsBM::CoverRow(unsigned int nRow1, unsigned int nRow2)
{
	if (m_pData[nRow1] == (m_pData[nRow1] & m_pData[nRow2])) 
		return 1; /* i-�� ������ ����������� j-�� */
	if (m_pData[nRow2] == (m_pData[nRow1] & m_pData[nRow2])) 
		return -1; /* i-�� ������ ��������� j-�� */
	return 0;                     
}

//-------------------------------CoverRow(int nRow1, int nRow2, const ULONG mask)
__int8 CsBM::CoverRow(unsigned int nRow1, unsigned int nRow2, const size_t mask)
{ 
  if ((m_pData[nRow1] & mask) == (mask & m_pData[nRow1] & m_pData[nRow2])) 
    return 1; /* i-�� ������ ����������� j-�� */
  if ((m_pData[nRow2] & mask) == (mask & m_pData[nRow1] & m_pData[nRow2])) 
    return -1; /* i-�� ������ ��������� j-�� */
  return 0;                     
}

#ifndef _LINUX
////////////////////////////////////////////////////////////////////////////////////////
// Serialization
//---------------------------------------------------------------Serialize(CArchive& ar)
void CsBM::Serialize(CArchive& ar)
{
#ifdef _DEBUG
	ASSERT_VALID(this);
#endif
	if (ar.IsStoring())
	{
		ar << (unsigned int)m_nSize;
		ar << (__int8)m_nBitLength;
		for (unsigned int i = 0; i < m_nSize; i++)
			ar << (size_t)m_pData[i];
	}
	else
	{
		ar >> (unsigned int)m_nSize;
		ar >> (__int8)m_nBitLength;
		SetSize(m_nSize, m_nBitLength, m_nGrowBy);
		for (unsigned int i = 0; i < m_nSize; i++)
			ar >> (size_t)m_pData[i];
	}
}

////////////////////////////////////////////////////////////////////////////////////////
// Diagnostics

//----------------------------------------------------------Dump(CDumpContext& dc) const
#ifdef _DEBUG
void CsBM::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);
#define MAKESTRING(x) #x
	AFX_DUMP1(dc, "a " MAKESTRING(CsBM) " with ", m_nSize);
	AFX_DUMP0(dc, " elements");
#undef MAKESTRING
	if (dc.GetDepth() > 0)
	{
		AFX_DUMP0(dc, "\n");
		for (unsigned int i = 0; i < m_nSize; i++)
		{
			AFX_DUMP1(dc, "\n\t[", i);
			AFX_DUMP1(dc, "] = ",NULL);
		}
	}
}

//-------------------------------------------------------------------------AssertValid()
void CsBM::AssertValid() const
{
	if (m_pData == NULL)
	{
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nBitLength >= 0);
	}
	else
	{
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nSize <= m_nMaxSize);
		ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(size_t)));
		ASSERT(m_nBitLength >= 0);
		ASSERT(m_nGrowBy > 0);
	}
}
#endif //_DEBUG


//---------------------------------------------------------------Serialize(CArchive& ar)
// Serialization
//---------------------------------------------------------------
void CsBM::Serialize(CArch& ar)
{
	if (ar.IsStoring())
	{
		ar << (unsigned int)m_nSize;
		ar << (__int8)m_nBitLength;
		for (unsigned int i = 0; i < m_nSize; i++)
		{
			BYTE * byteData = (BYTE *)&(m_pData[i]);
			for (__int8 j = 0; j * SIZE_BYTE < SIZE_SHORTBOOL; ++j)
				ar << (BYTE)byteData[j];
		}
	}
	else
	{
		ar >> (unsigned int)m_nSize;
		ar >> (__int8)m_nBitLength;
		SetSize(m_nSize, m_nBitLength, m_nGrowBy);
		for (unsigned int i = 0; i < m_nSize; i++)
		{
			BYTE * byteData = (BYTE *)&(m_pData[i]);
			for (__int8 j = 0; j * SIZE_BYTE < SIZE_SHORTBOOL; ++j)
				ar >> (BYTE)byteData[j];
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////
////////////////////// Operation of weighting,finding and casing ///////////////////////
//------------------------------------------------------------------- CharBit(CString s)
void CsBM::CharBit(const CString s)
{
	unsigned int j = 0;
	__int8 max = 0, i = 0;
	CString w = s;
	if (w.GetLength() <= 2)
	{
		Init();
		return;
	}
	if (w[w.GetLength() - 2] != '\r')
		w = w + "\r\n";
	while (!w.IsEmpty())
	{
		i = (__int8)w.Find('\n');
		if (i > max + 1)
			max = i - 1;
		w = w.Right(w.GetLength() - i - 1);
		j++;
	}

	ASSERT(max <= SIZE_SHORTBOOL);
	if (m_pData == NULL)
		AllocMatrix(j, max);
	else
	{
		SetSize(j, max, m_nGrowBy);
		Zero();
	}

	w = s; j = 0;
	if (w[w.GetLength() - 2 ] != '\r')
		w = w + "\r\n";
	while (!w.IsEmpty())
	{
		max = (__int8)w.Find('\n');
		for (i = 0; i < max - 1; i++)
			if (w[i] == '1')
				SetBitAt(j, i, TRUE);
			else
				ASSERT(w[i] == '0'|| w[i] == '.' || w[i] == '-');
			w = w.Right(w.GetLength() - max - 1);
			j++;
	}
}

//------------------------------------------------------------- CharBit(CStringArray& s)
void CsBM::CharBit(const CStringArray& s)
{
	__int8 max = 0, i;
	if (s.GetSize() == 0)
	{
		Init();
		return;
	}
	for (unsigned int k = 0; k < s.GetSize(); k++)
	{
		i = (__int8)s[k].GetLength();
		if (i > max)
			max = i;    // max - length of row
	}
	ASSERT(max <= SIZE_SHORTBOOL);
	if (m_pData == NULL)
		AllocMatrix((unsigned int)s.GetSize(), max);
	else
	{
		SetSize((unsigned int)s.GetSize(), max, m_nGrowBy);
		Zero();
	}
	for (unsigned int k = 0; k < s.GetSize(); k++)
	{
		max = s[k].GetLength();
		CString w(s[k]);
		for(i = 0; i < max; i++)
			if (w[i] == '1')
				SetBitAt(k, i, TRUE);
			else
				ASSERT(w[i] == '0' || w[i] == '.' || w[i] == '-');
	}
}
#else
//------------------------------------------------------------------- CharBit(CString s)
void CsBM::CharBit(const string s)
{ 
  CharBit(s.c_str());
}

//------------------------------------------------------------- CharBit(CStringArray& s)
void CsBM::CharBit(const vector <string>& s)
{
  int k, i, max = 0;
  if (s.size()==0) { Init(); return;}
  for (k = 0; k < s.size(); k++) {
    i = s[k].length();
    if (i > max) max = i;    // max - length of row
  }
  ASSERT(max <= 32);
  if (m_pData == NULL) AllocMatrix(s.size(), max);
  else { SetSize(s.size(), max, m_nGrowBy); Zero(); }
  for (k = 0; k < s.size(); k++) {
    max = s[k].length();
    string w(s[k]);
    for(i = 0; i < max; i++)
      if (w[i] == '1') SetBitAt(k, i, true);
      else             ASSERT(w[i] == '0' || w[i] == '.' || w[i] == '-');
  }
}

#endif //_LINUX

//------------------------------------------------------------------- CharBit(CString s)
void CsBM::CharBit(char* s)
{
	int j = 0, i = 0, max = 0;
	char *w, *pdest;
	size_t len = strlen(s);
	w =(char*)malloc(len + 2);
#ifndef _MSVC9
	strcpy(w,s);
#else
	strcpy_s(w, len + 2, s);
#endif
	if (len <= 2)
	{
		Init();
		return;
	}
	if (w[len - 2] != '\r')
#ifndef _MSVC9
		strcat(w,"\r\n");
#else
		strcat_s(w, len + 2, "\r\n");
#endif
	while (!w[0] != 0)
	{
		pdest = strchr(w,'\n');
		j = (int)(pdest - w + 1);
		if (j > max + 1)
			max = j - 1;
#ifndef _MSVC9
		strcpy(w, pdest+1);
#else
		strcpy_s(w, len + 2, pdest+1);
#endif
		i++;
	}
	ASSERT(max <= SIZE_SHORTBOOL);
	if (m_pData == NULL)
		AllocMatrix(i, max);
	else
	{
		SetSize(i, max, m_nGrowBy);
		Zero();
	}
#ifndef _MSVC9
	strcpy(w,s);
#else
	strcpy_s(w, len+2, s);
#endif
	j = 0;
	if (w[len - 2 ] != '\r') 
#ifndef _MSVC9
		strcat(w,"\r\n");
#else
		strcat_s(w, len+2, "\r\n");
#endif
	while (!w[0] != 0)
	{
		pdest = strchr(w,'\n');
		j = (int)(pdest - w + 1);
		for(i = 0; i < max - 1; i++)
			if (w[i] == '1')
				SetBitAt(j, i, TRUE);
			else
				ASSERT(w[i] == '0'|| w[i] == '.' || w[i] == '-');
#ifndef _MSVC9
		strcpy(w, pdest+1);
#else
		strcpy_s(w, len+2, pdest+1);
#endif
		j++;
	}
	free(w);
}

