#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
//#include <sstream>
#include <cmath>
using namespace std;
namespace RPS
{
typedef double real;
//---------------------------------------------------------------------------
typedef long spos;
//--------------------------------------------------------------------
#include <time.h>
void Err () {cerr<<"\nFatal Error !"; exit (0);}
void Err (const char* a, const char* b, int er=0)
{cerr<<a<<b; if (er!=0) exit (0);
}
void Err (const char* a, int er=0)
{cerr<<a; if (er!=0) exit (0);
}
//-----------------------------------------------------------------------------
//int idbg=0;
//ofstream dbg;
//-----------------------------------------------------------------------------
                              // powinno byc :
                              // sizeof(int2)=2*sizeof(int1)
                              // sizeof(int4)=4*sizeof(int1)
typedef char* pchar;
typedef int* pint;
typedef signed char int1;
typedef int1* pint1;
typedef short int2;
typedef int2* pint2;
typedef unsigned short uint2;
typedef int int4;
typedef int4* pint4;
typedef unsigned char uint1;
typedef uint1* puint1;
//-----------------------------------------------------------------------------
class fsi
{public:
 fsi (const char* fna);
 int isope () const {return iso;}
 void get (spos& sp, int& l);
 void rea (spos sp, int n, pint1 b);
 private:
 int iso;
 spos free;        // adres wolnego miejsca na dysku
 spos fsp; int lg; // adres pierwszego strumienia, dlugosc
 ifstream inp;
};
//-----------------------------------------------------------------------------
fsi::fsi (const char* fna)
{inp.open (fna,ios_base::in|ios_base::binary);
 iso=!inp.fail ();
 free=16;
 if (iso)
 {inp.seekg (0);
  int4 i4; inp.read (pchar(&i4),sizeof(int4)); fsp=i4;
  int2 i2; inp.read (pchar(&i2),sizeof(int2)); lg=i2;
  iso=!inp.fail ();
//if (idbg) dbg<<endl<<"Open "<<setw(8)<<fsp<<setw(5)<<lg<<setw(2)<<iso;
 }
}
void fsi::get (spos& sp, int& l)
{sp=fsp; l=lg;         // poczatek pierwszego strumienia
}
void fsi::rea (spos sp, int n, pint1 b)
{inp.seekg (sp);
 inp.read (pchar(b),n*sizeof(int1));
}
//--------------------------------------------------------------------
typedef fsi* pfsi;
class istrm;
typedef istrm* pistrm;
class istrm
{public:
 istrm ();
 istrm (const char* fna);
 istrm (istrm& s, int is);
 void ope (const char* fna);
 void ope (istrm& s, int is);
 int isope () const {return iso;}
 int gnps () const {return nps;}
 void rea (int n, pint1 c);
 void clo ();
 ~istrm ();
 protected:
 private:
 int iso;
// pstrm up;         // up==NULL - poczatek drzewa
 pfsi pfs;
                   //
 pint1 b;          // bufor
 const int lg;     // dlugosc bufora
 int lpb,l;
 spos sb; int lb;  // poczatek i dlugosc kolejnego bufora
                   //
 int nps;          // nps - tyle zostalo przylaczonych strumieni
 spos* tsb; pint tlb; int lps;  // wskazniki do przylaczonych strumieni
                                     // lps - aktualna dlugosc tablic
 void init_istrm ();
 void inib ();
 void cpb (int n, pint1 c);
 void rdhd (int& n, spos& s);
};
//--------------------------------------------------------------------
istrm::istrm () : lg(1024)
{init_istrm ();
}
istrm::istrm (const char* fna) : lg(1024)
{init_istrm ();
 ope (fna);
}
istrm::istrm (istrm& s, int is) : lg(1024)
{init_istrm ();
 ope (s,is);
}
istrm:: ~istrm ()
{if (iso) clo ();
 if (b!=NULL) delete [] b;
 if (lps>0) {delete [] tsb; delete [] tlb;} 
}
void istrm::init_istrm ()
{iso=0;
 b=NULL;
 lps=0;
}
void istrm::inib ()
{if (b==NULL) b=new int1 [lg];
 l=0;                   // pusty bufor
                        // czytamy ile zostalo przylaczonych strumieni
//int isb=sb,ilb=lb;
 int2 i2; rea (2,pint1(&i2)); nps=i2;
 if (nps>lps)           // wiecej strumieni niz dlugosc tablic
 {if (lps>0) {delete [] tsb; delete [] tlb;} 
  lps=nps; tsb=new spos [lps]; tlb=new int [lps];
 }
                      // wczytujemy poczatki wszystkich przylaczonych strumieni
 for (int i=0; i<nps; ++i)
 {int4 i4; rea (4,pint1(&i4)); tsb[i]=i4;
  int2 i2; rea (2,pint1(&i2)); tlb[i]=i2;  
 }
//if (idbg) dbg<<endl<<"/ "<<setw(12)<<isb<<setw(8)<<ilb<<setw(5)<<nps<<setw(2)<<iso<<" /";
}
void istrm::ope (const char* fna)
{if (iso) clo ();
 pfs=new fsi (fna);
 iso=pfs->isope ();
 if (iso) {pfs->get (sb,lb); inib ();}else delete pfs; // nie udalo sie otworzyc
}
void istrm::ope (istrm& s, int is)
{if (iso) clo ();
 iso=s.iso;            // powinno byc 1
 if (iso)
 {pfs=s.pfs;
  if (is>=0 && is<s.nps) {sb=s.tsb[is]; lb=s.tlb[is]; inib ();} else iso=0;
//if (idbg) dbg<<endl<<"Ope "<<setw(12)<<sb<<setw(8)<<lb<<setw(12)<<i4<<setw(8)<<i2;
 }
}
void istrm::clo ()
{if (iso)
 { 
//  if (up==NULL) delete pfs; else --up->nas;
  iso=0;
 }
}
void istrm::rdhd (int& n, spos& s)
{int4 i4; cpb (4,pint1(&i4)); s=i4;
 int2 i2; cpb (2,pint1(&i2)); n=i2;
//if (idbg) dbg<<endl<<"Rdhd "<<setw(12)<<s<<setw(8)<<n;
}
void istrm::cpb (int n, pint1 c)
{for (int i=0; i<n; ++i) c[i]=b[lpb+i];
//if (idbg) {dbg<<endl; for (i=0; i<n; ++i) dbg<<setw(4)<<int((unsigned char)(c[i]));}
 lpb+=n; l-=n;
}
void istrm::rea (int n, pint1 c)
{int lr=(n<=l?n:l);            // tyle mozemy wziasc z bufora
 if (lr>0) cpb (lr,c);
 if (lr<n)                     // dane nie mieszcza sie w buforze
 {if (sb!=0)
  {pfs->rea (sb,lb,b);
                               // lb powinno byc mniejsze od lg
   lpb=0; l=lb;
   rdhd (lb,sb);               // sb,lb poczatek i dlugosc nastepnego bufora
   rea (n-lr,c+lr);            // cztamy reszte danych
  }else                        // sb==0 nie ma wiecej danych, blad 
  {clo (); 
  }
 }
}
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------
void prt (const char* fna)
{ifstream inp (fna,ios_base::in|ios_base::binary);
 if (!inp) Err ("Blad otwarcia inp.");
 int n=0;
 while (inp)
 {unsigned char c;
  inp.read (reinterpret_cast<char* const>(&c),1);
//  if (inp) if (idbg) dbg<<setw(6)<<n<<setw(4)<<int(c)<<endl;
  ++n;
 }
 inp.close ();
}
typedef double real;
typedef real* preal;
//--------------------------------------------------------------------
void rdarg (int l, preal t,
                      int& iar, int larg, const char *const arg[], const char* c)
{for (int i=1; i<=l; ++i)
 {++iar; if (iar>=larg) Err ("Brakuje argumentow.",1);
  t[i]=atof(arg[iar]);
 }
 t[0]=1;    // wczytane, t[1] - t[l] - argumenty
}
//--------------------------------------------------------------------
//int rps (int argc, pchar argv[])
int rps (const string& f)
{//if (idbg) dbg.open ("rps.dbg");
 const int argc=1; const char* const argv[1]={""};// argv[0]="";
 real sc=792./16384.;
 sc/=3.;
//prt (argv[1]);
 real tra[3],sca[3],rot[2],cot[7];
 tra[0]=sca[0]=rot[0]=cot[0]=0;

 int iar=1,ist=0;
 while (iar<argc)
 {if (argv[iar][0]!='-')
  {if (ist>0) Err ("Powtorzona nazwa zbioru : ",argv[iar],1);
   ist=iar;
  }else switch (argv[iar][1])
  {case 't' :
    rdarg (2,tra,iar,argc,argv," Translate : ( -t tx ty ) wymaga 2 argumentow.");
    break;
   case 's' :
    rdarg (2,sca,iar,argc,argv," Scale : ( -s sx sy ) wymaga 2 argumentow.");
    break;
   case 'r' :
    rdarg (1,rot,iar,argc,argv," Rotate : ( -r angle ) wymaga 1 argumentu.");
    break;
   case 'c' :
    rdarg (6,cot,iar,argc,argv," Concatenate : ( -c a b c d tx ty ) wymaga 6 argumentow.");
    break;
   case 'D' :
//    if (idbg==0) {dbg.open ("rps.dbg",ios_base::out); if (!!dbg) idbg=1;}
    break;
   default :
    Err ("Nieznany parametr : ",argv[iar],1);
    break;
  }
  ++iar;
 }
 if (sca[0]==0) {sca[0]=1; sca[1]=sca[2]=sc;} else {sca[1]*=sc; sca[2]*=sc;}
 int1 op; uint1 udat[4];
 istrm l((f+".plo").c_str()),r,d,c;
 //istrm l(ist>0?argv[ist]:"plo.plo"),r,d,c;
// if (idbg) dbg<<endl<<"isope "<<l.isope()<<"   ";
 if (!l.isope ()) Err ("Blad otwarcia zbioru : ",f.c_str(),1);
 //if (!l.isope ()) Err ("Blad otwarcia zbioru : ",ist>0?argv[ist]:"plo.plo",1);
 int llx,lly,urx,ury;
 int4 i4; l.rea (4,pint1(&i4)); llx=i4; l.rea (4,pint1(&i4)); lly=i4;
 l.rea (4,pint1(&i4)); urx=i4; l.rea (4,pint1(&i4)); ury=i4;
 llx=int(llx*sc); lly=int(lly*sc); urx=int(urx*sc); ury=int(ury*sc);
 int lfon; pchar* fon=NULL; int lcfon; pchar cfon=NULL;
 int2 i2; l.rea (2,pint1(&i2)); lfon=i2; 
// if (idbg) dbg<<endl<<setw(12)<<lfon<<setw(12)<<i2;
 if (lfon>0)
 {fon=new pchar[lfon];
  for (int i=0; i<lfon; ++i) fon[i]=NULL;
 }
 l.rea (2,pint1(&i2)); lcfon=i2;
 if (lfon>0) cfon=new char[lcfon+2*lfon];// do kazdej nazwy fontu dodajemu '/' i 0
 int lgf; pint tgf=NULL;
 l.rea (2,pint1(&i2)); lgf=i2;
//if (idbg) dbg<<endl<<"lgf "<<lgf;
 if (lgf>0)              // fonty dla wszystkich rysunkow
 {tgf=new int [lgf];
  int ic=0;
  for (int i=0; i<lgf; ++i)
  {int2 i2; l.rea (2,pint1(&i2)); tgf[i]=i2;
   uint1 u1; l.rea (1,pint1(&u1)); int is=u1;
   fon[tgf[i]]=cfon+ic;
   cfon[ic++]='/'; l.rea (is,pint1(cfon+ic)); ic+=is; cfon[ic++]=0;
  }
 }
// if (idbg) for (int j=0; j<lfon; ++j) if (fon[j]!=NULL) dbg<<endl<<setw(4)<<j<<' '<<fon[j];
 int lry=l.gnps ();
//----------------------------------------------
 time_t ltime;
 time (&ltime);
 ofstream ps ((f+".ps").c_str());
 ps<<"%!PS-Adobe-3.0\n"
     "%%Creator: (psstream 1.0 (C) 1998 KN)\n"
     "%%Title: (plo.ps file)\n"
     "%%CreationDate: "<<ctime(&ltime)
   <<"%%Pages: "<<lry<<endl
   <<"%%LanguageLevel: 2\n"
     "%%PageOrder: Ascend\n"
     "%%BoundingBox: "<<llx<<' '<<lly<<' '<<urx<<' '<<ury<<endl;
 if (lgf>0)
 {ps<<"%%DocumentNeededResources:";
  for (int i=0; i<lgf; ++i)
  {if ((i%2)==0) {if (i!=0) ps<<"\n%%+"; ps<<" font";}
   ps<<' '<<fon[tgf[i]]+1;
  }
  ps<<endl;
 }
 ps<<"%%DocumentSuppliedResources: procset (psstream proc) 1.0 0\n"
     "%%EndComments\n"
     "%%BeginProlog\n"
     "%%BeginResource: procset (psstream proc) 1.0 0\n"
     "  /PloDict 11 dict def\n"
     "  PloDict begin\n"
     "    /bop {/SI save def 0 0 moveto} def\n"
     "    /eop {SI restore showpage} def\n"
     "    /m {moveto} bind def\n"
     "    /l {lineto} bind def\n"
     "    /s {stroke} bind def\n"
     "    /f {selectfont} bind def\n"
     "    /v {moveto show} bind def\n"
     "    /r 0 def\n"
     "    /V {gsave translate r rotate 0 0 moveto show grestore} bind def\n"
     "    /w {setlinewidth} bind def\n"
     "    /c {setcmykcolor} bind def\n"
     "  end\n"
     "%%EndResource\n"
     "%%EndProlog\n"
     "%%BeginSetup\n"
     "  PloDict begin\n";
 if (lgf>0)
  for (int i=0; i<lgf; ++i) ps<<"%%IncludeResource: font "<<fon[tgf[i]]+1<<endl;
 ps<<"%%EndSetup\n";
//     "%%\n"
//----------------------------------------------
 for (int i=0; i<lry; ++i)
 {r.ope (l,i);
//  if (idbg) dbg<<endl<<"isope "<<r.isope()<<"   ";
  int pllx,plly,purx,pury;
  int4 i4; r.rea (4,pint1(&i4)); pllx=i4; r.rea (4,pint1(&i4)); plly=i4;
  r.rea (4,pint1(&i4)); purx=i4; r.rea (4,pint1(&i4)); pury=i4;
  pllx=int(pllx*sc); plly=int(plly*sc); purx=int(purx*sc); pury=int(pury*sc);
//if (idbg) dbg<<endl<<setw(12)<<pllx<<setw(12)<<plly;
  int2 i2; r.rea (2,pint1(&i2)); int lf=i2;
//if (idbg) dbg<<endl<<"lf "<<lf;
  if (lf>0)              // rysunek uzywa fontow, lf<=lgf
   for (int i=0; i<lf; ++i) {int2 i2; r.rea (2,pint1(&i2)); tgf[i]=i2;}
//----------------------------------------------
 ps<<"%%Page: "<<i+1<<' '<<i+1<<endl
   <<"%%PageBoundingBox: "<<pllx<<' '<<plly<<' '<<purx<<' '<<pury<<endl;
 if (lf>0)
 {ps<<"%%PageResources:";
  for (int i=0; i<lf; ++i)
  {if ((i%2)==0) {if (i!=0) ps<<"\n%%+"; ps<<" font";}
   ps<<' '<<fon[tgf[i]]+1;
  }
  ps<<endl;
 }
 ps<<"%%BeginSetup\n"
     "  bop\n"
     "%%EndPageSetup\n";
//----------------------------------------------
if (tra[0]==1) ps<<tra[1]<<' '<<tra[2]<<" translate"<<endl;
if (sca[0]==1) ps<<sca[1]<<' '<<sca[2]<<" scale"<<endl;
if (rot[0]==1) ps<<rot[1]<<" rotate"<<endl;
if (cot[0]==1) ps<<'['<<cot[1]<<' '<<cot[2]<<' '<<cot[3]<<' '<<cot[4]
                 <<' '<<cot[5]<<' '<<cot[6]<<"] concat"<<endl;
  int lsr=r.gnps ();
//  if (idbg) dbg<<endl<<"Rys "<<setw(4)<<i<<" lsr "<<lsr<<endl;
  c.ope (r,0);
//  if (idbg) dbg<<setw(4)<<0<<setw(4)<<c.gnps();
  c.rea (1,&op);
//if (idbg) dbg<<endl<<"Op  "<<int(uint1(op));
  int isc=-1,isi=-1,ior=0,ifo=-1,ix,iy;
  while (c.isope())
  {uint2 u2; int4 i4; 
   unsigned char z;
   switch (op)
   {case 1 :
     c.rea (2,pint1(&u2)); z=uint2(u2);
//     if (idbg) dbg<<endl<<"Znak "<<char(u2);
     c.rea (4,pint1(&i4)); ix=i4;
//     if (idbg) dbg<<setw(8)<<i4;
     c.rea (4,pint1(&i4)); iy=i4;
//     if (idbg) dbg<<setw(8)<<i4;
     if (isc) ps<<fon[ifo]<<' '<<isi<<" f\n";
     isc=0;
     if (z<32 || z>127) ps<<"(\\"<<oct<<int(z)<<dec<<')';
     else
     {ps<<'(';
      if (z=='(' || z==')' || z=='\\') ps<<'\\';
      ps<<z<<')';
     }
     ps<<' '<<ix<<' '<<iy;
     ps<<(ior==0?" v\n":" V\n");
     break;
    case 2 :
     c.rea (4,pint1(&i4)); ifo=i4; isc=1;
//     if (idbg) dbg<<endl<<"ifn  "<<setw(8)<<i4;
     break;
    case 3 :
     c.rea (4,pint1(&i4)); isi=i4; isc=1;
//     if (idbg) dbg<<endl<<"isi  "<<setw(8)<<i4;
     break;
    case 4 :
     c.rea (4,pint1(&i4)); ior=i4/100;
     ps<<"/r "<<ior<<" def\n";
//     if (idbg) dbg<<endl<<"ior  "<<setw(8)<<i4;
     break;
    case 5 :                 // kolor cmyk
     c.rea (4,pint1(udat));
     ps<<udat[0]/255.<<' '<<udat[1]/255.
       <<' '<<udat[2]/255.<<' '<<udat[3]/255.<<' '<<'c'<<endl;
//if (idbg) {dbg<<endl<<"color "; for (int i=0; i<4; ++i) dbg<<setw(4)<<int(udat[i]);}
     break;
    default :
cerr<<endl<<"\nERR "<<int(op);
//if (idbg) dbg<<endl<<"ERR "<<int(uint1(op));
     break;
   }
   c.rea (1,&op);
//if (idbg) dbg<<endl<<"Op  "<<int(uint1(op));
  }
  for (int j=1; j<lsr; ++j)
  {d.ope (r,j);
//   if (idbg) dbg<<setw(4)<<j<<setw(4)<<d.gnps();
   int ll=0;                           // liczy punkty w linii
   ps<<"1 w"<<endl;
   d.rea (1,&op);
   while(d.isope())
   {int2 i2; int4 i4; int k,ix,iy;

    switch (op)
    {case  1 :
      int n;
      d.rea (2,pint1(&i2)); n=i2;
      d.rea (2,pint1(&i2)); ix=i2;
      d.rea (2,pint1(&i2)); iy=i2;
      ll=0;
      ps<<ix<<' '<<iy<<' '<<"m ";
//if (idbg) dbg<<endl<<n<<' '<<ix<<' '<<iy<<' '<<"m ";
      for (k=1; k<n; ++k)
      {d.rea (2,pint1(&i2)); ix=i2;
       d.rea (2,pint1(&i2)); iy=i2;
       ++ll; if ((ll%5)==0) ps<<endl;
       ps<<ix<<' '<<iy<<' '<<"l ";
      }
      ps<<'s'<<endl;
//if (idbg) dbg<<endl<<"Linia : "<<setw(5)<<n<<setw(2)<<d.isope();
      break;
     case 2 :                // grubosc linii
      d.rea (4,pint1(&i4));
      ps<<i4/100.<<' '<<'w'<<endl;
      break;
     case 3 :                 // kolor cmyk
      d.rea (4,pint1(udat));
      ps<<udat[0]/255.<<' '<<udat[1]/255.
        <<' '<<udat[2]/255.<<' '<<udat[3]/255.<<' '<<'c'<<endl;
      break;
     default :
      cerr<<endl<<"ERR";
      break;
    }     

    d.rea (1,&op);
   }
  }
//----------------------------------------------
 ps<<"  eop\n"
     "%%PageTrailer\n";
//----------------------------------------------
 }
 ps<<"%%Trailer\n"
     "  end\n"
     "%%EOF\n";
 if (lgf>0) delete [] tgf;
 if (lfon>0) {delete [] fon; delete [] cfon;}
 return 0;
}
/*
   0 "/AvantGarde-Book",
   1 "/AvantGarde-BookOblique",
   2 "/AvantGarde-Demi",
   3 "/AvantGarde-DemiOblique",
   4 "/Bookman-Demi",
   5 "/Bookman-DemiItalic",
   6 "/Bookman-Light",
   7 "/Bookman-LightItalic",
   8 "/Courier",
   9 "/Courier-Bold",
  10 "/Courier-BoldOblique",
  11 "/Courier-Oblique",
  12 "/Helvetica",
  13 "/Helvetica-Bold",
  14 "/Helvetica-BoldOblique",
  15 "/Helvetica-Condensed",
  16 "/Helvetica-Condensed-Bold",
  17 "/Helvetica-Condensed-BoldObl",
  18 "/Helvetica-Condensed-Oblique",
  19 "/Helvetica-Narrow",
  20 "/Helvetica-Narrow-Bold",
  21 "/Helvetica-Narrow-BoldOblique",
  22 "/Helvetica-Narrow-Oblique",
  23 "/Helvetica-Oblique",
  24 "/NewCenturySchlbk-Bold",
  25 "/NewCenturySchlbk-BoldItalic",
  26 "/NewCenturySchlbk-Italic",
  27 "/NewCenturySchlbk-Roman",
  28 "/Palatino-Bold",
  29 "/Palatino-BoldItalic",
  30 "/Palatino-Italic",
  31 "/Palatino-Roman",
  32 "/Symbol",
  33 "/Times-Bold",
  34 "/Times-BoldItalic",
  35 "/Times-Italic",
  36 "/Times-Roman",
  37 "/ZapfChancery-MediumItalic",
  38 "/ZapfDingbats",
*/
}
