/* file: walkertools.h
  Walker tree node-positioning library (originally part of CraneFoot)
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
    Makinen V-P, Parkkonen M, Wessman M, Groop P-H, Kanninen T and Kaski K,
    High-throughput pedigree drawing,
    Eur J Hum Gen, 13, 987-989, 2005.

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

#ifndef walkertools_INCLUDED
#define walkertools_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "walker.h"

#define walker_VERSION "1.1.0"
#define WIDTH          1.0

typedef struct {
  int parent;
  int first_child;
  int last_child;
  int sibling;
  int left_peer;
  int right_peer;
  int contour;
  int gen;
  int n_left_sibs;
  float x;
  float xmod;
  float xshift;
  float xchange;
  float width;
} node_t;

typedef struct {
  int n;
  int cap;
  int gen;
  int depth;
  float modifier;
  int* vtx2node;
  int* node2vtx;
  walker_vertex_t* vertices;
  node_t* nodes;
  node_t** edge;
} tree_t;

extern void  walker_dfs(tree_t*, int);
extern int   walker_branch(tree_t*, int);
extern int   walker_trace(tree_t*, int, int);
extern void  walker_measure(tree_t*, int, int);
extern void  walker_balance(tree_t*, int);
extern void  walker_fix(tree_t*, int);

#endif /* walkertools_INCLUDED */




