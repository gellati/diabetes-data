/* file: graphobject.print_nodes.cpp
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

static void draw_emblem(PostScript&, Emblem&, Vertex&);
static void draw_label(PostScript&, Emblem&, Vertex&);

/*
 *
 */
void
GraphObject::print_nodes(PostScript& ps, map<string, Vertex>& vertices) {
  map<string, Vertex>::iterator pos;
  for(pos = vertices.begin(); pos != vertices.end(); pos++)
    draw_emblem(ps, emblems[pos->first], pos->second);
  for(pos = vertices.begin(); pos != vertices.end(); pos++)
    draw_label(ps, emblems[pos->first], pos->second);
}

/*
 *
 */
static void
draw_emblem(PostScript& ps, Emblem& e, Vertex& v)  {
  float r = e.size;
  char buffer[1023];
  char* ptr = buffer;
  
  /* Check size. */
  if(r <= 0.0) return;
  //if(r > 1.0) r = 1.0;
  if(r < 0.15) r = 0.15;
  r = 0.25*sqrt(r);

  /* Node outline. */
  ptr += sprintf(ptr, "0.6 FG ");
  ptr += sprintf(ptr, "%.3f %.3f %.3f ", v.x, v.y, r);
  int n_shapes = PostScript::shape_count();
  if((e.shape > 1) && (e.shape <= n_shapes))
    ptr += sprintf(ptr, "s.%d ", e.shape);
  else
    ptr += sprintf(ptr, "s.1 ");

  /* Color. */
  if(e.color >= 0) {
    vector<float> rgb = get_rgb(e.color);
    ptr += sprintf(ptr, "GS %.3f %.3f %.3f ", rgb[0], rgb[1], rgb[2]);
    ptr += sprintf(ptr, "SC F GR ");
  }
  else 
    ptr += sprintf(ptr, "GS BG F GR ");

  /* Pattern. */
  int n_patterns = PostScript::pattern_count();
  if((e.pattern > 0) && (e.pattern <= n_patterns)) {
    ptr += sprintf(ptr, "GS clip %.3f %.3f %.3f ", v.x, v.y, r);
    ptr +=  sprintf(ptr, "p.%d GR\n", e.pattern);
  }
  ptr += sprintf(ptr, "S\n");

  ps.append(buffer);
}

/*
 *
 */
static void
draw_label(PostScript& ps, Emblem& e, Vertex& v)  {
  float r = e.size;
  char buffer[511];
  char* ptr = buffer;

  /* Check size. */
  if(r <= 0.0) return;
  if(r > 1.0) r = 1.0;
  if(r < 0.15) r = 0.15;
  r = 0.25*sqrt(r);

  /* Label. */
  if(e.label == "") return;
  ptr += sprintf(ptr, "2.1 FG HELV %.3f ", (v.x + r + 0.15));
  ptr += sprintf(ptr, "%.3f M (", (v.y - 0.1));
  ps.append(string(buffer) + pacify(e.label) + ") SH\n");
}
