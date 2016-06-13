#include "ps.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;
//--------------------------------------------------------------------
//int idbg=0;
//ofstream dbg("dbg.txt");
extern int idbg;
extern ofstream dbg;
void Err () {cerr<<"\nFatal Error !"; exit (0);}
void Err (const char* const a, const char* const b, const int er=0)
{cerr<<a<<b; if (er!=0) exit (0);
}
void Err (const char* const a, const int er=0)
{cerr<<a; if (er!=0) exit (0);
}
#undef min
#undef max
inline int min (int i, int j) {return i<j?i:j;}
inline int max (int i, int j) {return i>j?i:j;}
inline float min (float i, float j) {return i<j?i:j;}
inline float max (float i, float j) {return i>j?i:j;}
inline double min (double i, double j) {return i<j?i:j;}
inline double max (double i, double j) {return i>j?i:j;}
//--------------------------------------------------------------------
                                       // nowy rysunek w nowym zbiorze
pstream_io::pstream_io (const char* fna, const dafm* pafm)
{init_pstream_io ();
 ope (fna,pafm);
}
                                       // nowy rysunek w nowym zbiorze
void pstream_io::ope (const char* fna, const dafm* pafm)
{clo ();
 if (rstr==NULL) rstr=new xstrm (fna,pafm); else rstr->ope (fna);
 opx ();
}
void pstream_io::opx ()
{if (fstr==NULL) fstr=new xstrm ((xstrm&)(*rstr)); else fstr->ope (*rstr);
 op (cstr,*fstr);
 op (dstr,*fstr);
 iso=1;
}
                                       // nowy rysunek
pstream_io::pstream_io (pstream_io& sp)
{init_pstream_io ();
 ope (sp);
}
                                      // ten sam rysunek
pstream_io::pstream_io (pstream_io& sp, int nst)
{init_pstream_io ();
 ope (sp,nst);
}
void pstream_io::init_pstream_io ()
{iso=0;
 rstr=fstr=cstr=dstr=NULL;
}
                                       // nowy rysunek, sp!=*this
                                       // w sp  powinno byc iso==1
void pstream_io::ope (pstream_io& sp)
{clo ();
 cp (rstr,sp.rstr);
 opx ();
}
                                       // nowy rysunek, powinno byc iso==1
void pstream_io::ope ()
{if (iso)
 {dstr->clo ();                // dstr - wolny
  if (fstr->gnas()==1)
  {cstr->clo (); fstr->clo (); // cstr, fstr - wolny
  }else fstr=cstr=NULL;
  opx (); 
 }
}
                                       // ten sam rysunek, sp!=*this
                                       // w sp  powinno byc iso==1
void pstream_io::ope (pstream_io& sp, int nst)
{clo ();
 cp (rstr,sp.rstr); cp (fstr,sp.fstr); cp (cstr,sp.cstr);
 op (dstr,*fstr); iso=1;
}
                                       // ten sam rysunek,  powinno byc iso==1
void pstream_io::ope (int nst)
{if (iso)
 {dstr->clo ();
  dstr->ope (*fstr);
 }
}
void pstream_io::cp (pstrm& pstr, pstrm ps)
{if (pstr!=NULL) delete pstr;
 pstr=ps;
}
void pstream_io::op (pstrm& pstr, strm& s)
{if (pstr==NULL) pstr=new strm (s); else pstr->ope (s);
}
void pstream_io::clo ()
{if (iso)
 {dstr->clo ();                // dstr - wolny
  if (fstr->gnas()==1)
  {cstr->clo (); fstr->clo (); // cstr, fstr - wolny
   if (rstr->gnas()==0) rstr->clo (); else rstr=NULL; // ew. rstr wolny
  }else rstr=fstr=cstr=NULL;
  iso=0;
 }
}
pstream_io::~pstream_io ()
{clo ();
 if (rstr!=NULL) delete rstr;
 if (fstr!=NULL) delete fstr;
 if (cstr!=NULL) delete cstr;
 if (dstr!=NULL) delete dstr;
}
//--------------------------------------------------------------------
void pls::err (int ie) {cerr<<"\n Blad nr : "<<ie;}
//--------------------------------------------------------------------------
trans::trans ()
{map (0.,1.,0.,1.);
}
void trans::map (creal& uc, creal& ud, creal& dc, creal& dd, int sca)
{if (dc<0. || dd<0. || dc>1. || dd>1.) Err ("map"," 1 : ",1);//Err ();
 da=int(32767.*dc); db=int(32767.*dd);
 int deld=db-da;
 if (deld==0) Err ("map"," 2 : ",1);//Err ();
 ua=uc; ub=ud; sc=sca;
 real ra=ua,rb=ub;
 if (sc==pls::log)
 {if (ua<=0. || ub<=0.) Err ("map - skala logarytmiczna"," 3 : ",1);//Err ();
  ra=log10(ra); rb=log10(rb);
 }
 rb-=ra;
 if (rb==0.) Err ("map"," 4 : ",1);//Err ();
 p=deld/rb;
 q=da-p*ra;
}
int trans::tra (creal& u)
{real t=u;
 if (sc==pls::log)
 {if (t<=0.) Err ("tra"," 1 : ",1);//Err ();
  t=log10(t);
 }
 t=t*p+q;
 if (t<0.) t=0.; else if (t>32767.) t=32767.;
 return int(t);
}
real trans::tra (int d)
{if (d<0) d=0; else if (d>32767) d=32767;
 real u=(d-q)/p;
 if (sc==pls::log) u=pow(10.,u);      // moze byc nadmiar (niedomiar)
 return u;
}
//--------------------------------------------------------------------------
pstream_base::pstream_base (const char* fna) : pstream_io (fna,&afm)
{init_pstream_base ();
}
pstream_base::pstream_base (pstream_base& pb) : pstream_io ((pstream_io&)pb)
{init_pstream_base ();
} 
pstream_base::pstream_base (pstream_base& pb, int nst)
                                         : pstream_io ((pstream_io&)pb, nst)
{init_pstream_base ();
}
pstream_base::~pstream_base () {frame();}
void pstream_base::init_pstream_base ()
{lpk=0;
 putf (0,0);
// optn_ini ();
 setcolor (7); setlinewidth (.25);
}
void pstream_base::putf (int nx, int ny)   // nx>=0, ny>=0
{              // konczymy poprzednia linie
               // op code 1 - linia
               // lpk+1 - tyle punktow
 if (lpk>0) dstr->rewc1 (hpk,1).rew2(hpk,lpk+1);
 lpk=0;                  // zaczynamy linie
 mz[0]=nx; mz[1]=ny;
}
void pstream_base::putv (int nx, int ny)
{if (nx!=mz[0] || ny!=mz[1])
 {++lpk;                 // liczymy punkty
  int rp=getlinewidth()/2;  // polowa grubosci linii w bitach
  if (lpk==1)            // poczatek linii
  {hpk=dstr->res (3);    // 3 bajty op code - 1, dlugosc - 2
   dstr->wri2 (mz[0]).wri2 (mz[1]);
   pxstrm(fstr)->addb (mz[0],mz[1],rp);
  }
  dstr->wri2 (nx).wri2 (ny);
  mz[0]=nx; mz[1]=ny;
  pxstrm(fstr)->addb (mz[0],mz[1],rp);
 }
}
void pstream_base::set (creal& xa, creal& xb, creal& ya, creal& yb,
                        creal& xc, creal& xd, creal& yc, creal& yd, int itype)
{xy[0].map(xc,xd,xa,xb,(itype-1)%2==0?pls::lin:pls::log);
 xy[1].map(yc,yd,ya,yb,(itype-1)/2==0?pls::lin:pls::log);
}
void pstream_base::setx (creal& xa, creal& xb, creal& xc, creal& xd, int itype)
{xy[0].map(xc,xd,xa,xb,itype);
}
void pstream_base::sety (creal& ya, creal& yb, creal& yc, creal& yd, int itype)
{xy[1].map(yc,yd,ya,yb,itype);
}
void pstream_base::frame ()
{
if (idbg) dbg<<endl<<"Frame"<<endl;
 putf (0,0);                // ew. konczymy linie
 ope ();
 init_pstream_base ();
}
void pstream_base::frame (int nst)
{
 putf (0,0);                // ew. konczymy linie
 ope (nst);
 init_pstream_base ();
}
void pstream_base::movf (int nx, int ny)
{putf (nx,ny);
}
void pstream_base::movv (int nx, int ny)
{putv (nx,ny);
}
void pstream_base::movp (int nx, int ny, int p)
{if (p>0) movv (nx,ny); else movf (nx,ny);
}
void pstream_base::point (const real& x, const real& y, int isi, int ipt)
{point15 (xy[0].tra(x),xy[1].tra(y),isi,ipt);
}
void pstream_base::point15 (int ix, int iy, int isi, int ipt)
{movf (ix-isi,iy-isi); movv (ix+isi,iy+isi);
 movf (ix-isi,iy+isi); movv (ix+isi,iy-isi);
}
void pstream_base::pwrt (const real& x, const real& y, const char* c,
                         int isi, int ior, int ice)
{pwrt15 (xy[0].tra(x),xy[1].tra(y),c,isi,ior,ice);
}
void pstream_base::pwrt15 (int ix, int iy, const char* c,
                           int isi, int ior, int ice)
{const real pi=3.14159265,rd=180./pi;
 const real sc=.001;
 real ror=real(ior%360)/rd,s=fonsiz(isi)*sc,x=real(ix),y=real(iy);
 real si=sin(ror),co=cos(ror);
 if (ice>=0)
 {real w=txtsiz(c)*s;
  if (ice==0) w/=2.;
  x-=w*co; y-=w*si;
 }
 {const int no_color=3;
  cstr->wopt (dstr->ropt (no_color),5);
 }
                  // (x,y) poczatek pisania po uwzglednieniu centrowania
 cstr->wopt ((ior%360)*100,4);
 initxt (c);
 if (idbg) dbg<<endl<<c;
 while (txtsiz())
 {real wx=fon::imx*s,wy=fon::imy*s;  // (x+wx*co-wy*si,y+wx*si+wy*co)
  if (cstr->wopt (fon::ifn,2))
  {pxstrm(fstr)->mrk (fon::ifn);
   pxstrm(rstr)->mrk (fon::ifn);
  }
  real siz=fon::isi*s;
  cstr->wopt (int(siz),3);
  cstr->wri1 (1).wri2 (fon::ich);
  real xc=x+wx*co-wy*si+.5,yc=y+wx*si+wy*co+.5;
  cstr->wri4 (int(xc)).wri4 (int(yc));
  if (fon::b.gisb())      // istniejacy znak, 
  {if ((ior%360)!=0) fon::b.rot (si,co);
   pxstrm(fstr)->addb (fon::b,siz*sc,xc,yc); //xc,yc - w bitach, b*siz*sc - tez w bitach

//-----------
/*
   int llx,lly,urx,ury;
   fon::b.getb (llx,lly,urx,ury);
   llx=int(xc+llx*siz*sc); lly=int(yc+lly*siz*sc);
   urx=int(xc+urx*siz*sc); ury=int(yc+ury*siz*sc);
   putf (llx,lly); putv (urx,lly); putv (urx,ury); putv (llx,ury); putv (llx,lly);
   putf (0,0);
*/
//-----------

  }

if (idbg) dbg<<endl<<char(fon::ich)<<setw(5)<<fon::ich<<setw(5)<<fon::ifn
            <<setw(5)<<fon::isi<<setw(5)<<fon::imx<<setw(5)<<fon::imy
            <<setw(6)<<int4(x+wx*co-wy*si+.5)<<setw(6)<<int4(y+wx*si+wy*co+.5);
 }
} 
void pstream_base::setopt (int op1, int op2, int iva)
{if (dstr->copt::wopt(iva,op1))
 {putf (mz[0],mz[1]);        // wymuszamy koniec linii
  dstr->wri1 (op2).wri4 (iva);
 }
}
                                 // grubosc linii w w punktach
int pstream_base::setlinewidth (const real& w)
{const int op_linewidth=2,no_linewidth=2;
 int iw=dstr->ropt (no_linewidth);
 setopt (no_linewidth,op_linewidth,int(w*100.*20.68686868+.5));
 return iw;
}
                                 // grubosc linii w bitach
int pstream_base::getlinewidth ()
{const int no_linewidth=2;
 return dstr->ropt (no_linewidth)/100;
}
void pstream_base::slinewidth (int w)
{const int op_linewidth=2,no_linewidth=2;
 setopt (no_linewidth,op_linewidth,w);
}
int pstream_base::glinewidth ()
{const int no_linewidth=2;
 return dstr->ropt (no_linewidth);
}
int pstream_base::setcolor (int co)
{int4 i4=0;
 const int op_color=3,no_color=3;
 puint1 u=puint1(&i4);
 if ((co&7)==7) u[3]=255;
 else
 {if ((co&1)==1) u[0]=255;
  if ((co&2)==2) u[1]=255;
  if ((co&4)==4) u[2]=255;
 }
 int c=dstr->ropt (no_color);
 setopt (no_color,op_color,int(i4));
if (idbg) dbg<<" setcolor";
 return c;
}
         // CMYK : 4 - bajty
CMYK pstream_base::setcolor (CMYK co)
{const int op_color=3,no_color=3;
 int c=dstr->ropt (no_color);
 setopt (no_color,op_color,int(co));
if (idbg) dbg<<" setcolor";
 return CMYK(c);
}
void pstream_base::scolor (int c)
{const int op_color=3,no_color=3;
 setopt (no_color,op_color,c);
}
int pstream_base::gcolor ()
{const int no_color=3;
 return dstr->ropt (no_color);
}
//--------------------------------------------------------------------------
/*
void option::optn_ini ()
{for (int i=0; i<37; ++i) mvop[i]=msop[i];
}
*/
//--------------------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------
dpat::dpat () : nsld(1), npt(0)
{dpopt (pls::solid|pls::gap);
 del ("{\"$_^|}");
}
void dpat::del (const char* s)
{int i,j,ok,l=sizeof(sc)-1;
 if (int(strlen(s))==l)
 {for (ok=1, i=0; i<l; ++i) for (j=i+1; j<l; ++j) if (s[i]==s[j]) ok=0;
  if (ok) for (i=0; i<l; ++i) sc[i]=s[i];
 }
}
void dpat::dpopt (int optn, int val)
{if (val<=0) val=100;               // wartosc standardowa
  if (optn&pls::solid) nseg[1]=val;
  if (optn&pls::gap) nseg[0]=-val;
}
void dpat::setpt (const char* pt, int iptsi)
{int l=int(strlen(pt))+1;     // tyle miejsca na punkt z 0
 nptsi=iptsi;            // wielkosc punktu
 npt=1;                  // rysujemy punkt w wezlach
 if (nsld>=0) ldt=0;     // nie ma szablonu
 lpt=ltse-l;
 if (lpt<ldt) err (10);  // brakuje miejsca na punkt w tablicy ts
 for (int i=0; i<l; ++i) ts[lpt+i]=pt[i];
}
void dpat::setpt ()
{npt=0;
 lpt=ltse;
}
void dpat::setdp (const char* c, int isi)
{int ic,ifc=0,it=0,ise=-1,jsi=fonsiz(isi);
 nsi=isi;
 dec=.75*jsi;
 if (npt==0) lpt=ltse;   // nie ma punktow czla tablica ts na szablon
 ldt=0;
 lfds=0;
 lfts=ldse;
 lds=-1;
 lts=ldse;
 nds[lts]=0;
 while (c[ifc]!=0)
 {ic=index(sc,c[ifc]);
  if (ic>=2)             //   " $ _ ^ |
  {if (ic<4)             //   " $
   {it=0;
    int ir=lrep(c+ifc);
    ifc+=ir;
    ise=ic-2;            //   " - 0, $ - 1
    ++lds;
    if (lds>=lts) err (1);
    nds[lds]=nseg[ise]*ir;
   }else
   {if (ic<6) it=ic+ic-9;//   _ - -1, ^ - 1
    else                 //   |
    {lfds=lds+1;
     lfts=lts;
     ise=-1;
    }
    ++ifc;
   }
  }else                  // { pozostale
  {if (ic==1)            // {
   {int ie1=0,ie2;
    for (++ifc; c[ifc]!=0; ++ifc)
    {ie2=0;
     if (c[ifc]==sc[6]) ie2=1;   // sc[6]=='}'
     if (ie1==ie2)
     {if (ldt>=lpt) err (2);
      ts[ldt++]=c[ifc];
     }
     if (ie1>ie2) break;
     ie1=ie2;
    }
   }else                  // pozostale
   {for (; c[ifc]!=0; ++ifc)
    {if (index(sc+1,c[ifc])>0) break;
     if (ldt>=lpt) err (2);
     ts[ldt++]=c[ifc];
    }
   }
   if (ldt>=lpt) err (2);
   ts[ldt++]=0;
   if (nds[lts]!=ldt)
   {//int ii=int(txtsiz(ts+nds[lts])/21.*jsi);
    int ii=int(txtsiz(ts+nds[lts])*.001*jsi);
    if (it<=0) ii=-ii;    // tekst po linia lub w linii
    --lts;
    if (lts<=lds) err (4);
    nds[lts]=ldt;         // poczatek nastepnego tekstu
    if (it==0)            // tekst po " & ( w linii )
    {if (lds+4>=lts) err (5);
     nds[lds+1]=nds[lds+2]=nds[lds+4]=0;
     nds[lds+3]=ii;
     lds+=4;
    }else                 // tekst po _ ^ ( pod lub nad linia )
    {if (lds+2>=lts) err (6);
     nds[lds+1]=0;
     nds[lds+2]=ii;
     lds+=2;
    }
   }
   it=0;                  // jesli nie bylo _ lub ^ to tekst w linii
  }                       // endif { pozostale
 }                        // endif po znakach c
                          // lds==0 gdy same $ lub "
                          //        wtedy nsld==0 dla " i nsld==1 dla $
                          //    >0  bardziej zlozony szablon
                          //        wtedy nsld==-1
 nsld=1;
 if (lds>=0)
 {if (ise<0) err (7);     // ostatnie | w szablonie
  if (lds>0) ise=-1;
  nsld=ise;
 }
}
int dpat::index (const char* c, char d)
{int i;
 for (i=0; c[i]!=0 && c[i]!=d; ++i);
 return c[i]==0?0:i+1;
}
int dpat::lrep (const char* c)
{int i;
 for (i=1; c[i]!=0 && c[i]==c[0]; ++i);
 return i;   // tyle jednakowych
}
//----------------------------------------------------------------
v_base::v_base (const char* fna)
                                : dpat(), pstream_base (fna), nlsta(0), ndim(2)
{
}
v_base::v_base (v_base& vb)
                  : dpat(), pstream_base ((pstream_base&)vb), nlsta(0), ndim(2)
{
}
v_base::v_base (v_base& vb, int nst)
             : dpat(), pstream_base ((pstream_base&)vb, nst), nlsta(0), ndim(2)
{
}
void v_base::dvct (pint nxy)
{
int i;
//int ndxy[ndime],ndsi,ndsi1,nt,is,ndl[2][ndime];
//real d,h;
//int ixy[ndime],idxy[ndime];
 if (nlsta<=0)
 {if (nsld>=0)
  {movf (nxy[0],nxy[1]);
   ip=1;
  }else
  {ip=0;
   ids=-1;
   its=ldse+1;
   ds=0.;
   itm=0;
   igt=0;
   nps[0]=0;
   nps[1]=0;
   dsl[0]=0.;
   dsl[1]=0.;
  }
  nlsta=1;
 }else
 {if (nsld<0)
  {for (d=0., i=0; i<ndim; ++i)
   {ndxy[i]=nxy[i]-nxyp[i];
    d+=real(ndxy[i])*real(ndxy[i]);
   }
   d=sqrt(d);
   while (d>ds)
   {for (i=0; i<ndim; ++i) ixy[i]=nxyp[i]+int(ds/d*ndxy[i]);
    movp (ixy[0],ixy[1],ip);
    ++ids;
    if (ids>lds) ids=lfds;
    ndsi=nds[ids];
    while (ndsi==0)
    {if (itm==0)
     {--its;
      if (its<=lts) its=lfts;
      ndsi1=nds[ids+1];
      if (ndsi1==0)
      {if (igt==0)
       {nt=its;
        itm=1;
        ib=0;
        for (i=0; i<ndim; ++i) nb[0][i]=ixy[i];
        ++ids;
//       }else ids+3;
       }else ids=3;       //  ???? tak bylo : }else ids+3;
      }else
      {is=0;
       h=dec/d;
       if (ndsi1<0) {ndsi1=-ndsi1; is=1;  h=-h;}
       if (nps[is]!=0)
       {pwrt15 (nbp[is][0],nbp[is][1],ts+nds[ntp[is]],nsi,nor[is],0);
        movf (ixy[0],ixy[1]);
       }
       ntp[is]=its;
       nps[is]=1;
       ndl[is][0]=-int(ndxy[1]*h);
       ndl[is][1]= int(ndxy[0]*h);
       for (i=0; i<ndim; ++i) nbp[is][i]=ixy[i]+ndl[is][i];
       nor[is]=nfaz(ndxy);
       nsl[is]=ndsi1/2;
       dsl[is]=-ds;
       ++ids;
      }
     }else
     {itm=0;
      for (i=0; i<ndim; ++i) idxy[i]=ixy[i]-nb[0][i];
      pwrt15 (nb[0][0]+idxy[0]/2,nb[0][1]+idxy[1]/2,
                  ts+nds[nt],nsi,nfaz(idxy),0);
      movf (ixy[0],ixy[1]);
     }
     ++ids;
     if (ids>lds) ids=lfds;
     ndsi=nds[ids];
    }
    ip=1;
    if (ndsi<0) {ip=0;  ndsi=-ndsi;}
    ds+=ndsi;
   }
   ds-=d;
   dsl[0]+=d;
   dsl[1]+=d;
   if (itm!=0)
   {if (ib<lbe) ++ib;
    for (i=0; i<ndim; ++i) nb[ib][i]=nxy[i];
   }
  }
  movp (nxy[0],nxy[1],ip);
 }
 for (i=0; i<ndim; ++i) nxyp[i]=nxy[i];
}
void v_base::dlst ()
{int i;
 if (nsld<0)
 {if (itm!=0)
  {igt=1;
   for (i=0; i<ndim; ++i) nxyp[i]=nb[0][i];
   ++ids;
   ds=0.;
   ip=0;
   itm=0;
   for (i=0; i<=ib; ++i) dvct (&nb[i][0]);
  }
  for (int is=0; is<2; ++is) if (nps[is]!=0 && nsl[is]<=dsl[is])
   pwrt15 (nbp[is][0],nbp[is][1],ts+nds[ntp[is]],nsi,nor[is],0);
 }
}
int v_base::nfaz (pint nxy)
{const real pi=3.14159265,w=180./pi;
 real fa;
 if (nxy[1]!=0)
 {fa=90.-atan(real(nxy[0])/real(nxy[1]))*w;
  if (nxy[1]<0) fa+=180;
 }else
 {fa=0.;
  if (nxy[0]<0) fa=180.;
 }
 return int(fa);
}
//----------------------------------------------------------------
v_smth::v_smth (const char* fna) : v_base(fna), ncycl(0), sdist(2.5), dlt(10.)
{vset (pls::smth);  // w v_base() nlsta=0
}
v_smth::v_smth (v_smth& vs)
                         : v_base((v_base&)vs), ncycl(0), sdist(2.5), dlt(10.)
{vset (pls::smth);  // w v_base() nlsta=0
}
v_smth::v_smth (v_smth& vs, int nst)
                    : v_base((v_base&)vs, nst), ncycl(0), sdist(2.5), dlt(10.)
{vset (pls::smth);  // w v_base() nlsta=0
}
void v_smth::vset (int optn, const real& val)
{if (nlsta==0)
 {if (optn&pls::smth) rl=val>0.?val:1.e-4;
 }
}
void v_smth::vct (pint xx)
{int i,j;
 real hi,d,dxr[ndime];
/*
 common /vct0/ip,il/vct1/hl,xf(ndime),xl(ndime),dxl(ndime)
     x ,x(ndime,0:me),dx(0:me+1,ndime),h(0:me)
*/
 if (nlsta==0)
 {nlsta=-1;
  il=0;
  for (j=0; j<ndim; ++j) x[0][j]=xx[j];
  ip=ncycl==0;
 }else
 {for (hi=0., j=0; j<ndim; ++j)
  {dx[j][il+1]=d=xx[j]-x[il][j];
   hi+=d*d;
  }
  hi=sqrt(hi);
  if (hi>sdist)
  {h[il]=hi;
   for (j=0; j<ndim; ++j) dx[j][il+1]/=hi;
   if (il==me)
   {for (j=0; j<ndim; ++j) dxr[j]=(dx[j][m]*h[m]+dx[j][m+1]*h[m-1])
                                 /(h[m-1]+h[m]);
    rln (m,dxr);
    for (j=0; j<ndim; ++j) dx[j][0]=dxr[j];
    for (i=m; i<=me; ++i)
    {h[i-m]=h[i];
     for (j=0; j<ndim; ++j) {x[i-m][j]=x[i][j]; dx[j][i+1-m]=dx[j][i+1];}
    }
    il=me-m;
   }
   if (ip==0)
   {hl=h[0];
    for (j=0; j<ndim; ++j)
    {xf[j]=xx[j];
     xl[j]=x[0][j];
     dxl[j]=dx[j][1];
    }
    --il;
   }
   ++il; ++ip;
   for (j=0; j<ndim; ++j) x[il][j]=xx[j];
  }
 }
}
void v_smth::rln (int n, preal dxr)
{int i,j,xx[ndime];
 real hi,hp,c,c1,c2,w,d;
 for (i=0; i<n; ++i)
 {hi=h[i];
  hp=hi*rl;
  if (hp>.5)
  {ph[i]=-exp(-hp);
   c=ph[i]*ph[i];
   c1=1.-c;
   c2=c1/hp;
//  h[i] powinno byc dodatnie
//    c1=c1*hp/hi
   c1=c1*rl;
   q[i]=(1.-c2+c)/c1;
   r[i+1]=(c2+2.*ph[i])/c1;
  }else
  {c=hp*hp;
   ph[i]=phi(c);
   w=hi/(1.+c*ph[i]);
   c*=.25;
   c2=1.+c*phi(c);
   q[i]=(.5*c2*c2-ph[i])*w;
   r[i+1]=ph[i]*w;
  }
 }
 for (j=0; j<ndim; ++j)
 {
  if (ip<=me+1)
  {if (ncycl==0) d=dx[j][1]-(dx[j][2]-dx[j][1])*h[0]/(h[0]+h[1]);
   else
   {d=(dxl[j]*h[0]+dx[j][1]*hl)/(hl+h[0]);
    dxl[j]=d;
   }
   dx[j][0]=d;
  }
  for (i=0; i<n; ++i)  dx[j][i]=dx[j][i+1]-dx[j][i];
  dx[j][n]=dxr[j]-dx[j][n];
 }
 for (q[n]=0, i=n; i>0; --i) q[i]=q[i]+q[i-1];
 trd (n+1,q,r+1,me+2,ndim,dx[0]);
 if (ip<=me+1) dvct (x[0]);
 real dt=0.,phii,t,t1,e,e1;
 for (i=0; i<n; ++i)
 {hi=h[i];
  hp=rl*hi;
  phii=ph[i];
  while (dt<hi)
  {t=dt/hi;
   t1=1.-t;
//    if (hp>.5) then
// 
//  Uwaga !, ph(i) jest czym innym w zaleznosci od warunku hp>.5 
//           warunek (hp>.5) dla hp~.5 jest spelniony w petli 10
//           a nie jest w petli 80. Prawdopodobnie raz hp jest brane
//           z rejestru (wieksza dokladnosc) a raz z pamieci (mniejsza
//           dokladnosc). Dlatego w petli 10 jesli hp>.5 zmienilem
//           znak ph(i) na ujemny.
// 
   if (phii<=0.)
   {e=exp(-t*hp);
    e1=exp(-t1*hp);
    c=1.-phii*phii;
    for (j=0; j<ndim; ++j)
    xx[j]=int(t*x[i+1][j]+t1*x[i][j]+(dx[j][i+1]*(e1*(1.-e*e)/c-t)+
              dx[j][i]*(e*(1.-e1*e1)/c-t1))/(rl*rl));
   }else
   {e=t*hp;
    e1=t1*hp;
    c=hi*hi/(1.+phii*hp*hp);
    for (j=0; j<ndim; ++j)
    xx[j]=int(t*(x[i+1][j]+dx[j][i+1]*c*(t*t*phi(e*e)-phii))+
              t1*(x[i][j]+dx[j][i]*c*(t1*t1*phi(e1*e1)-phii)));
   }
   dvct (xx);
   dt+=dlt;
  }
  dvct (x[i+1]);
  dt-=hi;
 }
}
//void v_smth::lst ()
//{dlst();
//}
void v_smth::lst ()
{
// subroutine lst
// parameter (ndime=3,m=100,mlp=4,me=m-2+mlp)
// integer x,xf,xl
// common /ops/nsmth,nsld,ncycl,npt,sdist,rl,dlt,nlsta,ndim
// common /vct0/ip,il/vct1/hl,xf(ndime),xl(ndime),dxl(ndime)
//     x ,x(ndime,0:me),dx(0:me+1,ndime),h(0:me)
 if (ip>1)
 {if (ncycl!=0) {vct (xl); vct (xf);}
  if (il<mlp-1) for (int i=0; i<=il; ++i) dvct (x[i]);
  else
  {if (ncycl==0)
   for (int j=0; j<ndim; ++j)
   dxl[j]=dx[j][il]+(dx[j][il]-dx[j][il-1])*h[il-1]/(h[il-2]+h[il-1]);
   rln (il,dxl);
  }
  dlst ();
 }
}
void v_smth::trd (int n, preal a, preal r, int nm, int l, preal x)
{int i,j;
 real u; preal xi;
 for (i=1; i<n; ++i)
 {u=r[i-1]/a[i-1];
  a[i]-=u*r[i-1];
  for (xi=x+i, j=0; j<l; ++j, xi+=nm) *xi-=*(xi-1)*u;
 }
 for (xi=x+(n-1), j=0; j<l; ++j, xi+=nm) *xi/=a[n-1];
 for (i=n-2; i>=0; --i)
 for (xi=x+i, j=0; j<l; ++j, xi+=nm) *xi=(*xi-*(xi+1)*r[i])/a[i];
}
real v_smth::phi (const real& x)
{return ((.27713991169e-5*x+.19840927713e-3)*x+.83333336379e-2)*x+.16666666666;
}
//----------------------------------------------------------------
vects::vects (const char* fna) : v_smth(fna), nsmth(0)
{
}
vects::vects (vects& ve) : v_smth((v_smth&)ve), nsmth(0)
{
}
vects::vects (vects& ve, int nst) : v_smth((v_smth&)ve, nst), nsmth(0)
{
}
void vects::frst () {last();}
void vects::frst (int optn)
{last ();
 nsmth=(optn&pls::smth)==pls::smth;
 ncycl=(optn&pls::cycl)==pls::cycl;
}
void vects::frst (const real& x1, const real& x2, int optn)
{frst (optn);
 vect (x1,x2);
}
void vects::vect (const real& x1, const real& x2)
{vect15 (xy[0].tra(x1),xy[1].tra(x2));
}
void vects::vect15 (int ix1, int ix2)
{int ix[2]={ix1,ix2};
 if (nsmth==0) dvct (ix); else vct (ix);
 if (npt!=0) rpt (ix);
}
void vects::last ()
{if (nlsta!=0) {if (nsmth==0) dlst (); else lst ();  nlsta=0; nsmth=ncycl=0;}
}
void vects::rpt (pint ix)
{pwrt15 (ix[0],ix[1],ts+lpt,nptsi,0,0);
 movf (nxyp[0],nxyp[1]);
}
void vects::crve (int n, preal x, preal y, int optn)
{frst (optn);
 for (int i=0; i<n; ++i) vect (x[i],y[i]);
 last ();
}
void vects::line (const real& xa, const real& ya, const real& xb, const real& yb)
{frst (xa,ya); vect (xb,yb); last ();
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//
//        16384 bitow - 11 cali - 792 punkow  (wysokosc strony A4)
//        1 pkt = 20.686868 bitow = 2048/99 bitow
//
//        isi - w punktach
int fon::fonsiz (int isi)
{static const int mss[]={103, 145, 207, 248};  //5, 7, 10, 12 pkt
 int rv=abs(isi);
 return rv>=0 && rv<=3 ? mss[rv] : int(20.68686868*isi);   // w bitach
}
int fon::txtsiz (const char* dpc)
{ws=1.; ifo=mx=my=idy=0;
 int i=-1;
 while (dpc[++i]!=0)
  if (dpc[i]=='<' && dpc[i+1]!='<') sppprc(i,dpc);
  else
  {if (dpc[i]=='<') ++i;         // dwa '<' zastepujemy jednym
   mx+=int(afm.CharW(dpc[i],ifo)*ws);
  }
//cerr<<endl<<"txtsiz "<<setw(6)<<mx<<" "<<dpc;
 return mx;
}
void fon::initxt (const char* dpc)
{c=dpc; id=0;
 ws=1.; ifo=mx=my=idy=0;
}
int fon::txtsiz ()
{while (c[id]=='<' && c[id+1]!='<') {sppprc(id,c); ++id;}
 imx=mx; imy=my;
 if (c[id]!=0)
 {if (c[id]=='<') ++id;         // dwa '<' zastepujemy jednym
  mx+=int(afm.CharW(c[id],ifo)*ws);
  ich=c[id]; ifn=ifo; isi=int(ws*1000.);
  afm.BBox (ich,ifn,b);
  ++id;
 }else ich=-1;
 return ich>=0;
}
void fon::sppprc (int& i, const char* dpc)
{static int nd[] = { 1,1,0,1,1,1,1,0,0,0,0 };
                  // F P R S W X Y A B D 
 static char op[]="FPRSWXYABD";
 int j,il,is,l=sizeof(op)-1;       // dpc[i]=='<'
 while (dpc[++i]!='>' && dpc[i]!=0)
 {for (j=0; j<l; ++j) if (dpc[i]==op[j]) break;
  il=0;
  if (nd[j]!=0)
  {is=dpc[++i]=='-';
   if (dpc[i]=='+' || dpc[i]=='-') ++i;
   while (dpc[i]!=0 && isdigit(dpc[i]) ) il=il*10+(dpc[i++]-'0');
   if (is) il=-il;
   --i;
  }
  switch (j)
  {case 0 :   // F
    ifo=il;
    break;
   case 1 :   // P
    is=max(0,min(9,il));
    msx[is]=mx;
    msy[is]=my;
    break;
   case 2 :   // R
    ifo=0;
    break;
   case 3 :   // S
    ws=abs(il)/100.;
    break;
   case 4 :   // W
    is=max(0,min(9,il));
    mx=msx[is];
    my=msy[is];
    break;
   case 5 :   // X
    mx+=il;
    break;
   case 6 :   // Y
    my+=il;
    break;
   case 7 :   // A
    ws=.6;
    idy=30;
    my+=idy;
    break;
   case 8 :   // B
    ws=1.;
    my-=idy;
    idy=0;
    break;
   case 9 :   // D
    ws=.6;
    idy=-15;
    my+=idy;
    break;
   default:   // nie znaleziony, blad
    break;
  }  
 }
 if (dpc[i]==0) --i;
}
//----------------------------------------------------------------------
axe::axe ()
{tick4 (12,8,12,8);
 labmod ("%10.2f","%10.2f",30,30,0,0,0);
}
void axe::labmod (const char* const fmtx, const char* const fmty, int isizx, int isizy,
                     int ixdec, int iydec, int ixor)
{static const char* const cn[2] = {"NUMX","NUMY"};
 int j;
 mxor = ixor;
 msi[0] = isizx;
 msi[1] = isizy;
 mdc[0] = ixdec;
 mdc[1] = iydec;
    // spr. dlugosci
 strcpy (mft[0],fmtx);
 strcpy (mft[1],fmty);
 for (int i=0; i<2; ++i)
 {mum[i] = sppnft(mft[i]);
  if (mum[i] > 20) Err ("LABMOD",cn[i],-8);//spperr (-8, "LABMOD", cn[i]);
  j=mdc[i];
  mdc[i]= j==0 ? 655 : j==1 ? 0 : j*32;//msc[msf[i]];
 }
} 
void axe::labmod (const int ixy, const char* const fmtxy, const int isizxy)
{static const char* const cn[2] = {"NUMX","NUMY"};
 int j;
 mxor = 0;
 msi[ixy] = isizxy;
 mdc[ixy] = 0;
    // spr. dlugosci
 strcpy (mft[ixy],fmtxy);
 {mum[ixy] = sppnft(mft[ixy]);
  if (mum[ixy] > 20) Err ("LABMOD",cn[ixy],-8);//spperr (-8, "LABMOD", cn[i]);
  j=mdc[ixy];
  mdc[ixy]= j==0 ? 655 : j==1 ? 0 : j*32;//msc[msf[i]];
 }
} 
int axe::sppnft (pchar c)
{int v=0,i=0;
 while (c[i]>='0' && c[i]<='9') v=v*10+(c[i++]-'0');
 return v;
}
void axe::tick4 (int iax, int iix, int iay, int iiy)
{int ii=32;//msc[msf[0]];
 mar[0]=iay*ii;
 mir[0]=iiy*ii;
 ii=32;//msc[msf[1]];
 mar[1]=iax*ii;
 mir[1]=iix*ii;
} 
int axe::labx (int nxy) const
{return lxy[nxy][0];
}
int axe::laby (int nxy) const
{return lxy[nxy][1];
}
real axe::sppard (creal& x)
{return x-floor(x);
} 
int axe::sign (int a, int b)
{if (a<0) a=-a;
 return b>=0 ? a : -a;
}
real axe::sign (creal& a, creal& b)
{real x=a<0. ? -a : a;
 return b>=0. ? x : -x;
}
void pstream_base::axs (int wxy, creal& yc, creal& xa, creal& xb,
                   creal& xlf, creal& dlx, int nmi, int ti, int ndc)
{static const real sc[]={1.3,.4}; real ws[]={.5,0.};
 static const real in[]={.75,1.2};
 static const int black=7;
 real p,q,w,dlxl,xlfl,ta,dx,xf,qk,pk,xk,tb,eps,xal,xbl,dxl,xfl;
 int ka,kb,kc,jf,id,i,ii,j,k,n,il,idc,ice,kdc,ntk[2],
     ntl[2],ntr[2],ngr,nti,idt,iss,nxy;
 int ino=glinewidth(),coo=setcolor (black);
 nxy=wxy%2;               // 0 - os X, 1 - os Y
 ngr=wxy/2;
 nti=ti;
 if (nxy!=0) lstr=0;
 ka=xy[nxy].tra(xa);
 kb=xy[nxy].tra(xb);
 kc=xy[1-nxy].tra(yc);
 idc=ndc;
 if (ngr==0)
 {ntk[0]=abs(mir[nxy]);
  ntk[1]=abs(mar[nxy]);
  if (nti!=0) nti=sign(1,nti);
  for (i=0; i<=1; ++i)
  {j=ntk[i]/2;
   ntl[i]=(nti-1)*j+kc;
   ntr[i]=(nti+1)*j+kc;
  }
  idt=(nti+sign(1,idc))*ntk[1]/2;
 }else
 {for (i=0; i<=1; ++i)
  {ntl[i]=kc;
   ntr[i]=nti;
  }
  idt=nti-kc;
  if (idt!=sign(idt,idc)) idt=0;
  sppptl (nti,ka,kb,nxy,in[1]);
 }
 nsc[nxy]=kb/2+ka/2;
 ice=-sign (nxy,idc);            // 0 - os X, -idc - os Y,  idc==ndc
 iss=fonsiz (msi[nxy]);
 if (idc==-1 || idc==1) idc=sign(int(iss*sc[nxy]),idc);
 kdc=kc+idc+idt;
//-------------
 int h0=int(.766*fonsiz(msi[nxy]));   // wysokosc '0' w bitach
 int ep=h0/6;                         // dodatkowy odstep
 kdc=(ndc<0?ntl[1]-ep-(nxy==0?int(1.25*h0):0):ntr[1]+ep);
 if (nxy==0 && ti>0 && ndc<0) kdc-=ep; // tylko gdy liczba z wykladnikiem
 if (nxy==1) kdc-=h0/2;   // Prezes
 int dk=(nxy==0?0:-h0/2);
//-------------
 n=max(1,nmi);
 sppptl (kc,ka,kb,nxy,in[1]);
 if (xy[nxy].scale()==pls::lin)
 {dx=sign(dlx,xb-xa);
  ta=sppard((xa-xlf)/dx);
  xf=xa-ta*dx;
  p=n*ta;
  q=n*(xb-xa)/dx;
  eps=q*1e-5;
  qk=(kb-ka)/q;
  pk=ka-p*qk;
  ii=int(p+q+eps);
  for (i=int(p-eps+1.); i<=ii; ++i)
  {xk=pk+i*qk;
   k=int(xk);
   j=0;
   if (i%n == 0)
   {j=1;                                // os X (k,kdc), 0, os Y (kdc,k), -ndc
    if (idc!=0) spppvl (k+dk,kdc,nxy,xf+i/n*dx,ice);
   }
   sppptl (k,ntl[j],ntr[j],1-nxy,in[j]);
  }
 }else
 {xal=log(xa);
  xbl=log(xb);
  xlfl=log(xlf);
  dlxl=log(dlx);
  dxl=sign(dlxl,xbl-xal);
  w=(dlx-1.)/n;
  if (dxl*dlxl<0.) w=-w/dlx;
  ta=sppard((xal-xlfl)/dxl);
  xfl=xal-ta*dxl;
  q=(xbl-xal)/dxl;
  tb=ta+q*1.00001;
  qk=(kb-ka)/q;
  pk=ka-ta*qk;
  jf=int((exp(ta*dxl)-1.)/w-1.e-5+1.);
  il=n*int(tb)+int((exp(sppard(tb)*dxl)-1)/w);
  for (i=jf; i<=il; ++i)
  {xk=pk+(i/n+log(i%n*w+1.)/dxl)*qk;
   k=int(xk);
   j=0;
   if (i%n == 0)
   {j=1;
    if (idc!=0) spppvl (k+dk,kdc,nxy,exp(xfl+i/n*dxl),ice);
   }
   sppptl (k,ntl[j],ntr[j],1-nxy,in[j]);
  }
 }
 if (nxy!=0) ws[1]=lstr*.001;
 if (idc!=0) idc=(idc+sign(int(ws[nxy]*iss),idc))/2;
 id=abs(idc);
 ndlr[nxy]=ntl[1]+(idc-id);
 ndlr[nxy+2]=ntr[1]+(idc+id);
if (idbg) dbg<<endl<<"ntl "<<setw(8)<<ntl[1]<<setw(8)<<idc<<setw(8)<<id;
if (idbg) dbg<<endl<<"ndlr"<<setw(3)<<nxy<<setw(8)<<ndlr[nxy]
                  <<setw(3)<<nxy+2<<setw(8)<<ndlr[nxy+2];
 slinewidth (ino);
 scolor (coo);
} 
void pstream_base::spppvl (int ix, int iy, int nxy, creal& v, int nce)
{int n[2],i,l,im;
 char val[20],ex[73];
 n[nxy]=ix;
 n[1-nxy]=iy;
 sprintf(val,mft[nxy],v);
 l=i=0;
 do
 {if (toupper(val[i])=='E')
  {++i;
   im=val[i]=='-';
   if (val[i]=='+' || im) ++i;// opuszczamy + po E
   while(val[i]=='0') ++i;    // opuszczamy zbedne 0 po E
   if (val[i]==' ') val[i]=0; // gdyby byla spacja
   if (xy[nxy].scale()==pls::log)    // skala logarytmiczna
   {if (val[i]==0) --i;       // byl zerowy wykladnik, musi zostac jedno 0
    if (strncmp(ex,"1.000000",min(l,8))==0) l=0;
   }
   if (val[i]!=0)
//   {if (l>0) {strncpy(ex+l,"\264",1); l+=1;}
   {if (l>0) {strncpy(ex+l,"<Y300>.<Y-300>",14); l+=14;}
    strncpy(ex+l,"10<S70Y500>-",11+im);
    l+=11+im;
   }
  }
  if (val[i]!=' ') ex[l++]=val[i];
 }while(val[i++]!=0);
 if (l>1 && ex[l-2]=='.') ex[l-2]=0; // l>1 powinno byc zawsze
 pwrt15 (n[0],n[1],ex,msi[nxy],0,nce);
if (idbg) dbg<<endl<<ex;
 if (nxy!=0) lstr=max(lstr,txtsiz(ex));
// cout<<"\n ex : "<<ex<<endl;
}
void pstream_base::sppptl (int j, int ia, int ib, int it, const real& in)
{int n[2];
 setlinewidth (in);
 n[it]=ia;
 n[1-it]=j;
 movf (n[0],n[1]);
 n[it]=ib;
 movv (n[0],n[1]);
} 
void pstream_base::labaxs (int nxy, const char* const c, int nsi, int ndc, int nor)
{int n[2];
 n[nxy]=nsc[nxy];
 int i=1-nxy;
 n[i]=ndlr[nxy+1+sign(1,ndc)]+ndc;
if (idbg) dbg<<endl<<"labaxs ndlr"<<setw(3)<<nxy+1+sign(1,ndc)<<setw(8)<<n[i];
// int ii=sign(sppsiz(nsi)-1, ndc);
 if (ndc==1 || ndc==-1) n[i]+=sign(fonsiz(nsi)*3/2-1,ndc);
 pwrt15(n[0],n[1],c,nsi,(nxy+1-sign(1,nor))*90,0);
if (idbg) dbg<<endl<<c<<setw(6)<<n[0]<<setw(6)<<n[1];
 lxy[nxy][0]=n[0];
 lxy[nxy][1]=n[1];
} 
void pstream_base::grid (int wxy, const real& yc, const real& yd, const real& xa,
                    const real& xb, const real& xlf, const real& dlx,
                    int nmi, int nti, int ndc)
{int nxy=wxy%2;
 if (abs(nti)<=1)
 {axs(nxy,yd,xa,xb,xlf,dlx,nmi,-nti,0);
  axs(nxy,yc,xa,xb,xlf,dlx,nmi, nti,ndc);
 }else
  axs(nxy+2,yc,xa,xb,xlf,dlx,nmi,xy[1-nxy].tra(yd),ndc);
} 
//--------------------------------------------------------------------
//dafm fon::afm("plo.afm");
//vects plo;
//--------------------------------------------------------------------


