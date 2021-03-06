/*
 * MicroHH
 * Copyright (c) 2011-2017 Chiel van Heerwaarden
 * Copyright (c) 2011-2017 Thijs Heus
 * Copyright (c) 2014-2017 Bart van Stratum
 *
 * This file is part of MicroHH
 *
 * MicroHH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * MicroHH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with MicroHH.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <cmath>
#include <algorithm>
#include "grid.h"
#include "fields.h"
#include "master.h"
#include "diff_2.h"
#include "defines.h"
#include "model.h"

Diff_2::Diff_2(Model* modelin, Input* inputin) : Diff(modelin, inputin)
{
    swdiff = "2";
}

Diff_2::~Diff_2()
{
}

void Diff_2::set_values()
{
    // Get the maximum time step for diffusion.
    double viscmax = fields->visc;
    for (FieldMap::iterator it = fields->sp.begin(); it!=fields->sp.end(); it++)
        viscmax = std::max(it->second->visc, viscmax);

    dnmul = 0;
    for (int k=grid->kstart; k<grid->kend; k++)
        dnmul = std::max(dnmul, std::abs(viscmax * (1./(grid->dx*grid->dx) + 1./(grid->dy*grid->dy) + 1./(grid->dz[k]*grid->dz[k]))));
}

unsigned long Diff_2::get_time_limit(unsigned long idt, double dt)
{
    return idt * dnmax / (dt * dnmul);
}

double Diff_2::get_dn(const double dt)
{
    return dnmul*dt;
}

#ifndef USECUDA
void Diff_2::exec()
{
    diff_c(fields->ut->data, fields->u->data, grid->dzi, grid->dzhi, fields->visc);
    diff_c(fields->vt->data, fields->v->data, grid->dzi, grid->dzhi, fields->visc);
    diff_w(fields->wt->data, fields->w->data, grid->dzi, grid->dzhi, fields->visc);

    for (FieldMap::const_iterator it = fields->st.begin(); it!=fields->st.end(); it++)
        diff_c(it->second->data, fields->sp[it->first]->data, grid->dzi, grid->dzhi, fields->sp[it->first]->visc);
}
#endif

void Diff_2::diff_c(double* restrict at, double* restrict a, double* restrict dzi, double* restrict dzhi, double visc)
{
    const int ii = 1;
    const int jj = grid->icells;
    const int kk = grid->ijcells;

    const double dxidxi = 1./(grid->dx * grid->dx);
    const double dyidyi = 1./(grid->dy * grid->dy);

    for (int k=grid->kstart; k<grid->kend; k++)
        for (int j=grid->jstart; j<grid->jend; j++)
#pragma ivdep
            for (int i=grid->istart; i<grid->iend; i++)
            {
                const int ijk = i + j*jj + k*kk;
                at[ijk] += visc * (
                        + ( (a[ijk+ii] - a[ijk   ]) 
                          - (a[ijk   ] - a[ijk-ii]) ) * dxidxi 
                        + ( (a[ijk+jj] - a[ijk   ]) 
                          - (a[ijk   ] - a[ijk-jj]) ) * dyidyi
                        + ( (a[ijk+kk] - a[ijk   ]) * dzhi[k+1]
                          - (a[ijk   ] - a[ijk-kk]) * dzhi[k]   ) * dzi[k] );
            }
}

void Diff_2::diff_w(double* restrict wt, double* restrict w, double* restrict dzi, double* restrict dzhi, double visc)
{
    const int ii = 1;
    const int jj = grid->icells;
    const int kk = grid->ijcells;

    const double dxidxi = 1./(grid->dx*grid->dx);
    const double dyidyi = 1./(grid->dy*grid->dy);

    for (int k=grid->kstart+1; k<grid->kend; k++)
        for (int j=grid->jstart; j<grid->jend; j++)
#pragma ivdep
            for (int i=grid->istart; i<grid->iend; i++)
            {
                const int ijk = i + j*jj + k*kk;
                wt[ijk] += visc * (
                        + ( (w[ijk+ii] - w[ijk   ]) 
                          - (w[ijk   ] - w[ijk-ii]) ) * dxidxi 
                        + ( (w[ijk+jj] - w[ijk   ]) 
                          - (w[ijk   ] - w[ijk-jj]) ) * dyidyi
                        + ( (w[ijk+kk] - w[ijk   ]) * dzi[k]
                          - (w[ijk   ] - w[ijk-kk]) * dzi[k-1] ) * dzhi[k] );
            }
}
