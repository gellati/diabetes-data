/* file: graphobject.h
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

#ifndef graphobject_INCLUDED
#define graphobject_INCLUDED

#include <map>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <float.h>
#include <time.h>
#include <math.h>
#include "himmeli.h"
#include "tablet.h"
#include "scriptum.h"
#include "utilities.h"

#define graph_VERSION     "3.0.1"
#define MARGIN_WIDTH      1.5
#define NULL_FILTER_code  -1
#define ABS_FILTER_code   0
#define FRAC_FILTER_code  1

using namespace std;
using namespace himmeli;
using namespace tablet;
using namespace scriptum;

struct Emblem {
  int color;
  int shape;
  int pattern;
  float size;
  string label;
};

struct Arc {
  int color;
  float width;
  float weight;
  string label;
};

class CompareEdge {
public:
  bool operator()(const Edge& e1, const Edge& e2) const {
    return (e1.weight < e2.weight);
  };
};

class Locus {
private:
  string l_name;
public:
  int column;
  string heading;
  string file_name;
public:
  Locus() {
    l_name = "";
    heading = "";
    column = -1;
    file_name = "";
  };
  Locus(const string& tag, const Row& r) {
    l_name = tag;
    heading = r[1];
    column = -1;
    file_name = r[2];
  };
  void display() const {
    printf("\t%-22s ", l_name.c_str());
    if(heading.length() > 0) {
      string s = ("'" + file_name + "'");
      printf("%-16s ", s.c_str());
      s = ("'" + heading + "'");
      printf("%-16s ", s.c_str());
      if(column >= 0) printf("[%2d]\n", (column + 1));
      else printf("not found\n");
    }
    else
      printf("empty\n");
  };
  string name() const {return l_name;};
};

class GraphObject {
private:
  bool arrow_mode;
  bool decoration_mode;
  bool increment_mode;
  string label_mode;
  bool legend_mode;
  bool tree_mode;
  bool verbose_mode;
  char delimiter;
  unsigned int figure_limit;
  float distance_unit;
  float time_limit;
  string edge_weight_transform;
  vector<float> edge_weight_filter;
  vector<float> edge_weight_mask;
  ConfigTable cfg;
  vector<Component> components;
  map<string, Arc> arcs;
  map<string, Emblem> emblems;
  map<string, Table> tables;
  map<string, vector<float> > coordinates;
  map<string, map<string, Locus> > variables;
  bool configure();
  bool import();
  vector<float> print_legend(PostScript&);
  void print_links(PostScript&, map<string, Vertex>&, vector<Edge>&);
  void print_nodes(PostScript&, map<string, Vertex>&);
public:
  GraphObject();
  GraphObject(const string&);
  Component operator[](const int);
  unsigned int cardinality();
  void print();
  void run();
  std::string name() {return cfg["GraphName"][1];};
  void save();
  unsigned int size();
};

#endif /* graphobject_INCLUDED */
