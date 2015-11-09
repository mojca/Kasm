#include "rdafm.h"
#include <cstring>
#include <iomanip>
using namespace std;
extern int idbg;
extern ofstream dbg;
//--------------------------------------------------------------------
void Err (const char* const mes, const int ier);
//--------------------------------------------------------------------
bbox::bbox ()
{clrb ();
}
void bbox::setb (bbox& b)
{if (b.isb)
 {isb=1;
  llx=b.llx; lly=b.lly; urx=b.urx; ury=b.ury;
 }
}
void bbox::setb (int lx, int ly, int rx, int ry)
{isb=1;
 llx=lx; lly=ly; urx=rx; ury=ry;
}
int bbox::getb (int& lx, int& ly, int& rx, int& ry)
{lx=llx; ly=lly; rx=urx; ry=ury;
 return isb;
}
void bbox::clrb ()
{isb=0;
 llx=lly=0; urx=ury=0;
}
void bbox::addb (bbox& b)
{if (b.isb)
 {if (isb)
  {if (b.llx<llx) llx=b.llx;
   if (b.lly<lly) lly=b.lly;
   if (b.urx>urx) urx=b.urx;
   if (b.ury>ury) ury=b.ury;
  }else setb (b);
 }
}
void bbox::addb (bbox& b, const real& s, const real& x, const real& y)
{if (b.isb) addb (int(x+b.llx*s),int(y+b.lly*s),int(x+b.urx*s),int(y+b.ury*s));
}
void bbox::addb (int lx, int ly, int rx, int ry)
{if (isb)
 {if (lx<llx) llx=lx;
  if (ly<lly) lly=ly;
  if (rx>urx) urx=rx;
  if (ry>ury) ury=ry;
 }else setb (lx,ly,rx,ry);
}
void bbox::addb (int nx, int ny, int r)
{if (isb) addb (nx-r,ny-r,nx+r,ny+r); else setb (nx-r,ny-r,nx+r,ny+r);
}
void bbox::rot (const real& si, const real& co)
{int n[4]={llx,urx,lly,ury},i=co<0.?1:0,j=si<0.?1:0;
 llx=int(co*n[i]-si*n[3-j]+.5);
 urx=int(co*n[1-i]-si*n[2+j]+.5);
 lly=int(co*n[2+i]+si*n[j]+.5);
 ury=int(co*n[3-i]+si*n[1-j]+.5);
}
//--------------------------------------------------------------------
int rdafm::CharW (int c) const
{return (c>=0 && c<256 && ec[c]>=0)?wx[ec[c]]:0;
}
int rdafm::CharW (const char* c) const
{int ia=0,ib=lc-1,is,i;
 do
 {is=(ia+ib)/2;
  i=strcmp (c,fn+ifn[is]);
  if (i<0) ib=is-1; else ia=is+1;
 }while (i!=0 && ia<ib);
 return i==0?wx[is]:0;
}
int rdafm::BBox (int c, bbox& b) const
{int ir=(c>=0 && c<255 && ec[c]>=0);
 if (ir)
 {int i=ec[c];
  b.setb (llx[i],lly[i],urx[i],ury[i]);
 }else b.clrb ();
 return ir;
}
void rdafm::lst (ofstream& out)
{static int ifo=0;
 out<<setw(4)<<ifo<<' '<<'"'<<'/'<<fn<<'"'<<','<<endl; ++ifo;
 return;
 out<<"------------------------"<<endl;
 out<<"Font Name : "<<fn<<setw(6)<<lc<<endl;
 out<<"------------------------"<<endl;
 int i;
 for (i=0; i<256; ++i) {out<<setw(4)<<ec[i]; if (((i+1)%32)==0) out<<endl;}
 for (i=0; i<lc; ++i)
 {out<<setw(5)<<i<<setw(5)<<" "<<setw(5)<<wx[i]<<setw(5)
     <<setw(5)<<llx[i]<<setw(5)<<lly[i]<<setw(5)<<urx[i]<<setw(5)<<ury[i]
     <<' '<<(fn+ifn[i])<<endl;
 }
 if (idbg) dbg<<endl<<setw(5)<<CharW('a')<<setw(5)<<CharW("a")<<setw(5)<<CharW("abc");
 if (CharW('a')!=CharW("a")) if (idbg) dbg<<"  "<<fn;
}
//--------------------------------------------------------------------
int dafm::CharW (int c, int ifo) const
{//cerr<<endl<<"CharW "<<c<<" "<<ifo;
 return pa[(ifo>=0 && ifo<lf)?ifo:0].CharW (c);
}
int dafm::CharW (const char* c, int ifo) const
{return pa[(ifo>=0 && ifo<lf)?ifo:0].CharW (c);
}
int dafm::BBox (int c, int ifo, bbox& b) const
{return pa[(ifo>=0 && ifo<lf)?ifo:0].BBox (c,b);
}
#include "dafm.h"
