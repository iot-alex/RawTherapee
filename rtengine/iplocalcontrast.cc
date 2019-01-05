/* -*- C++ -*-
 *
 *  This file is part of RawTherapee.
 *
 *  Ported from G'MIC by Alberto Griggio <alberto.griggio@gmail.com>
 *
 *  The original implementation in G'MIC was authored by Arto Huotari, and was
 *  released under the CeCILL free software license (see
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.html)
 *
 *  RawTherapee is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RawTherapee is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RawTherapee.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef _OPENMP
#include <omp.h>
#endif

#include "improcfun.h"
#include "gauss.h"
#include "array2D.h"

namespace rtengine
{

void ImProcFunctions::localContrast(LabImage *lab)
{
    if (!params->localContrast.enabled) {
        return;
    }

    const int width = lab->W;
    const int height = lab->H;
    const float a = params->localContrast.amount;
    const float dark = params->localContrast.darkness;
    const float light = params->localContrast.lightness;
    array2D<float> buf(width, height);
    const float sigma = params->localContrast.radius / scale;

#ifdef _OPENMP
    #pragma omp parallel if(multiThread)
#endif
    gaussianBlur(lab->L, buf, width, height, sigma);

#ifdef _OPENMP
    #pragma omp parallel for if(multiThread)
#endif

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float bufval = (lab->L[y][x] - buf[y][x]) * a;

            if (dark != 1 || light != 1) {
                bufval *= (bufval > 0.f) ? light : dark;
            }

            lab->L[y][x] = std::max(0.0001f, lab->L[y][x] + bufval);
        }
    }
}

void ImProcFunctions::localContrastloc(LabImage *lab, int scale, int rad, int amo, int darkn, int lightn, float **loctemp)
{

    const int width = lab->W;
    const int height = lab->H;
    const float a = (float)(amo) / 100.f;
    const float dark = (float)(darkn) / 100.f;
    const float light = (float)(lightn) / 100.f;
    array2D<float> buf(width, height);
    const float sigma = (float)(rad) / scale;

#ifdef _OPENMP
    #pragma omp parallel if(multiThread)
#endif
    gaussianBlur(lab->L, buf, width, height, sigma);

#ifdef _OPENMP
    #pragma omp parallel for if(multiThread)
#endif

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float bufval = (lab->L[y][x] - buf[y][x]) * a;

            if (dark != 1 || light != 1) {
                bufval *= (bufval > 0.f) ? light : dark;
            }

            // lab->L[y][x] = std::max(0.0001f, lab->L[y][x] + bufval);
            loctemp[y][x] = std::max(0.0001f, lab->L[y][x] + bufval);

        }
    }
}

} // namespace rtengine
