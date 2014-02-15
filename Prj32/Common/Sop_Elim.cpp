/////////////////////////////////////////////////////////////////
// Ver.2.1.1    20.06.2007
//               AfxMessageBox - ��� LINUX �� ������� + ����� ������� NULL
// Ver.2.1.0    07.07.2006
//               ������� � LINUX.

/////////////////////////////////////////////////////////////////
// Ver.2.0.3     21.03.2006
//             ���������� ������ ������������ �� ��������
//             ForMK_V(CBV &bvX) ���������� �� ������ CBM;
// Ver.2.0.0     17.03.2006   Russian == English
/////////////////////////////////////////////////////////////////
//1  CSOP RedSeq();
//2  CSOP SeqSdnf_F();
//3  CSOP DelInt_VR(CBV &bvXY, int rest);
//4  CSOP CSOP::Sop_toMTM(MTM &Mtm);
//5 CBM ForMA_V(CBV &bvX);  
//6 CBM ForMK_V(CBV &bvX);  
//7 void ForLDNF_V(CBV &bvFY, CBM &BmA, CBM &BmK, MTM &MTm);
//-------------------------------------------------------------------
//7 extern CTM MapLog_ToFi(MTM &MTm, CBM &BmA, int y, int i);
//8 extern CTM MapLog_ToFi(MTM &MTm, CBM &BmA, CBV &bvXY, int i);

#ifdef _JOINT
#include "Sop.h"
#else
#include "../Common/Sop.h"
#endif

// #ifdef _DEBUG
// #undef THIS_FILE
// static char BASED_CODE THIS_FILE[] = __FILE__;
// #endif

/////////////////////////////////////////////////////////////////////
//______________________________________________________
// �������� ������� ���������� ��������� ����� 
// "��������������� ��������" � ���������� �������������
// ����������, ������������� ������ ������������� ��� 
// ������� ����������.
//______________________________________________________

CSOP CSOP::RedSeq()
{
  int c,d,e,i,j,k,g=1,y=GetnInterVar(),p=GetnCon(),
     n=GetnArg()-y, m=GetnFun()-y;
  char s;
  CSOP Sop1;
  CBM BmFT(GetFunBM(),0);
  CTM TmXT=m_tmCon.Trans();
  CTV tvK(p,'-'), tvI(p,'-');
  CBV bvC, bvF((BYTE)1,m+y), bvR((BYTE)1,p), bvA=bvR,
      bvX((BYTE)1,n+y), bv1;
  while(g) 
  {
     g=0; i=m-1; bvA.Zero();
    while((i=bvF.LeftOne(i))!=-1) 
    { // ����� ����������, ������������ ������ '0' ��� '1' 
      c=0;
      if((bvC=BmFT.GetRowBv(i,bvR)).IsZero())
      {c=2; bvR^=TmXT.GetRowOne(i-m+n,bvR);}// i-� ������� ������ �� ����� 
      if((tvI=TmXT.GetRowTv(i-m+n,bvR)).GetDefs().IsZero())
      { c=1; bvA|=bvC;}// (i-m+n)-� ������� ����� �� ��������
      if(c) 
      { bvF.SetBitAt(i,0); bvX.SetBitAt(i-m+n,0);}
      else 
      {
        j=-1; d=0;
        while((j=bvC.LeftOne(j))!=-1) 
        {
          if(m_tmCon.IsTrivial(bvX,j)) 
          {// j-� ������ ����� �� ��������
            c=3; bv1=m_bmFun.GetRowBv(j,bvF); k=m-1;
            while((k=bv1.LeftOne(k))!=-1) 
            {
              bvF.SetBitAt(k,0);
              bvR^=TmXT.GetRowZero(k-m+n,bvR);
              bvX.SetBitAt(k-m+n,0);
              bvA|=BmFT.GetRowBv(k,bvR);
            }
            break;
          }
          d++; e=j;
        }
        if(!c && (d==1)) 
        {
          k=m_tmCon.LeftDef(e,-1,bvX);
          if(((j=m_tmCon.LeftDef(e,k,bvX))==-1)) 
          {
            c=4;tvK=TmXT.GetRowTv(k,bvR); s='1';
            if(tvK.GetBitAt(e)!=s) 
            { tvI.InvertDefs(); s='0';}
            bvF.SetBitAt(i,0);
            bvX.SetBitAt(i-m+n,0);
            bvA|=bvC;
            bvR^=tvK.Orthogon(tvI);
            tvK.OrInPlace(0,tvI.GetZeros());
            tvK.OrInPlace(1,tvI.GetOnes());
            TmXT.SetRowTv(k,tvK); j=-1;
            while((j=bvR.LeftOne(j))!=-1)
              m_tmCon.SetBitAt(j,k,TmXT.GetBitAt(k,j));
          }
        }
      }
      if(c) g=1;
    }
    if(g) 
    {
      i=-1; bvA&=bvR;
      while((i=bvA.LeftOne(i))!=-1)
        if(m_bmFun.GetRowBv(i,bvF).IsZero()) bvR.SetBitAt(i,0);
    }
  }
// ��������� ������
  Sop1.m_tmCon=m_tmCon.Minor(bvR,bvX);
  Sop1.m_bmFun=m_bmFun.Minor(bvR,bvF);
  y=bvX.CountBit()-n;
  m_tmCon.SetSize(0, n+y);
  m_bmFun.SetSize(0, m+y);
  DorD(1, Sop1);
  SetnInterVar(y);
  return *this; 
}

//_____________________________________________________________
// ���������� ������������� ���������� � ������� ��� 
// ����� ����������� ����������� ��� � ������ ����
// � ���������� �������� � ��������� �� ������������
//_____________________________________________________________

CSOP CSOP::SeqSdnf_F()
{
  CBV bvXY (1,GetnArg()); 
  bvXY<<=GetnInterVar();
  return DelInt_VR (bvXY, 0);
}

//_____________________________________________________________
// ���������� ������������� ���������� � ������� ��� 
// ����� ����������� ����������� ��� � ������ ����
// � ���������� �������� � ��������� �� ������������
// (��������� ����������)
//_____________________________________________________________

CSOP CSOP::DelInt_VR(CBV &bvXY, int rest)
{
  int nn=(~bvXY).LeftOne(-1); if(nn==-1) return *this; 
  int a, b, d, e, f, g, h, i, j, k, r, w, 
      n=bvXY.CountBit(), y=GetnArg()-n, yy= GetnArg()-nn, 
      mm= GetnFun()-yy, m=GetnFun()-y; 
  MTM MTm; MTm.resize(m+y);
  CBM BmA, BmK;
  CTM Tm, TmM, TmE, TmEE, TmEE_I, TmF, TmFF;
  CTV tvC, tv;
  CBV bv1, bvA((BYTE)0,n+y), bvE, bvF, bvC, bvD, bvR, bvFY((BYTE)1,mm); 
  bvFY.Concat(bvXY.Extract(nn,yy));
  BmA= ForMA_V (bvXY);// ������������ (m+y)*(n+y) ������� ���������� BmA
  BmK= ForMK_V (BmA, bvXY);// ������������ k*y ������� �������� BmK
  if(!BmK.GetCountR()) return  *this;
  bvD=BmK.Disjunction();
  r=bvD.CountBit();
  ForLDNF_V (bvFY, BmA, BmK, MTm);//������������ ������ ��� ��������� MTm
  // ����� �� ������� BmK ��������� (j-�) ������������� ��� TmE
  for(i=BmK.GetCountR()-1; i>=0; i--) 
  {
    bvC=BmK.GetRowBv(i); 
    while(!bvC.IsZero())
    {
      g=0;
      if(r<=rest) 
      {
        a=-1; g=1; 
        while((a=bvD.LeftOne(a))!=-1)
        {bvXY.SetBitAt(a,1); bvFY.SetBitAt(a-nn+mm,1);}
        break;
      }
      a=-1; w=999999;
      while((a=bvC.LeftOne(a))!=-1)
      if((b=MTm[a-nn+mm].GetCountR())<w) {w=b; j=a;}
      bvC.SetBitAt(j,0);
      TmE=MTm[mm+j-nn];
      bvE=BmA.GetRowBv(mm+j-nn);
// ����� ��  ������� BmA ��������� (k-�) ������������� ��� TmF 
// ��� ����������� � ��� ��� TmE
      bvR=BmA.GetColumnBv(j); k=-1;
      while((k=bvR.LeftOne(k))!=-1)
      if(MTm[k].GetCountR()) 
      {
        TmF=MTm[k]; 
        bvF=BmA.GetRowBv(k);
        a=-1; e=0;// ��������� ������������� ���������� � ��� TmF
        while((a=bvF.LeftOne(a))!=j) e++;
        tvC=TmF.GetColumnTv(e); bvF.SetBitAt(a,0);
        bvA=bvE|bvF;     // ����������� �������� ���������� ������������� � ������������� TmE � TmF
        h=bvA.CountBit();
        BmA.SetRow(k,bvA);
        TmEE=TmE.DistAL(-1, bvE, bvA);// ���������� ��������� ���������� ������������� ��� (TmE -> TmEE)   while((a=bv.LeftOne(a))!=-1)
        TmFF= TmF.DistAL(e, bvF, bvA);// ���������� ��������� ���������� ������������� ��� (TmF -> TmFF)   while((a=bv.LeftOne(a))!=-1)
        bv1=tvC.GetUnDefs(); // ����������� ��� TmEE � ��� TmFF
        f=-1;
        TmM.SetSize(0,TmM.GetCountC());
        Tm.SetSize(0,Tm.GetCountC());
        while ((f=bv1.LeftOne(f))!=-1)
        TmM.Add(TmFF.GetRowTv(f));
        d=0; f=-1;
        while ((f=tvC.LeftDef(f))!=-1) 
        {
          tv=TmFF.GetRowTv(f);
          if(tvC.GetBitAt(f)=='1')
          Tm.DisConDis(tv, TmEE );//����������� �� ����� '1'
          else 
          {
            if(!d) { TmEE_I = TmEE.InverS(1); d=1;}
            Tm.DisConDis(tv, TmEE_I);//����������� �� ����� '0'
          }
        }
        TmM.DorD(1,Tm);
        MTm[k]=TmM; 
      }
      r--; bvD.SetBitAt(j,0);
    }
    if(g) break;
  }
  CSOP SopRes((n=bvXY.CountBit()),(m=bvFY.CountBit()),0);  //21.03.2006
  bvF.SetSize(m);
  bvF.Zero(); 
  i=-1; a=0; 
  while((i=bvFY.LeftOne(i))!=-1) 
  {
    TmF=MTm[i]; 
    bvA=BmA.GetRowBv(i);
    TmFF= TmF.DistAL(-1, bvA, bvXY);
    bvF.SetBitAt(a,1); 
    for(k=0; k<TmFF.GetCountR(); k++)
    SopRes.DorD(1,TmFF.GetRowTv(k),bvF);
    bvF.SetBitAt(a,0); a++; 
  }
  bvA=bvXY; a=-1; b=0;
  bvXY.SetSize(n); bvXY.One();
  while((a=bvA.LeftOne(a))!=-1) 
  {
    if(bvD.GetBitAt(a))
    {bvXY.SetBitAt(b,0);}
    b++;
  }
  SopRes.SetnInterVar((~bvXY).CountBit());
  return SopRes; 
 }

//__________________________________________________________________
// ���������� ��������� Sop ���� f(X)=c((E1)(E2)...(Ek)), ��� Ei - 
// ������� ��������� ���������, � ���� f(X)=c((D1)(D2)...(Dk)), 
// ��� Di - ��� � ����������� ������ �� ��������� ������� ���������� X.
// �������������� ������� ����� ���������� ���� ������������� ����������,
// ������������ ���������� ������ � ���������� Ei. ��������� ������������
// � ����� Sop_R, � ����� �������������� � ���� ������� �������� ������ Mtm,
// ������� ������� �����������, ����� �������� �������� �������������� ������� f(X).
//___________________________________________________________________

CSOP CSOP::Sop_toMTM(MTM &Mtm)
{
  int a=0, i, y=GetnInterVar(), m=GetnFun()-y, 
      n=GetnArg()-y, p=GetnCon();
  CSOP Sop1;
  CTM Tm; CBV bv=m_bmFun.GetColumnBv(0);
  if(m!=1 || bv.CountBit()!=1)
  {
#ifndef _LINUX
  AfxMessageBox("Other type"); 
#endif
  return Sop1;
  }   // Romanov 04.03.2006 NULL
  CBV bvXY((BYTE)1,n+y), bvX=bvXY<<y, bvY; 
  bvXY=bvX | (bvY=m_tmCon.GetRowDef(bv.LeftOne(-1)));
  Sop1=DelInt_VR (bvXY, 0);
  CBM Bm=Sop1.m_bmFun.Trans(); y=Bm.GetCountR()-1;
  bvX=bvX.Extract(0,n); 
  for(i=0; i<y+1; i++)
    Mtm.push_back(Sop1.m_tmCon.Minor(Bm.GetRowBv(i),bvX));
  Sop1.SetnInterVar(y);
  return Sop1; 
}

//*************************************************************
// ������������ (m+y)*(n+y) ������� ���������� BmA ��� m+y 
// ���������� ��������� ������� this � m ���������, ���������� ��
// n ������� � y ������������� ����������. ������������ �������
// � ������ �������� � ������� ������ � this ����������� ���������
// � �������� bvX, bvQ � bvR, ��������������. 
// ************************************************************ 

CBM CSOP::ForMA_V(CBV &bvX)
{
  int g, i, j, n=bvX.CountBit(), nn=(~bvX).LeftOne(-1),
      y = GetnArg() - n, yy = GetnArg() - nn, 
      mm = GetnFun() - yy, m = GetnFun() - y;
        //CString sop, sBmA, sbvX, sbvA, sbvE, sbvF, sbvQ;  //sop=Sop.ToChar(); //sbvX=bvX.BitChar();
  CBV bvA=bvX, bvF((BYTE)1,mm), bvE, bv1, bvC;
  bvF.Concat(bvX.Extract(nn,yy));
  bvA.Zero(); bvF.One();
  CBM BmA(m+y, n+y), BmFT(GetFunBM(),0);
  bvE=bvA; bvE.SetBitAt(bvX.LeftOne(-1),1);
  for(i=0; i<m+y; i++)
  {
    if(!bvF.GetBitAt(i)) continue;
    bvC = BmFT.GetRowBv(i); bvA.Zero(); j=-1; g=0;
    while((j=bvC.LeftOne(j))!=-1)
    {
      if(!(bv1 = m_tmCon.GetRowDef(j)).IsZero())
        bvA|=bv1;
      else 
      {
        bvF &= ~(bv1 = m_bmFun.GetRowBv(j));
        j=-1;
        while((j=bv1.LeftOne(j))!=-1)
        BmA.SetRow(j,bvE);          //sBmA=BmA.BitChar();
        g=1; break;
      }
    }
    if(!g) BmA.SetRow(i,bvA);       //sBmA=BmA.BitChar();
  }
  return BmA;
}

//-------------------------------------------------------------------

//*************************************************************
// ������������ k*y ������� �������� BmK ��� m+y ���������� 
// ��������� ������ Sop � m ���������, ���������� ��
// n ������� � y ������������� ����������. 
// ************************************************************ 

CBM CSOP::ForMK_V (CBM &BmA, CBV &bvX)
{
  int i, k, nn=(~bvX).LeftOne(-1), yy=BmA.GetCountC()-nn,
      mm=BmA.GetCountR()-yy;
  CBV bv, bvK=bvX, bvXY=~bvX, bvF((BYTE)1,mm);
  bvF.Concat(bvX.Extract(nn,yy));
  CBM BmK; 
  bvK.Zero(); i=-1;
  while((i=bvF.LeftOne(i))!=-1)
    bvK |= BmA.GetRowBv(i,bvXY);
  while(!bvK.IsZero())
  {
    BmK.Add(bvK); bv=bvK;
    i=-1; bvK.Zero();
    while((i=bv.LeftOne(i))!=-1)
      bvK |= BmA.GetRowBv(i-nn+mm, bvXY);
    if(!bvK.IsZero())
      for(k=0; k<BmK.GetCountR(); k++)
      {
        BmK.SetRow(k,BmK.GetRowBv(k)-bvK);
        if(BmK.GetRowBv(k).IsZero()) 
	      {
#ifndef _LINUX
          AfxMessageBox("There is a circle in the net!");
#endif
	        return BmK;
        }
      }
  }
  return BmK;
}

//*************************************************************
// ������������ ������ ��������� ��� ��������� ������� DNF ��� m+y 
// ���������� ��������� ������� Sop � m ���������, ���������� ��
// n ������� � y ������������� ����������, �� ������� ���������� 
// BmA � ������� �������� BmK. 
// ************************************************************ 
void CSOP::ForLDNF_V(CBV &bvFY, CBM &BmA, CBM &BmK, MTM &MTm)
{
  int i, j, y=(~bvFY).CountBit(), n=BmA.GetCountC()-y, 
      m=bvFY.CountBit(), mm=(~bvFY).LeftOne(-1),
      nn=GetnArg()+mm-GetnFun(); 
  CBV bv;
  CBM BmFT(GetFunBM(),0);     //sBmFT=BmFT.BitChar();
  CTM TmF, TmFF;
  i=-1;
  while((i=bvFY.LeftOne(i))!=-1)
  {
    TmF=m_tmCon.Minor(BmFT.GetRowBv(i),BmA.GetRowBv(i));
    TmFF.SetSize(0,TmF.GetCountC());
    TmFF.DorD(2,TmF); 
    MTm[i]=TmFF;
  }
  for(i=0; i<BmK.GetCountR(); i++)
  {
    j=-1; bv=BmK.GetRowBv(i);
    while((j=bv.LeftOne(j))!=-1)
    {
      TmF=m_tmCon.Minor(BmFT.GetRowBv(j-nn+mm),BmA.GetRowBv(j-nn+mm));
      TmFF.SetSize(0,TmF.GetCountC());
      TmFF.DorD(2,TmF); 
      MTm[j-nn+mm]=TmFF;
    }
  }
}

//***************************************************
// ������� ������� �� m+y ���������� ��������� �� n+y
// ���������� ������������ i-� �������� ����������. 
// ������ j-e ��������� �������������� j-� ��������� ���
// � ������ MTm � j-� ������� � ������� ���������� BmA.
// (n,m � y - ����� �����., ������. � ��������. ����������).
//***************************************************

CTM MapLog_ToFi(MTM &MTm, CBM &BmA, int y, int i)
{
  int a, b, c, d, e, f, g, h, j, k, m=(BmA.GetCountR()-y), w,
      n=(BmA.GetCountC()-y);
                    //CString sBmA, sBmK, sTm, stv, stvC, sbvA, sbvE, sbvF, sbvR, 
                    //stvF, sbvK, sTmM, sTmE, sTmEE, sTmEE_I, sTmF, sTmFF, sbv1;
  CBM BmK; 
  CTM Tm, TmM, TmE, TmEE, TmEE_I, TmF, TmFF;
  CTV tvC, tv;
  CBV bv1, bvA((BYTE)0,n+y), bvE, bvF, bvC, bvR, bvK((BYTE)0,y), bv=bvK;

// ������������ k*y ������� �������� BmK
  bvK=BmA.GetRowBv(i).Extract(n,y);
  while(!bvK.IsZero())
  {
    BmK.Add(bvK);bv=bvK;
    j=-1; bvK.Zero();
    while((j=bv.LeftOne(j))!=-1)
      bvK|=BmA.GetRowBv(m+j).Extract(n,y);
    if(!bvK.IsZero())
      for(k=0; k<BmK.GetCountR(); k++)
      {
        BmK.SetRow(k,BmK.GetRowBv(k)-bvK);
        if(BmK.GetRowBv(k).IsZero()) return TmFF;
      }
  }                  //sBmK=BmK.BitChar();
// ����� �� ������� BmK ��������� (j-�) ������������� ��� TmE
  for(g=BmK.GetCountR()-1; g>=0; g--)
  {
    bvC=BmK.GetRowBv(g); 
    while(!bvC.IsZero())
    {
      a=-1; w=999999;
      while((a=bvC.LeftOne(a))!=-1)
      if((b=MTm[a+m].GetCountR())<w) {w=b; j=a;}
      bvC.SetBitAt(j,0);
      TmE=MTm[m+j];         //sTmE=TmE.BitChar();
      bvE=BmA.GetRowBv(m+j);
// ����� ��  ������� BmA ��������� (k-�) ������������� ��� TmF 
// ��� ����������� � ��� ��� TmE
      bvR=BmA.GetColumnBv(n+j);
// ����� ��  ������� BmA ��������� (k-�) ������������� ��� TmF 
// ��� ����������� � ��� ��� TmE
      k=-1;
      while((k=bvR.LeftOne(k))!=-1)
      {
        if(MTm[k].GetCountR())
        {
          TmF=MTm[k];
          bvF=BmA.GetRowBv(k);    //sTmF=TmF.BitChar();
// ��������� ������������� ���������� � ��� TmF
          a=-1; e=0;
          while((a=bvF.LeftOne(a))!=(n+j)) e++;
          tvC=TmF.GetColumnTv(e); bvF.SetBitAt(a,0);    //stvC=tvC.BitChar();//sbvF=bvF.BitChar();
// ����������� �������� ���������� ������������� � ������������� TmE � TmF
          bvA=bvE|bvF;
          h=bvA.CountBit();            //sbvA=bvA.BitChar();
          BmA.SetRow(k,bvA);            //sBmA=BmA.BitChar();
// ���������� ��������� ���������� ������������� ��� (TmE -> TmEE)   while((a=bv.LeftOne(a))!=-1)
                                                                     //TmEE=TmE;
          TmEE.SetSize(TmE.GetCountR(),(h)?h:TmE.GetCountC());
          TmEE.Clear('-');
          a=-1; c=d=0;
          while((a=bvA.LeftOne(a))!=-1)
          {
            if(bvE.GetBitAt(a))
            {
              for(b=0; b<TmE.GetCountR(); b++)
                TmEE.SetBitAt(b,c,TmE.GetBitAt(b,d));
              d++;
            }
            c++;
          }           //sTmEE=TmEE.BitChar();
// ���������� ��������� ���������� ������������� ��� (TmF -> TmFF)   while((a=bv.LeftOne(a))!=-1)
          TmFF.SetSize(TmF.GetCountR(),(h)?h:TmF.GetCountC());
          TmFF.Clear('-');
          a=-1; c=0; d=(e)?0:1; 
          while((a=bvA.LeftOne(a))!=-1)
          {
            if(bvF.GetBitAt(a))
            {
              for(b=0; b<TmF.GetCountR(); b++)
                TmFF.SetBitAt(b,c,TmF.GetBitAt(b,d));
              d++; if(d==e) d++;
            }
            c++;
          }           //sTmFF=TmFF.BitChar();
// ����������� ��� TmEE � ��� TmFF
          bv1=tvC.GetUnDefs(); 
          f=-1;
          TmM.SetSize(0,TmM.GetCountC());
          Tm.SetSize(0,Tm.GetCountC());
          while ((f=bv1.LeftOne(f))!=-1)
            TmM.Add(TmFF.GetRowTv(f));
          d=0; f=-1;
          while ((f=tvC.LeftDef(f))!=-1)
          {
            e=0; b=Tm.GetCountR();
            tv=TmFF.GetRowTv(f);
            if(tvC.GetBitAt(f)=='1')
            {           //����������� �� ����� '1'
              for(a=0; a<TmEE.GetCountR(); a++)
                if(!TmEE.AreOrthog(a,tv))
                {
                  Tm.Add(TmEE.Intersec(a,tv));
                  if(Tm.AreEqual((b+e), tv)) 
                  { Tm.RemoveRow(b,e); break;}
                  e++;
                }
            }
            else
            {         //����������� �� ����� '0'
              if(!d)
              {
                TmEE_I=TmEE.InverS(1); d=1;       //sTmEE_I=TmEE_I.BitChar();
              }
              for(a=0; a<TmEE_I.GetCountR(); a++)
                if(!TmEE_I.AreOrthog(a,tv))
                {
                  Tm.Add(TmEE_I.Intersec(a,tv));
                  if(Tm.AreEqual((b+e), tv)) 
                  { Tm.RemoveRow(b,e); break;}
                  e++;
                }
            }
          }         //sTm=Tm.BitChar();     //sTmM=TmM.BitChar();
          TmM.DorD(1,Tm);         //sTmM=TmM.BitChar();
          MTm[k]=TmM; 
        }
      }
    }
  }
  bvF.SetSize(m);
  bvF.Zero(); 
  TmF=MTm[i];
  bvA=BmA.GetRowBv(i);          //sTmF=TmF.BitChar();
  TmFF.SetSize(TmF.GetCountR(),n);
  TmFF.Clear('-');
  for(a=c=d=0; a<n; a++)
  {
    if(bvA.GetBitAt(a))
    {
      for(b=0; b<TmF.GetCountR(); b++)
        TmFF.SetBitAt(b,c,TmF.GetBitAt(b,d));
      d++;
    }
    c++;
  }
  return TmFF; 
}

//***************************************************
// ������� ������� �� m+y ���������� ��������� �� n+y
// ���������� ������������ i-� �������� ����������. 
// ������ j-e ��������� �������������� j-� ��������� ���
// � ������ MTm � j-� ������� � ������� ���������� BmA.
// (n,m � y - ����� �����., ������. � ��������. ����������).
//***************************************************

CTM MapLog_ToFi(MTM &MTm, CBM &BmA, CBV &bvXY, int p)
{
  int a, b, d, e, f, h, i, j, k,  w,
     n=bvXY.CountBit(), y=BmA.GetCountC()-n, m=(BmA.GetCountR()-y),
     nn=(~bvXY).LeftOne(-1), mm=m-n+nn;
                  //CString sBmA, sBmK, sTm, stv, stvC, sbvA, sbvE, sbvF, 
                  //stvF, sbvK, sTmM, sTmE, sTmEE, sTmEE_I, sTmF, sTmFF, sbv1;
  CTM Tm, TmM, TmE, TmEE, TmEE_I, TmF, TmFF;
  CTV tvC, tv;
  CBV bv1, bvA((BYTE)0,n+y), bvE, bvC, bvR,
     bvK((BYTE)0,y), bv=bvK, bvF((BYTE)0,m+y);
  bvF.SetBitAt(p,1);
  CSOP wSop;
  CBM BmK=wSop.ForMK_V(BmA,bvXY);      //sBmK=BmK.BitChar();   //sBmA=BmA.BitChar();
// ����� �� ������� BmK ��������� (j-�) ������������� ��� TmE
  for(i=BmK.GetCountR()-1; i>=0; i--)
  {
    bvC=BmK.GetRowBv(i); 
    while(!bvC.IsZero())
    {
      a=-1; w=999999;
      while((a=bvC.LeftOne(a))!=-1)
        if((b=MTm[a-nn+mm].GetCountR())<w) {w=b; j=a;}
      bvC.SetBitAt(j,0);
      TmE=MTm[mm+j-nn];     //sTmE=TmE.BitChar();
      bvE=BmA.GetRowBv(mm+j-nn);    //sbvE=bvE.BitChar();
// ����� ��  ������� BmA ��������� (k-�) ������������� ��� TmF 
// ��� ����������� � ��� ��� TmE
      bvR=BmA.GetColumnBv(j); k=-1;
      while((k=bvR.LeftOne(k))!=-1)
        if(MTm[k].GetCountR())
        {
          TmF=MTm[k]; 
          bvF=BmA.GetRowBv(k);      //sTmF=TmF.BitChar();
// ��������� ������������� ���������� � ��� TmF
          a=-1; e=0;
          while((a=bvF.LeftOne(a))!=j) e++;
          tvC=TmF.GetColumnTv(e); bvF.SetBitAt(a,0);    //stvC=tvC.BitChar(); //sbvF=bvF.BitChar();
// ����������� �������� ���������� ������������� � ������������� TmE � TmF
          bvA=bvE|bvF;
          h=bvA.CountBit();
          BmA.SetRow(k,bvA);    //sBmA=BmA.BitChar();
          TmEE=TmE.DistAL(-1, bvE, bvA);
// ���������� ��������� ���������� ������������� ��� (TmE -> TmEE)   while((a=bv.LeftOne(a))!=-1)
                                                          //sTmEE=TmEE.BitChar();
          TmFF=TmF.DistAL(e, bvF, bvA);
// ���������� ��������� ���������� ������������� ��� (TmF -> TmFF)   while((a=bv.LeftOne(a))!=-1)
                                                          //sTmFF=TmFF.BitChar();
// ����������� ��� TmEE � ��� TmFF
          bv1=tvC.GetUnDefs(); 
          f=-1;
          TmM.SetSize(0,TmM.GetCountC());
          Tm.SetSize(0,Tm.GetCountC());
          while ((f=bv1.LeftOne(f))!=-1)
            TmM.Add(TmFF.GetRowTv(f));
          d=0; f=-1;
          while ((f=tvC.LeftDef(f))!=-1)
          {
            tv=TmFF.GetRowTv(f);
            if(tvC.GetBitAt(f)=='1')
              Tm.DisConDis(tv, TmEE);//����������� �� ����� '1'
            else
            { if(!d) {TmEE_I=TmEE.InverS(1); d=1;}
            Tm.DisConDis(tv, TmEE_I);//����������� �� ����� '0'
          }
        }   //sTm=Tm.BitChar(); //sTmM=TmM.BitChar();
        TmM.DorD(1,Tm);     //sTmM=TmM.BitChar();
        MTm[k]=TmM; //WE[k]=TmM.GetCountR();
      }
    }
  }
  TmF=MTm[p]; 
  bvA=BmA.GetRowBv(p);      //sTmF=TmF.BitChar();
  TmFF=TmF.DistAL(-1, bvA, bvXY); //sTmFF=TmFF.BitChar();
  return TmFF; 
}
