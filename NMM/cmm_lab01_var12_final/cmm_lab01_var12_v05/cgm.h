#ifndef CGM_H_INCLUDED
#define CGM_H_INCLUDED

#include <cmath>

class CGM
{
public:
    void init(unsigned int *gi_s, unsigned int *gj_s, double *di_s, double *gg_s, double *rp_s, unsigned int n_s);
    void solve(double *&solution);

private:
    void make_LLT_decomposition();
    void mul_matrix(double *f, double *&x);
    void solve_L(double *f, double *&x);
    void solve_LT(double *f, double *&x);
    void solve_LLT(double *f, double *&x);
    double dot_prod(double *a, double *b);

    unsigned int n;
    unsigned int *gi, *gj;
    double *di, *gg, *rp, *r, *x0, *z, *p, *s;
    double *L_di, *L_gg;
};

#endif // CGM_H_INCLUDED
