/////////////////////////////////////////////////////////////////
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.0     08.03.2005
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
// Inerator Boolean vector
#ifndef _ITERBOOLEAN_
#define _ITERBOOLEAN_

#include "../Common/BaseBool.h"
/////////////////////////////////////////////////////////////////////////////
// Boolean vector iterator class
class CiBV {
   friend class CBV;
public:
// Constructors and destructor
  CiBV();
  CiBV(CBV& bv);
  ~CiBV() {};

// Methods
  //         CiBV i(bv);
  //         for (k = i.Begin(); k>=0; k = i.Next()) { . . . }
  int Begin(int First = -1);
  int End(int Last = -1);
  int Next();
  int Prev();
// Operators

protected:
  // Attributes
  BYTE *m_adr;
  BYTE m_current;
  int m_len;
  int m_offset;
};
 
#endif 