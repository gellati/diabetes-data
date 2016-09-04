/* file: componentobject.simulate.cpp
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

#include "engine.h"
#include "componentobject.h"

/*
 *
 */
unsigned int
ComponentObject::simulate(const float limit) {
  unsigned int i;
  unsigned int n = 0;
  unsigned int n_links = size();
  bool tree_flag = (parameters["TreeMode"] == "on");
  bool verbose = (parameters["VerboseMode"] != "off");

  /* Starting configuration. */
  if(n_simu == 0) walk();
 
  /* Simulation parameters. */
  unsigned int counter = 0;
  clock_t alert = clock();
  time_t start = time(NULL);
  float dt = 0.0;
  float temp0 = 1.5;
  if(!tree_flag) {
    temp0 = log(1.0 + nodes.size());  
    if(nodes.size() < n_links)
      temp0 += log(1.0 + n_links - nodes.size());
  }
  srand(start);
  srand48(start);

  /* Initialize simulation engine. */
  Engine engine(nodes, tree_flag);

  /* Simulated annealing. */
  float temp = temp0;
  for(n = 0; (temp > 0.02) && (dt < 1.0); n++) {     
    if(fabs(clock() - 1.0*alert) > CLOCKS_PER_SEC/2) {
      alert = clock();
      if(n%2) dt += 0.5/limit;
      else dt = difftime(time(NULL), start)/limit;
      if(dt >= 1.0) break;
      float r = exp(10*(dt - 0.6));
      temp = (1.0 - r/(1.0 + r))*temp0;
      
      /* Progress monitor. */
      if(verbose) {
	switch(counter) {
	case 0: printf("\r|"); break;
	case 2: printf("\r/"); break;
	case 4: printf("\r-"); break;
	case 6: printf("\r\\"); break;
	}
	if(counter%2 == 0) {
	  printf("\t\t%d\t%3.0f%%\t%.2f ", n, 100.0*dt, temp);
	  fflush(stdout);
	}
	counter = (counter + 1)%8;
      }   
    }
    
    /* Update configuration. */
    engine.iterate(temp);
    temp *= (1.0 - 1.0/(100.0 + n_links));
  }
  if(verbose) printf("\r%80s\r", "");

  /* Update positions. */
  engine.align(0.0);
  for(i = 0; i < nodes.size(); i++) {
    vector<float> p = engine[i];
    nodes[i].x = p[0];
    nodes[i].y = p[1];
  }

  n_simu += n;
  return n;
}
