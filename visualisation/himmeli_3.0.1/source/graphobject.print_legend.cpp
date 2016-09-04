/* file: graphobject.print_legend.cpp
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

#define MAX_LABEL_LENGTH 16

static vector<float> get_limits(vector<Component>&);

/*
 *
 */
vector<float>
GraphObject::print_legend(PostScript& ps) {
  unsigned int i;
  vector<float> dimen(2, 0.0);

  /* Read edge info items. */
  vector<int> e_color_codes;
  vector<string> e_color_labels;
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("EdgeColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    e_color_codes.push_back((int)(row.number(2)));
    e_color_labels.push_back(row[1]);
  }

  /* Read vertex info items. */
  vector<int> v_color_codes;
  vector<int> v_pattern_codes;
  vector<int> v_shape_codes;
  vector<string> v_color_labels;
  vector<string> v_pattern_labels;
  vector<string> v_shape_labels;
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexColorInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    v_color_codes.push_back((int)(row.number(2)));
    v_color_labels.push_back(row[1]);
  }
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexPatternInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    v_pattern_codes.push_back((int)(row.number(2)));
    v_pattern_labels.push_back(row[1]);
  }
  for(i = 0; i < cfg.size(); i++) {
    Row row = cfg.get("VertexShapeInfo", i);
    if(row.size() < 1) break;
    if(row.size() < 2) continue; 
    v_shape_codes.push_back((int)(row.number(2)));
    v_shape_labels.push_back(row[1]);
  }
  
  /* Compute legend dimensions. */
  unsigned int n_erows = 0;
  unsigned int n_vrows = 0;
  map<string, Table>::iterator tpos;
  for(tpos = tables.begin(); tpos != tables.end(); tpos++) {
    map<string, Locus>& loci = variables[tpos->first];
    if(loci["EdgeColorVariable"].column >= 0) n_erows++;
    if(loci["EdgeWidthVariable"].column >= 0) n_erows++;
    if(loci["VertexColorVariable"].column >= 0) n_vrows++;
    if(loci["VertexLabelVariable"].column >= 0) n_vrows++;
    if(loci["VertexPatternVariable"].column >= 0) n_vrows++;
    if(loci["VertexShapeVariable"].column >= 0) n_vrows++;
    if(loci["VertexSizeVariable"].column >= 0) n_vrows++;
  }
  if(n_erows > 0) n_erows += e_color_codes.size();
  if(n_vrows > 0) {
    n_vrows += v_color_codes.size();
    n_vrows += v_pattern_codes.size();
    n_vrows += v_shape_codes.size();
  }

  /* Check dynamic range. */
  vector<float> lim = get_limits(components);
  if(decoration_mode && (lim[1] > lim[0])) n_erows += 3;
  if((n_erows < 1) && (n_vrows < 1)) {
    ps.append("\n%% legend\n/LEGEND {} def\n");
    return dimen;
  }

  /* Determine legend size. */
  char buffer[2047];
  char* ptr = buffer;
  ptr += sprintf(ptr, "\n%% legend\n/LEGEND {\n");
  ptr += sprintf(ptr, "HELV_FIXED_16\n");
  dimen[0] = 12.0;
  dimen[1] = (n_erows + n_vrows + 2.0);

  /* Print colorbar. */
  float x = 1.0;
  float y = (dimen[1] - 1.3);
  if(decoration_mode && (lim[1] > lim[0])) {
    ptr += sprintf(ptr, "0.8 FG %.3f %.3f M ", x, y);
    ptr += sprintf(ptr, "(Automatic edge coloring:) SH\n");
    y -= 1.0;
    x += 0.5;
    ptr += sprintf(ptr, "GS\n");
    for(i = 0; i < 40; i++) {
      vector<float> rgb = get_rgb(get_color((i + 0.5)/40.0));
      ptr += sprintf(ptr, "%.3f %.3f 0.1 %.3f rect ", x, (y + 0.2), 0.27);
      ptr += sprintf(ptr, "%.2f %.2f %.2f ", rgb[0], rgb[1], rgb[2]);
      ptr += sprintf(ptr, "SC GS F GR S\n");
      x += 0.2;
      ps.append(buffer); ptr = buffer; *ptr = '\0';
    }
    ptr += sprintf(ptr, "GR\n");
    x = 1.0;
    y -= 0.7;
    if(cfg["EdgeWeightTransform"][1] == "none") {
      ptr += sprintf(ptr, "%.3f %.3f M (0.0) SH\n", x, y);
      x += 0.2*40;
      ptr += sprintf(ptr, "%.3f %.3f M (1.0) SH\n", x, y);
    }
    else {
      ptr += sprintf(ptr, "%.3f %.3f M (min) SH\n", x, y);
      x += 0.2*40;
      ptr += sprintf(ptr, "%.3f %.3f M (max) SH\n", x, y);
    }
    y -= 1.3;
  }
  ps.append(buffer); ptr = buffer; *ptr = '\0';

  /* Edge color info. */
  x = 1.0;
  string heading = pacify(cfg["EdgeColorVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    ptr += sprintf(ptr, "0.8 FG %.3f %.3f M ", x, y);
    ptr += sprintf(ptr, "(Edge color:) SH\n");
    ptr += sprintf(ptr, "%.3f %.3f M ", (x + 4.5), y);
    ptr += sprintf(ptr, "(%s) SH\n", heading.c_str());
    y -= 1.0;
    x = 2.5;
    ptr += sprintf(ptr, "GS\n");
    for(i = 0; i < e_color_labels.size(); i++) {
      string label = pacify(e_color_labels[i], MAX_LABEL_LENGTH);
      vector<float> rgb = get_rgb(e_color_codes[i]);
      ptr += sprintf(ptr, "%.3f %.3f %.3f s.7 GS\n", x, (y + 0.2), 0.35);
      ptr += sprintf(ptr, "%.2f %.2f %.2f ", rgb[0], rgb[1], rgb[2]);
      ptr += sprintf(ptr, "SC F GR S\n");
      ptr += sprintf(ptr, "%.3f %.3f M ", (x + 3), y);
      ptr += sprintf(ptr, "(%s) SH\n", label.c_str());
      y -= 1.0;
      ps.append(buffer); ptr = buffer; *ptr = '\0';
    }
    ptr += sprintf(ptr, "GR\n");
  }
  ps.append(buffer); ptr = buffer; *ptr = '\0';

  /* Edge width info. */
  x = 1.0;
  heading = pacify(cfg["EdgeWidthVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    ptr += sprintf(ptr, "0.8 FG %.3f %.3f M ", x, y);
    ptr += sprintf(ptr, "(Edge width:) SH\n");
    ptr += sprintf(ptr, "%.3f %.3f M ", (x + 4.5), y);
    ptr += sprintf(ptr, "(%s) SH\n", heading.c_str());
    y -= 1.0;
  }
  ps.append(buffer); ptr = buffer; *ptr = '\0';

  /* Vertex color info. */
  x = 1.0;
  heading = pacify(cfg["VertexColorVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    ptr += sprintf(ptr, "0.8 FG %.3f %.3f M ", x, y);
    ptr += sprintf(ptr, "(Vertex color:) SH\n");
    ptr += sprintf(ptr, "%.3f %.3f M ", (x + 4.5), y);
    ptr += sprintf(ptr, "(%s) SH\n", heading.c_str());
    y -= 1.0;
    x = 2.5;
    ptr += sprintf(ptr, "GS\n");
    for(i = 0; i < v_color_labels.size(); i++) {
      string label = pacify(v_color_labels[i], MAX_LABEL_LENGTH);
      vector<float> rgb = get_rgb(v_color_codes[i]);
      ptr += sprintf(ptr, "%.3f %.3f %.3f s.1 GS\n", x, (y + 0.2), 0.35);
      ptr += sprintf(ptr, "%.2f %.2f %.2f ", rgb[0], rgb[1], rgb[2]);
      ptr += sprintf(ptr, "SC F GR S\n");
      ptr += sprintf(ptr, "%.3f %.3f M ", (x + 3), y);
      ptr += sprintf(ptr, "(%s) SH\n", label.c_str());
      y -= 1.0;
      ps.append(buffer); ptr = buffer; *ptr = '\0';
    }
    ptr += sprintf(ptr, "GR\n");
  }
  ps.append(buffer); ptr = buffer; *ptr = '\0';

  /* Vertex pattern info. */
  x = 1.0;
  heading = pacify(cfg["VertexPatternVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    ptr += sprintf(ptr, "0.8 FG %.3f %.3f M ", x, y);
    ptr += sprintf(ptr, "(Vertex pattern:) SH\n");
    ptr += sprintf(ptr, "%.3f %.3f M ", (x + 4.5), y);
    ptr += sprintf(ptr, "(%s) SH\n", heading.c_str());
    y -= 1.0;
    x = 2.5;
    ptr += sprintf(ptr, "GS\n");
    for(i = 0; i < v_pattern_labels.size(); i++) {
      string label = pacify(v_pattern_labels[i], MAX_LABEL_LENGTH);
      unsigned int pattern = v_pattern_codes[i]; 
      if((pattern > 0) && (pattern < PostScript::pattern_count())) {
	ptr += sprintf(ptr, "%.3f %.3f %.3f ", x, (y + 0.2), 0.35);
	ptr += sprintf(ptr, "s.7 gsave clip\n");
	ptr += sprintf(ptr, "%.3f %.3f %.3f ", x, (y + 0.2), 0.35);
	ptr += sprintf(ptr, "p.%d grestore stroke\n", pattern);
      }
      else
	ptr += sprintf(ptr, "%.3f %.3f M (?) SHC\n", x, y);
      ptr += sprintf(ptr, "%.3f %.3f moveto ", (x + 3), y);
      ptr += sprintf(ptr, "(%s) show\n", label.c_str());
      y -= 1.0;
      ps.append(buffer); ptr = buffer; *ptr = '\0';
    }
    ptr += sprintf(ptr, "GR\n");
  }
  ps.append(buffer); ptr = buffer; *ptr = '\0';

  /* Vertex shape info. */
  x = 1.0;
  heading = pacify(cfg["VertexShapeVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    ptr += sprintf(ptr, "0.8 FG %.3f %.3f M ", x, y);
    ptr += sprintf(ptr, "(Vertex shape:) SH\n");
    ptr += sprintf(ptr, "%.3f %.3f M ", (x + 4.5), y);
    ptr += sprintf(ptr, "(%s) SH\n", heading.c_str());
    y -= 1.0;
    x = 2.5;
    for(i = 0; i < v_shape_labels.size(); i++) {
      string label = pacify(v_shape_labels[i], MAX_LABEL_LENGTH);
      vector<float> rgb = get_rgb(v_shape_codes[i]);
      unsigned int shape = v_shape_codes[i]; 
      if((shape >= 1) && (shape <= PostScript::shape_count())) {
	ptr += sprintf(ptr, "%.3f %.3f %.3f ", x, (y + 0.2), 0.3);
	ptr += sprintf(ptr, "s.%d S\n", shape);
      }
      else
	ptr += sprintf(ptr, "%.3f %.3f M (?) SHC\n", x, y);
      ptr += sprintf(ptr, "%.3f %.3f M ", (x + 3), y);
      ptr += sprintf(ptr, "(%s) ", label.c_str());
      ptr += sprintf(ptr, "SH\n");
      y -= 1.0;
      ps.append(buffer); ptr = buffer; *ptr = '\0';
    }
  }

  /* Vertex size info. */
  x = 1.0;
  heading = pacify(cfg["VertexSizeVariable"][1], MAX_LABEL_LENGTH);
  if(heading.length() > 0) {
    ptr += sprintf(ptr, "0.8 FG %.3f %.3f M ", x, y);
    ptr += sprintf(ptr, "(Vertex size:) SH\n");
    ptr += sprintf(ptr, "%.3f %.3f M ", (x + 4.5), y);
    ptr += sprintf(ptr, "(%s) SH\n", heading.c_str());
    y -= 1.0;
  }
  ps.append(buffer); ptr = buffer; *ptr = '\0';
  ps.append("} def\n");

  return dimen;
}

/*
 *
 */
static vector<float>
get_limits(vector<Component>& components) {
  unsigned int i, k;
  vector<float> lim(4);

  lim[0] = FLT_MAX;
  lim[1] = -FLT_MAX;
  lim[2] = FLT_MAX;
  lim[3] = -FLT_MAX;
  for(i = 0; i < components.size(); i++) {
    vector<Edge> edges = components[i].edges();
    for(k = 0; k < edges.size(); k++) {
      if(lim[0] > edges[k].weight) lim[0] = edges[k].weight;
      if(lim[1] < edges[k].weight) lim[1] = edges[k].weight;
    }
    map<string, Vertex>::iterator pos;
    map<string, Vertex> vertices = components[i].vertices();
    for(pos = vertices.begin(); pos != vertices.end(); pos++) {
      Vertex v = pos->second;
      float s = (v.strength_in + v.strength_out);
      if(lim[2] > s) lim[2] = s;
      if(lim[3] < s) lim[3] = s;
    }
  }

  return lim;
}

