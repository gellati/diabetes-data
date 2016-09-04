/* file: graphobject.print.cpp
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

static void customize(PostScript&, Table&);
static vector<float> get_dimen(map<string, Vertex>&, float);
static bool prepare_ps(PostScript&, Component&,
			vector<float>, vector<float>);

/*
 *
 */
void
GraphObject::print() {
  vector<float> ldimen(2, 0.0);
  unsigned int i;

  /* Create ps template. */
  if(verbose_mode) cout << "\nMedia instructions:\n";
  PostScript ps = PostScript(cfg["GraphName"][1] + ".ps");
  customize(ps, cfg);

  /* Pre-print legend. */
  if(legend_mode) ldimen = print_legend(ps);
  else ps.append("\n% NO LEGEND\n");

  /* Print components to a single file. */
  if(verbose_mode) cout << "\nSaving results:\n";
  for(i = 0; i < components.size(); i++) {
    if(i >= 9999) break;
    map<string, Vertex> vertices = components[i].vertices();    
    vector<float> dimen = get_dimen(vertices, distance_unit);

    /* Prepare page. */
    if(!prepare_ps(ps, components[i], dimen, ldimen)) {
      printf("WARNING! Could not print component %d.\n", (i + 1));
      continue;
    }

    /* Print component. */
    vector<Edge> edges;
    if(tree_mode) edges = components[i].tree();
    else edges = components[i].edges();
    print_links(ps, vertices, edges);
    print_nodes(ps, vertices);
  }
  
  /* Close main document. */
  ps.close();
  if(verbose_mode) {
    cout << "\tWrote " << clarify(ps.size()) << " bytes to '"
	 << ps["FileName"] << "'.\n";
  }

  /* Print components to separate files. */
  for(i = 0; i < components.size(); i++) {
    if(i >= figure_limit) break;
    map<string, Vertex> vertices = components[i].vertices();    
    vector<float> dimen = get_dimen(vertices, distance_unit);

    /* Determine canvas size. */
    char buffer[64];
    string page_size = cfg["PageSize"][2];
    if((page_size == "auto") | (page_size == "")) {
      if(cfg["PageOrientation"][1] == "portrait")
	sprintf(buffer, "%.2f,%.2f", dimen[0], dimen[1]);
      else
	sprintf(buffer, "%.2f,%.2f", dimen[1], dimen[0]);
      page_size = string(buffer);
    }
    
    /* Create eps template. */
    sprintf(buffer, "_%04d.eps", (i + 1));
    PostScript eps = PostScript(cfg["GraphName"][1] + string(buffer));
    customize(eps, cfg);
    eps.assign("DocumentMode", "encaps"); 
    eps.assign("PageSize", page_size);
    eps.assign("VerboseMode", "off");

    /* Prepare page. */
    ldimen = vector<float>(2, 0.0);
    if(!prepare_ps(eps, components[i], dimen, ldimen)) {
      printf("WARNING! Could not print component %d.\n", (i + 1));
      continue;
    }

    /* Print component. */
    vector<Edge> edges;
    if(tree_mode) edges = components[i].tree();
    else edges = components[i].edges();
    print_links(eps, vertices, edges);
    print_nodes(eps, vertices);
    eps.close();
    if(verbose_mode) {
      cout << "\tWrote " << clarify(eps.size())
	   << " bytes to '" << eps["FileName"] << "'.\n";
    }
  }
}

/*
 *
 */
static void
customize(PostScript& ps, Table& cfg) {
  unsigned int i;
  for(i = 0; i < cfg.size(); i++) {
    Row r = cfg[i];
    ps.assign(r[0], r[1]);
  }
  ps.assign("Creator", ("Himmeli " + string(graph_VERSION)));
}

/*
 *
 */
static vector<float>
get_dimen(map<string, Vertex>& vertices, float u) {
  float bbox[4] = {FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};
  vector<float> dimen;

  /* Determine bounding box. */
  map<string, Vertex>::iterator pos;
  for(pos = vertices.begin(); pos != vertices.end(); pos++) {
    float x = (pos->second).x;
    float y = (pos->second).y;
    x *= u;
    y *= u;
    if(x < bbox[0]) bbox[0] = x;
    if(x > bbox[2]) bbox[2] = x;
    if(y < bbox[1]) bbox[1] = y;
    if(y > bbox[3]) bbox[3] = y;
    (pos->second).x = x;
    (pos->second).y = y;
  }
  bbox[0] -= 0.8*MARGIN_WIDTH;
  bbox[1] -= MARGIN_WIDTH;
  bbox[2] += 1.2*MARGIN_WIDTH;
  bbox[3] += MARGIN_WIDTH;

  /* Remove offsets. */
  for(pos = vertices.begin(); pos != vertices.end(); pos++) {
    (pos->second).x -= bbox[0];
    (pos->second).y -= bbox[1];
  }
  dimen.push_back(bbox[2] - bbox[0]);
  dimen.push_back(bbox[3] - bbox[1]);
  
  return dimen;
}

/*
 *
 */
static bool
prepare_ps(PostScript& ps, Component& comp,
	    vector<float> dimen, vector<float> ldimen) {
  if(dimen[0] <= 0.0) return false;
  if(dimen[1] <= 0.0) return false;
  if(dimen[0] > 1000.0) return false;
  if(dimen[1] > 1000.0) return false;

  /* Make page template. */
  ps.new_page();
  float canvas_width = ps.width();
  float canvas_height = ps.height();
  if(ldimen[0]*ldimen[1] > 0.0) {
    if((ldimen[0] > canvas_width) || (ldimen[1] > canvas_height)) {
      printf("WARNING! Legend does not fit on page.\n");
      ldimen[0] = 0.0;
      ldimen[1] = 0.0;
    }
    canvas_width -= 0.4*(ldimen[0]);
  }
  
  /* Determine scale. */
  float x = (dimen[0] + 1e-10);
  float y = (dimen[1] + 1e-10);
  float scale = canvas_width/(x + 1e-10);
  if(scale > canvas_height/(y + 1e-10))
    scale = canvas_height/(y + 1e-10);
  if(scale > 1.0) scale = 1.0;
  if(scale < 0.01) {
    printf("WARNING! %s: Page is too small.\n", __FILE__);
    return false;
  }

  /* Put legend in place. */
  char buffer[1024];
  if(ldimen[0]*ldimen[1] > 0.0) {
    sprintf(buffer, "GS %.4f 0 translate 0.4 0.4 scale LEGEND GR\n",
	    canvas_width);
    ps.append(buffer);
  }

  /* Center and scale figure. */
  x = 0.5*(canvas_width - dimen[0]*scale);
  y = 0.5*(canvas_height - dimen[1]*scale);
  if((x > 1e-6) || (y > 1e-6)) {
    sprintf(buffer, "%.4f %.4f translate\n", x, y);
    ps.append(buffer);
  }
  if(scale < 1.0) {
    sprintf(buffer, "%f %f scale\n", scale, scale);
    ps.append(buffer);
  }

  return true;
}

