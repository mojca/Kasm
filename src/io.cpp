#include <cstring>
#include <iostream>
#include <iomanip>
#include "io.h"
//#include <string.h>
using namespace std;
extern int idbg;
extern ofstream dbg;
//-----------------------------------------------------------------------------
fso::fso (const char* fna)
{out.open (fna,ios::out|ios::in|ios::binary|ios::trunc);
 iso=!out.fail ();
 free=16;
}
streampos fso::alloc (int n)
{streampos sp=free;
 free+=n;
 return sp;
}
void fso::set (streampos sp, int l)
{fsp=sp; lg=l;         // zapamietujemy poczatek pierwszego strumienia
}
//ofstream dbg ("dbg");
void fso::wri (streampos sp, int n, const int1* b)
{out.seekp (sp);
 out.write (pchar(b),n*sizeof(int1));
}
void fso::rea (streampos sp, int n, pint1 b)
{out.seekg (sp);
 out.read (pchar(b),n*sizeof(int1));
}
void fso::clo ()
{out.seekp (0);
 int4 i4=fsp; out.write (pchar(&i4),sizeof(int4));
 int2 i2=lg; out.write (pchar(&i2),sizeof(int2));
 out.close ();
}
fso::~fso ()
{if (iso) clo ();
}
//--------------------------------------------------------------------
copt::copt () : lopt(16)
{val=new int[lopt]; sval=new int[lopt];
 init_copt ();
}
copt::~copt ()
{delete [] val; delete [] sval;
}
void copt::init_copt ()
{for (int i=0; i<lopt; ++i) sval[i]=0;  // zadna z opcji nie ustawiona
}
int copt::wopt (int ival, int iopt)
{int ir=1;
 if (iopt>=0 && iopt<lopt)
 {ir=(sval[iopt]==0 || val[iopt]!=ival);
  if (ir) {sval[iopt]=1; val[iopt]=ival;}
 }
 return ir;           // ir==1, opcja ulegla zmianie, trzeba wypisac
                      // ewentualnie za duzy numer opcji (lopt za male)
}
int copt::ropt (int iopt)
{return iopt>=0 && iopt<lopt?val[iopt]:0;
}
//--------------------------------------------------------------------
void link::ope ()
{prev=next=last=NULL;
}
//                     nie dziala z icc
void link::ope (strm& s, pstrm ps)
{next=last=NULL;
 typedef link* plink;
 prev=plink(&s)->last; plink(&s)->last=ps;
// prev=s.last; s.last=ps;
 if (prev!=NULL) plink(prev)->next=ps;
// if (prev!=NULL) prev->next=ps;
}
void link::clo (pstrm up)
{while (last!=NULL) last->clo ();
 typedef link* plink;
 if (prev!=NULL) plink(prev)->next=next;
 if (next!=NULL) plink(next)->prev=prev;
 if (up!=NULL && next==NULL) plink(up)->last=prev;
// if (prev!=NULL) prev->next=next;
// if (next!=NULL) next->prev=prev;
// if (up!=NULL && next==NULL) up->last=prev;
}
//--------------------------------------------------------------------
strm::strm () : lg(1024), lh(64), lhe(4*64)
{init_strm ();
}
strm::strm (const char* fna) : lg(1024), lh(64), lhe(4*64)
{init_strm ();
 ope (fna);
}
strm::strm (strm& s) : lg(1024), lh(64), lhe(4*64)
{init_strm ();
 ope (s);
}
strm:: ~strm ()
{if (iso) clo ();
 if (b!=NULL) {delete [] b; delete [] h;}
}
void strm::init_strm ()
{lhd=4+2;
 iso=0;
 b=NULL;
}
void strm::inib ()
{if (b==NULL) {b=new int1 [lg+lg]; h=new int [lhe];}
 for (int i=0; i<lhe; ++i) h[i]=0;
 ih=0;
 sp=spa=0;
 lpb=0; l=lg; nps=0; nas=0;
 wrhd (0,0);            // rezerwujemy miejsce
 hnps=res (2);          // tu wpiszemy ile zostalo przylaczonych strumieni
 init_copt ();
}
void strm::wrhd (int n, streampos s)
{cpb (int4(s)); cpb(int2(n));
}
void strm::cpb (int2 i2)
{cpb (2,pint1(&i2));
}
void strm::cpb (int4 i4)
{cpb (4,pint1(&i4));
}
void strm::cpb (int n, const int1* c)
{for (int i=0; i<n; ++i) b[lpb+i]=c[i];
 lpb+=n; l-=n;
}
void strm::cpb (int n, int1 d)
{for (int i=0; i<n; ++i) b[lpb+i]=d;
 lpb+=n; l-=n;
}
void strm::rwhd (int n)
{up->rewc4 (hs,spa).rew2 (hs,n);
}
int strm::res (int n)
{int ir=-2;
 if (iso)
 {ir=ih;                // ih pokazuje na wolny lub ih==-1
  if (ih>=0)
  {int hih=(l>0?lpb:((lpb==lg?lg:0)+lhd));
   wrs (1);
   h[ih]=hih;
   h[ih+2]=0;
   do {ih+=4; if (ih==lhe) ih=0;}while (ih!=ir && h[ih]!=0);
   if (ih==ir) ih=-1;
   if (n>1) wrs (n-1);
                       // pisanie wrs w dwoch czesciach bierze sie stad, ze 
                       // w przypadku gdy oba bufory sa pelne i najpier jeden 
                       // z nich trzeba wypisac na zbior tablica h zostanie 
                       // zle uaktualniona
  }
 }
if (ir<0) cerr<<"Err ---+---"<<setw(8)<<ir<<endl;
 return ir;  // -2 blad, nie jest otwarty, -1 blad, nie ma miejsca w h
}
             // rew  - tylko raz mozna pisac na hps
             // rewc - mozna pisac na hps wiele razy (nie w to samo miejsce
             //        tylko dalej), h[hps] jest uaktualniane
void strm::rew (int hps, int n, const int1* c)
{if (iso && hps>=0 && hps<lhe && hps%4==0 && h[hps]!=0)
 {rwb (hps,n,c);
  h[hps]=h[hps+2]=0;
 }
}
void strm::rewc (int hps, int n, const int1* c)
{if (iso && hps>=0 && hps<lhe && hps%4==0 && h[hps]!=0) rwb (hps,n,c);
}
                             // h[hps]   <pamiec>     <dysk>        <dysk>
                             // h[hps+1]    -         <dlugosc>     <dlugosc>
                             // h[hps+2]    0         <pamiec>      <dysk>
                             // h[hps+3]    -         <dlugosc>     <dlugosc>
void strm::rwb (int hps, int n, const int1* c)
{int i,ip=h[hps];
 if (ip>0)                   // poczatek pisania jest w buforze
 {int ik=(ip<lg?lg:2*lg);    // koniec bufora
  int il=ik-ip;              // tyle miejsca w buforze
  int nl=(n<=il?n:il);       // tyle mozemy wpisac
  for (i=0; i<nl; ++i) b[ip+i]=c[i];
  ip+=nl;                    // ip<=ik
  if (ip>=ik) ip=2*lg-ik+lhd;
  h[hps]=ip;                 // umozliwia dalsze pisanie
  if (n>nl) rwb (hps,n-nl,c+nl); // wypisujemy reszte,
                             //     nie sprawdzamy czy za duzo
 }else                       // ip<0 poczatek pisania na dysku
 {streampos sp=-ip;          // adres na dysku
  int il=h[hps+1];           // tyle miejsca w buforze
  int nl=(n<=il?n:il);       // tyle mozemy wpisac
  if (nl>0) pfs->wri (sp,nl,c);        // piszemy na zbior
  if (nl==n)                 // wszystko wypisalismy
  {h[hps]-=nl;               // h[hps]<0
   h[hps+1]-=nl;
  }else                      // nie wszystko wypisane
  {if (h[hps+2]>0)           // w pamieci
   {h[hps]=h[hps+2];
    h[hps+2]=0;
   }else                     // na dysku
   {h[hps]=h[hps+2]-lhd;     // odejmujemy bo h[hps+2]<0
    h[hps+1]=h[hps+3]-lhd;
    streampos sa=-h[hps+2],sb;
    int4 i4; int2 i2;
    pfs->rea (sa,4,pint1(&i4)); sb=i4;
    pfs->rea (int(sa)+4*sizeof(int1),2,pint1(&i2));
    if (sb==spa)             // jeszcze jest w pamieci
    {h[hps+2]=(lpb<lg?lg:0)+lhd;
     h[hps+3]=i2-lhd;
    }else                    // juz na dysku
    {h[hps+2]=-sb;
     h[hps+3]=i2;
    }
   }
   rwb (hps,n-nl,c+nl);      // wypisujemy reszte,  nie sprawdzamy czy za duzo
  }
 }
}
void strm::wri (int n, const int1* c)
{if (iso) wrs (n,c);
}
void strm::wrs (int n, const int1* c)
{int lr=(n<=l?n:l);
 if (lr>0) {if (c==NULL) cpb (lr,int1(0xff)); else cpb (lr,c);}
 if (lr<n)                     // dane nie mieszcza sie w buforze
 {if (spa==streampos(0))                  // pierwszy raz pelny bufor, lpb==lg
  {sp=spa=pfs->alloc (lg);     // przydzielamy dla niego miejsce na dysku
   if (up==NULL) pfs->set (spa,lg); else rwhd (lg);
   l=lg;                       // wypelnilismy bufor (lpb,l) czescia danych
   wrhd (0,0);                 // bufor (lpb,l) gotowy do pisania
  }else wrb (0);
  wrs (n-lr,c==NULL?c:c+lr);   // wypisujemy reszte danych
 }
}
void strm::cls ()
{int lgr=lg-l;
 if (spa==streampos(0))                   // niepelny jeden bufor
 {sp=spa=pfs->alloc (lgr);     // przydzielamy dla niego miejsce na dysku
  if (up==NULL) pfs->set (spa,lgr); else rwhd (lgr);
  lpb+=l;
  l=lg;
 }else wrb (0);                // piszemy przedostatni bufor
 wrb (lgr);                    // piszemy ostatni bufor
}
void strm::wrb (int ng)
{int lgr,mg; 
 streampos spb;
                               // lgr - dlugosc biezacego bufora
                               // mg - dlugosc bufora do wypisania na zbior
 int lpa=2*lg-(lpb+l);         // lpb+l==lg || lpb+l==2*lg
                               // jesli bufor nie byl pusty dodajemy l
                               // lpa - poczatek poprzedniego/nastepnego bufora
                               // spa - adres dyskowy gdzie pisac poprzedni bufor
 if (ng<=0)                    // pisanie nie ostatniego bufora
 {lgr=lg-l;
  spb=pfs->alloc(lgr); mg=lg;  // adres dyskowy dla biezacego bufora
 }else                         // ostatni bufor pisany na zbior
 {spb=0; mg=ng;
  lgr=0;
 }
 l=lg; lpb=lpa;
 wrhd (lgr,spb);
 pfs->wri (spa,mg,b+lpa);      // piszemy poprzedni/ostatni bufor na zbior
 for (int i=0; i<lhe; i+=4)    // uaktualniamy tablice h
 {int j=h[i]-lpa;
  if (h[i]>0 && j>=0 && j<lg)
  {h[i]=-(int(spa)+j); // adres dyskowy po zapisaniu bufora
   h[i+1]=mg-j;
   h[i+2]=lg-lpa+lhd;          // dalsze dane w pamieci
   h[i+3]=lgr-lhd;             // 
  }else if (h[i]<0 && h[i+2]>0)  // bufory (dysk,pamiec)->(dysk,dysk)
  {h[i+2]=-spa;
   h[i+3]+=lhd;
  }
 }
 if (ng<=0) spa=spb;           // ? if
}
void strm::ope (const char* fna)
{if (iso) clo ();
 up=NULL;                      // otwieramy jako poczatek drzewa
 pfs=new fso (fna);
 iso=pfs->isope ();
 if (iso) {link::ope (); inib ();} else delete pfs;  // nie udalo sie otworzyc
}
void strm::ope (strm& s)
{if (iso) clo ();
 iso=s.iso;            // powinno byc 1
 if (iso)
 {link::ope (s,this);
  pfs=s.pfs; up=&s;
  hs=s.res (lhd);      // pod tym adresem zapiszemy
                       // w strumieniu wyzej (up) poczatek i dlugosc
                       // dla biezacego strumienia
  ++s.nps;             // liczymy w s przylaczone strumienie
  ++s.nas;             // przylaczone strumienie w s (w danej chwili)
  inib ();
 }
}
void strm::clo ()
{if (iso)
 {link::clo (up);
  rew2 (hnps,nps);       // wpisujemy ilosc przylaczonych strumieni 
  cls ();
                         // zmniejszamy ilosc aktualnie przylaczonych strumieni
  if (up==NULL) delete pfs; else --up->nas;
  iso=0;
if (nas!=0) cerr<<"nas : (powinno byc zero) "<<nas<<endl;
 }
}
/*
void strm::wro (int4 op4, int1 cop, int nop)
{if (wopt (op4,nop))
 {wri (1,&cop);
  wri (4,pint1(&op4));
 }
}
*/
int strm::wopt (int ival, int iopt)
{int ir=copt::wopt (ival,iopt);
 if (ir) wri1(iopt).wri4(ival);
 return ir;
}
//--------------------------------------------------------------------
xstrm::xstrm (const char* fna, const dafm* pafm) : strm(), pafm(pafm)
{lfo=pafm->FontL ();
if (idbg) dbg<<endl<<"************** lfo : "<<lfo<<endl;
 init_xstrm ();
 ope (fna);
}
xstrm::xstrm (xstrm& xs) : strm(), lfo(xs.lfo), pafm(NULL)
{init_xstrm ();
 ope (xs);
}
void xstrm::ope (const char* fna)
{strm::ope (fna);
 inix ();
}
void xstrm::ope (strm& s)
{strm::ope (s);
 inix ();
}
void xstrm::init_xstrm ()
{tfo=new int2 [lfo];
}
void xstrm::inix ()
{clrb ();
 for (int i=0; i<lfo; ++i) tfo[i]=0;
}
void xstrm::clo ()
{if (idbg) dbg<<endl<<"Zamykamy xstream "<<lfo;
// if (isb==0) {llx=lly=0; urx=ury=792;}
                              // up!=NULL, up jest xstrm, w io_base rstrm
                              // uaktualniamy zakres wszystkich rysunkow
 if (pafm==NULL) pxstrm(up)->addb ((bbox&)(*this));
 wri4 (llx); wri4 (lly); wri4 (urx);  wri4 (ury);
 int ih=res(2+(pafm==NULL?0:4));
 int i,il,ic;
 for (ic=0, il=0, i=0; i<lfo; ++i) 
  if (tfo[i]!=0)
  {++il;
   wri2 (i);
   if (pafm!=NULL)                   // wypisujemy nazwy fontow
   {const char* c=pafm->FontN (i);
    int is=int(strlen(c)); ic+=is;
//    uint1 u1=is; wri (1,pint1(&u1)); wri (is,pint1(c));
    wri1 (is); wri (is,cpint1(c));
if (idbg) dbg<<endl<<setw(4)<<i<<setw(4)<<int(is)<<' '<<c;
   }
  }
 if (pafm!=NULL) rewc2 (ih,lfo).rewc2 (ih,ic);
 rew2 (ih,il);
// if (fo!=NULL) cerr<<' '<<fo;
 if (idbg) {dbg<<endl; for (i=0; i<lfo; ++i) if (tfo[i]!=0) dbg<<setw(4)<<i;}
 strm::clo ();
}
xstrm::~xstrm ()
{delete [] tfo;
}
void xstrm::mrk (int l)
{if (l>=0 && l<lfo) tfo[l]=1;
}
