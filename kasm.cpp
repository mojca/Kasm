// g++ -O2 -Wall casm.cpp imag.cpp io.cpp kasm.cpp pasm.cpp ps.cpp rdafm.cpp rps.cpp -o KAsm
//---------------------------------------------------------------------------
#include "imag.h"
#include "pasm.h"
#include "casm.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
using namespace std;
typedef double real;
//---------------------------------------------------------------------------
ofstream logs; //("KAsm.log",ios::app);
//---------------------------------------------------------------------------
//inline real sqr (const real& x) {return x*x;}
void norm (const int n, real* const x, const real& s);
int kid (const real& xk);
real ext (const int n, const real* const x);
int fits (const int n, real* const x, int* const vws, const int lgap);
int cid (const int MaxK, const int SumK);
//---------------------------------------------------------------------------
const string toupper (const string& s)
{string u;
 for (int l=int(s.length()),i=0; i<l; ++i) u+=toupper(s[i]);
 return u;
}
//---------------------------------------------------------------------------
namespace RPS {int rps (const string& f);}
//---------------------------------------------------------------------------
//                  tablica wsk ma 9 - elementow, 8 wskaznikow i ich suma
void wwsk (const bool pd, const int* const vws, const real* const h,
           const real* const d, const real* const hf, const real* const df,
           ofstream& out, int* const wsk)
{const int np=(pd?180:0),kp=(pd?1:0);    // uwzgledniamy dzien poprzedni
 //for (int i=0; i<1440; ++i) out<<setw(14)<<h[i+np]<<setw(14)<<d[i+np]
 //<<setw(14)<<h[i+np]-hf[i+np]<<setw(14)<<d[i+np]-df[i+np]<<'\n';
 const int m=8,mw=3*60;
 //int wsk[m];   // wskazniki
 bool allWsk=true;
 int SumK=0,MaxK=0;
 for (int j=0; j<m; ++j)
 if (vws[kp+j]==0)
 {real dy=max(ext(mw,h+np+j*mw),ext(mw,d+np+j*mw));
  real dyf=max(ext(mw,hf+np+j*mw),ext(mw,df+np+j*mw));
  if (dy>dyf && !!logs) logs<<"\n dy, dyf : "<<setw(12)<<dy<<setw(12)<<dyf;
  int K=kid(dy),Kf=kid(dyf);
//  if (K>Kf) {logs<<setw(5)<<K<<setw(5)<<Kf; if (K>5) {logs<<" *"; K=Kf;}}
  if (K>Kf) {if (!!logs) logs<<setw(5)<<K<<setw(5)<<Kf; K=Kf;}
  wsk[j]=K;
  SumK+=K;
  if (MaxK<K) MaxK=K;
 }
 else
 {allWsk=false;
  wsk[j]=-1;
 }
 wsk[m]=(allWsk?SumK:-1);
 if (!!out)                 // tylko gdy out otwarty (gdy obliczamy dla jednego dnia)
 {for (int j=0; j<=m; ++j) out<<setw(3)<<wsk[j];
  out<<setw(2)<<(allWsk?cid(MaxK,SumK):-1);
 }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//              Days - z tylu dni liczymy
//                tp - 0, 1, 2 ,3 sasiednie dni
//               oxy - true : XY, false : HD
//                Hm - srednioroczne H gdy oxy==false
//                K9 - 0 gdy z danych, ibin==true
//---------------------------------------------------------------------------
class KAsm
{public:
 KAsm ();
 ~KAsm ();
 void opt (const char* const c);
 void setK9 (const char* const c);
 void setHm (const char* const c);
 void setDays (const char* const c);
 void wsk (const string& s, const string& sout, const char* const cpth);
 private:
 void allK (const string& sout);
 void selK (const string sout);
 bool iplo,iupd,ibin,oxy,icmp,ilog;
 int tp,Days;
 int K9;
 real Hm;
 real scl;   // skalowanie wspolczynnikow wygladzania w rs1
 gfn* pgf;
 ofstream out;
 string obs,syear;
};
KAsm::KAsm () : iplo(false), iupd(false), ibin(false), icmp(false), ilog(false), tp(3), Days(-1)
               ,scl(1.)
{
}
KAsm::~KAsm ()
{
}
void KAsm::opt (const char* const c)
{bool r=false;
 switch (tolower(c[0]))
 {case 'p': iplo=true; break;
  case 'u': ibin=iupd=true; break;   // tylko w zbiorze binarnym mozna zmieniac K
  case 'b': ibin=true; break;
  case 'c': ibin=icmp=true; break;   // tylko ze zbiorem binarnym mozna porownywac wskazniki
  case '0': case '1': case '2': case '3': tp=c[0]-'0'; break;
  case 'l': ilog=true; break;        // piszemy na log dodatkowa informacje
  default : r=true; break;
 }
 if (r || strlen(c)!=1) {cerr<<"Error - unknown parameter : "<<'-'<<c<<'\n'; exit (0);}
}
                               // dla danych binarnych K9==0 oznacza, ze K9 ze zbioru
                               // po K9 moze byc : i mnoznik zmieniajacy wspolczynniki
                               // wygladzania dla obserwatorium
void KAsm::setK9 (const char* const c)
{istringstream ist(c);
 ist>>K9;
 if (!ist || K9<0) K9=-1;
 else
 {if (ist.peek()!=EOF)   // cos po K9
  {char d;
   ist>>d>>scl;
   if (!ist || d!=':' || scl<=0. || ist.peek()!=EOF) K9=-1;
  }
 }
// K9=((c[0]=='0' && (c[1]==0 || c[1]==':'))?0:atoi(c));
 if (K9==0) ibin=true;         // wymuszamy zbior binarny
 if (K9<0) {cerr<<"Error - K9 is incorrect : "<<c<<'\n'; exit (0);}
}
//              Hm : srednioroczne H lub -1 gdy z danych lub 0 gdy nie uzywane
void KAsm::setHm (const char* const c)
{const bool rHm=(c[0]=='0' && c[1]==0);
 if (rHm) {ibin=true; Hm=-1; oxy=false;}           // wymuszamy zbior binarny
 else
 {oxy=((c[0]=='x' || c[0]=='X') && (c[1]=='y' || c[1]=='Y') && c[2]==0);
                               // oxy - liczymy w ukladzie xy, Hm zbedne (Hm==0.)
                               // rHm - Hm bierzemy z danych  (Hm<0.)
  if (oxy) Hm=0.;
  else
  {Hm=atof(c);
   if (Hm<=0.) {cerr<<"Error - H mean value is incorrect : "<<c<<'\n'; exit (0);}
  }
 }
//cout<<(oxy?"XY":"HD")<<'\n';
}
void KAsm::setDays (const char* const c)
{Days=atoi(c);
 if (Days<=0) {cerr<<"Error - days parameter is wrong - incorrect number of days : "<<c<<'\n'; exit (0);}
}
//                       oxy to Hm - nie uzywane (Hm==0)
//                       Hm<0 to Hm nalezy wziasc z danych
void KAsm::allK (const string& sout)
{dmv dv[3];
 int idp=0, idc=1,idn=2;
 dv[idc].rdData (-1,*pgf); dv[idn].rdData (0,*pgf);
 K9=pgf->rdK9(K9);
 const int n=24*60,nb=3*60; const int nl=n+2*nb;
 real x[nl],y[nl],xd[nl],yd[nl];      // xd, yd : czesci zaburzone
 real* xx[2]={x,y}; real* yy[2]={xd,yd};
 bool up;
 {const int l=sout.length(); int j=-1; for (int i=0; i<l; ++i) if (sout[i]=='/' || sout[i]=='\\') j=i;
  for (++j; j<l && !(isalpha(sout[j]) && sout[j]==toupper(sout[j])); ++j) ;
  up=(j<l);
 }
 const string fsout=sout+(up?".DKA":".dka");
 ofstream dka (fsout.c_str(),ios_base::binary);
 if (!dka) {cerr<<"Error - unable to open a file (dka) : "<<fsout<<'\n'; exit(0);}
//    "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 "
 dka<<"                           "<<toupper(obs)<<"\r\n\r\n"
    <<"               K - Index Values for "<<setw(4)<<syear
    <<"     ( K9-limit = "<<K9<<"nT )\r\n\r\n"
    <<"   DA-MON-YR  DAY #    1    2    3    4      5    6    7    8       SK        \r\n\r\n";
 int wsk[9];
 const bool rHm=(Hm<0.);              // musimy czytac Hm ze zbioru
 const real scx=500./real(K9);        // dla H (nT)  lub X (nT)
 real scy=(Hm>0.?scx*Hm:scx);         // dla D (rad) lub Y (nT)
 const int nday=pgf->nDay();
 int dif[22],K[8];           //  19 - roznice, 1 - ilosc porownan, 2 - brakujace
 for (int i=0; i<22; ++i) dif[i]=0;
 for (int i=0; i<Days; ++i)
 {{const int id=idp; idp=idc; idc=idn; idn=id;}
  dv[idn].rdData (i+1,*pgf);
  if (dv[idc].isData())               // mozemy liczyc wskazniki
  {int ctp=0,m=0;
   if (dv[idp].getData(nb,x,y,n-nb)) {ctp+=1; m+=nb;}
   dv[idc].getData(n,x+m,y+m,0); m+=n;
   if (rHm)                           // mamy uklad hd i Hm ze zbioru
   {const real cHm=dv[idc].getHm();   // ze zbioru, gdy nie ma w zbiorze ???
                                      // musimy czytac ze zbioru dla kazdego dnia
                                      // bo moze sie zdarzyc, ze zmieniamy rok i Hm sie zmieni
    if (cHm<=0.) {cerr<<"Error - data file does not contain H mean value.\n"; exit (0);}
    scy=scx*cHm;
//cout<<"Hm ze zbioru danych : "<<cHm<<'\n';
   }
   if (dv[idn].getData(nb,x+m,y+m,0)) {ctp+=2; m+=nb;}
   const bool pd=(ctp==1||ctp==3);      // dzien poprzedni
   const int lvws=10,igap=6;          // lvws=nl/180
   int vws[lvws]={0,0,0,0,0,0,0,0,0,0};  // okresla uniewaznione wskazniki
   fits(m,x,vws,igap); fits(m,y,vws,igap);
   //const int ir0=fits(m,x,vws,igap),ir1=fits(m,y,vws,igap);
   norm (m,x,scx); norm (m,y,scy); // x w nT, y moze byc w radianach
   rs::rs1 (m,xx,yy,scl);    // yy - czesc zaburzona xx, QR
   wwsk (pd,vws,xd,yd,x,y,out,wsk);
   if (iupd) pgf->upd (i,wsk);
   const string fday=pgf->fDay(i);
//   out<<"    "<<ctp<<"  "<<fday<<'\n';
   //out<<"    "<<tp<<"  "<<dv[idc].fDay()<<'\n';
   dka<<"   "<<fday.substr(3,2)<<'-'<<toupper(fday.substr(0,3))<<'-'<<fday.substr(5,2)
      <<"   "<<setfill('0')<<setw(3)<<i+nday+1<<" "<<setfill(' ');
   for (int j=0; j<8; ++j) {if (j==4) dka<<"  "; dka<<setw(5)<<wsk[j];}
   dka<<setw(9)<<wsk[8];//<<'\n';
   if (icmp)               // mozna porownywac, jest rowniez ibin
   {bool g=false;
    dv[idc].GetK (K); dka<<"   "; // for (int j=0; j<8; ++j) dka<<setw(5)<<K[j];
    for (int j=0; j<8; ++j)
    {if (wsk[j]<0) ++dif[20]; else if (K[j]<0) ++dif[21]; else
     {++dif[19];
      int jd=wsk[j]-K[j]+9; if (jd<0) jd=0; else if (jd>18) jd=18;
      ++dif[jd]; if (abs(wsk[j]-K[j])>2) g=true;
      if (abs(wsk[j]-K[j])>0) dka<<setw(3)<<wsk[j]-K[j]; else dka<<"  .";
     }
    }
    if (g) dka<<"  !!!";
    dka<<"\r\n";
   }else dka<<"        \r\n";
  }else                    // brakuje danych dla biezacego dnia
  {
  }
 }
 if (icmp)
 {const real p=(dif[19]>0?100./real(dif[19]):1.);
  ios_base::fmtflags oflags=dka.setf(ios_base::fixed,ios_base::floatfield); dka.precision(1);
  dka<<"\r\n";
  for (int j=0; j<19; ++j)
  {dka<<setw(3)<<j-9<<setw(6)<<dif[j];
   if (dif[j]>0) dka<<setw(8)<<real(dif[j])*p<<" %\r\n"; else dka<<"\r\n";
  }
  dka<<"\r\n"; for (int j=19; j<22; ++j) dka<<setw(9)<<dif[j]<<"\r\n";
  dka.setf(oflags,ios_base::floatfield); dka.precision(6);  // domyslne
 }
}
void KAsm::selK (const string sout)
{const int n=24*60,nb=3*60; const int nl=n+2*nb;
 real cHm;
 int ctp=tp;
 int K[8];
 real x[nl],y[nl],xd[nl],yd[nl];                   // xd, yd : czesci zaburzone
 real* xx[2]={x,y}; real* yy[2]={xd,yd};
 int m=0;               // ilosc danych
 if (ctp==1 || ctp==3)
 {if (pgf->rd(-1,nb,x,y,n-nb)) m+=nb;
  else {ctp-=1; cerr<<"Warning - missing data related preceding day\n.";}
 }
 if (!pgf->rd(0,n,x+m,y+m,0)) {cerr<<"Error - unable to open a file containig input data.\n"; exit (0);}
 cHm=(Hm>0.?Hm:pgf->getHm()); pgf->GetK (K);
 m+=n;
 //if (Hm>0.) cHm=Hm;      // ignorujemy Hm z danych
 if (!oxy && cHm<=0.) {cerr<<"Error - data file does not contain H mean value.\n"; exit (0);}
 const real scx=500./real(pgf->rdK9(K9));  // dla H (nT)  lub X (nT)
 const real scy=(oxy?scx:scx*cHm);       // dla D (rad) lub Y (nT)
 if (ctp==2 || ctp==3)
 {if (pgf->rd(1,nb,x+m,y+m,0)) m+=nb;
  else {ctp-=2; cerr<<"Warning - missing data related succeeding day.\n";}
 }
 const bool pd=(ctp==1||ctp==3); // dzien poprzedni
// cout<<m<<'\n';
 const int lvws=10,igap=6;       // lvws=nl/180
 int vws[lvws]={0,0,0,0,0,0,0,0,0,0};  // okresla uniewaznione wskazniki
//for (int j=0; j<m; ++j) x[j]=vUNDEF; // test
//             gdy ciag x lub y ma same niezdefiniowane wartosci to fits wypenia zerami
 fits(m,x,vws,igap); fits(m,y,vws,igap);
 //const int ir0=fits(m,x,vws,igap),ir1=fits(m,y,vws,igap);
// if (ir0>1 || ir1>1) Err (0,"Brakuje zapisow na poczatku lub koncu danych.");
 ofstream out ((sout+".K").c_str(),ios::app);
 if (!out) {cerr<<"Error - unable to open a output file (K): "<<(sout+".K")<<'\n'; exit(0);}
 norm (m,x,scx); norm (m,y,scy); // dane sa w nT
 rs::rs1 (m,xx,yy,scl);    // yy - czesc zaburzona xx, QR
 int wsk[9];
 wwsk (pd,vws,xd,yd,x,y,out,wsk);
 const string fday=pgf->fDay(0);
 out<<"    "<<ctp<<"  "<<fday<<'\n';
 if (iplo)
 {{rwsk rw (sout);
   rw.bor (fday.c_str(),pd,xx,yy,vws,oxy,ibin?K:NULL);
  }
  RPS::rps (sout);
 }
}
void KAsm::wsk (const string& s, const string& sout, const char* const cpth)
{if (ilog)
 {logs.open ((sout+".log").c_str(),ios::app);
  if (!logs) {cerr<<"Error - unable to open a output file (log) : "<<(sout+".log")<<'\n'; exit(0);}
 }else logs.setstate(ios_base::failbit);   // nie uzywamy logs
 if (Days>0) out.setstate(ios_base::failbit);   // nie uzywamy outs
 else             // outs tylko gdy jeden dzien
 {out.open ((sout+".K").c_str(),ios::app);
  if (!out) {cerr<<"Error - unable to open a output file (K): "<<(sout+".K")<<'\n'; exit(0);}
 }
//                 jesli ostatni argument (path) jest pusty lub zakonczony '\' lub '/' (unix)
//                 to uznajemy ze to jest sciezka i dodajemy standardowa nazwe
 string pth=cpth; const int lpth=int(pth.size());
 if (lpth==0 || pth[lpth-1]=='\\' || pth[lpth-1]=='/') pth+=(ibin?"#o#Y#s.bin":"#s#D#Y.#o");
if (!!logs) logs<<pth<<'\n';
 if (ibin) pgf=new gfnbin(s,pth,oxy,iupd); else pgf=new gfntxt(s,pth,oxy);
 obs=s.substr(9,3); syear=s.substr(5,4);  // 01jan2001bel
 if (Days>0) allK (sout); else selK (sout);
 delete pgf;
}
//---------------------------------------------------------------------------
int main (const int argc, const char* const argv[])
{KAsm as;
//
 //cout<<"Liczba argumentow : "<<argc<<'\n';
 //for (int i=0; i<argc; ++i) cout<<argv[i]<<'\n';
 //cout<<"--------------------\n";
//
 int larg=0; const char** arg=new const char*[argc-1];
 for (int i=1; i<argc; ++i)
// if (argv[i][0]=='-' || argv[i][0]=='/') as.opt (argv[i]+1); else arg[larg++]=argv[i];
 if (argv[i][0]=='-') as.opt (argv[i]+1); else arg[larg++]=argv[i];
 if (larg!=4 && larg!=5)
 {cerr<<
"Usage :\n>KAsm obs:ddmmmyyyy[:days] {K9|0}[:smth] H|0|xy out [path] [-0|-1|-2|-3] [-b]\n"
"      [-p] [-c] [-u] [-l]\n"
"|      separates permissible values of parameter\n"
"[]     parameter is not obligatory\n"
"{}     encloses list of permissible parameter values\n"
"obs    3 char IAGA code, e.g. HLP\n" 
"dd     day of month (01..31), starting date\n"
"mmm    month (JAN..DEC), starting date\n"
"yyyy   year, starting date, e.g. 2001\n"
"days   number of days, obligatory, e.g. 365\n"
"       Note:  parameter 'days' is not allowed in case of use of switch -p\n"
"K9|0   K9 limit K9-index in nT, examples: 450, 700\n"
"       0 means that K9 stored in 1-min Intermagnet binary file(s) is to be used\n"
"smth   smoothing coefficient for the observatory, default 1.0\n"
"H|0|xy H annual value of H-component in nT, K-indices will be calculated\n"
"        from HD elements, e.g. 18720, 8245\n"
"       0 means that H stored  1-min Intermagnet binary file(s) is to be used,\n"
"        K-indices will be calculated from HD components\n"
"       xy K-indices will be calculated from XY components\n"
"out    name of output text file, this name will be supplemented with extensions\n"
"        'dka', 'K', 'plo', 'ps', 'log'\n"
"path   directory with input data, e.g. d:\\data\\binary\\, must end with backslash\n"
"-0|-1|-2|-3    -0 K's will be calculated from one day\n"
"               -1 K's calculated from preceding and current day\n"
"               -2 K's calculated from current and subsequent day\n"
"               -3 K's calculated from preceding, current and subsequent day\n"
"-b     program expects 1-min Intermagnet binary file(s)\n"
"-p     program creates figure *.ps, this option works under condition\n"
"        that parameter 'days' is missing\n"
"-c     current K's are compared with previous K's stored in 1-min Intermagnet\n"
"        binary file(s), result of comparison is placed in output text file\n"
"-u     current K's are stored to 1-min Intermagnet binary file(s)\n"
"-l     causes creating *.log file containing debug information\n"
"KAsm version 1.09 (09112015) Copyright (c) 2003-2015, Krzysztof Nowozynski.\n";
//"KAsm version 1.08 (19092011) Copyright (c) 2003-2011, Krzysztof Nowozynski.\n";
//"KAsm version 1.07 (24062011) Copyright (c) 2003-2011, Krzysztof Nowozynski.\n";
//"KAsm version 1.06 (10032011) Copyright (c) 2003-2011, Krzysztof Nowozynski.\n";
//"KAsm version 1.05 (09032011) Copyright (c) 2003-2011, Krzysztof Nowozynski.\n";
//"KAsm version 1.04 (08032011) Copyright (c) 2003-2011, Krzysztof Nowozynski.\n";
//"KAsm version 1.03 (25042007) Copyright (c) 2003-2007, Krzysztof Nowozynski.\n";
//"KAsm version 1.02 (19022004) Copyright (c) 2003-2004, Krzysztof Nowozynski.";
//"KAsm version 1.01 (18022004) Copyright (c) 2003-2004, Krzysztof Nowozynski.";
  exit (0);
 }
//                           data i nazwa obserwtorium
 const int ls=int(strlen(arg[0]));
 string s;
 bool r=false;                   // 0123456789 12345        0123456789 123456
 if (ls>=12 && arg[0][3]==':')   // bel:1jan2001:366   lub  bel:01jan2001:366
 {int l=9;
  if (!isdigit(arg[0][5])) {s+='0'; --l;} // jedna cyfra dla dnia
  if (ls>=l+4)
  {s.append(arg[0]+4,l); s.append (arg[0],3);
   for (int i=0; i<12; ++i) s[i]=tolower(s[i]);
   if (ls==l+4) r=true;
   else {if (ls>l+5 && arg[0][l+4]==':') {as.setDays (arg[0]+(l+5)); r=true;}}
  }
 }
 if (!r) {cerr<<"Error - obs parameter is incorrect : "<<arg[0]<<'\n'; exit (0);}
 as.setK9 (arg[1]);
 as.setHm (arg[2]);
 as.wsk (s,arg[3],larg==5?arg[4]:"");
 delete [] arg;
 return 0;
}
// imgf imf (argv[1],larg==7?argv[6]:"#S#D#Y.#o");
// const int itst=imf.test();
//cout<<"Test : "<<itst<<'\n';
//---------------------------------------------------------------------------
       // n>0, odejmujemy wartosc minimalna i skalujemy,
       //      wszystkie wartosci powinny byc zdefiniowane
void norm (const int n, real* const x, const real& s)
{real xm=x[0];
 for (int i=1; i<n; ++i) if (x[i]<xm) xm=x[i];
 for (int i=0; i<n; ++i) x[i]=s*(x[i]-xm);
}
//---------------------------------------------------------------------------
/*
int main (const int argc, const char* const argv[])
{
 //cout<<atan2(1.,4.)<<' '<<atan2(-1.,4.)<<' '<<atan2(-1.,-4.)<<' '<<atan2(1.,-4.)<<'\n';
 //exit (0);
 return 0;
}
*/
//---------------------------------------------------------------------------
