/* poppler.cc: glib wrapper for poppler
 * Copyright (C) 2005, Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include "poppler.h"

GQuark poppler_error_quark (void)
{
  static GQuark q = 0;

  if (q == 0)
    q = g_quark_from_static_string ("poppler-quark");

  return q;
}

/**
 * poppler_get_backend:
 * @void: 
 * 
 * Returns the backend compiled into the poppler library.
 * 
 * Return value: The backend used by poppler
 **/
PopplerBackend
poppler_get_backend (void)
{
#if defined (HAVE_CAIRO)
  return POPPLER_BACKEND_CAIRO;
#elif defined (HAVE_SPLASH)
  return POPPLER_BACKEND_SPLASH;
#else
  return POPPLER_BACKEND_UNKNOWN;
#endif
}

static const char poppler_version[] = PACKAGE_VERSION;

/**
 * poppler_get_version:
 * @void: 
 * 
 * Returns the version of poppler in use.  This result is not to be freed.
 * 
 * Return value: the version of poppler.
 **/
const char *
poppler_get_version (void)
{
  return poppler_version;
}
