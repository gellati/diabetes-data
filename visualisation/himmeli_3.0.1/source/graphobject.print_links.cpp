/* file: graphobject.print_links.cpp
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

#include <algorithm> 
#include "graphobject.h"

static void draw_edge(PostScript&, Vertex&, Vertex&, Emblem&, Arc&, bool);

/*
 *
 */
void
GraphObject::print_links(PostScript& ps, map<string, Vertex>& vertices, 
			 vector<Edge>& edges) {
  unsigned int i;
  char delim = delimiter;
  if(delim == '\0') delim = ' ';

  std::sort(edges.begin(), edges.end(), CompareEdge());
  for(i = 0; i < edges.size(); i++) {
    string head = edges[i].head;
    string tail = edges[i].tail;
    string key = (head + delim + tail);
    Vertex vH = vertices[head];
    Vertex vT = vertices[tail];
    Emblem e = emblems[head];
    Arc a = arcs[key];
    draw_edge(ps, vH, vT, e, a, arrow_mode);
  }
}

/*
 *
 */
static void
draw_edge(PostScript& ps, Vertex& vH, Vertex& vT, Emblem& e, Arc& a,
	  bool arrow_flag) {
  float r = e.size;
  float width = a.width;
  char buffer[511];
  char* ptr = buffer;

  /* Check head vertex size. */
  if(r > 1.0) r = 1.0;
  if(r < 0.15) r = 0.15;
  r = 0.25*sqrt(r);

  /* Check line width. */
  if(a.weight <= 0.0) return;
  if(width <= 0.0) return;
  if(width < 0.15) width = 0.15;
  if(width > 1.0) width = 1.0;
  width *= 3;

  /* Compute angle. */
  float x = (vH.x - vT.x);
  float y = (vH.y - vT.y);
  float phi = 0.0;
  if(x > 0) phi = atan(y/(x + FLT_EPSILON));
  else phi = (M_PI + atan(y/(x - FLT_EPSILON)));

  /* Color. */
  ptr += sprintf(ptr, "1.2 FG ");
  if(a.color >= 0) {
    vector<float> rgb = get_rgb(a.color);
    ptr += sprintf(ptr, "%.2f %.2f %.2f SC\n", rgb[0], rgb[1], rgb[2]);
  }
  ps.append(buffer); ptr = buffer;

  /* Plain line or arrowhead. */
  if(!arrow_flag) {
    ptr += sprintf(ptr, "%.3f SL ", width);
    ptr += sprintf(ptr, "%.3f %.3f M ", vT.x, vT.y);
    ptr += sprintf(ptr, "%.3f %.3f L ", vH.x, vH.y);
    ptr += sprintf(ptr, "S\n");
    ps.append(buffer); ptr = buffer;
  }
  else {
    float w = sqrt(0.037*width);
    float x0 = (vH.x - (r + 0.1)*cos(phi));
    float y0 = (vH.y - (r + 0.1)*sin(phi));
    ptr += sprintf(ptr, "%.3f %.3f M ", x0, y0);
    x = (x0 - w*cos(phi + 0.13*M_PI));
    y = (y0 - w*sin(phi + 0.13*M_PI));
    ptr += sprintf(ptr, "%.3f %.3f L ", x, y);
    x = (x0 - w*cos(phi - 0.13*M_PI));
    y = (y0 - w*sin(phi - 0.13*M_PI));
    ptr += sprintf(ptr, "%.3f %.3f L\n", x, y);
    ptr += sprintf(ptr, "GS BG CL S GR F\n");
    ps.append(buffer); ptr = buffer;
    
    x = (vH.x - (r + w)*cos(phi));
    y = (vH.y - (r + w)*sin(phi));
    ptr += sprintf(ptr, "%.3f SL ", width);
    ptr += sprintf(ptr, "%.3f %.3f M ", vT.x, vT.y);
    ptr += sprintf(ptr, "%.3f %.3f L ", x, y);
    ptr += sprintf(ptr, "S\n");
    ps.append(buffer); ptr = buffer;
  }

  /* Label. */
  if(a.label != "") {
    x = 0.5*(vH.x + vT.x);
    y = 0.5*(vH.y + vT.y);
    r = 0.014*atoi(ps["FontSize"].c_str());
    ptr += sprintf(ptr, "2.1 FG HELV_SMALL %.3f ", x);
    ptr += sprintf(ptr, "%.3f M (", (y - r/1.5));
    ps.append(string(buffer) + pacify(a.label) + ") SHC\n");
  }
}
