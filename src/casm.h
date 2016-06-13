//---------------------------------------------------------------------------
typedef double real;
extern const real UNDEF;   // testowanie czy wartosc niezdefiniowana
//---------------------------------------------------------------------------
class rs
{public:
 static void rs1 (int n, real* x[2], real* y[2], const real& scl=1.);
 private:
 static const real del;
 static const real dela;    // do liczenia wag
};
//---------------------------------------------------------------------------
typedef real* preal;
//---------------------------------------------------------------------------
//
//  [  c   s ]  [ a ]  [ sqrt(a*a+b*b) ]
//  [ -s   c ]  [ b ]  [     0         ]
//
class giv
{public:
 giv () : c(1.), s(0.) {}
 giv& operator () (real& a, real& b);
 real rot (const real& a, const real& b);
 private:
 real c,s;
};
//---------------------------------------------------------------------------
class slv
{public:
 slv (int n);
 void solve1 (const real& del, const real& dela, preal* x, preal* y);
 ~slv ();
 private:
 void sol (preal f, preal x);
 void sol (const real& rl, real* f[2], real* x[2]);
 void rx (preal f, preal x, const real& xl, const real& sa, const real& sb);
 void wg (preal y, const real& dela);
 void wg (preal* y, const real& dela);
// real ext (int n, preal x);
 int n;
 preal d,p,q,rl;
};
//---------------------------------------------------------------------------
void fit (int n, preal x);
//---------------------------------------------------------------------------
