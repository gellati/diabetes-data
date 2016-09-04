/* file: componentobject.h
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

#ifndef componentobject_INCLUDED
#define componentobject_INCLUDED

#include <map>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "himmeli.h"
#include "utilities.h"

#define component_VERSION "1.0.0"
#define MAX_RC_DEPTH      10000

using namespace std;
using namespace himmeli;

struct Link {
  int head;
  int tail;
  float weight;
};

struct Node {
  unsigned int tree_degree_in;
  unsigned int tree_degree_out;
  float x;
  float y;
  float z;
  string name;
  vector<Link> links_in;
  vector<Link> links_out;
};

class ComponentObject {
private:
  int center;
  int radius;
  unsigned int n_simu;
  vector<Node> nodes;
  map<string, string> parameters;
  void walk();
public:
  ComponentObject() {};
  ComponentObject(const ComponentObject*);
  ComponentObject(const vector<Link>&, const vector<string>&);
  bool assign(const string&, const string&);
  unsigned int cardinality() const {return nodes.size();}; 
  vector<Edge> edges() const;
  unsigned int simulate(const float);
  unsigned int size() const {
    unsigned int i, n = 0;
    for(i = 0; i < nodes.size(); i++)
      n += (nodes[i].links_in).size();
    return n;
  };
  vector<Edge> tree() const;
  void update(const map<string, vector<float> >&);
  map<string, Vertex> vertices() const;
};

#endif /* componentobject_INCLUDED */
