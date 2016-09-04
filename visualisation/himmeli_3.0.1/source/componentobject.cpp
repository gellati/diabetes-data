/* file: componentobject.cpp
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

#include "componentobject.h"

#define ROOT     'R'
#define NORMAL   'N'
#define LINK     'L'
#define REJECTED 'E'

using namespace std;
using namespace himmeli;

class CompareLink {
public:
  bool operator()(const Link& l1, const Link& l2) const {
    return (l1.weight > l2.weight);
  };
};

static int  join(int, int, vector<int>&, int, int);

/*
 *
 */
ComponentObject::ComponentObject(const ComponentObject* co) {
  center = co->center;
  radius = co->radius;
  n_simu = co->n_simu;
  nodes = co->nodes;
  parameters = co->parameters;
}

/*
 *
 */
ComponentObject::ComponentObject(const vector<Link>& topology, 
				 const vector<string>& names) {
  /* Private variables. */
  n_simu = 0;
  center = -1;
  radius = -2;

  /* Default parameters. */
  parameters["TreeMode"] = "off";
  parameters["VerboseMode"] = "on";

  /* Sort links by weight. */
  vector<Link> links = topology;
  sort(links.begin(), links.end(), CompareLink());

  /* Create index maps. */
  unsigned int i;
  vector<int> tags((links.size() + 1), 0); 
  vector<int> array(names.size());
  vector<int> forest(names.size());
  for(i = 0; i < array.size(); i++) {
    array[i] = i;
    forest[i] = -1;
  }

  /* Kruskal's algorithm. */
  unsigned int n_roots = array.size();
  unsigned int n_links = links.size();
  while(n_links > 0) {
    unsigned int n = 0;
    unsigned int n_trees = 0;
    for(i = 0; i < n_links; i++) {
      int head = array[links[i].head];
      int tail = array[links[i].tail];

      /* Move connectors to front. */
      tags[i] = join(head, tail, forest, n_roots, n_trees);
      if(tags[i] == ROOT) n_trees++;
      if(tags[i] == LINK) {
	Link tmp = links[n];
	links[n] = links[i];
	links[i] = tmp;
	tags[i] = tags[n];
	tags[n] = LINK; 
	n++;
	continue;
      }
    }
    n_links = n;
    
    /* Update mapping and reset forest. */
    for(i = 0; i < array.size(); i++)
      array[i] = forest[array[i]];
    for(i = 0; i < n_trees; i++)
      forest[i] = -1;
    n_roots = n_trees;
  }
  if(n_roots > 1) {  
    fprintf(stderr, "ERROR! %s: Graph not connected.\n", __FILE__);
    exit(1);
  }

  /* Move accepted edges to the beginning of the list. */
  unsigned int n_trunk = 0;
  for(i = 0; i < links.size(); i++) {
    if(tags[i] == REJECTED) continue;
    Link tmp = links[n_trunk];
    links[n_trunk] = links[i];
    links[i] = tmp;
    n_trunk++;
  }

  /* Create nodes. */
  for(i = 0; i < names.size(); i++) {
    Node nd;
    nd.tree_degree_in = 0;
    nd.tree_degree_out = 0;
    nd.x = 0.0;
    nd.y = 0.0;
    nd.z = 0.0;
    nd.name = names[i];
    nd.links_in = vector<Link>(0);
    nd.links_out = vector<Link>(0);
    nodes.push_back(nd);
  }

  /* Update edge lists. */
  for(i = 0; i < n_trunk; i++) {
    int head = links[i].head;
    int tail = links[i].tail;
    nodes[head].tree_degree_in += 1;
    nodes[tail].tree_degree_out += 1;
    (nodes[head].links_in).push_back(links[i]);
    (nodes[tail].links_out).push_back(links[i]);
    tags[i] = 0;
  }
  for(; i < links.size(); i++) {
    int head = links[i].head;
    int tail = links[i].tail;
    (nodes[head].links_in).push_back(links[i]);
    (nodes[tail].links_out).push_back(links[i]);
    tags[i] = 0;
  }

  /* Store node degrees. */
  for(i = 0; i < nodes.size(); i++)
    array[i] = (nodes[i].tree_degree_in + nodes[i].tree_degree_out);

  /* Iteratively tag links that have a leaf terminus until 
     a single link remains. */
  radius = 1;
  Link ln = links[0];
  while(n_trunk > 2) {
    unsigned int n = 0;
    for(i = 0; i < n_trunk; i++) {
      int head = links[i].head;
      int tail = links[i].tail;
      if((array[head] <= 1) || (array[tail] <= 1)) continue;
      tags[head] = radius;
      tags[tail] = radius;
      ln = links[i];
      links[i] = links[n];
      links[n++] = ln;
    }
    
    for(i = n; i < n_trunk; i++) {
      array[links[i].head] -= 1;
      array[links[i].tail] -= 1;
    }
    n_trunk = n;
    radius++;
  }
  
  /* Select center vertex. */
  if(n_trunk == 1) {
    if(tags[ln.head] > tags[ln.tail]) center = ln.head;
    else center = ln.tail;  
  }
  else {
    if((links[0].head == links[1].head) ||
       (links[0].head == links[1].tail))
      center = links[1].head;
    else
      center = links[1].tail;
  }
}

/*
 *
 */
static int
join(int head, int tail, vector<int>& forest, int n_forest, int layer) {
  if(head < 0) return -1;
  if(tail < 0) return -1;
  if(head >= n_forest) return -1;
  if(tail >= n_forest) return -1;
  if((forest[head] < 0) && (forest[tail] < 0)) {
    forest[head] = layer;
    forest[tail] = layer;
    return ROOT;
  }
  if(forest[head] < 0) {
    forest[head] = forest[tail];
    return NORMAL;
  }
  if(forest[tail] < 0) {
    forest[tail] = forest[head];
    return NORMAL;
  }
  if(forest[head] != forest[tail])
    return LINK;
  return REJECTED;
}
