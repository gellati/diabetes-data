/* file: walker.h
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

#ifndef walker_INCLUDED
#define walker_INCLUDED

/*
 * All indices are assumed to refer to the current vertex array.
 */
typedef struct {

  /* Relations. Negative values are considered missing links. */
  int parent;
  int child;
  int sibling;

  /* A positive value indicates that the vertex needs to be adjusted half way
     in the horizontal space available for it and its subtree. Otherwise
     it is put as close to the left neighboring subtree as possible. */
  int is_centering;

  /* These will be over-written by the node-positioning function. */
  float x;
  float y;

  /* Horizontal and vertical space requirement. */
  float width;
  float height;

  /* Attachment poinst to parent and child vertices in the tree. */
  float up_attach;
  float down_attach;

  /* Not used. */
  void* user_data;

} walker_vertex_t;

/*
 * Walker II, John Q.
 * "A Node-positioning Algorithm for General Trees"
 * Software Pract Exp, 1990, 20, 685-705
 *
 * The first argument specifices the tree topology and the second the
 * number of vertices.
 */
extern void walker(walker_vertex_t*, int);


#endif /* walker_INCLUDED */




