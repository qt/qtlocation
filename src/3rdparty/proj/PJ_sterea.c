/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_sterea.c 1504 2009-01-06 02:11:57Z warmerdam $";
/*
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define PROJ_PARMS__ \
	double phic0; \
	double cosc0, sinc0; \
	double R2; \
	void *en;

#define PJ_LIB__
#include <projects.h>

PROJ_HEAD(sterea, "Oblique Stereographic Alternative") "\n\tAzimuthal, Sph&Ell";

# define DEL_TOL	1.e-14
# define MAX_ITER	10

FORWARD(e_forward); /* ellipsoid */
        double cosphi, sinphi, coslam, k;

	lp = pj_gauss(lp, P->en);
        sinphi = sin(lp.phi);
        cosphi = cos(lp.phi);
        coslam = cos(lp.lam);
        k = P->k0 * P->R2 / (1. + P->sinc0 * sinphi + P->cosc0 * cosphi * coslam);
        xy.x = k * cosphi * sin(lp.lam);
        xy.y = k * (P->cosc0 * sinphi - P->sinc0 * cosphi * coslam);
	return (xy);
}
INVERSE(e_inverse); /* ellipsoid */
        double rho, c, sincc, coscc;

	xy.x /= P->k0;
	xy.y /= P->k0;
	if((rho = hypot(xy.x, xy.y))) {
		c = 2. * atan2(rho, P->R2);
                sincc = sin(c);
                coscc = cos(c);
                lp.phi = asin(coscc * P->sinc0 + xy.y * sincc * P->cosc0 / rho);
                lp.lam = atan2(xy.x * sincc, rho * P->cosc0 * coscc -
                        xy.y * P->sinc0 * sincc);
	} else {
		lp.phi = P->phic0;
		lp.lam = 0.;
	}
	return(pj_inv_gauss(lp, P->en));
}
FREEUP; if (P) { if (P->en) free(P->en); free(P); } }
ENTRY0(sterea)
	double R;

	if (!(P->en = pj_gauss_ini(P->e, P->phi0, &(P->phic0), &R))) E_ERROR_0;
	P->sinc0 = sin(P->phic0);
	P->cosc0 = cos(P->phic0);
	P->R2 = 2. * R;
	P->inv = e_inverse;
	P->fwd = e_forward;
ENDENTRY(P)
