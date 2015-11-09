#include <fstream>
using namespace std;
#include "rdafm.h"
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
//--------------------------------------------------------------------
class fso
{public:
 fso (const char* fna);
 int isope () const {return iso;}
 streampos alloc (int n);
 void set (streampos sp, int l);
 void wri (streampos sp, int n, const int1* b);
 void rea (streampos sp, int n, pint1 b);
 ~fso ();
 private:
 int iso;
 streampos free;        // adres wolnego miejsca na dysku
 streampos fsp; int lg; // adres pierwszego strumienia, dlugosc
 fstream out;
 void clo ();
};
//--------------------------------------------------------------------
typedef fso* pfso;
class strm;
typedef strm* pstrm;
//--------------------------------------------------------------------
class link
{public:
 void ope ();
 void ope (strm& s, pstrm ps);
 protected:
 void clo (pstrm up);
 private:
 pstrm prev,next,last;
};
//--------------------------------------------------------------------
class copt
{public:
 copt ();
 void init_copt ();
 int wopt (int ival, int iopt);
 int ropt (int iopt);
 ~copt ();
 private:
 const int lopt;
 pint val;
 pint sval;
};
//--------------------------------------------------------------------
//class strm : private link         // nie dziala z icc
class strm : protected link, public copt
{public:
 strm ();
 strm (const char* fna);
 strm (strm& s);
 virtual void ope (const char* fna);
 virtual void ope (strm& s);
 int isope () const {return iso;}
 int gnas () const {return nas;}
 int res (int n);
 void wri (int n, const int1* c);
 strm& wri1 (int d) {int1 i1=int1(d); wri (1,cpint1(&i1)); return *this;}
 strm& wri2 (int d) {int2 i2=int2(d); wri (2,cpint1(&i2)); return *this;}
 strm& wri4 (int d) {int4 i4=int4(d); wri (4,cpint1(&i4)); return *this;}
 void rew (int hps, int n, const int1* c);
 void rewc (int hps, int n, const int1* c);
 strm& rewc1 (int hps, int d)
                    {int1 i1=int1(d); rewc (hps,1,cpint1(&i1)); return *this;}
 strm& rewc2 (int hps, int d)
                    {int2 i2=int2(d); rewc (hps,2,cpint1(&i2)); return *this;}
 strm& rewc4 (int hps, int d)
                    {int4 i4=int4(d); rewc (hps,4,cpint1(&i4)); return *this;}
 void rew1 (int hps, int d) {int1 i1=int1(d); rew (hps,1,cpint1(&i1));}
 void rew2 (int hps, int d) {int2 i2=int2(d); rew (hps,2,cpint1(&i2));}
 void rew4 (int hps, int d) {int4 i4=int4(d); rew (hps,4,cpint1(&i4));}
// void wro (int4 op4, int1 cop, int nop);
 int wopt (int ival, int iopt);         // zaslania nazwe w copt
 virtual void clo ();
 virtual ~strm ();
 protected:
 pstrm up;         // up==NULL - poczatek drzewa
 private:
 int iso;
 pfso pfs;
                   //
 streampos sp;     // adres poczatku strumienia na dysku
 pint1 b;          // podwojny bufor
 const int lg;     // dlugosc bufora
 streampos spa;
 int lpb,l;

 int lhd;
 
 pint h;
 const int lh,lhe;
 int ih;
                   //
 int hs,nps,nas,hnps;  // nps - tyle zostalo przylaczonych strumieni
                       // nas - tyle jest w tej chwili przylaczonych strumieni
                       
 void init_strm ();
 void inib ();
 void wrhd (int n, streampos s);
 void rwhd (int n);
 void cpb (int2 i2);
 void cpb (int4 i4);
 void cpb (int n, const int1* c);
 void cpb (int n, int1 d);
 void wrs (int n, const int1* c=NULL);
 void wrb (int ng);
 void rwb (int hps, int n, const int1* c);
 void cls ();
};
//-----------------------------------------------------------------------------
class xstrm;
typedef xstrm* pxstrm;
class xstrm : public strm, public bbox
{public:
 xstrm (const char* fna, const dafm* pafm);
 xstrm (xstrm& xs);
 void ope (const char* fna);    // virt
 void ope (strm& s);            // virt
 void clo ();                   // virt
 ~xstrm ();                     // virt
 void mrk (int l);
 private:
 void init_xstrm ();
 void inix ();
 int lfo;                  // ilosc mozliwych fontow
 pint2 tfo;                // tablica uzytych fontow
 const dafm* pafm;
};
