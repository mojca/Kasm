//---------------------------------------------------------------------------
#include "ps.h"
#include <string>
//---------------------------------------------------------------------------
class rwsk
{public:
 rwsk (const std::string& sout) : plo ((sout+".plo").c_str()) {}
 void bor (const char* const mes, const bool pd, const real* const x[2],
           const real* const y[2], const int* const vws, const bool oxy, int kd[8]);
 private:
 real extr (const int n, const real* const x, real& xi, real& xa);
 void val (const real& y, real dkf[8], const real& axf);
 void ska (const real& x, const real& h, const real& hi);
 void pkid (const char* const lab, const real& y, const int ki[8],
            const real& axf);
 void difr (const real& y, const int ka[8], const int kb[7], const real& axf);
 void rys (const int n, const real* const x, const real* const y,
           const real& xs, const real& ys);
 vects plo;
};
//---------------------------------------------------------------------------
