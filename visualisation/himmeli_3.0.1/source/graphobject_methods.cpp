/* file: graphobject_methods.cpp
  Himmeli graph visualization software
  Copyright (C) 2006 Ville-Petteri Makinen

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the
  Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  CITATION
    Makinen V-P, Himmeli, URL:http://www.iki.fi/~vpmakine/

  CONTACT
    Ville-Petteri Makinen, M.Sc. (Tech.)

    Laboratory of Computational Engineering
    Helsinki University of Technology, P.O.Box 9203  
    Tekniikantie 14 02150, Espoo, Finland  
    Tel: +358 9 451 4826
    Fax: +358 9 451 4830

    Folkhalsan Research Center
    Biomedicum Helsinki P.O.Box 63
    Haartmaninkatu 8 00014, Helsinki, Finland
    Tel: +358 9 191 25462
    Fax: +358 9 191 25452

    Email: vmakine2@lce.hut.fi
    WWW:   http://www.iki.fi/~vpmakine
*/

#include "graphobject.h"
#include "utilities.h"

/*
 *
 */
Component
GraphObject::operator[](const int i) {
  int n = components.size();
  if(i < 0) return Component();
  if(i >= n) return Component();
  return components[n];
}

/*
 *
 */
unsigned int
GraphObject::cardinality() {
  unsigned int i;
  unsigned int n = 0;
  for(i = 0; i < components.size(); i++) 
    n += components[i].cardinality();
  return n;
}

/*
 *
 */
void
GraphObject::run() {
  unsigned int i;
  unsigned int n_links = size();
  time_t start = time(NULL);

  /* Copy existing coordinates if any. */
  if(n_links < 1) return;
  if(increment_mode)
    for(i = 0; i < components.size(); i++)
      components[i].update(coordinates);
  if(time_limit < FLT_EPSILON) return;

  /* Configure components. */
  for(i = 0; i < components.size(); i++) {
    if(tree_mode) components[i].assign("TreeMode", "on");
    else components[i].assign("TreeMode", "off");
    if(verbose_mode) components[i].assign("VerboseMode", "on");
    else components[i].assign("VerboseMode", "off");
  }

  /* Compute layout. */
  if(verbose_mode) cout << "\nComputing layout:\n";
  for(i = 0; i < components.size(); i++) {
    unsigned int n = 0;
    time_t now = time(NULL);
    float grace = time_limit*(components[i].size())/n_links;
    n = components[i].simulate(grace);
    if(verbose_mode) { 
      cout << "\t[" << (i + 1) << "]\t" << n << '\t' 
	   << "iterations in " << difftime(time(NULL), now) << "s\n";
    }
  }
  if(verbose_mode) { 
    cout << "\tLayout computed in "
	 << difftime(time(NULL), start) << "s.\n";
  }
}

/*
 *
 */
unsigned int
GraphObject::size() {
  unsigned int i;
  unsigned int n_links = 0;
  for(i = 0; i < components.size(); i++) 
    n_links += components[i].size();
  return n_links;
}
