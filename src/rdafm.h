//--------------------------------------------------------------------
#include <fstream>
using namespace std;
//--------------------------------------------------------------------
typedef double real;
typedef short int2;
typedef int2* pint2;
typedef char* pchar;
//--------------------------------------------------------------------
class bbox
{public:
 bbox ();
 void setb (bbox& b);
 void setb (int lx, int ly, int rx, int ry);
 int  getb (int& lx, int& ly, int& rx, int& ry);
 void clrb ();
 void addb (bbox& b);
 void addb (int lx, int ly, int rx, int ry);
 void addb (int nx, int ny, int r);
 void addb (bbox& b, const real& s, const real& x, const real& y);
 void rot (const real& si, const real& co);
 int gisb () const {return isb;}
 protected:
 int llx,lly,urx,ury;
 private:
 void xchg (int& a, int& b) {int i=a; a=b; b=i;}
 int isb;                  // czy kolejny bbox
};
//--------------------------------------------------------------------
class rdafm
{public:
 //rdafm (ifstream& inp);
 void lst (ofstream& out);
 const char* FontN () const {return fn;}
 int CharW (int c) const;
 int CharW (const char* c) const;
 int BBox (int c, bbox& b) const;
 //~rdafm ();
 //private:
 int lc,lfn;
 const char* fn;
 pint2 ec,wx,ifn,llx,lly,urx,ury;
};
//--------------------------------------------------------------------
typedef rdafm* prdafm;
class dafm;
typedef dafm* pdafm;
class dafm
{public:
 int CharW (int c, int ifo) const;
 int CharW (const char* c, int ifo) const;
 const char* FontN (int ifo)
                         const {return pa[(ifo>=0 && ifo<lf)?ifo:0].FontN ();}
 int FontL () const {return lf;}
 int BBox (int c, int ifo, bbox& b) const;
 //dafm (const char* fna);
 //~dafm ();
// static const dafm* use (const char* const fna="f:\\mprog\\src\\ps\\plo.afm");
 //static const dafm* use (const char* const fna="\\plo.afm");
 private:
 static const int lf;
 static const rdafm pa[];
};
//--------------------------------------------------------------------
