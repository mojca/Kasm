#include "pasm.h"
#include <sstream>
//#include <iostream>
//using namespace std;
//---------------------------------------------------------------------------
int idbg=0; ofstream dbg;//("dbg.txt");
//---------------------------------------------------------------------------
int kid (const real& xk);
real ext (const int n, const real* const x);
//---------------------------------------------------------------------------
real rwsk::extr (const int n, const real* const x, real& xi, real& xa)
{xi=xa=x[0];
 for (int i=1; i<n; ++i)
 {if (x[i]<xi) xi=x[i];
  if (x[i]>xa) xa=x[i];
 }
 return xa-xi;
}
//  0 16
//  1 24
//  2 32
//
void rwsk::val (const real& y, real dkf[8], const real& axf)
{plo.pwrt (axf,y,"Val: ",24,0,1);
 ostringstream ost;
 ost.setf(ios_base::fixed,ios_base::floatfield); ost.precision(1);
 for (int i=0; i<8; ++i)
 {ost.str(""); ost<<dkf[i];
  plo.pwrt (3.*(real(i)+.5),y,ost.str().c_str(),24,0,0);
 }
}
void rwsk::ska (const real& x, const real& h, const real& hi)
{int kw[]={0,5,10,20,40,70,120,200,330,500,32000};
 real yp,y=0.;
 plo.line (x,y,x+30.,y);
 plo.pwrt (x+30.,y,"<Y-300>0",16,0,-1);
 ostringstream ost;
 for (int j=1; j<=10; ++j)
 {yp=y;
  y=kw[j];
  if (y>h) break;
  plo.line (x,y,x+30.,y);
  if (y>=hi)
  {ost.str(""); ost<<"<Y-300>"<<kw[j];
   plo.pwrt (x+30.,y,ost.str().c_str(),16,0,-1);
  }
  if (y-yp>=hi)
  {ost.str(""); ost<<"<Y-300>"<<j-1;
   plo.pwrt (x+10.,(kw[j]+kw[j-1])/2.,ost.str().c_str(),16,0,1);
  }
 }
 plo.line (x+15.,0.,x+15.,yp);
}
void rwsk::pkid (const char* const lab, const real& y, const int ki[8],
           const real& axf)
{char dg[][2]={"0","1","2","3","4","5","6","7","8","9"};
 plo.pwrt (axf,y,lab,24,0,1);
 for (int i=0; i<8; ++i) plo.pwrt (3.*(real(i)+.5),y,ki[i]>=0?dg[ki[i]]:"*",
                                   24,0,0);
}
void rwsk::rys (const int n, const real* const x, const real* const y,
                const real& xs, const real& ys)
{plo.setdp ("$\""); plo.setcolor (6);
 plo.frst (); for (int i=0; i<n; ++i) plo.vect (real(i+1),x[i]-y[i]+xs);
 plo.setdp ("$"); plo.setcolor (7);
 plo.frst (); for (int i=0; i<n; ++i) plo.vect (real(i+1),x[i]+xs);
 plo.frst (); for (int i=0; i<n; ++i) plo.vect (real(i+1),y[i]+ys);
}
                      // do porownania, kb[7] ???
void rwsk::difr (const real& y, const int ka[8], const int kb[8], const real& axf)
{plo.pwrt (axf,y,"Dif: ",24,0,1);
 ostringstream ost;
 for (int i=0; i<8; ++i)
 {ost.str("");
  if (ka[i]>=0 && kb[i]>=0)
  {if (ka[i]==kb[i]) ost<<'-'; else ost<<ka[i]-kb[i];
  }else ost<<'*';
  //if (c[0]=='0') c[0]='-';   // zerowa roznice wskaznikow zastepujemy kreska -
  plo.pwrt (3.*(real(i)+.5),y,ost.str().c_str(),24,0,0);
 }
}
//#include <iostream>
//                           x - pelne skladowe
//                           y - ich czesc zaburzona
void rwsk::bor (const char* const mes, const bool pd, const real* const x[2],
                const real* const y[2], const int* const vws, const bool oxy, int kd[8])
{const int ndx=24*60,nmx=0; const int nx=ndx+nmx+nmx;
 const int np=(pd?180:0),kp=(pd?1:0);    // uwzgledniamy dzien poprzedni
 const int m=8,mw=3*60;
 real dkf[2][m],dkx[2][m]; int kdf[3][m],kdx[3][m];
 bool gr[m];
 for (int j=0; j<m; ++j)
 {for (int i=0; i<2; ++i)
  {kdx[i][j]=kid(dkx[i][j]=ext(mw,x[i]+np+j*mw));
   kdf[i][j]=kid(dkf[i][j]=ext(mw,y[i]+np+j*mw));
  }
  const int kx=kdx[2][j]=max(kdx[0][j],kdx[1][j]);
  const int kf=max(kdf[0][j],kdf[1][j]);
  gr[j]=(kx<kf);   // wskaznik z pelnej skladowj mniejszy
  //kdf[2][j]=(vws[kp+j]==0?min(max(kdf[0][j],kdf[1][j]),kdx[2][j]):-1);
  kdf[2][j]=(vws[kp+j]==0?(gr[j]?kx:kf):-1);
 }
 const real hax=.05,hl=.03,ha=.25,c1=.15;
 real xi[2],xa[2],yi[2],ya[2],dx[2],dy[2],h[2];
 const real wd=.5;
 plo.setlinewidth (wd); plo.setcolor (7);
 for (int i=0; i<2; ++i)
 {dx[i]=extr(nx,x[i]+np,xi[i],xa[i]);
  dy[i]=extr(nx,y[i]+np,yi[i],ya[i]);
  h[i]=dx[i]+dy[i];
 }
 if (h[0]<1.) h[0]=5.; if (h[1]<1.) h[1]=5.;  // gdy za male wartosci
 real hm=max(h[0],h[1]);
 real ha1=max(h[0]/hm,.05)*ha;
 real ha2=max(h[1]/hm,.05)*ha;
 real d1=c1+ha1;
 real v1=d1+hl/2.;
 real e1=v1+hl;
 real ax3=e1+hax;
 real c2=ax3+hl/2.;
 real d2=c2+ha2;
 real v2=d2+hl/2.;
 real e2=v2+hl;
 real ax1=c1-hax;
 real ax2=e2+hax;
 real dif=ax2+hax;
 real comp=dif+hl;
 real hand=comp+hl;
 int ig=nx/60;
 int igp=nmx/60;
 real axf=real(0-igp);
 real axl=real(ig-igp);
//cerr<<"axf, axl : "<<axf<<' '<<axl<<'\n';
// plo.set (0.,1.,0.,1.,0.,1.,0.,1.,1);
 plo.pwrt15 (26250,200,"The curves on this plot represent the original signals multiplied by 500/K9limit",20,0,1);
 plo.set (.1,.8,0.,1.,axf,axl,0.,1.,1);
 plo.setlinewidth (1.2);
 plo.line (0.,ax1,0.,ax2);
 plo.line (24.,ax1,24.,ax2);
 plo.setlinewidth (wd);
 plo.labmod ("%3.0f","%3.0f",24,24,0,0,0);
 plo.axs (0,ax1,axf,axl,0.,3.,1,1,-1);
 plo.labaxs (0,mes,32,-1,0);
 plo.axs (0,ax3,axf,axl,0.,3.,1,0,0);
 plo.axs (0,ax2,axf,axl,0.,3.,1,-1,1);
 val (v1,dkf[0],axf);
 pkid (oxy?"X: ":"H: ",e1,kdf[0],axf);
 val (v2,dkf[1],axf);
 pkid (oxy?"Y: ":"D: ",e2,kdf[1],axf);
 pkid ("KAsm:",comp,kdf[2],axf);
 //pkid ("Comp:",comp,kdf[2],axf);
 if (kd!=NULL)
 {pkid ("Comp:",hand,kd,axf);
// pkid ("Hand:",hand,kd,axf);
  difr (dif,kd,kdf[2],axf);
 }
 //cerr<<"c1, d1, nx, h[0] : "<<c1<<' '<<d1<<' '<<nx<<' '<<h[0]<<'\n';
 plo.set (.1,.8,c1,d1,1.,real(nx),0.,h[0],1);
 rys (nx,x[0]+np,y[0]+np,-xi[0],dx[0]-yi[0]);
 ska (real(nx+20),hm,hm*hl/ha/2.);
 //cerr<<"c2, d2, nx, h[1] : "<<c2<<' '<<d2<<' '<<nx<<' '<<h[1]<<'\n';
 plo.set (.1,.8,c2,d2,1.,real(nx),0.,h[1],1);
 rys (nx,x[1]+np,y[1]+np,-xi[1],dx[1]-yi[1]);
 ska (real(nx+20),hm,hm*hl/ha/2.);
 plo.frame ();
}
