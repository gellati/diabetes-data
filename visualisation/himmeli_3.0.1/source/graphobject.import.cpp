/* file: graphobject.import.cpp
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

#define DEFAULT_SIZE    0.8
#define DEFAULT_WEIGHT  0.6
#define DEFAULT_WIDTH   0.4

static map<string, Arc> get_arcs(vector<Edge>&, char, bool);
static map<string, Emblem> get_emblems(vector<Edge>&, bool, bool);
static void normalize(vector<float>&, string, vector<float>);

/*
 *
 */
bool
GraphObject::import() {
  unsigned int i;
  vector<Edge> edges;
  map<string, Table>::iterator tpos;

  if(verbose_mode) cout << "\nImporting graph:\n";

  /* Find topology variables. */
  map<string, Locus>& loci = variables[cfg["EdgeFile"][1]];
  int jhead = loci["EdgeHeadVariable"].column;
  int jtail = loci["EdgeTailVariable"].column;
  int jweight = loci["EdgeWeightVariable"].column;

  /* Prepare edge table. */
  Table& edge_table = tables[cfg["EdgeFile"][1]];
  vector<int> key_cols(2);
  key_cols[0] = jhead;  
  key_cols[1] = jtail;  
  edge_table.sort(key_cols);

  /* Create graph representation. */
  string label1 = "";
  string label2 = "";
  for(i = 0; i < edge_table.size(); i++) {
    Row r = edge_table[i];
    if(r.rank() < 1) continue;
    if(r[jhead] == "") continue;
    if(r[jtail] == "") continue;
    if(r[jhead] == r[jtail]) continue;
    if((r[jhead] == label1) && (r[jtail] == label2) ) {
      cout << "WARNING! Duplicate edge '" << label2 
	   << "'->'" << label1 << "' detected in file '"
	   << edge_table.source() << "'.\n";
      continue;
    }
    label1 = r[jhead];
    label2 = r[jtail];

    Edge e;
    e.head = label1;
    e.tail = label2;
    e.weight = r.number(jweight);
    if(jweight < 0) e.weight = 1.0;
    if(e.weight < FLT_EPSILON) {
      cout << "WARNING! Edge '" << label2 
	   << "'->'" << label1 << "' rejected due to low weight.\n";
      continue;
    }
    edges.push_back(e);
  }

  /* Normalize edge weights and apply weight filter. */
  vector<float> weights(edges.size());
  for(i = 0; i < edges.size(); i++)
    weights[i] = edges[i].weight;
  normalize(weights, cfg["EdgeWeightTransform"][1], edge_weight_filter);
  for(i = 0; i < edges.size(); i++)
    edges[i].weight = weights[i];

  /* Create components. */
  components = Component::create(edges);
  if(components.size() < 1) {
    cout << "WARNING! Could not find any components in '"
	 << edge_table.source() << "'.\n";
    return false;
  }
  if(verbose_mode) {
    for(i = 0; i < components.size(); i++) {
      cout << "\t[" << (i + 1) << "]\t" << components[i].cardinality()
	   << '\t' << components[i].size() << "\n";
    }
  }

  /* Automatic visualization attributes. */
  bool label_flag = false;
  char delim = delimiter;
  if((label_mode == "on") || (label_mode == "vertex"))
    label_flag = true; 
  if(delim == '\0') delim = ' ';
  arcs = get_arcs(edges, delim, decoration_mode);
  emblems = get_emblems(edges, decoration_mode, label_flag);

  /* Update edge visualization from input file(s). */
  label_flag = false;
  if((label_mode == "on") || (label_mode == "edge"))
    label_flag = true; 
  for(tpos = tables.begin(); tpos != tables.end(); tpos++) {
    Table& t = tpos->second;
    map<string, Locus>& loci = variables[tpos->first];
    int head_var = t.column(cfg["EdgeHeadVariable"][1]);
    int tail_var = t.column(cfg["EdgeTailVariable"][1]);
    int color_var = loci["EdgeColorVariable"].column;
    int label_var = loci["EdgeLabelVariable"].column;
    int weight_var = loci["EdgeWeightVariable"].column;
    int width_var = loci["EdgeWidthVariable"].column;
    if(head_var < 0) continue;
    if(tail_var < 0) continue;

    /* Read edge information. */
    char buf[64];
    for(i = 0; i < t.size(); i++) {
      Row r = t[i];
      if(r.rank() < 1) continue;
      if(r[tail_var].length() < 1) continue;
      if(r[head_var].length() < 1) continue;

      string key = (r[head_var] + delim + r[tail_var]);
      if(arcs.count(key) < 1) continue;

      Arc& arc = arcs[key];
      if((color_var >= 0) && (r[color_var].length() > 0))
	arc.color = (int)(r.number(color_var));
      if(weight_var >= 0)
	arc.weight = r.number(weight_var); 
      if((weight_var >= 0) && label_flag) {
	float w = r.number(weight_var);
	if(w < 10) sprintf(buf, "%.2f", w);
	else if(w < 100) sprintf(buf, "%.1f", w);
	else sprintf(buf, "%.0f", w);
	arc.label = string(buf);
      }
      if((label_var >= 0) && label_flag)
	arc.label = r[label_var];
      if((width_var >= 0) && (r[width_var].length() > 0)) 
	arc.width = r.number(width_var);
    }
    if(label_var >= 0) label_flag = false; /* enforce empty labels */
  }    

  /* Apply edge weight mask. */
  weights = vector<float>(arcs.size());
  map<string, Arc>::iterator pos;
  for(i = 0, pos = arcs.begin(); pos != arcs.end(); pos++, i++)
    weights[i] = (pos->second).weight;
  normalize(weights, cfg["EdgeWeightMask"][1], edge_weight_mask);
  for(i = 0, pos = arcs.begin(); pos != arcs.end(); pos++, i++)
    if(weights[i] <= 0.0) (pos->second).width = -1.0;

  /* Update vertex visualization from input file(s). */
  label_flag = false;
  if((label_mode == "on") || (label_mode == "vertex"))
    label_flag = true; 
  for(tpos = tables.begin(); tpos != tables.end(); tpos++) {
    Table& t = tpos->second;
    map<string, Locus>& loci = variables[tpos->first];
    int name_var = t.column(cfg["VertexNameVariable"][1]);
    int color_var = loci["VertexColorVariable"].column;
    int label_var = loci["VertexLabelVariable"].column;
    int shape_var = loci["VertexShapeVariable"].column;
    int pattern_var = loci["VertexPatternVariable"].column;
    int size_var = loci["VertexSizeVariable"].column;
    int x_var = loci["VertexXVariable"].column;
    int y_var = loci["VertexYVariable"].column;
    int z_var = loci["VertexZVariable"].column;
    if(name_var < 0) continue;

    /* Read vertex information. */
    for(i = 0; i < t.size(); i++) {
      Row r = t[i];
      if(r.rank() < 1) continue;
      if(r[name_var].length() < 1) continue;
      if(emblems.count(r[name_var]) < 1) continue;

      Emblem& emblem = emblems[r[name_var]];
      if((color_var >= 0) && (r[color_var].length() > 0)) 
	emblem.color = (int)(r.number(color_var));
      if((label_var >= 0) && label_flag)
	emblem.label = r[label_var];
      if((shape_var >= 0) && (r[shape_var].length() > 0)) 
	emblem.shape = (int)(r.number(shape_var));
      if((pattern_var >= 0) && (r[pattern_var].length() > 0)) 
	emblem.pattern = (int)(r.number(pattern_var));
      if((size_var >= 0) && (r[size_var].length() > 0)) 
	emblem.size = r.number(size_var);
      
      vector<float> coord(0);
      if(r[x_var].length() > 0) coord.push_back(r.number(x_var));
      if(r[y_var].length() > 0) coord.push_back(r.number(y_var));
      if(z_var >= 0) {
	if(r[z_var].length() > 0) coord.push_back(r.number(z_var));
	if(coord.size() < 3) coord.clear();
      } 
      if(coord.size() > 1)
	coordinates[r[name_var]] = coord;
    }
  }    

  return true;
}

/*
 *
 */
static map<string, Arc>
get_arcs(vector<Edge>& edges, char delim, bool deco) {
  unsigned int i;
  map<string, Arc> arcs;

  for(i = 0; i < edges.size(); i++) {
    string key = (edges[i].head + delim + edges[i].tail);
    Arc a = {-1, DEFAULT_WIDTH, DEFAULT_WEIGHT, ""};
    if(deco) {
      a.color = get_color(edges[i].weight);
      a.width = edges[i].weight;
    }
    arcs[key] = a;
  }

  return arcs;
}

/*
 *
 */
static map<string, Emblem>
get_emblems(vector<Edge>& edges, bool deco, bool label_flag) {
  unsigned int i;
  map<string, Emblem> emblems;

  /* Collect raw statistics. */
  for(i = 0; i < edges.size(); i++) {
    string head = edges[i].head;
    string tail = edges[i].tail;
    Emblem eH = {-1, -1, -1, DEFAULT_SIZE, ""};
    Emblem eT = {-1, -1, -1, DEFAULT_SIZE, ""};
    if(label_flag) {
      eH.label = head;
      eT.label = tail;
    }

    if(deco) {
      /* Reset counters. */
      if(emblems.count(head) < 1) {
	eH.color = 0;
	eH.size = 0.0;
	emblems[head] = eH;
      }
      if(emblems.count(tail) < 1) {
	eT.color = 0;
	eT.size = 0.0;
	emblems[tail] = eT;
      }
      
      /* Update statistics. */
      eH = emblems[head];
      eH.color += 1;
      eH.size += edges[i].weight;
      eT = emblems[tail];
      eT.color += 1;
      eT.size += edges[i].weight;
    }

    emblems[head] = eH;
    emblems[tail] = eT;
  }
  if(!deco) return emblems;
  
  /* Compute statistics. */
  float c_max = 0.0;
  float s_min = FLT_MAX;
  float s_max = -FLT_MAX;
  map<string, Emblem>::iterator pos;
  for(pos = emblems.begin(); pos != emblems.end(); pos++) {
    Emblem e = pos->second;
    if(e.color > c_max) c_max = e.color;
    if(e.size < s_min) s_min = e.size;
    if(e.size > s_max) s_max = e.size;
  }

  /* Use degree for size and strength for color. */
  c_max = log(1.001 + c_max);
  s_max = log(1.001 + s_max - s_min);
  for(pos = emblems.begin(); pos != emblems.end(); pos++) {
    Emblem e = pos->second;
    unsigned int degree = e.color;
    float strength = e.size;
    e.color = get_color(log(1.0 + degree)/c_max);
    e.size = (0.1 + log(1.0 + strength - s_min)/s_max);
    pos->second = e;
  }

  return emblems;
}

/*
 *
 */
static void
normalize(vector<float>& weights, string mode, vector<float> filter) {
  unsigned int i, k;
  unsigned int n = weights.size();
  unsigned int n_edges = 0;
  float w_min = FLT_MAX;
  float w_max = -FLT_MAX;
 
  /* Find limits. */
  if(n < 1) return;
  for(i = 0; i < n; i++) {
    if(weights[i] < w_min) w_min = weights[i];
    if(weights[i] > w_max) w_max = weights[i];
  }

  /* Uniform weights. */
  if(w_min == w_max)
    for(i = 0; i < n; i++)
      weights[i] = DEFAULT_WEIGHT; 
  
  /* Apply linear filter. */
  if(filter[0] == ABS_FILTER_code) {
    for(i = 0; i < n; i++) {
      if((weights[i] < filter[1]) || (weights[i] > filter[2]))
	weights[i] = -1.0;
    }
  }
  if(w_min == w_max) return;

  /* Sort weights and apply fractional filter. */
  vector<int> indices = sort(weights);
  if(filter[0] == FRAC_FILTER_code) {
    unsigned int ka = (unsigned int)(filter[1]*n);
    unsigned int kb = (unsigned int)(filter[2]*n);
    for(k = 0; (k < ka) && (k < n); k++) {
      i = indices[k];
      weights[i] = -1.0;
    }
    for(k = (kb + 1); k < n; k++) {
      i = indices[k];
      weights[i] = -1.0;
    }
  }
  if(mode == "none") return;

  /* Update limits. */
  w_min = FLT_MAX;
  w_max = -FLT_MAX;
  n_edges = 0;
  for(i = 0; i < n; i++) {
    if(weights[i] < 0.0) continue;
    if(weights[i] < w_min) w_min = weights[i];
    if(weights[i] > w_max) w_max = weights[i];
    n_edges++;
  }
  if(n_edges < 1) return;

  /* Linear transform. */
  if(w_max <= 0.0) w_max = FLT_EPSILON;
  if(mode == "lin") {
    for(i = 0; i < n; i++) {
      if(weights[i] < 0.0) continue;
      weights[i] /= w_max;
    }
    return;
  }

  /* Logarithmic transform. */
  if(mode == "log") {
    w_max = log(1.001 + w_max - w_min);
    for(i = 0; i < n; i++) {
      if(weights[i] < 0.0) continue;
      weights[i] = log(1.0 + weights[i] - w_min)/w_max;
    }
    return;
  }

  /* Rank transform. */
  unsigned int counter = 0;
  if(mode == "rank") {
    w_max = 1.0*n_edges;
    for(k = 0; k < n; k++) {
      i = indices[k];
      if(weights[i] < 0.0) continue;
      weights[i] = (++counter)/w_max;
    }
    return;
  }

  /* Default transform. */
  counter = 0;
  float w1_max = w_max;
  float w2_max = log(1.001 + w_max - w_min);
  float w3_max = 1.0*n_edges;
  for(k = 0; k < n; k++) {
    i = indices[k];
    if(weights[i] < 0.0) continue;
    float w1 = (weights[i])/w1_max;
    float w2 = log(1.0 + weights[i] - w_min)/w2_max;
    float w3 = (++counter)/w3_max;
    weights[i] = (w1 + w2 + w3)/3.0;
  }
}
