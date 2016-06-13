#include "casm.h"
#include <cmath>
#include <algorithm>     // min, max
using namespace std;
//-----------------------------------------------------------------------------
real ext (const int n, const real* const x);
//-----------------------------------------------------------------------------
giv& giv::operator () (real& a, real& b)
{real p=c*a+s*b;
 b=c*b-s*a;
 a=p;
 return *this;
}
real giv::rot (const real& a, const real& b)
{int sa=0,sb=0;
 real aa=a,bb=b,r;
 if (aa<0.) {aa=-aa; sa=1;} if (bb<0.) {bb=-bb; sb=1;}
 if (aa>bb)
 {real q=b/a;
  r=sqrt(1.+q*q);
  c=1./r; if (sa) c=-c;
  s=q*c;
  r*=aa;
 }else if (bb>0.)
 {real q=a/b;
  r=sqrt(1.+q*q);
  s=1./r; if (sb) s=-s;
  c=q*s;
  r*=bb;
 }else {c=1.; s=0.; r=0.;}
 return r;
}
//---------------------------------------------------------------------------
slv::slv (int n) : n(n)      // n>1
{d=new real [n]; p=new real [n-1]; q=new real [n-2];
 rl=new real [n];
}
slv::~slv ()
{delete [] rl; delete [] q; delete [] p; delete [] d;
}
//
//   [ T ]     [ 0  ]
//   [   ] x - [    ]
//   [ L ]     [ Lf ]
//
// T macierz o trzech diagonalach  (n-2)*n,  i-ty wiersz :  1 -2 1
// L - diagonalna macierz wagowa  (rl)
// x - rozwiazanie zadania najmniejszych kwadratow
//
void slv::sol (preal f, preal x)
{int n2=n-2;
 real r=0.,rlc=rl[0],rln=rl[1];
 real fc=rlc*f[0],fn=rln*f[1];
 giv gv;
 for (int i=0; i<n2; ++i)
 {d[i]=gv.rot (1.,rlc);        // zerujemy rl[i]
  real s,fp;
  gv(p[i]=-2.,r)(q[i]=1.,s=0.)(x[i]=0.,fp=fc);
  fc=fn; fn=rl[i+2]*f[i+2];    // b[i]
  rlc=gv.rot (rln,r);          // zerujemy r
  gv(r=0.,s)(fc,fp);
  rln=gv.rot (rl[i+2],s);      // zerujemy s
  gv(fn,fp);
 }
// d[n2]=rlc; p[n2]=r; x[n2]=fc;
// d[n-1]=rln; x[n-1]=fn;
//--------------------------
 rx (f,x,fn/rln,fc/rlc,r/rlc);
}
void slv::sol (const real& rl, real* f[2], real* x[2])
{int n2=n-2;
 real r=0.,rlc=rl,rln=rl;
 real fc0=rlc*f[0][0],fn0=rln*f[0][1];
 real fc1=rlc*f[1][0],fn1=rln*f[1][1];
 giv gv;
 for (int i=0; i<n2; ++i)
 {d[i]=gv.rot (1.,rlc);         // zerujemy rl[i]
  real s,fp0,fp1;
  gv(p[i]=-2.,r)(q[i]=1.,s=0.)(x[0][i]=0.,fp0=fc0)(x[1][i]=0.,fp1=fc1);
  fc0=fn0; fn0=rl*f[0][i+2];          // b[i]
  fc1=fn1; fn1=rl*f[1][i+2];          // b[i]
  rlc=gv.rot (rln,r);           // zerujemy r
  gv(r=0.,s)(fc0,fp0)(fc1,fp1);
  rln=gv.rot (rl,s);            // zerujemy s
  gv(fn0,fp0)(fn1,fp1);
 }
// d[n2]=rlc; p[n2]=r; x[n2]=fc;
// d[n-1]=rln; x[n-1]=fn;
//--------------------------
 rx (f[0],x[0],fn0/rln,fc0/rlc,r/rlc);
 rx (f[1],x[1],fn1/rln,fc1/rlc,r/rlc);
}
void slv::rx (preal f, preal x, const real& xl, const real& sa, const real& sb)
{x[n-1]=xl;
 x[n-2]=sa-sb*xl;
 for (int i=n-3; i>=0; --i) x[i]=(x[i]-p[i]*x[i+1]-q[i]*x[i+2])/d[i];
//Test (n,x,f,rl);
 for (int i=0; i<n; ++i) x[i]=f[i]-x[i];     // czesc zaburzona
}
                // n - powinno byc wielokrotnoscia 60
void slv::wg (preal y, const real& dela)
{for (int i=0; i<n; i+=60)
 {real q=ext(60,y+i)/6.66-1.; if (q>50.) q=50.;
  q=dela*exp(-q/2.);
  for (int j=0; j<60; ++j) rl[i+j]=q;
 }
}
void slv::solve1 (const real& del, const real& dela, real* x[2], real* y[2])
{sol (del,x,y);                         // czesc zaburzona ze stalymi wagami
 wg (y[0],dela);                           // liczymy wagi
 sol (x[0],y[0]);                             // czesc zaburzona z obliczonymi wagami
 wg (y[1],dela);                           // liczymy wagi
 sol (x[1],y[1]);                             // czesc zaburzona z obliczonymi wagami
}
//real slv::ext (int n, preal x)
//{real xi=x[0],xa=x[0];
// for (int i=1; i<n; ++i) {if (x[i]<xi) xi=x[i]; if (x[i]>xa) xa=x[i];}
// return xa-xi;
//}
real ext (const int n, const real* const x)
{real xi=x[0],xa=x[0];
 for (int i=1; i<n; ++i) {if (x[i]<xi) xi=x[i]; if (x[i]>xa) xa=x[i];}
 return xa-xi;
}
int kid (const real& xk)
{const int lkw=11;
 static const int kw[lkw]={0,5,10,20,40,70,120,200,330,500,32000};
 int jf=0,jl=lkw-1;
                           //  kw[jf]<=xk<kw[jl]
 do
 {int i=(jf+jl)/2;
  if (xk<real(kw[i])) jl=i; else jf=i;
 }while (jf+1<jl);
 return jf;
}
int cid (const int MaxK, const int SumK)
{int c=1;
 if ((MaxK<4 && SumK<18) || (MaxK==4 && SumK<13)) c=0;
  else if (SumK>30 || MaxK>6 || (MaxK==6 && SumK>25)) c=2;
 return c;
}
                // n - powinno byc wielokrotnoscia 180
void slv::wg (preal* y, const real& dela)
{int mx=180;
 for (int i=0; i<n; i+=mx)
 {int ki=kid(max(ext(mx,y[0]+i),ext(mx,y[1]+i)));
  for (int j=0; j<mx; ++j) rl[i+j]=(ki>=3?dela/sqrt(80.):dela);
 }
}
//---------------------------------------------------------------------------
// w ciagu x sa wartosci niezdefiniowane przedzielone co najwyzej
// jedna wartoscia zdefiniowana
// na poczatku i na koncu moga byc jedna lub dwie wartosci zdefiniowane
// kazdym 180 elementom tablicy x odpowiada 1 element vws
// lgap - tyle brakujacych wartosci uniewaznia wskaznik
int fits (const int n, real* const x, int* const vws, const int lgap)
{int i,m,ir=0,is=0;
 for (i=0; i<n && x[i]>=UNDEF; ++i);
 const real xi=(i<n?x[i]:0.);     // 0. - gdy same niezdefiniowane wartosci
 for (int j=0; j<i; ++j) x[j]=xi;  // uzupelniamy poczatek
 //if (i<n) for (int j=0; j<i; ++j) x[j]=x[i];  // uzupelniamy poczatek
 for (int jl=(i-1+180)/180,j=0; j<jl; ++j) ++vws[j]; // te wskazniki sa zle
 for (m=n-1; m>i && x[m]>=UNDEF; --m);
 if (m<n-1)
 {for (int j=m+1; j<n; ++j) x[j]=x[m]; // uzupelniamy koniec
  for (int jl=(n-1+180)/180,j=(m+1)/180; j<jl; ++j) ++vws[j]; // te wskazniki sa zle
 }
 if (i>0) ir+=2; if (m<n-1) ir+=4;
                // oposcilismy na poczatku i na koncu wartosci niezdefiniowane
                // i<m to x[i], x[m] - skrajne wartosci zdefiniowane
 while (i<m)    // moga byc wartosci niezdefiniowane
 {              // x[i], x[m] - skrajne wartosci zdefiniowane
  for (int kl=0,k=i+1; k<=m; ++k)
  if (x[k]>=UNDEF) ++kl;   // liczymy dlugosc dziury
  else
  {if (kl>lgap)            // dziura od x[k-kl] do x[k-1]
   {for (int jl=(k-1+180)/180,j=(k-kl)/180; j<jl; ++j) ++vws[j]; // te wskazniki sa zle
   }
   kl=0;
  }
  int lp=i;     // poczatek
  for (++i; i<m && x[i]<UNDEF; ++i);  // szukamy pierwszej wartosci niezdefiniowanej
  if (i<m)    // x[i] - niezdefiniowane
  {if (i-lp>2) lp=i-2;     // nie wiecej jak dwa na brzegu
                           // szukamy konca
   for (i+=2; i<m && (x[i-1]>=UNDEF || x[i]>=UNDEF); ++i);
   fit (i-lp+1, x+lp); is=1;
   if (i<m) --i;  // nowy poczatek, moga byc jeszcze wartosci niezdefiniowane
  }
 }
 ir+=is;
 return ir;  // 0 - bez luk, 1 - uzupelnane luki
             // luki na : 2 - lewym brzegu, 4 - prawym brzegu
}
//---------------------------------------------------------------------------
             // m - dlugosc ciagu x
             // na poczatku i na koncu 1 lub 2 wartosci zdefiniowane
             // miedzy wartosciami niezdefiniowanymi moze byc dokladnie
             // jedna wartosc zdefiniowana
             // przynajmniej 1 wartosc niezdefiniowana w ciagu x
             // 
void fit (const int m, preal x)
{const int ml=m-(x[m-2]<UNDEF?2:1); // indeks : ostatnia niewiadoma + 1
 int n=0;
 for (int i=1; i<ml; ++i) if (x[i]>=UNDEF) ++n; // liczymy niewiadome
 preal b=new real[n+n+n-1+(n-2>0?n-2:0)];
 preal d=b+n; preal f=d+n; preal g=f+n-1;
 for (int l=n-2,i=0; i<l; ++i) g[i]=0.;
 giv gv;
 int k,j;
 real r,t;
 k=0;                     // x[0]<UNDEF
 if (x[1]>=UNDEF)         // 1 na brzegu
 {r=0.; t=0.; j=1;
 }else
 {r=1.; t=-x[0]+2.*x[1]; j=2;
 }
                // r, t, k, j
 while (j<ml)
 {b[k]=-x[j-1]; d[k]=-2.; ++j;
  if (x[j]<UNDEF) b[k]+=-x[j];     // 1 x 1
  else                             // conajmniej 2 x 2
  {f[k]=1.; ++j;
   while (x[j]>=UNDEF)             // conajmniej 3 x 3
   {d[k]=gv.rot (d[k],r); gv (f[k],r=0.)(b[k],t);
    d[k]=gv.rot (d[k],1.);
    gv (f[k],d[k+1]=-2.)(g[k]=0.,f[k+1]=1.)(b[k],b[k+1]=0.);
    ++k; ++j;
   }
   d[k]=gv.rot (d[k],r); gv (f[k],r=0.)(b[k],t);
   d[k]=gv.rot (d[k],1.);
   gv (f[k],d[k+1]=-2.)(b[k],b[k+1]=-x[j]);
   ++k;
  }
  d[k]=gv.rot (d[k],r); gv (b[k],t);
  if (j!=m-1)             // nie koniec albo koniec i 2 na prawym brzegu
  {d[k]=gv.rot (d[k],1.);
   t=2.*x[j];
   if (j==ml) t+=-x[j+1];  // koniec, 2 na prawym brzegu
   else                    // nie koniec
   {gv (f[k]=0.,r=1.); ++j;
   }
   gv (b[k],t);
  }
  ++k;
 }
 k=n-1; j=ml-1;      // j - index ostatniej niewiadomej
 real yd,yf,yg;
 yd=b[k]/d[k]; x[j]=yd;
 if (k>0)
 {--k; yf=yd; yd=(b[k]-yf*f[k])/d[k];
  --j; if (x[j]<UNDEF) --j; x[j]=yd;
  for (--k; k>=0; --k)
  {yg=yf; yf=yd; yd=(b[k]-yf*f[k]-yg*g[k])/d[k];
   --j; if (x[j]<UNDEF) --j; x[j]=yd;
  }
 }
 delete [] b;
}
//---------------------------------------------------------------------------
//const real rs::del=5e-7;
//const real rs::dela=1.e-8;    // do liczenia wag
// dla Belska
const real rs::del=5.e-7;
const real rs::dela=7.e-7;    // do liczenia wag
                     // wyznacza czesc zaburzona x i zapisuje na y
void rs::rs1 (int n, real* x[2], real* y[2], const real& scl)
{const real d=sqrt(del*scl);
 const real da=sqrt(dela*scl);    // do liczenia wag
 slv sv (n);
 sv.solve1 (d,da,x,y);
}
//---------------------------------------------------------------------------
