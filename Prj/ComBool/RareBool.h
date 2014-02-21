/////////////////////////////////////////////////////////////////
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.0     08.03.2005
/////////////////////////////////////////////////////////////////
// Rare Boolean vector
#ifndef _RAREBOOLEAN_
#define _RAREBOOLEAN_

#include "../Common/BaseBool.h"
#include <vector>
#include <utility>

typedef std::vector<int> VECTOR;

class CrBV
{
public:
//******************************* Constructors\Destructor *******************************
  CrBV();                                           //(1)
  CrBV(size_t Len, BOOL Inv = TRUE);                   //(2)
  CrBV(const CrBV& rbv);                            //(3)
  CrBV(VECTOR* v, size_t Len, BOOL Inv);               //(3a)
  CrBV(const CBV& bv, BOOL Inv = TRUE );            //(4)
  CrBV(const char* pch, size_t Len, BOOL Inv = TRUE);  //(5)
  ~CrBV();                                          //(6)

//*********************** Functions for getting class's parametrs ***********************
  size_t GetLength() const;                             //(7) inline
  BOOL GetForm() const;                              //(7a) inline
  VECTOR * GetData();                                //(7b) inline 
//*************************************** Reading ***************************************
  BOOL GetBitAt(size_t nIndex) const;                       //(8)
#ifndef _LINUX
  CString BitChar(char One = '1',char Zero='0') const;   //(9)
  CString BitList() const;                               //(10)
#else
  char* BitChar(char One = '1',char Zero='0') const;   //(9)
  char* BitList() const;                               //(10)
#endif
  CrBV Extract(size_t nFirst,size_t nCount);                   //(11)

//*************************************** Writing ***************************************
  void SetBitAt(size_t nIndex, BOOL bit);                   //(12)

//*********************** Operators and functions of assignment *************************
  const CrBV& operator=(const CrBV& bvr);                //(13)
  const CrBV& operator=(const CBV& bv);                  //(14)
  void Reverse();                                        //(15)
  void Empty();                                          //(16) inline
  void Zero();                                           //(17) inline
  void One();                                            //(18) inline

//********************************** Logic operations ***********************************
  void Invert();                                         //(19) inline
  void Diz(const CrBV& rbv1, const CrBV& rbv2);          //(20)
  void Con(const CrBV& rbv1, const CrBV& rbv2);          //(21)
  void Xor(const CrBV& rbv1, const CrBV& rbv2);          //(22)
  void Dif(const CrBV& rbv1, const CrBV& rbv2);          //(23)

  friend CrBV operator|(const CrBV& bv1, const CrBV& bv2); //(24)
  friend CrBV operator&(const CrBV& bv1, const CrBV& bv2); //(25)
  friend CrBV operator^(const CrBV& bv1, const CrBV& bv2); //(26)
  friend CrBV operator-(const CrBV& bv1, const CrBV& bv2); //(27)
  friend CrBV operator~(const CrBV& bv2);                  //(28)

//********************************** Shift operations ***********************************
  friend CrBV operator<<(const CrBV& bv1, size_t nShift);  //(29)
  friend CrBV operator>>(const CrBV& bv1, size_t nShift);  //(30)
  void LoopLeftShift(size_t nShift);                              //(31)
  void LoopRightShift(size_t nShift);                             //(32)


//************************** Operators of advanced assignment ***************************
  const CrBV& operator |=(const CrBV& bvr);             //(33) 
  const CrBV& operator &=(const CrBV& bvr);             //(34)
  const CrBV& operator ^=(const CrBV& bvr);             //(35)
  const CrBV& operator -=(const CrBV& bvr);             //(36)
  const CrBV& operator <<=(size_t nShift);                 //(37)
  const CrBV& operator >>=(size_t nShift);                 //(38)

//******************** Operations of weighting, finding and casing **********************
  size_t CountBit() const;                                 //(39) inline
  size_t LeftOne(size_t nNext = -1) const;                    //(40)
  size_t RightOne(size_t nNext = -1) const;                   //(41) 

//**************************** Operations of concatinations *****************************
  void Concat(const CrBV& bv);                   //(42)

//******************************* Compareing operations *********************************
  BOOL IsEmpty() const;                          //(43) inline
  BOOL IsZero() const;                           //(44) inline
  BOOL IsOne() const;                            //(45) inline
  BOOL operator ==(const CrBV& bv2);      //(46)
  BOOL operator !=(const CrBV& bv2);      //(47) inline
  BOOL operator >(const CrBV& bv2);       //(48)
  BOOL operator <(const CrBV& bv2);       //(49)
  BOOL operator >=(const CrBV& bv2);      //(50)
  BOOL operator <=(const CrBV& bv2);      //(51)

#ifndef _LINUX
  //****************************** Input/Output operations ********************************
#ifdef _DEBUG
  friend CDumpContext& operator<<(CDumpContext& dc,const CrBV& bv); //(52)
#endif
  friend CArchive& operator<<(CArchive& ar, const CrBV& bv);      //(53)
  friend CArchive& operator>>(CArchive& ar, CrBV& bv);            //(54)
#endif

//***************************** Advanced access to memory *******************************

protected:
  VECTOR m_nVect;
  BOOL m_invert;        // TRUE - normal, FALSE - inversion
  size_t m_length;         // Length of boolean vector
  
//******************************** Protected functions **********************************
public:
  BOOL Find(size_t X, size_t& Pos) const;                           //(56)
  void SetU(VECTOR& v0, const VECTOR& v1, const VECTOR& v2);  //(57)
  void SetI(VECTOR& v0, const VECTOR& v1, const VECTOR& v2);  //(58)
  void SetD(VECTOR& v0, const VECTOR& v1, const VECTOR& v2);  //(59) 
  void SetA(VECTOR& v0, const VECTOR& v1, const VECTOR& v2);  //(60)

  BOOL Equal11_00(const CrBV* rbv) const;                //(61)
  BOOL Equal10_01(const CrBV* rbv) const;                //(62)
  BOOL Absorb11(const CrBV* rbv) const;                  //(63)
  BOOL Absorb00(const CrBV* rbv) const;                  //(64)
  BOOL Con01(const CrBV& rbv2, BOOL Opt);                //(65) 
};



//***************************************************************************************
//                          inline functions
//***************************************************************************************
//----------------------------------------------------------------------(7)
inline size_t CrBV::GetLength() const { return m_length; }
//----------------------------------------------------------------------(7a)
inline BOOL CrBV::GetForm() const { return m_invert; }
//----------------------------------------------------------------------(7b)
inline VECTOR * CrBV::GetData() { return &m_nVect; }
//----------------------------------------------------------------------(16)
inline void CrBV::Empty() { m_length = 0; m_invert = TRUE; m_nVect.clear(); }
//----------------------------------------------------------------------(17)
inline void CrBV::Zero() { m_invert = FALSE; m_nVect.clear(); }
//----------------------------------------------------------------------(18)
inline void CrBV::One() { m_invert = TRUE; m_nVect.clear(); }
//----------------------------------------------------------------------(19)
inline void CrBV::Invert() { m_invert = !m_invert; }
//----------------------------------------------------------------------(39)
inline size_t CrBV::CountBit() const
{
  if (m_invert) return m_nVect.size();
  else return m_length - m_nVect.size();
}
//----------------------------------------------------------------------(43)
inline BOOL CrBV::IsEmpty() const { return (m_length==0); }
//----------------------------------------------------------------------(44)
inline BOOL CrBV::IsZero() const 
{
  if (m_invert) return (m_nVect.size()==0);
  else  return (m_nVect.size()==m_length);
}
//----------------------------------------------------------------------(45)
inline BOOL CrBV::IsOne() const 
{
  if (!m_invert) return (m_nVect.size()==0);
  else  return (m_nVect.size()==m_length);
}

#ifndef _LINUX
#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif

#endif