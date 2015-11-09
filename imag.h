//---------------------------------------------------------------------------
#include <fstream>
#include <string>
using namespace std;
typedef double real;
//---------------------------------------------------------------------------
extern const real UNDEF;   // testowanie czy wartosc niezdefiniowana
extern const real vUNDEF; // wartosc niezdefiniowana
//---------------------------------------------------------------------------
inline real sqr (const real& x) {return x*x;}
//---------------------------------------------------------------------------
typedef int int4;          // 32 bitowe slowo, do czytania binarnego
//---------------------------------------------------------------------------
int lMonth (const int month, const int year);
int relDay (const int day, const int month, const int year);
void dmDate (const int rday, const int year, int& day, int& month);
int absDay (const int day, const int month, const int year);
void dmyDate (int ndt[3], const int absday);
//---------------------------------------------------------------------------
class imgf
{public:
 imgf (const string& file, const string& templ);
 bool ef (const int ad, ifstream& inp);
 int test ();
 protected:
 static const string months[12];
 string path,obs;
 const string templ;
 string fday;        // nazwa (kanoniczna) ostatnio wczytanego dnia
 int ndt[3];      // ad z ef() jest zamienione nd dzien, miesiac i rok
 int aday;
 private:
};
//---------------------------------------------------------------------------
//                          Hm - jest potrzebne gdy oxy==false, to znaczy wynik w ukadzie HD
//                               dane binarne zawieraja Hm, dla danych tekstowych trzeba podac
class gfn
{public:
 gfn (const string& file, const string& templ, const bool oxy);
// int aDay () const {return aday;}
 int nDay () const {return nday;}
 virtual bool rd (const int rday, const int n, real* const x, real* const y,
                  const int nskp)=0;
//                jesli KK9>0 to sprawdzamy czy takie same w zbiorze (o ile istnieje)
//                jesli KK9<=0 to bierzemy K9 ze zbioru (musi istniec)
 virtual int rdK9 (const int KK9) {return 0;}
 const string fDay (const int rday);
// int getK9 () const {return K9;}
 virtual real getHm () const {return 0.;}
 virtual void GetK (int* const K) const {}
 virtual bool upd (const int rday, const int* const Ki) {return true;}
 virtual ~gfn () {}
 protected:
 bool ef (const int ad, fstream& inp);
 void trans (const bool xy, const int n, real* const x, real* const y);
 static const int m=24*60;
 int ndt[3];      // ad z ef() jest zamienione nd dzien, miesiac i rok
 int aday,nday;   // numer aday w roku (od 0)
 bool oxy;        // true gdy wynik ma byc w ukladzie xy
// ios_base::open_mode opm;
//std::_Ios_Openmode opm;
 ios_base::openmode opm;
 private:
 static const string months[12];
 string obs;
 const string templ;
 //string fday;        // nazwa (kanoniczna) ostatnio wczytanego dnia
};
//---------------------------------------------------------------------------
class gfnbin : public gfn
{public:
 gfnbin (const string& file, const string& templ, const bool oxy, const bool iupd)//=false)
       : gfn(file,templ,oxy), mon(0), yea(-1), K9(0)
 {if (iupd) opm|=ios_base::out;     // rowniez do pisania
 }
 bool rd (const int rday, const int n, real* const x, real* const y, const int nskp);
 int rdK9 (const int KK9);
 real getHm () const {return Hm;}
 void GetK (int* const K) const {for (int i=0; i<8; ++i) K[i]=gfnbin::K[i];}
 bool upd (const int rday, const int* const Ki);
 virtual ~gfnbin () {}
 private:
 int rd (fstream& inp, const int n, real* const x, real* const y, const int nskp);
 fstream inp;
 int mon,yea;               // w zbiorze inp
 int K9,K[8];     // z danych binarnych
 real Hm;         // z danych binarnych
};
//---------------------------------------------------------------------------
//                          Hm<=0. oznacza ze wynik w ukladzie XY i Hm nie jest potrzebne
class gfntxt : public gfn
{public:
 gfntxt (const string& file, const string& templ, const bool oxy) : gfn(file,templ,oxy)
 {
 }
 bool rd (fstream& inp, const int lw, char* const w);
 bool rd (const int rday, const int n, real* const x, real* const y, const int nskp);
 int rdK9 (const int KK9) {return KK9;}
 private:
 int rd (fstream& inp, const int n, real* const x, real* const y, const int nskp);
};
//---------------------------------------------------------------------------
//                      Hm==0. gdy czytamy ze zbioru tekstowego, gdzie nie ma danych
//                      gdy czytamy ze zbioru binarnego Hm jest obliczne,
//                      ??? jak jest zaznaczony brak Hm w zbiorze binarnym
class dmv
{public:
 dmv () : isRd(false), Hm(0.) {}
 void rdData (const int rday, gfn& gf)
 {isRd=gf.rd(rday,m,x,y,0); Hm=gf.getHm(); gf.GetK (K);}
 bool isData () const {return isRd;}
 bool getData (const int n, real* const xx, real* const yy, const int nskp);
 real getHm () const {return Hm;}
 void GetK (int* const K) const {for (int i=0; i<8; ++i) K[i]=dmv::K[i];}
 private:
 static const int m=24*60;
 real x[m],y[m];
 bool isRd;
 real Hm;          // Hm - ze zbioru danych (binarnych)
 int K[8];
 //string fday;
};
//---------------------------------------------------------------------------
