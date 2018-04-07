/* movstat/movmean.c
 *
 * Routines related to a moving window mean
 * 
 * Copyright (C) 2018 Patrick Alken
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
 
#include <config.h>
#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_movstat.h>

#include "movstat_common.c"

/*
gsl_movstat_mean()
  Apply moving mean to input vector

Inputs: endtype - end point handling criteria
        x       - input vector, size n
        y       - output vector, size n
        w       - workspace
*/

int
gsl_movstat_mean(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w)
{
  int status = movstat_apply(gsl_movstat_accum_mean, endtype, x, y, w);
  return status;
}
