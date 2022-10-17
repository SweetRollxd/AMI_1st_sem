#include "cgm.h"

#include <iostream>
using namespace std;

void CGM::init(unsigned int *gi_s, unsigned int *gj_s, double *di_s, double *gg_s, double *rp_s, unsigned int n_s)
{
    gi = gi_s;
    gj = gj_s;
    di = di_s;
    gg = gg_s;
    rp = rp_s;
    n = n_s;

    unsigned int m = gi[n];
    r = new double [n];
    x0 = new double [n];
    z = new double [n];
    p = new double [n];
    s = new double [n];

    L_di = new double [n];
    L_gg = new double [m];

    for(unsigned int i = 0; i < n; i++)
    {
        L_di[i] = di[i];
        x0[i] = 0;
    }

    for(unsigned int i = 0 ; i < m ; i++)
        L_gg[i] = gg[i];
}

void CGM::make_LLT_decomposition()
{

    double sum_d, sum_l;

    for(unsigned int k = 0; k < n ; k++)
    {

        sum_d = 0;
        unsigned int i_s = gi[k], i_e = gi[k+1];

        for(unsigned int i = i_s; i < i_e ; i++)
        {

            sum_l = 0;
            unsigned int j_s = gi[gj[i]], j_e = gi[gj[i]+1];

            for(unsigned int m = i_s; m < i; m++)
            {
                for(unsigned int j = j_s; j < j_e; j++)
                {
                    if(gj[m] == gj[j])
                    {
                        sum_l += L_gg[m]*L_gg[j];
                        j_s++;
                    }
                }
            }
            L_gg[i] = (L_gg[i] -  sum_l)/L_di[gj[i]];

            sum_d += L_gg[i]*L_gg[i];
        }
        L_di[k] = sqrt(L_di[k] - sum_d);

    }
}

double CGM::dot_prod(double *a, double *b)
{
    double d_p = 0;
    for(unsigned int i = 0; i < n; i++)
        d_p += a[i]*b[i];
    return d_p;
}

void CGM::mul_matrix(double *f, double *&x)
{

    for(unsigned int i = 0; i < n; i++)
    {
        double v_el = f[i];
        x[i] = di[i]*v_el;
        for(unsigned int k = gi[i], k1 = gi[i+1]; k < k1; k++)
        {
            unsigned int j = gj[k];
            x[i] += gg[k]*f[j];
            x[j] += gg[k]*v_el;
        }
    }
}

void CGM::solve_L(double *f, double *&x)
{

    for(unsigned int k = 1, k1 = 0; k <= n; k++, k1++)
    {
        double sum = 0;

        for(unsigned int i = gi[k1]; i < gi[k]; i++)
            sum += L_gg[i]*x[gj[i]];

        x[k1] = (f[k1] - sum)/L_di[k1];
    }

}

void CGM::solve_LT(double *f, double *&x)
{

    for(unsigned int k = n, k1 = n-1; k > 0; k--, k1--)
    {

        x[k1] = f[k1]/L_di[k1];
        double v_el = x[k1];

        for(unsigned int i = gi[k1]; i < gi[k]; i++)
            f[gj[i]] -= L_gg[i]*v_el;
    }
}

void CGM::solve_LLT(double *f, double *&x)
{
    solve_L(f, x);
    solve_LT(x,x);
}

void CGM::solve(double *&solution)
{

    // Параметры решателя
    unsigned int max_iter = 1000;
    double eps = 1E-15;

    mul_matrix(x0, r);
    make_LLT_decomposition();

    for(unsigned int i = 0; i < n ; i++)
        r[i] = rp[i] - r[i];

    solve_LLT(r, z);
    for(unsigned int i = 0; i < n; i++)
        p[i] = z[i];

    double alpha, betta, prod_1, prod_2;
    double discr, rp_norm;

    rp_norm = sqrt(dot_prod(rp,rp));

    prod_1 = dot_prod(p, r);

    bool end = false;

    for(unsigned int iter = 0; iter < max_iter && !end; iter++)
    {

        discr = sqrt(dot_prod(r,r));

        if(discr != discr || rp_norm != rp_norm)
            cerr << "Error: NaN detected!" << endl;

        if(eps < discr/rp_norm)
        {

            mul_matrix(z, s);

            alpha = prod_1 / dot_prod(s, z);

            for(unsigned int i = 0; i < n ; i++)
            {
                x0[i] += alpha * z[i];
                r[i] -= alpha * s[i];
            }

            solve_LLT(r, p);
            prod_2 = dot_prod(p, r);

            betta = prod_2 / prod_1;

            prod_1 = prod_2;

            for(unsigned int i = 0; i < n; i++)
                z[i] = p[i] + betta*z[i];
        }
        else
            end = true;
    }

    for(unsigned int i = 0; i < n; i++)
        solution[i] = x0[i];
}
