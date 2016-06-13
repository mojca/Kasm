//---------------------------------------------------------------------------
#include "imag.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
using namespace std;
extern ofstream logs;
const string toupper (const string& s);   //  w kasm.cpp
//---------------------------------------------------------------------------
const real UNDEF=999999.;   // testowanie czy wartosc niezdefiniowana
const real vUNDEF=2.*UNDEF; // wartosc niezdefiniowana
//---------------------------------------------------------------------------
const string imgf::months[12]={"jan","feb","mar","apr","may","jun",
                              "jul","aug","sep","oct","nov","dec"};
imgf::imgf (const string& file, const string& templ) : templ(templ)
{const int lf=int(file.size());
 if (lf<11 || file[lf-4]!='.')
  {cerr<<"Error - name of file is incorrect : "<<file<<'\n'; exit (0);}
 if (lf>11) path=file.substr(0,lf-11);
 obs=file.substr(lf-3);
 string mon; for (int j=0; j<3; ++j) mon+=tolower(file[lf-11+j]);
 int month=0; for (int j=0; j<12 && month==0; ++j) if (mon==months[j]) month=j+1;
 if (month==0) {cerr<<"Error - name of file (month) is incorrect : "<<file<<'\n'; exit (0);}
 int dy=0;
 for (int j=0; j<4 && dy>=0; ++j)
 {const char c=file[lf-8+j];
  if (isdigit(c)) dy=dy*10+(c-'0'); else dy=-1;
 }
 int day=dy/100;
 int year=dy%100;
 year+=(year<50?2000:1900);
 //cout<<path<<' '<<day<<' '<<month<<' '<<year<<' '<<obs<<'\n';
 if (day<=0 || day>lMonth(month,year)) {cerr<<"Error - day number is incorrect : "<<file<<'\n'; exit (0);}
 aday=absDay(day,month,year);
}
//                   szablon dla 1 feb 2003
//                   dzien : #d =    1
//                           #D =   01
//                           #a =    1 (dzien w roku numerowany od 1)
//                           #A =  001 (dzien w roku numerowany od 1)
//                 miesiac : #m =    2
//                           #M =   02
//                           #s =  feb
//                           #S =  FEB
//                     rok : #y =    3
//                           #Y =   03
//                           #f = 2003
//           obserwatorium : #o
//                           ad - absolutna numeracja dni
bool imgf::ef (const int ad, ifstream& inp)
{ostringstream ost;
 ost<<path;
 const int lt=int(templ.size());
 dmyDate (ndt,ad);
//if (!!logs) logs<<setw(8)<<ad<<setw(3)<<ndt[0]<<setw(3)<<ndt[1]<<setw(5)<<ndt[2]<<' ';
 int i=0;
 while (i<lt)
 {if (templ[i]!='#') ost<<templ[i];
  else
  {switch (templ[++i])
   {case 'd': ost<<ndt[0]; break;
    case 'D': ost<<setfill('0')<<setw(2)<<ndt[0]; break;
    case 'a': ost<<relDay(ndt[0],ndt[1],ndt[2]); break;
    case 'A': ost<<setfill('0')<<setw(3)<<relDay(ndt[0],ndt[1],ndt[2]); break;
    case 'm': ost<<ndt[1]; break;
    case 'M': ost<<setfill('0')<<setw(2)<<ndt[1]; break;
    case 's': ost<<months[ndt[1]-1]; break;
    case 'S': ost<<toupper(months[ndt[1]-1]); break;
    case 'y': ost<<(ndt[2]%100); break;
    case 'Y': ost<<setfill('0')<<setw(2)<<(ndt[2]%100); break;
    case 'f': ost<<ndt[2]; break;
    case 'o': ost<<obs; break;
    case 'O': ost<<toupper(obs); break;
    default : cerr<<"Error - skeleton is incorrect : "<<templ<<'\n'; exit (0);
   }
  }
  ++i;
 }
// if (!!logs) logs<<ost.str()<<'\n';
 inp.open (ost.str().c_str(),ios_base::in|ios_base::binary);
 if (!inp) {cerr<<"Warning - unable to open a file : "<<ost.str()<<'\n';}// exit (0);}
 ost.str("");
 ost<<months[ndt[1]-1]<<setfill('0')<<setw(2)<<ndt[0]<<setw(2)<<(ndt[2]%100)
    <<'.'<<obs;               //"#S#D#Y.#o"
 fday=ost.str();
 return !!inp;
}
//                      testuje typ zbioru
//                      1 - tekst
//                      0 - bin
//                     -1 - nieznany
//                     -2 - blad otwarcia
//                     -3 - blad czytania
int imgf::test ()
{ifstream inp;
 int r=(ef(aday,inp)?0:-2);
 if (r==0)    // zbior otwarty;
 {char b[4];  // pierwsze cztery znaki zbioru
  inp.read (b,sizeof(b));
              // wczytany, b : " obs" lub "obs "
  r=(!inp?-3:(b[0]==' '?0:(b[3]==' '?1:-1)));
 }
 return r;
}
//---------------------------------------------------------------------------
bool gfnbin::upd (const int rday, const int* const Ki)
{int ndt[3];
 dmyDate (ndt,aday+rday);
 int4 K4[8]; for (int i=0; i<8; ++i) K4[i]=(Ki[i]<0?999:Ki[i]*10);
 bool r;
 if (mon==ndt[1] && yea==ndt[2]) // zbior danych otwarty
 {inp.seekp (((ndt[0]-1)*5888+5876)*sizeof(int4));
  inp.write (reinterpret_cast<char* const>(K4),sizeof(K4));
  r=!!inp;
 }else                           // potrzebny nam zbior juz zostal zamkniety
 {fstream inp;
  r=ef(aday+rday,inp);        // otwieramy zbior z miesiecznym zapisem
  inp.seekp (((ndt[0]-1)*5888+5876)*sizeof(int4));
  inp.write (reinterpret_cast<char* const>(K4),sizeof(K4));
  r=!!inp;
 }
// cout<<setw(3)<<ndt[0]<<setw(3)<<ndt[1]<<setw(5)<<ndt[2]<<"   ";
// for (int i=0; i<8; ++i) cout<<setw(3)<<Ki[i]; cout<<'\n';
 if (!r) cerr<<"Writting error of updating K-indices to binary file.\n";
 return r;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool gfnbin::rd (const int rday, const int n, real* const x, real* const y, const int nskp)
{dmyDate (ndt,aday+rday);
 if (mon!=ndt[1] || yea!=ndt[2]) // nowy zbior danych 
 {inp.close (); inp.clear ();
  ef (aday+rday,inp);        // otwieramy zbior z miesiecznym zapisem
  mon=ndt[1]; yea=ndt[2];
 }
 if (!!inp) if (rd(inp,n,x,y,nskp)!=0) {cerr<<"Reading error from input file.\n"; exit (0);}
//cout<<"\nbin\n"; for (int i=0; i<n; ++i) cout<<setw(4)<<i<<setw(14)<<x[i]-x[0]<<setw(14)<<y[i]-y[0]<<'\n';
 return !!inp;
}
//---------------------------------------------------------------------------
//                           czytamy binarne dane intermagnetu
//                           dzien jest w ndt[0]
//                           nskp>=0
int gfnbin::rd (fstream& inp, const int n, real* const x, real* const y, const int nskp)
{int4 hdr[16],dat[m];
 inp.seekg ((ndt[0]-1)*5888*sizeof(int4));
 const int l=min(n,m-nskp);
 if (!!inp)
 {inp.read(reinterpret_cast<char* const>(hdr),sizeof(hdr));
  if (!!inp)
  {Hm=real(hdr[7])*.3438;
   if (K9>0 && hdr[10]>0 && K9!=hdr[10]) {cerr<<"Warning - K9 value is not fixed.\n";}
   K9=hdr[10];
   const int lic=4;
   string icomp(reinterpret_cast<char* const>(hdr+5),lic); // te skladowe sa w danych
   for (int i=0; i<lic; ++i) icomp[i]=toupper(icomp[i]);   // HDZF XYZF HDZG XYZG " HDZ" " XYZ"
   const int inc=(icomp[0]==' '?1:0);
//cout<<"Hm : "<<Hm<<' '<<icomp<<"  K9 : "<<K9<<'\n';
   const bool xy=(icomp[0+inc]=='X' && icomp[1+inc]=='Y');
   if (!xy && !(icomp[0+inc]=='H' && icomp[1+inc]=='D'))
    {cerr<<"Error - coordinate system is unknown (should be XYZF or HDZF) : "<<icomp<<'\n'; exit (0);}
   real sdg[2]={.1,.1};
   if (!xy) sdg[1]=1./34377.46770784939; // z dziesiatek minut na radiany
   //if (!xy) sdg[1]=(oxy?1.:Hm)/34377.46770784939;
   inp.read(reinterpret_cast<char* const>(dat),sizeof(dat));
   if (!!inp)
   {for (int j=0; j<l; ++j) x[j]=(dat[j+nskp]==999999?vUNDEF:real(dat[j+nskp])*sdg[0]);
    inp.read(reinterpret_cast<char* const>(dat),sizeof(dat));
    if (!!inp)
    {for (int j=0; j<l; ++j) y[j]=(dat[j+nskp]==999999?vUNDEF:real(dat[j+nskp])*sdg[1]);
//for (int j=0; j<l; ++j) cout<<setw(4)<<j<<setw(12)<<dat[j+nskp]<<'\n';
     trans (xy,l,x,y);
     int4 K4[8];
     inp.seekg(((ndt[0]-1)*5888+5876)*sizeof(int4)); inp.read(reinterpret_cast<char* const>(K4),sizeof(K4));
     if (!!inp) {for (int j=0; j<8; ++j) K[j]=(K4[j]==999?-1:K4[j]/10);}
     //int4 K[12];
     //inp.seekg(((ndt[0]-1)*5888+5872)*sizeof(int4)); inp.read(reinterpret_cast<char* const>(K),sizeof(K));
     //if (inp) {for (int j=0; j<12; ++j) cout<<setw(j<4?7:3)<<(K[j]==999?-1:K[j]/10); cout<<' '<<hdr[10]<<'\n';}
    }
   }
  }
 }
 return (!inp?1:0);  // 1 - blad czytania danych
}
//---------------------------------------------------------------------------
bool gfntxt::rd (const int rday, const int n, real* const x, real* const y, const int nskp)
{fstream inp;
 const bool r=ef (aday+rday,inp);        // otwieramy zbior
 if (r) if (rd(inp,n,x,y,nskp)!=0) {cerr<<"Reading error from input file.\n"; exit (0);}
//cout<<"\ntxt\n"; for (int i=0; i<n; ++i) cout<<setw(4)<<i<<setw(14)<<x[i]-x[0]<<setw(14)<<y[i]-y[0]<<'\n';
 return r;
}
// wiersz moze miec lw-2 znaki nastenie \r\n albo \n
bool gfntxt::rd (fstream& inp, const int lw, char* const w)
{bool r=false;
 if (inp.read(w,lw-1))
 {if (w[lw-2]=='\n') {w[lw-2]='\r'; w[lw-1]='\n'; r=true;}
  else if (w[lw-2]=='\r' && inp.read(w+(lw-1),1)) r=(w[lw-1]=='\n');
 }
 return r;
}
//                           opuszczamy nskp liczb a nastepnie
//                           wczytujemy n liczb do tablic
//                           x[n], y[n] w nT
int gfntxt::rd (fstream& inp, const int n, real* const x, real* const y, const int nskp)
{enum {IDC=0,D=4,DOY=12,H=16,COMP=19,T=24,GIN=26,COLALONG=30,DECBAS=39,R=46};
 const int lh=9,ld=8;
                             // polozenie spacji w naglowku
 static const int psh[lh]={D-1,DOY-1,H-1,COMP-1,T-1,GIN-1,COLALONG-1,DECBAS-1,R-1};
 static const int psd[ld]={7,15,23,30,39,47,55,62}; // polozenie spacji konczacej liczbe
 real dg[8],sdg[8]={.1,.1,.1,.1,.1,.1,.1,.1};
 bool xy=true;               // uklad wspolrzednych
 int l=-nskp;                // kolejne dane
 const int lw=64;            // dlugosc wiersza
 char h[lw],w[lw];           // linia naglowka i wiersza
 int hour=0;
 int r=0;
// if (!inp.read(h,lw)) r=1;   // blad czytania
 if (!rd(inp,lw,h)) r=1;
 else                        // test poprawnosci naglowka
 {if (h[lw-2]!='\r' || h[lw-1]!='\n') r=2; // CrLf konczy wiersz
  for (int j=0; j<lh && r==0; ++j) if (h[psh[j]]!=' ') r=3;
  if ((h[H]!=' ' && h[H]!='0') || h[H+1]!='0') r=3;   // godzina 0
                             // skladowa D wyrazamy w nT, 10*60*180/pi
                             // lub gdy wynik w ukladzie xy to w radianach
  //if (h[COMP]=='H' || h[COMP]=='h') {xy=false; sdg[1]=sdg[5]=(oxy?1.:Hm)/34377.46770784939;}
                             // skladowa D jest w setkach minut !!!
                             // skladowa D wyrazamy w radianach, 100*60*180/pi
  if (h[COMP]=='H' || h[COMP]=='h') {xy=false; sdg[1]=sdg[5]=1./343774.6770784939;}
 }
 for (int i=1; i<24*31 && r==0; ++i)
 {//if (!inp.read(w,lw)) r=1;   // blad czytania
  if (!rd(inp,lw,w)) r=1;
   else if (w[lw-2]!='\r' || w[lw-1]!='\n') r=2; // CrLf konczy wiersz
  if (r==0)
  {w[lw-2]=' '; w[lw-1]=0;
   if (i%31==0)               // naglowek
   {++hour;                   // test poprawnosci naglowka
    h[H]='0'+hour/10; h[H+1]='0'+hour%10; if (w[H]==' ') w[H]='0';
    for (int j=0; j<lw-2 && r==0; ++j) if (w[j]!=h[j]) r=3;
   }else                      // wiersz danych
   {if (w[31]!=' ') r=4;      // blad wiersza danych
    for (int k=0,j=0; j<ld && r==0; ++j) // wczytujemy 8 liczb
    {while (k<lw && w[k]==' ') ++k;  // opuszczamy spacje
     char s='+';
     if (w[k]=='+' || w[k]=='-') s=w[k++]; // znak liczby
     int v=0; while (k<lw && isdigit(w[k])) v=v*10+(w[k++]-'0');
     if (s=='-') v=-v;
     if (w[k]!=' ' || k!=psd[j]) r=4; else dg[j]=(v==999999||v==9999999?vUNDEF:real(v)*sdg[j]);
    }
    if (r==0)
    {if (l>=0 && l<n) {x[l]=dg[0]; y[l]=dg[1];} ++l; 
     if (l>=0 && l<n) {x[l]=dg[4]; y[l]=dg[5];} ++l; 
    }
   }
  }
 }
 const int nl=min(l,n);
 if (r==0 && nl>0) trans (xy,nl,x,y);
 return r;
}
//                    r : 0 - ok
//                        1 - blad czytania wiersza
//                        2 - wiersz nie jest zakonczony CrLf
//                        3 - blad wiersza nalowka
//                        4 - blad wiersza danych
//---------------------------------------------------------------------------
const string gfn::months[12]={"jan","feb","mar","apr","may","jun",
                              "jul","aug","sep","oct","nov","dec"};
//                     s.size()==12
gfn::gfn (const string& s, const string& templ, const bool oxy) : oxy(oxy), templ(templ)
{if (sizeof(int4)!=4) {cerr<<"Error - expected size of int4 should be 4, but is : "<<sizeof(int4)<<'\n'; exit (0);}
 opm=ios_base::in|ios_base::binary;   // do czytania
 obs=s.substr(9,3);     // 01jul2000bel
 int day=0,month=0,year=0;
 string mon=s.substr(2,3);
 for (int j=0; j<12 && month==0; ++j) if (mon==months[j]) month=j+1;
 bool r=true; for (int i=0; i<3 && r; ++i) r=(isalpha(obs[i])!=0);
 for (int j=0; j<4 && r; ++j) {r=(isdigit(s[5+j])!=0); if (r) year=10*year+(s[5+j]-'0');}
 for (int j=0; j<2 && r; ++j) {r=(isdigit(s[j])!=0); if (r) day=10*day+(s[j]-'0');}
 r=(r && month>0 && year>999 && day>0 && day<=lMonth(month,year));
 if (!r) {cerr<<"Error - date is incorrect : "<<s<<'\n'; exit (0);}
 aday=absDay(day,month,year);
 nday=aday-absDay(1,1,year);
}
const string gfn::fDay (const int rday)
{int ndt[3]; dmyDate (ndt,aday+rday);
 ostringstream ost;
 ost<<months[ndt[1]-1]<<setfill('0')<<setw(2)<<ndt[0]<<setw(2)<<(ndt[2]%100)<<'.'<<obs;               //"#S#D#Y.#o"
 return ost.str();
}
//                   szablon dla 1 feb 2003
//                   dzien : #d =    1
//                           #D =   01
//                           #a =    1 (dzien w roku numerowany od 1)
//                           #A =  001 (dzien w roku numerowany od 1)
//                 miesiac : #m =    2
//                           #M =   02
//                           #s =  feb
//                           #S =  FEB
//                     rok : #y =    3
//                           #Y =   03
//                           #f = 2003
//           obserwatorium : #o
//                           ad - absolutna numeracja dni
bool gfn::ef (const int ad, fstream& inp)
{ostringstream ost;
 //ost<<path;
 const int lt=int(templ.size());
 dmyDate (ndt,ad);
if (!!logs) logs<<setw(8)<<ad<<setw(3)<<ndt[0]<<setw(3)<<ndt[1]<<setw(5)<<ndt[2]<<' ';
 int i=0;
 while (i<lt)
 {if (templ[i]!='#') ost<<templ[i];
  else
  {switch (templ[++i])
   {case 'd': ost<<ndt[0]; break;
    case 'D': ost<<setfill('0')<<setw(2)<<ndt[0]; break;
    case 'a': ost<<relDay(ndt[0],ndt[1],ndt[2]); break;
    case 'A': ost<<setfill('0')<<setw(3)<<relDay(ndt[0],ndt[1],ndt[2]); break;
    case 'm': ost<<ndt[1]; break;
    case 'M': ost<<setfill('0')<<setw(2)<<ndt[1]; break;
    case 's': ost<<months[ndt[1]-1]; break;
    case 'S': ost<<toupper(months[ndt[1]-1]); break;
    case 'y': ost<<(ndt[2]%100); break;
    case 'Y': ost<<setfill('0')<<setw(2)<<(ndt[2]%100); break;
    case 'f': ost<<ndt[2]; break;
    case 'o': ost<<obs; break;
    case 'O': ost<<toupper(obs); break;
    default : cerr<<"Error - skeleton is incorrect : "<<templ<<'\n'; exit (0);
   }
  }
  ++i;
 }
 if (!!logs) logs<<ost.str()<<'\n';
 inp.open (ost.str().c_str(),opm);
 if (!inp) {cerr<<"Warning -  unable to open a file : "<<ost.str()<<'\n';}// exit (0);}
 return !!inp;
}
void gfn::trans (const bool xy, const int n, real* const x, real* const y)
{if (xy!=oxy)                // rozne uklady wspolrzednych
 {if (xy)                            // z xy na hd
  {for (int j=0; j<n; ++j)
   {if (x[j]>=UNDEF || y[j]>=UNDEF) x[j]=y[j]=vUNDEF;
    else
    {const real h=sqrt(sqr(x[j])+sqr(y[j]));
     y[j]=atan2(y[j],x[j]);        // D w radianach, problem gdy x[j] ujemne i y[j] zmienia znak, 
     //y[j]=atan2(y[j],x[j])*Hm;        // D w nT, problem gdy x[j] ujemne i y[j] zmienia znak, 
     x[j]=h;                          //         bo |kat| w poblizu 2*pi i sa skoki rzedu 2*pi
                                     // gdy y[j] nie zmienia znaku to D ma staly znak i zmiany
                                     // D sa male
                                     // moze wystarczy brac abs(x[j]) ???
                                     // uzywanie D ma sens tylko wtedy gdy jego zmiany sa male
                                     // czyli |x[j]|>>|y[j]| ???
//   ewentualnie "poprawic" nieciaglosc D (y[j]), problem gdy sa luki w zapisie
    }
   }
  }else                              // z hd(w radianach) na xy
  {for (int j=0; j<n; ++j)
   {if (x[j]>=UNDEF || y[j]>=UNDEF) x[j]=y[j]=vUNDEF;
    else
    {const real xx=x[j];  // H
     x[j]*=cos(y[j]);
     y[j]=xx*sin(y[j]); 
    }
   } 
  }
 }
}
int gfnbin::rdK9 (const int KK9)
{if (KK9==0 && K9==0) {cerr<<"Error - binary input file does not contain K9 value.\n"; exit (0);}
 if (KK9>0 && K9>0 && KK9!=K9)
   cerr<<"K9 from binary input file "<<K9<<" (ignored) is other than from command line "
       <<KK9<<" (used).\n";
  return (KK9>0?KK9:K9);
}
//int gfnbin::rdK9 (const int KK9)
//{if (KK9==0 && K9==0) {cerr<<"Error - binary input file does not contain K9 value.\n"; exit (0);}
// if (KK9>0 && K9>0 && KK9!=K9)
//  {cerr<<"K9 from binary input file "<<K9<<" is other than from command line "<<KK9<<'\n'; exit(0);}
// return (K9>0?K9:KK9);
//}
//---------------------------------------------------------------------------
                    // nskp >= 0
bool dmv::getData (const int n, real* const xx, real* const yy, const int nskp)
{if (isRd)
 {const int l=min(n,m-nskp);
  for (int i=0; i<l; ++i) xx[i]=x[nskp+i];
  for (int i=0; i<l; ++i) yy[i]=y[nskp+i];
 }
 return isRd;
}
//---------------------------------------------------------------------------
         // ilosc dni w miesiacu danego roku
 int lMonth (const int month, const int year)
 {const int y=year+(month-1)/12,m=(month-1)%12+1;
  return (489*m+489)/16-(489*m)/16-(m+14)/16+(m+13)/16
       +(month==2?-(y%4+3)/4-(100-y%100)/100+(400-y%400)/400:0);
 }
        // zamieniamy day, month, year na numer dnia w roku liczony od 1
 int relDay (const int day, const int month, const int year)
 {const int b=(month+13)/16; const int a=year+b;
  return day-b*(1+(year%4+3)/4)-(3*((a+99)/100))/4+(489*month)/16-30
         +(3*((year+99)/100))/4;
 }
         // odwrotnie do relDay, zamieniamy numeracje dnia w roku na day, month
         // rday - numer dnia w roku year liczony od 1
         // day, month - dzien, miesiac w roku year
 void dmDate (const int rday, const int year, int& day, int& month)
 {int iy=(4-year%4)/4-(100-year%100)/100+(400-year%400)/400;
  int id=(rday+305)%(365+iy);         // nr dnia w roku liczony od 1 marca (od zera)
  int a=id+id+60*(id/153);
  int b=a/61;
  day=(a%61)/2+1;
  month=(b-b/6+2)%12+1;
 }
         // zamieniamy day, month, year na dzien liczony
         // od 1 marca 0 roku, absolutna numeracje dni zaczynamy od zera
 int absDay (const int day, const int month, const int year)
 {const int base=-91;   // dzien 0 odpowiada 1 marca 0 roku
  const int b=(month+13)/16; const int a=year+b;
  return 365*year+day-2*b+(a+3)/4-(3*((a+99)/100))/4+(489*month)/16+base;
 }
         // wykorzystujemy pierwsze 3 elementy tablicy ndt
         // zamieniamy day, month, year z tablicy ndt na dzien liczony
         // od 1 marca 0 roku, absolutna numeracje dni zaczynamy od zera
 int absDay (const int ndt[3])
 {const int base=-91;   // dzien 0 odpowiada 1 marca 0 roku
  int b=(ndt[1]+9)/12;
  int a=ndt[2]+b;
  int c=ndt[0]-b-(3*a)/4-(3*((a+99)/100))/4+(275*ndt[1])/9+base;
  return 366*ndt[2]+c;
 }
         // wyznaczamy pierwsze 3 elementy tablicy ndt
         // odwrotnie do absDay, zamieniamy absolutna numeracje dni
         // na day, month, year i wpisujemy do tablicy ndt
 void dmyDate (int ndt[3], const int absday)
 {int q=(absday%146097)/36524;        // 0<=q<=4
  int d1=absday+(absday/146097)*3+q-q/4;
  q=(d1%1461)/365; q-=q/4;            // 0<=q<4
  int y=(d1/1461)*4+q;                // rok-1 liczony od 1 marca
  int id=d1%1461-q*365;               // nr dnia w roku
  ndt[2]=y+id/306;                    // rok
  int a=id+id+60*(id/153);
  int b=a/61;
  ndt[0]=(a%61)/2+1;
  ndt[1]=(b-b/6+2)%12+1;
 }
//---------------------------------------------------------------------------
