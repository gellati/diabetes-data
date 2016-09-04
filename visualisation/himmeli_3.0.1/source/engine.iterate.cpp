/* file: engine.iterate.cpp
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

/*
 *
 */
void
Engine::iterate(const float temp) {
  unsigned int i;
  unsigned int n_points = points.size();
  if(n_points < 2) return;

  /* Determine cell size and neighborhood radius. */
  vector<float> box = bbox();
  float width = box[2];
  float height = box[3];
  float cell_size = log(1.0 + bonds.size());
  if(cell_size < MIN_CELLSIZE) cell_size = MIN_CELLSIZE;

  /* Create cell structure. Note that a buffer of empty cells is
     left on the perimeter. */
  int m_rows = (int)(width/cell_size + 1.5);
  int m_cols = (int)(height/cell_size + 1.5); 
  vector<vector<int> > cells(m_rows*m_cols, vector<int>(0));
  for(i = 0; i < points.size(); i++) {
    int k = (int)((points[i].x)/cell_size + 0.5);
    int j = (int)((points[i].y)/cell_size + 0.5);
    cells[k*m_cols+j].push_back(i);
  }
  vector<Point> landscape = summarize(cells);

  /* Compute repulsive forces. */
  int r_nhood = (int)(1.0 + 0.1*(drand48()*m_rows + drand48()*m_cols));
  vector<int> nhood;
  vector<Point> g(points.size());
  for(i = 0; i < cells.size(); i++) {
    if(cells[i].size() < 1) continue;
    int row = i/m_cols;
    int col = i%m_cols;
    for(int k = (row - r_nhood); k <= (row + r_nhood); k++) {
      for(int j = (col - r_nhood); j <= (col + r_nhood); j++) {
	unsigned int index = (k*m_cols + j);
	if(index < 0) continue;
	if(index == i) continue;
	if(index >= cells.size()) continue;
	if(cells[index].size() < 1) continue; 
	nhood.push_back(index);
      }
    }
    repel(g, cells, landscape, nhood, i);
    nhood.clear();
  }
  
  /* Compute attractive forces. */ 
  attract(g, bonds);

  /* Update positions and compute energy. */
  for(i = 0; i < points.size(); i++) {
    float r = sqrt((g[i].x)*(g[i].x) + (g[i].y)*(g[i].y));
    if(r > temp) {
      g[i].x *= temp/r;
      g[i].y *= temp/r;
    }
    points[i].x += (0.8 + 0.4*drand48())*(g[i].x);
    points[i].y += (0.8 + 0.4*drand48())*(g[i].y);
  }
  rotate(points[0], drand48()*M_PI);
}
