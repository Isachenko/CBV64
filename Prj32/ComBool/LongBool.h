/////////////////////////////////////////////////////////////////
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1.1     03.12.2004   Russian == English
/////////////////////////////////////////////////////////////////

// Temporary Ulong * Boolean vector

#ifndef _LONGBOOL
#define _LONGBOOL
#include "../Common/BaseBool.h"

class ClBV
{
public:
//******************************* Constructors\Destructor *******************************
  ClBV();
  ClBV(CBV& bvSrc);
  ClBV(const BYTE* pbt, int nLenBit);
  ~ClBV(){};
   operator ULONG*() const;

   friend CBV operator|(const ClBV& bv1,const ClBV& bv2);
   friend CBV operator|(const CBV& bv1,const ClBV& bv2);
   friend CBV operator|(const ClBV& bv1,const CBV& bv2);
   friend CBV operator|(const BYTE* pbt,const ClBV& bv2);
   friend CBV operator|(const ClBV& bv1,const BYTE* pbt);
   friend CBV operator|(const ULONG* pwt,const ClBV& bv2);
   friend CBV operator|(const ClBV& bv1,const ULONG* pwt);
   
   friend CBV operator&(const ClBV& bv1,const ClBV& bv2);
   friend CBV operator&(const CBV& bv1,const ClBV& bv2);
   friend CBV operator&(const ClBV& bv1,const CBV& bv2);
   friend CBV operator&(const BYTE* pbt,const ClBV& bv2);
   friend CBV operator&(const ClBV& bv1,const BYTE* pbt);
   friend CBV operator&(const ULONG* pwt,const ClBV& bv2);
   friend CBV operator&(const ClBV& bv1,const ULONG* pwt);

   friend CBV operator^(const ClBV& bv1,const ClBV& bv2);
   friend CBV operator^(const CBV& bv1,const ClBV& bv2);
   friend CBV operator^(const ClBV& bv1,const CBV& bv2);
   friend CBV operator^(const BYTE* pbt,const ClBV& bv2);
   friend CBV operator^(const ClBV& bv1,const BYTE* pbt);
   friend CBV operator^(const ULONG* pwt,const ClBV& bv2);
   friend CBV operator^(const ClBV& bv1,const ULONG* pwt);
   
   friend CBV operator-(const ClBV& bv1,const ClBV& bv2);
   friend CBV operator-(const CBV& bv1,const ClBV& bv2);
   friend CBV operator-(const ClBV& bv1,const CBV& bv2);
   friend CBV operator-(const BYTE* pbt,const ClBV& bv2);
   friend CBV operator-(const ClBV& bv1,const BYTE* pbt);
   friend CBV operator-(const ULONG* pwt,const ClBV& bv2);
   friend CBV operator-(const ClBV& bv1,const ULONG* pwt);

   friend CBV operator~(const ClBV& bv2);
   
   void Invert(const ULONG* pwt);

   const ClBV& operator |=(const ClBV& bv1);
   const ClBV& operator |=(const CBV& bv1);
   const ClBV& operator |=(const BYTE* pbt);
   const ClBV& operator |=(const ULONG* pwt);
   const ClBV& operator &=(const ClBV& bv1);
   const ClBV& operator &=(const CBV& bv1);
   const ClBV& operator &=(const BYTE* pbt);
   const ClBV& operator &=(const ULONG* pwt);
   const ClBV& operator ^=(const ClBV& bv1);
   const ClBV& operator ^=(const CBV& bv1);
   const ClBV& operator ^=(const BYTE* pbt);
   const ClBV& operator ^=(const ULONG* pwt);
   const ClBV& operator -=(const ClBV& bv1);
   const ClBV& operator -=(const CBV& bv1);
   const ClBV& operator -=(const BYTE* pbt);
   const ClBV& operator -=(const ULONG* pwt);

//*********************** Operators and functions of assignment *************************
   const ClBV& operator =(const ClBV& bvSrc);
   const ClBV& operator =(const CBV& bvSrc);
   const ClBV& operator =(const BYTE* pbt);
   void AssignDiz(int nBitLength, const BYTE* v1, const BYTE* v2);
   void AssignDiz(int nBitLength, int Num, BYTE* v1, ...);
   void AssignCon(int nBitLength, const BYTE* v1, const BYTE* v2);
   void AssignCon(int nBitLength, int Num, BYTE* v1, ...);
   void AssignXor(int nBitLength, const BYTE* v1, const BYTE* v2);
   void AssignXor(int nBitLength, int Num, BYTE* v1, ...);
   void AssignDif(int nBitLength, const BYTE* v1, const BYTE* v2);


   BOOL IsZero() const;
   BOOL IsOne() const;

   friend BOOL operator==(const ClBV& bv1, const ClBV& bv2);
   friend BOOL operator==(const CBV& bv1, const ClBV& bv2);
   friend BOOL operator==(const ClBV& bv1, const CBV& bv2);
   friend BOOL operator==(const ClBV& bv1, const BYTE* pbt);
   friend BOOL operator==(const BYTE* pbt, const ClBV& bv2);
   friend BOOL operator==(const ClBV& bv1, const ULONG* pwt);
   friend BOOL operator==(const ULONG* pwt, const ClBV& bv2);

   friend BOOL operator!=(const ClBV& bv1, const ClBV& bv2);
   friend BOOL operator!=(const CBV& bv1, const ClBV& bv2);
   friend BOOL operator!=(const ClBV& bv1, const CBV& bv2);
   friend BOOL operator!=(const ClBV& bv1, const BYTE* pbt);
   friend BOOL operator!=(const BYTE* pbt, const ClBV& bv2);
   friend BOOL operator!=(const ClBV& bv1, const ULONG* pwt);
   friend BOOL operator!=(const ULONG* pwt, const ClBV& bv2);

   friend BOOL operator>(const ClBV& bv1, const ClBV& bv2);
   friend BOOL operator>(const CBV& bv1, const ClBV& bv2);
   friend BOOL operator>(const ClBV& bv1, const CBV& bv2);
   friend BOOL operator>(const ClBV& bv1, const BYTE* pbt);
   friend BOOL operator>(const BYTE* pbt, const ClBV& bv2);
   friend BOOL operator>(const ClBV& bv1, const ULONG* pwt);
   friend BOOL operator>(const ULONG* pwt, const ClBV& bv2);

   friend BOOL operator<(const ClBV& bv1, const ClBV& bv2);
   friend BOOL operator<(const CBV& bv1, const ClBV& bv2);
   friend BOOL operator<(const ClBV& bv1, const CBV& bv2);
   friend BOOL operator<(const ClBV& bv1, const BYTE* pbt);
   friend BOOL operator<(const BYTE* pbt, const ClBV& bv2);
   friend BOOL operator<(const ClBV& bv1, const ULONG* pwt);
   friend BOOL operator<(const ULONG* pwt, const ClBV& bv2);

   friend BOOL operator>=(const ClBV& bv1, const ClBV& bv2);
   friend BOOL operator>=(const CBV& bv1, const ClBV& bv2);
   friend BOOL operator>=(const ClBV& bv1, const CBV& bv2);
   friend BOOL operator>=(const ClBV& bv1, const BYTE* pbt);
   friend BOOL operator>=(const BYTE* pbt, const ClBV& bv2);
   friend BOOL operator>=(const ClBV& bv1, const ULONG* pwt);
   friend BOOL operator>=(const ULONG* pwt, const ClBV& bv2);

   friend BOOL operator<=(const ClBV& bv1, const ClBV& bv2);
   friend BOOL operator<=(const CBV& bv1, const ClBV& bv2);
   friend BOOL operator<=(const ClBV& bv1, const CBV& bv2);
   friend BOOL operator<=(const ClBV& bv1, const BYTE* pbt);
   friend BOOL operator<=(const BYTE* pbt, const ClBV& bv2);
   friend BOOL operator<=(const ClBV& bv1, const ULONG* pwt);
   friend BOOL operator<=(const ULONG* pwt, const ClBV& bv2);

protected:
  void DisInPlace(const ULONG* Vect1, int BitLen1);
  void ConInPlace(const ULONG* Vect1, int BitLen1);
  void AddInPlace(const ULONG* Vect1, int BitLen1);
  void ConNotInPlace(const ULONG* Vect1, int BitLen1);
  BOOL Equality(const ULONG* Vect2,int BitLen2) const;
  BOOL Pogl(const ULONG* Vect2,int BitLen2,BOOL Dist) const;
  BOOL PoglEq(const ULONG* Vect2,int BitLen2,BOOL Dist) const;

  void Init();

   ULONG* m_bVect;
   int m_nBitLength;
   int m_nLongLength;
   int m_nByteRest;
};

inline ClBV::operator ULONG*() const
{ return (ULONG*)m_bVect; }

//******************************* Disjunctions *******************************
//-----------------------------------------------------------
inline CBV operator |(const ClBV& bv1, const ClBV& bv2) 
{ return (bv2 | (BYTE*)((ULONG*)bv1)); }
//-----------------------------------------------------------
inline CBV operator |(const CBV& bv1, const ClBV& bv2) 
{ return (bv2 | (BYTE*)bv1); }
//-----------------------------------------------------------
inline CBV operator |(const ClBV& bv1, const CBV& bv2) 
{ return (bv1 | (BYTE*)bv2); }
//-----------------------------------------------------------
inline CBV operator |(const BYTE* pbt,const ClBV& bv2) 
{ return (bv2 | pbt); }
//-----------------------------------------------------------
inline CBV operator |(const ULONG* pwt, const ClBV& bv2) 
{ return (bv2 & (BYTE*)pwt); }
//-----------------------------------------------------------
inline CBV operator |(const ClBV& bv1, const ULONG* pwt) 
{ return (bv1 & (BYTE*)pwt); }
//******************************* Conjunctions *******************************
//-----------------------------------------------------------
inline CBV operator &(const ClBV& bv1, const ClBV& bv2) 
{ return (bv2 & (BYTE*)(ULONG*)bv1); }
//-----------------------------------------------------------
inline CBV operator &(const CBV& bv1, const ClBV& bv2) 
{ return (bv2 & (BYTE*)bv1); }
//-----------------------------------------------------------
inline CBV operator &(const ClBV& bv1, const CBV& bv2) 
{ return (bv1 & (BYTE*)bv2); }
//-----------------------------------------------------------
inline CBV operator &(const BYTE* pbt, const ClBV& bv2) 
{ return (bv2 & pbt); }
//-----------------------------------------------------------
inline CBV operator &(const ULONG* pwt, const ClBV& bv2) 
{ return (bv2 & (BYTE*)pwt); }
//-----------------------------------------------------------
inline CBV operator &(const ClBV& bv1, const ULONG* pwt) 
{ return (bv1 & (BYTE*)pwt); }
//******************************* Disjunction with ************************
//-----------------------------------------------------------
inline CBV operator ^(const ClBV& bv1,const ClBV& bv2) 
{ return (bv2 ^ (BYTE*)(ULONG*)bv1); }
//-----------------------------------------------------------
inline CBV operator ^(const CBV& bv1,const ClBV& bv2) 
{ return (bv2 ^ (BYTE*)bv1); }
//-----------------------------------------------------------
inline CBV operator ^(const ClBV& bv1,const CBV& bv2) 
{ return (bv1 ^ (BYTE*)bv2); }
//-----------------------------------------------------------
inline CBV operator ^(const BYTE* pbt,const ClBV& bv2)
{ return (bv2 ^ pbt); }
//-----------------------------------------------------------
inline CBV operator ^(const ULONG* pwt,const ClBV& bv2) 
{ return (bv2 ^ (BYTE*)pwt); }
//-----------------------------------------------------------
inline CBV operator ^(const ClBV& bv1, const ULONG* pwt) 
{ return (bv1 ^ (BYTE*)pwt); }
//******************************* Subtractions *******************************
//-----------------------------------------------------------
inline CBV operator -(const ClBV& bv1,const ClBV& bv2) 
{ return (bv1 - (BYTE*)((ULONG*)bv2)); }
//-----------------------------------------------------------
inline CBV operator -(const CBV& bv1,const ClBV& bv2) 
{ return ((BYTE*)bv1 - bv2); }
//-----------------------------------------------------------
inline CBV operator -(const ClBV& bv1,const CBV& bv2) 
{ return (bv1 - (BYTE*)bv2); }
//-----------------------------------------------------------
inline CBV operator -(const ULONG* pwt,const ClBV& bv2) 
{ return ((BYTE*)pwt - bv2); }
//-----------------------------------------------------------
inline CBV operator -(const ClBV& bv1, const ULONG* pwt) 
{ return (bv1 - (BYTE*)pwt); }

//******************************* Operations in place************************
//-----------------------------------------------------------
inline const ClBV& ClBV::operator |=(const ClBV& bv1)
{ DisInPlace(bv1.m_bVect, bv1.m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator |=(const CBV& bv1)
{ DisInPlace((ULONG*)(BYTE*)bv1, bv1.GetBitLength());    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator |=(const BYTE* pbt)
{ DisInPlace((ULONG*)pbt, m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator |=(const ULONG* pwt)
{ DisInPlace(pwt, m_nBitLength);    return *this;}

//-----------------------------------------------------------
inline const ClBV& ClBV::operator &=(const ClBV& bv1)
{ ConInPlace(bv1.m_bVect, bv1.m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator &=(const CBV& bv1)
{ ConInPlace((ULONG*)(BYTE*)bv1, bv1.GetBitLength());    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator &=(const BYTE* pbt)
{ ConInPlace((ULONG*)pbt, m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator &=(const ULONG* pwt)
{ ConInPlace(pwt, m_nBitLength);    return *this;}

//-----------------------------------------------------------
inline const ClBV& ClBV::operator ^=(const ClBV& bv1)
{ AddInPlace(bv1.m_bVect, bv1.m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator ^=(const CBV& bv1)
{ AddInPlace((ULONG*)(BYTE*)bv1, bv1.GetBitLength());    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator ^=(const BYTE* pbt)
{ AddInPlace((ULONG*)pbt, m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator ^=(const ULONG* pwt)
{ AddInPlace(pwt, m_nBitLength);    return *this;}


//-----------------------------------------------------------
inline const ClBV& ClBV::operator -=(const ClBV& bv1)
{ ConNotInPlace(bv1.m_bVect, bv1.m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator -=(const CBV& bv1)
{ ConNotInPlace((ULONG*)(BYTE*)bv1, bv1.GetBitLength());    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator -=(const BYTE* pbt)
{ ConNotInPlace((ULONG*)pbt, m_nBitLength);    return *this;}
//-----------------------------------------------------------
inline const ClBV& ClBV::operator -=(const ULONG* pwt)
{ ConNotInPlace(pwt, m_nBitLength);    return *this;}



//******************************* Operations of comparing ************************

//-----------------------------------------------------
inline BOOL operator ==(const ClBV& bv1, const ClBV& bv2)
{ return bv1.Equality(bv2.m_bVect, bv2.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator ==(const ClBV& bv1, const CBV& bv2)
{ return bv1.Equality((ULONG*)((BYTE*)bv2), bv2.GetBitLength());}
//-----------------------------------------------------
inline BOOL operator ==(const CBV& bv1, const ClBV& bv2)
{ return bv2.Equality((ULONG*)((BYTE*)bv1), bv1.GetBitLength());}
//-----------------------------------------------------
inline BOOL operator ==(const ClBV& bv1, const BYTE* p2)
{ return bv1.Equality((ULONG*)p2, bv1.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator ==(const BYTE* p1,const ClBV& bv2)
{ return bv2.Equality((ULONG*)p1, bv2.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator ==(const ClBV& bv1, const ULONG* p2)
{ return bv1.Equality(p2, bv1.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator ==(const ULONG* p1,const ClBV& bv2)
{ return bv2.Equality(p1, bv2.m_nBitLength);}

//-----------------------------------------------------
inline BOOL operator !=(const ClBV& bv1, const ClBV& bv2)
{ return !bv1.Equality(bv2.m_bVect, bv2.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator !=(const ClBV& bv1, const CBV& bv2)
{ return !bv1.Equality((ULONG*)((BYTE*)bv2), bv2.GetBitLength());}
//-----------------------------------------------------
inline BOOL operator !=(const CBV& bv1, const ClBV& bv2)
{ return !bv2.Equality((ULONG*)((BYTE*)bv1), bv1.GetBitLength());}
//-----------------------------------------------------
inline BOOL operator !=(const ClBV& bv1, const BYTE* p2)
{ return !bv1.Equality((ULONG*)p2, bv1.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator !=(const BYTE* p1, const ClBV& bv2)
{ return !bv2.Equality((ULONG*)p1, bv2.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator !=(const ClBV& bv1, const ULONG* p2)
{ return !bv1.Equality(p2, bv1.m_nBitLength);}
//-----------------------------------------------------
inline BOOL operator !=(const ULONG* p1, const ClBV& bv2)
{ return !bv2.Equality(p1, bv2.m_nBitLength);}

//-----------------------------------------------------
inline BOOL operator >(const ClBV& bv1, const ClBV& bv2)
{ return bv1.Pogl(bv2.m_bVect, bv2.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator >(const ClBV& bv1, const CBV& bv2)
{ return bv1.Pogl((ULONG*)((BYTE*)bv2), bv2.GetBitLength(), TRUE);}
//-----------------------------------------------------
inline BOOL operator >(const CBV& bv1, const ClBV& bv2)
{ return bv2.Pogl((ULONG*)((BYTE*)bv1), bv1.GetBitLength(), FALSE);}
//-----------------------------------------------------
inline BOOL operator >(const ClBV& bv1, const BYTE* p2)
{ return bv1.Pogl((ULONG*)p2, bv1.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator >(const BYTE* p1, const ClBV& bv2)
{ return bv2.Pogl((ULONG*)p1, bv2.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator >(const ClBV& bv1, const ULONG* p2)
{ return bv1.Pogl(p2, bv1.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator >(const ULONG* p1, const ClBV& bv2)
{ return bv2.Pogl(p1, bv2.m_nBitLength, FALSE);}

//-----------------------------------------------------
inline BOOL operator <(const ClBV& bv1, const ClBV& bv2)
{ return bv1.Pogl(bv2.m_bVect, bv2.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator <(const ClBV& bv1, const CBV& bv2)
{ return bv1.Pogl((ULONG*)((BYTE*)bv2), bv2.GetBitLength(), FALSE);}
//-----------------------------------------------------
inline BOOL operator <(const CBV& bv1, const ClBV& bv2)
{ return bv2.Pogl((ULONG*)((BYTE*)bv1), bv1.GetBitLength(), TRUE);}
//-----------------------------------------------------
inline BOOL operator <(const ClBV& bv1, const BYTE* p2)
{ return bv1.Pogl((ULONG*)p2, bv1.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator <(const BYTE* p1, const ClBV& bv2)
{ return bv2.Pogl((ULONG*)p1, bv2.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator <(const ClBV& bv1, const ULONG* p2)
{ return bv1.Pogl(p2, bv1.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator <(const ULONG* p1, const ClBV& bv2)
{ return bv2.Pogl(p1, bv2.m_nBitLength, TRUE);}

//-----------------------------------------------------
inline BOOL operator >=(const ClBV& bv1, const ClBV& bv2)
{ return bv1.PoglEq(bv2.m_bVect, bv2.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator >=(const ClBV& bv1, const CBV& bv2)
{ return bv1.PoglEq((ULONG*)((BYTE*)bv2), bv2.GetBitLength(), TRUE);}
//-----------------------------------------------------
inline BOOL operator >=(const CBV& bv1, const ClBV& bv2)
{ return bv2.PoglEq((ULONG*)((BYTE*)bv1), bv1.GetBitLength(), FALSE);}
//-----------------------------------------------------
inline BOOL operator >=(const ClBV& bv1, const BYTE* p2)
{ return bv1.PoglEq((ULONG*)p2, bv1.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator >=(const BYTE* p1, const ClBV& bv2)
{ return bv2.PoglEq((ULONG*)p1, bv2.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator >=(const ClBV& bv1, const ULONG* p2)
{ return bv1.PoglEq(p2, bv1.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator >=(const ULONG* p1, const ClBV& bv2)
{ return bv2.PoglEq(p1, bv2.m_nBitLength, FALSE);}

//-----------------------------------------------------
inline BOOL operator <=(const ClBV& bv1, const ClBV& bv2)
{ return bv1.PoglEq(bv2.m_bVect, bv2.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator <=(const ClBV& bv1, const CBV& bv2)
{ return bv1.PoglEq((ULONG*)((BYTE*)bv2), bv2.GetBitLength(), FALSE);}
//-----------------------------------------------------
inline BOOL operator <=(const CBV& bv1, const ClBV& bv2)
{ return bv2.PoglEq((ULONG*)((BYTE*)bv1), bv1.GetBitLength(), TRUE);}
//-----------------------------------------------------
inline BOOL operator <=(const ClBV& bv1, const BYTE* p2)
{ return bv1.PoglEq((ULONG*)p2, bv1.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator <=(const BYTE* p1, const ClBV& bv2)
{ return bv2.PoglEq((ULONG*)p1, bv2.m_nBitLength, TRUE);}
//-----------------------------------------------------
inline BOOL operator <=(const ClBV& bv1, const ULONG* p2)
{ return bv1.PoglEq(p2, bv1.m_nBitLength, FALSE);}
//-----------------------------------------------------
inline BOOL operator <=(const ULONG* p1, const ClBV& bv2)
{ return bv2.PoglEq(p1, bv2.m_nBitLength, TRUE);}

inline const ClBV& ClBV::operator =(const CBV& bvSrc)              //operator =
{ 
  m_bVect = (ULONG*)((BYTE*)bvSrc);
  m_nBitLength = bvSrc.GetBitLength();
  m_nLongLength = m_nBitLength/32;
  m_nByteRest = bvSrc.GetByteLength() - m_nLongLength*4;
  return *this;
}

inline const ClBV& ClBV::operator =(const ClBV& bvSrc)             //operator =
{ 
  m_bVect = bvSrc;
  m_nBitLength = bvSrc.m_nBitLength;
  m_nLongLength = bvSrc.m_nLongLength;
  m_nByteRest = bvSrc.m_nByteRest;
  return *this;
}

inline const ClBV& ClBV::operator =(const BYTE* pbt)                //operator =
{
  m_bVect = (ULONG*)pbt;
  return *this;
}

#ifndef _LINUX
#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif

#endif
