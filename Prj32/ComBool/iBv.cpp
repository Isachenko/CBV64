/////////////////////////////////////////////////////////////////
// Ver.2.1.0     03.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.0     08.03.2005
/////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////
// Iterator Boolean vector
#include "../ComBool/IterBool.h"

/////////////////////////////////////////////////////////////////////////////
// Boolean vector iterator class

//---------------------------------------------------------
CiBV::CiBV() 
{
  m_adr = NULL;
  m_current = 0;
  m_offset = m_len = 0;
}

//---------------------------------------------------------
CiBV::CiBV(CBV& bv)
{
  m_adr = (BYTE *)bv;
  m_current = m_adr[0];
  m_len = bv.GetByteLength();
  m_offset = 0;
}

//---------------------------------------------------------
int CiBV::Begin(int First)
{
  int bit;
  if (m_len == 0) return -1;
  if (First > -1) {
    m_offset = BIT_BYTE(First+1);  bit = ADR_BIT(First) + 1; 
    if (m_offset == m_len)  return (-1);
    if (bit == S_1) { m_current = m_adr[m_offset];}
    else { m_current = (m_adr[m_offset] << bit) >> bit;  }
  }
  else { m_current = m_adr[0]; m_offset = 0; }
  return Next();
}

//---------------------------------------------------------
int CiBV::Next()
{
  int pos = 0;

  if (m_current!=0) {
m0: if (m_current & 0xf0)
      if (m_current & 0xc0)
        if (m_current & 0x80) { m_current&=0x7f; return ((m_offset<<3)); }
        else                  { m_current&=0x3f; return ((m_offset<<3)+1); }
      else
        if (m_current & 0x20) { m_current&=0x1f; return ((m_offset<<3)+2); }
        else                  { m_current&=0x0f; return ((m_offset<<3)+3); }
    else
      if (m_current & 0x0c)
        if (m_current & 0x08) { m_current&=0x07; return ((m_offset<<3)+4); }
        else                  { m_current&=0x03; return ((m_offset<<3)+5); }
      else
        if (m_current & 0x02) { m_current&=0x01; return ((m_offset<<3)+6); }
        else                  { m_current =0; return ((m_offset<<5)+7); }
  }
  for (m_offset++; m_offset < m_len; m_offset++)
    if (m_adr[m_offset])  { m_current = m_adr[m_offset]; goto m0; }
  return (-1);
}

//---------------------------------------------------------
int CiBV::End(int Last)
{
  int bit;
  if (m_len == 0) return -1;
  if (Last == -1) { 
    m_offset = m_len - 1; 
    m_current = m_adr[m_offset];
  }
  else {
    m_offset = BIT_BYTE(Last);  
    if (m_offset >= m_len) return -1;
    bit = S_1 - ADR_BIT(Last); 
    if (bit == S_1) { 
      m_offset--; 
      m_current = m_adr[m_offset]; 
    }
    else 
      m_current = (m_adr[m_offset] >> bit) << bit;
  }
  return Prev();
}

//---------------------------------------------------------
int CiBV::Prev()
{
  int pos = S_1 - 1;
  if (m_current!=0) {
m0: if (m_current& 0x0f)
      if (m_current & 0x03) 
        if (m_current & 0x01) { m_current&=0xfe; return ((m_offset<<3)+7); }
        else                  { m_current&=0xfc; return ((m_offset<<3)+6); }
      else
        if (m_current & 0x04) { m_current&=0xf8; return ((m_offset<<3)+5); }
        else                  { m_current&=0xf0; return ((m_offset<<3)+4); }
    else
      if (m_current & 0x30) 
        if (m_current & 0x10) { m_current&=0xe0; return ((m_offset<<3)+3); }
        else                  { m_current&=0xc0; return ((m_offset<<3)+2); }
      else
        if (m_current & 0x40) { m_current&=0x80; return ((m_offset<<3)+1); }
        else                  { m_current = 0; return (m_offset<<3); }
  }
  for (m_offset--; m_offset >= 0; m_offset--)
    if(m_adr[m_offset])  { m_current = m_adr[m_offset]; goto m0; }
  return (-1);
}
