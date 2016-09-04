/* file: graphobject.save.cpp
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

/*
 *
 */
void
GraphObject::save() {
  unsigned int i, k;
  unsigned long n;
  if(components.size() < 1) return;
  if(verbose_mode) printf("\nSaving graph:\n");

  /* Open vertex file. */
  char iobuf[131072];
  string vfile = (cfg["GraphName"][1] + ".vertices.txt");
  FILE* output = fopen(vfile.c_str(), "w");
  if(output == NULL) {
    printf("WARNING! Cannot open '%s'.\n", vfile.c_str());
    return;
  }
  setvbuf(output, iobuf, _IOFBF, 131072);

  /* Check vertex name heading. */
  string heading = cfg["VertexNameVariable"][1];
  if(heading.length() < 1) heading = "NAME";

  /* Save topology. */
  n = 0;
  n += fprintf(output, "COMP\t%s\tX\tY\tZ\t", heading.c_str());
  n += fprintf(output, "DG_IN\tDG_OUT\tST_IN\tST_OUT\t");
  n += fprintf(output, "COLOR\tSHAPE\tSIZE\tLABEL\n");
  for(i = 0; i < components.size(); i++) {
    map<string, Vertex>::iterator pos;
    map<string, Vertex> vertices = components[i].vertices();
    for(pos = vertices.begin(); pos != vertices.end(); pos++) {
      Vertex v = pos->second;
      n += fprintf(output, "%d\t%s", (i + 1), (pos->first).c_str());
      n += fprintf(output, "\t%7.2f", v.x);
      n += fprintf(output, "\t%7.2f", v.y);
      n += fprintf(output, "\t%7.2f", v.z);
      n += fprintf(output, "\t%d", v.degree_in);
      n += fprintf(output, "\t%d", v.degree_out);
      n += fprintf(output, "\t%7.2f", v.strength_in);
      n += fprintf(output, "\t%7.2f", v.strength_out);

      Emblem e = emblems[pos->first];
      n += fprintf(output, "\t%06d", e.color);
      n += fprintf(output, "\t%d", e.shape);
      n += fprintf(output, "\t%7.2f", e.size);
      n += fprintf(output, "\t%s", (e.label).c_str());
      n += fprintf(output, "\n");
    }
  }
  fclose(output);
  if(verbose_mode)
    cout << "\tWrote " << clarify(n) << " bytes to '" << vfile << "'.\n";

  /* Open edge file. */
  string efile = (cfg["GraphName"][1] + ".edges.txt");
  output = fopen(efile.c_str(), "w");
  if(output == NULL) {
    printf("WARNING! Cannot open '%s'.\n", efile.c_str());
    return;
  }
  setvbuf(output, iobuf, _IOFBF, 131072);

  /* Save topology. */
  char delim = delimiter;
  if(delim == '\0') delim = ' ';
  n = 0;
  n += fprintf(output, "COMP\t%s\t", cfg["EdgeHeadVariable"][1].c_str());
  n += fprintf(output, "%s\t", cfg["EdgeTailVariable"][1].c_str());
  n += fprintf(output, "WEIGHT\tCOLOR\tWIDTH\tLABEL\n");
  for(i = 0; i < components.size(); i++) {
    vector<Edge> edges = components[i].edges();
    for(k = 0; k < edges.size(); k++) {
      string head = edges[k].head;
      string tail = edges[k].tail;
      n += fprintf(output, "%d", (i + 1));
      n += fprintf(output, "\t%s", head.c_str());
      n += fprintf(output, "\t%s", tail.c_str());

      Arc a = arcs[head + delim + tail];
      n += fprintf(output, "\t%g", a.weight);
      n += fprintf(output, "\t%06d", a.color);
      n += fprintf(output, "\t%7.2f", a.width);
      n += fprintf(output, "\t%s", (a.label).c_str());
      n += fprintf(output, "\n");
    }
  }
  fclose(output);
  if(verbose_mode)
    cout << "\tWrote " << clarify(n) << " bytes to '" << efile << "'.\n";

  /* Open configuration file. */
  string cfg_file = (cfg["GraphName"][1] + ".config.txt");
  output = fopen(cfg_file.c_str(), "w");
  if(output == NULL) {
    printf("WARNING! Cannot open '%s'.\n", cfg_file.c_str());
    return;
  }
  setvbuf(output, iobuf, _IOFBF, 131072);

  /* Greeting. */
  n = 0;
  n += fprintf(output, "# Himmeli %s\n", graph_VERSION);
  n += fprintf(output, "# Use this configuration file to reconstruct ");
  n += fprintf(output, "your layout quickly.\n\n");
  n += fprintf(output, "# Below is a reprint of the original configuration ");
  n += fprintf(output, "file. The new instructions\n# that override some ");
  n += fprintf(output, "of the originals are at the end of this file.\n");

  /* Reprint of original configuration file. */
  Row prev;
  Table* t = (Table*)(&cfg);  
  for(i = 0; i < t->size(); i++) {
    Row r = (*t)[i];
    if((r[0] == prev[0]) && (r[1] == prev[1])) continue;

    bool flag = false;
    for(k = 0; k < r.size(); k++) {
      if(r[k].length() < 1) break;
      if(r[k][0] == '#') break;
      n += fprintf(output, "\t%s", r[k].c_str());
      flag = true;
    }
    if(flag) {
      prev = r;
      n += fprintf(output, "\n");
    }
  }

  /* Save necessary instructions to reconstruct results. */
  n += fprintf(output, "\n# New instructions for reconstruction.\n");
  n += fprintf(output, "EdgeColorVariable    COLOR  %s\n", efile.c_str());
  n += fprintf(output, "EdgeLabelVariable    LABEL  %s\n", efile.c_str());
  n += fprintf(output, "EdgeWidthVariable    WIDTH  %s\n", efile.c_str());
  if(cfg["VertexNameVariable"][1] == "")
    n += fprintf(output, "VertexNameVariable   NAME\n");
  n += fprintf(output, "VertexColorVariable  COLOR  %s\n", vfile.c_str());
  n += fprintf(output, "VertexShapeVariable  SHAPE  %s\n", vfile.c_str());
  n += fprintf(output, "VertexSizeVariable   SIZE   %s\n", vfile.c_str());
  n += fprintf(output, "VertexLabelVariable  LABEL  %s\n", vfile.c_str());
  n += fprintf(output, "VertexXVariable      X      %s\n", vfile.c_str());
  n += fprintf(output, "VertexYVariable      Y      %s\n", vfile.c_str());
  n += fprintf(output, "VertexZVariable      Z      %s\n", vfile.c_str());
  n += fprintf(output, "TimeLimit            0\n");
  n += fprintf(output, "DecorationMode       off\n");
  n += fprintf(output, "IncrementMode        on\n");

  /* Make date stamp. */
  time_t now = time(NULL);
  n += fprintf(output, "\n# %s\n", ctime(&now));

  fclose(output);
  if(verbose_mode)
    cout << "\tWrote " << clarify(n) << " bytes to '" << cfg_file << "'.\n";
}
