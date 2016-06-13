#ifndef _PS_H
//--------------------------------------------------------------------
#define _PS_H
//--------------------------------------------------------------------
//#include <stdio.h>        // tu jest IBM sprintf
//#include <stdlib.h>
#include <fstream>
#include <cmath>
#include <iomanip>
//#include <string.h>
//#include <ctype.h>
using namespace std;
//--------------------------------------------------------------------
                              // powinno byc :
                              // sizeof(int2)=2*sizeof(int1)
                              // sizeof(int4)=4*sizeof(int1)
typedef char* pchar;
typedef int* pint;
typedef signed char int1;
typedef int1* pint1;
typedef unsigned char uint1;
typedef uint1* puint1;
typedef short int2;
typedef int2* pint2;
typedef int int4;
typedef int4* pint4;
typedef const int1* cpint1;
class CMYK
{public:
 CMYK (const int cmyk) : cmyk(cmyk) {}
 CMYK (const int c, const int m, const int y, const int k)
 {unsigned char* u=(unsigned char*)&cmyk;
  u[0]=c; u[1]=m; u[2]=y; u[3]=k;
 }
 operator int () const {return cmyk;}
 private:
 int cmyk;
};
//--------------------------------------------------------------------
#include "io.h"
//-----------------------------------------------------------------------------
class pstream_io
{public:
 pstream_io (const char* fna, const dafm* pafm);
 pstream_io (pstream_io& sp);
 pstream_io (pstream_io& sp, int nst);
 void ope (const char* fna, const dafm* pafm);
 void ope (const char* fna);
 void ope (pstream_io& sp);
 void ope ();
 void ope (pstream_io& sp, int nst);
 void ope (int nst);
 ~pstream_io ();

 protected:
 pstrm cstr,dstr;
 pstrm rstr,fstr;            // w rzeczywistosci pxstrm
 private:
 void init_pstream_io ();
 void cp (pstrm& pstr, pstrm ps);
 void op (pstrm& pstr, strm& s);
 void opx ();
 void clo ();
 int iso;
};
//--------------------------------------------------------------------------
typedef double real;
typedef real* preal;
typedef const real creal;
//--------------------------------------------------------------------------
class pls
{public:
 enum {lin=1, log=2};
 enum {smth=1, cycl=2, solid=4, gap=8};
 void err (int ie);
};
//--------------------------------------------------------------------------
class trans
{public:
 trans ();
 void map (creal& uc, creal& ud, creal& dc, creal& dd, int sca=pls::lin);
 int tra (creal& u);
 real tra (int d);
 int scale () const {return sc;}
 private:
 int da,db,sc;
 real ua,ub,p,q;
};
//--------------------------------------------------------------------------
class fon
{public:
 //fon () {} //{cerr<<endl<<"********** "<<afm.FontL()<<endl;}
 //fon () : afm () {std::cerr<<"fon\n";} //{cerr<<endl<<"********** "<<afm.FontL()<<endl;}
 //fon () : afm (*dafm::use()) {} //{cerr<<endl<<"********** "<<afm.FontL()<<endl;}
 int txtsiz (const char* dpc);
 void initxt (const char* dpc);
 int txtsiz ();
 static int fonsiz (int isi);
 dafm afm;
 //const dafm& afm;
 virtual ~fon () {}
 //virtual ~fon () {dafm::use (NULL);}
 protected:
 int ich,ifn,isi,imx,imy;     // numer znaku, numer zestawu znakow,
                              // wielkosc znaku, dlugosc textu
 bbox b;
 private:
 const char* c;
 real ws;
 int id,ifo,mx,my,idy,msx[10],msy[10];
 void sppprc (int& i, const char* dpc);
};
//  tu nie mozemy umiescic tworzenia skladowej statycznej, bo przy osobnym
//  kompilowaniu beda wielokrotne definicje, ponadto ta definicja musi sie
//  pojawic przed wszelkimi definicjami obiektow globalnych vects,
//  ktore uzywaja afm, wiec afm musi wczesniej byc utworzone.
//  Tworzenie obiektow globalnych odbywa sie w kolejnosci ich pojawiania sie.
//  Gdybysmy np. w ps.lib (zawierajacej ps.obj io.obj rdafm.obj) umiescili
//  taka definicje to w programie nie moglibysmy utworzyc globalnych
//  obiektow klasy vects, poniewaz biblioteka laduje sie na koncu i
//  tworzenie afm odbylo by sie za pozno.
//  Dlatego w programie uzywajacym ps.lib musimy sami zdefiniowac
//
//  const dafm fon::afm ("c:\\mprog\\src\\ps\\plo.afm");
//
//  przed globalnymi definicjami np.
//
//  vects plo1 ("plo1.plo");
//
//  W bibliotece ps.lib jest globalna definicja
//  vects plo;
//--------------------------------------------------------------------------
//dafm fon::afm("plo.afm");
//--------------------------------------------------------------------------
class axe
{public:
 axe ();
 void tick4 (int iax, int iix, int iay, int iiy);
 void labmod (const char* const fmtx, const char* const fmty, int isizx, int isizy,
                     int ixdec, int iydec, int ixor);
 void labmod (const int ixy, const char* const fmtxy, const int isizxy);
 protected:
 real sppard (creal& x);
 int sign (int a, int b);
 real sign (creal& a, creal& b);
 int msi[2], mar[2], mir[2];
 int mum[2], mdc[2], mxor;
 char mft[2][16];
 int lstr, nsc[2], ndlr[4],lxy[2][2];
 private:
 int sppnft (pchar c);
 int labx (int nxy) const;
 int laby (int nxy) const;
};
//--------------------------------------------------------------------------
class pstream_base : public pstream_io ,public axe, virtual public fon
{public:
 pstream_base (const char* fna="plo.plo");
 pstream_base (pstream_base& pb);
 pstream_base (pstream_base& pb, int nst);
 ~pstream_base ();
 void set (creal& xa, creal& xb, creal& ya, creal& yb,
           creal& xc, creal& xd, creal& yc, creal& yd, int itype);
void setx (creal& xa, creal& xb, creal& xc, creal& xd, int itype);
void sety (creal& ya, creal& yb, creal& yc, creal& yd, int itype);
 void movf (int nx, int ny);
 void movv (int nx, int ny);
 void movp (int nx, int ny, int p);
 void pwrt15 (int ix, int iy, const char* c, int isi, int ior, int ice);
 void point15 (int ix, int iy, int isi, int ipt);
// void optn (const char* op, int iv);
 int setlinewidth (const real& w);
 int getlinewidth ();
 void slinewidth (int w);
 int glinewidth ();
 int setcolor (int co);
 CMYK setcolor (CMYK co);
 void scolor (int c);
 int gcolor ();
 void frame ();
 void frame (int nst);
 int getscale (const int ixy) const {return xy[ixy].scale();}
 trans xy[2];
 void axs (int wxy, creal& yc, creal& xa, creal& xb,
                   creal& xlf, creal& dlx, int nmi, int ti, int ndc);
 void labaxs (int nxy, const char* const c, int nsi, int ndc, int nor);
 void grid (int wxy, const real& yc, const real& yd, const real& xa,
                    const real& xb, const real& xlf, const real& dlx,
                    int nmi, int nti, int ndc);
 void pwrt (const real& x, const real& y, const char* c, int isi, int ior, int ice);
 void point (const real& x, const real& y, int isi, int ipt);
 private:
 enum {maxin=31};
 int mz[2];
 void init_pstream_base ();
 void putf (int nx, int ny);
 void putv (int nx, int ny);
 void spppvl (int ix, int iy, int nxy, creal& v, int nce);
 void sppptl (int j, int ia, int ib, int it, const real& in);
 void setopt (int op1, int op2, int iva);
 int lpk,hpk;     // do psania linii
};
//--------------------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
                       // szablon i punkt
                       // nsld >=0 linia ciagla lub pusta
                       //      <0  szablon
                       // npt 0 - nie rys. punktu, 1 - rysujemy
                       // tablica ts
                       //   szablon 0:ldt-1
                       //   punkt   lpt:ltse-1
                       // tablica nds
                       //   0:lds     - opis segmentow szablonu
                       //   lts:ldse  - wskazniki do tekstu w ts
                       // lfts, lfds  - tu sie zaczyna czesc po | w szablonie
class dpat : virtual public fon, virtual public pls
{public:
 dpat ();
 void setdp (const char* c, int isi=0);
 void setpt (const char* pt, int iptsi=0);
 void setpt ();
 void del (const char* s);
 void dpopt (int optn, int val=0);
// private:
 enum {ldse=100,ltse=100};
 int nds[ldse+1],lds,lts,lfts,lfds;
 char ts[ltse];
 int ldt,lpt;
 int nsld,npt;
 int nsi,nptsi;
 real dec;
 char sc[8];  // ="{\"$_^|}";
 int nseg[2]; // ={-100,100};
 private:
 static int lrep (const char* c);
 static int index (const char* c, char d);
};
//----------------------------------------------------------------
class v_base : public dpat, public pstream_base
{public:
 v_base (const char* fna="plo.plo");
 v_base (v_base& vb);
 v_base (v_base& vb, int nst);
 void dvct (pint nxy);
 void dlst ();
 enum {ndime=3};
 int nlsta,ndim,nxyp[ndime];
 private:
 enum {ldse=100,lbe=100,ltse=100};
 int ib,nb[lbe+1][ndime],nsl[2],nbp[2][ndime],nor[2],ntp[2];
 int ip,ids,its,itm,igt,nps[2];
 real ds,dsl[2];
 int ndxy[ndime],ndsi,ndsi1,nt,is,ndl[2][ndime];
 real d,h;
 int ixy[ndime],idxy[ndime];
 static int nfaz (pint nxy);
};
//----------------------------------------------------------------
class v_smth : public v_base
{public:
 v_smth (const char* fna="plo.plo");
 v_smth (v_smth& vs);
 v_smth (v_smth& vs, int nst);
 void vct (pint ix);
 void lst ();
 void vset (int optn, const real& val=0.);
 int ncycl;
 private:
 real sdist,rl,dlt;
 void rln (int n, preal dxr);
 static void trd (int n, preal a, preal r, int nm, int l, preal x);
 static real phi (const real& x);
// parameter (ndime=3,m=100,mlp=4,me=m-2+mlp)
// integer xx(ndime),x,xf,xl
// common /ops/nsmth,nsld,ncycl,npt,sdist,rl,dlt,nlsta,ndim
// common /vct0/ip,il/vct1/hl,xf(ndime),xl(ndime),dxl(ndime)
//     x ,x(ndime,0:me),dx(0:me+1,ndime),h(0:me)/vct2/ph(0:me-1),q(0:me),
//     x r(0:me)
 enum {m=100,mlp=4,me=m-2+mlp};
 int ip,il,x[me+1][ndime],xf[ndime],xl[ndime];
 real hl,dxl[ndime],dx[ndime][me+2],h[me+1];
 real ph[me],q[me+1],r[me+1];
};
//----------------------------------------------------------------
class vects : public v_smth
{public:
 vects (const char* fna="plo.plo");
 vects (vects& ve);
 vects (vects& ve, int nst);
 void frst ();
 void frst (int optn);
 void frst (const real& x1, const real& x2, int optn=0);
 void vect (const real& x1, const real& x2);
 void vect15 (int ix1, int ix2);
 void last ();
 void crve (int n, preal x, preal y, int optn=0);
 void line (const real& xa, const real& ya, const real& xb, const real& yb);
 private:
 void rpt (pint ix);
 int nsmth;
};
//--------------------------------------------------------------------
//extern vects plo;
//--------------------------------------------------------------------
#endif
