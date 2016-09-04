/* file: graphobject.cpp
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

static bool check_parameters(ConfigTable&);
static void print_greeting(int);
static void print_checklist();

/*
 *
 */
GraphObject::GraphObject() {
  print_greeting(1);
}

/*
 *
 */
GraphObject::GraphObject(const string& fname) {
  /* Check configuration file. */
  cfg = ConfigTable(fname, '\0');
  if(cfg.size() < 1) {
    cout << "WARNING! Configuration file '" << cfg.source()
	 << "' is empty or cannot be opened.\n";
    return;
  }

  /* Load data from input file(s). */
  if(cfg["VerboseMode"][1] != "off") print_greeting(0);
  if(!check_parameters(cfg)) {
    cerr << "ERROR! Erroneous instructions found.\n";
    print_checklist();
  }  
  else if(!configure()) {
    cerr << "ERROR! Configuration failed.\n";
    print_checklist();
  }
  else if(!import()) {
    cerr << "ERROR! Import failed.\n";
    print_checklist();
  }
}

/*
 *
 */
static bool
check_parameters(ConfigTable& cfg) {
  bool flag = true;

  /* Check non-implemented parameters. */
  if(cfg["3DMode"].size() > 0) {
    cout << "WARNING! 3DMode not implemented.\n";
    flag = false;
  }

  /* Check obsolite parameters. */
  if(cfg["EdgeWeightUnit"].size() > 0) {
    cout << "WARNING! \"EdgeWeightUnit\" is obsolite.\n";
    flag = false;
  }
  if(cfg["IndicatorMode"].size() > 0) {
    cout << "WARNING! \"IndicatorMode\" is obsolite, "
	 << "use \"ArrowMode\" instead.\n";
    flag = false;
  }
  if(cfg["PaperSize"].size() > 0) {
    cout << "WARNING! \"PaperSize\" is obsolite, "
	 << "use \"PageSize\" instead.\n";
    flag = false;
  }
  if(cfg["QuietRun"].size() > 0) {
    cout << "WARNING! \"QuietRun\" is obsolite, "
	 << "use \"VerboseMode\" instead.\n";
    flag = false;
  }
  if(cfg["Separator"].size() > 0) {
    cout << "WARNING! \"Separator\" is obsolite, "
	 << "use \"Delimiter\" instead.\n";
    flag = false;
  }

  /* Check critical parameters. */
  if(cfg["EdgeFile"].size() < 2) {
    cout << "WARNING! Edge file not defined.\n";
    flag = false;
  }
  if(cfg["GraphName"].size() < 2) {
    cout << "WARNING! Graph name not defined.\n";
    flag = false;
  }
  if(cfg["EdgeHeadVariable"].size() < 2) {
    cout << "WARNING! Edge head variable not defined.\n";
    flag = false;
  }
  if(cfg["EdgeTailVariable"].size() < 2) {
    cout << "WARNING! Edge tail variable not defined.\n";
    flag = false;
  }

  return flag;
}

/*
 *
 */
static void
print_greeting(int level) {
  cout << Graph::version() << "\tGraph\n";
  cout << Component::version() << "\tComponent\n";
  cout << PostScript::version() << "\tPostScript\n";
  cout << Table::version() << "\tTable\n";

  cout << "\nHimmeli, copyright (C) 2006 Ville-Petteri Makinen\n";
  cout << "Himmeli comes with ABSOLUTELY NO WARRANTY.\n";
  cout << "This is free software, and you are welcome to redistribute it\n";
  cout << "under certain conditions; see the license file for details.\n\n";

  if(level < 1) return;

  cout << "  Usage: himmeli CFGFILE\n\n";

  cout << "  Format of CFGFILE:\n";
  cout << "  PARAM                  VALUE1      VALUE2      VALUE3\n";
  cout << "  # ... comment line ...\n";
  cout << "\n";

  cout << "  # File parameters:\n";
  cout << "  EdgeFile               (string)\n";
  cout << "  GraphName              (string)\n";
  cout << "  VertexFile             (string)\n";
  cout << "\n";

  cout << "  # Structural parameters:\n";
  cout << "  EdgeHeadVariable       (string)\n";
  cout << "  EdgeTailVariable       (string)\n";
  cout << "  EdgeWeightVariable     (string)    (string)\n";
  cout << "  VertexNameVariable     (string)    (string)\n";
  cout << "  VertexXVariable        (string)    (string)\n";
  cout << "  VertexYVariable        (string)    (string)\n";
  cout << "  VertexZVariable        (string)    (string)\n";
  cout << "\n";

  cout << "  # Visualization parameters:\n";
  cout << "  EdgeColorVariable      (string)    (string)\n";
  cout << "  EdgeLabelVariable      (string)    (string)\n";
  cout << "  EdgeWidthVariable      (string)    (string)\n";
  cout << "  VertexColorVariable    (string)    (string)\n";
  cout << "  VertexLabelVariable    (string)    (string)\n";
  cout << "  VertexPatternVariable  (string)    (string)\n";
  cout << "  VertexShapeVariable    (string)    (string)\n";
  cout << "  VertexSizeVariable     (string)    (string)\n";
  cout << "  EdgeColorInfo          (string)    (integer)   # multiple\n";
  cout << "  VertexColorInfo        (string)    (integer)   # multiple\n";
  cout << "  VertexPatternInfo      (string)    (integer)   # multiple\n";
  cout << "  VertexShapeInfo        (string)    (integer)   # multiple\n";
  cout << "\n";

  cout << "  # Formatting and functional parameters:\n";
  cout << "  ArrowMode              on/off\n";
  cout << "  BackgroundColor        (integer)\n";
  cout << "  DecorationMode         on/off\n";
  cout << "  Delimiter              (character)/tab/ws\n";
  cout << "  DistanceUnit           (real)\n";
  cout << "  FigureLimit            (integer)\n";
  cout << "  ForegroundColor        (integer)\n";
  cout << "  EdgeWeightFilter       abs/frac    (real)    (real)\n";
  cout << "  EdgeWeightMask         abs/frac    (real)    (real)\n";
  cout << "  EdgeWeightTransform    auto/lin/log/rank/off\n";
  cout << "  FontSize               (integer)               # pt\n";
  cout << "  IncrementMode          on/off\n";
  cout << "  LabelMode              on/off/edge/vertex\n";
  cout << "  LegendMode             on/off\n";
  cout << "  PageOrientation        landscape/portrait\n";
  cout << "  PageSize               a0...a5/letter  a0...a5/letter/auto\n";
  cout << "  TimeLimit              (integer)               # sec\n";
  cout << "  TreeMode               on/off\n";
  cout << "  VerboseMode            on/off\n";
  cout << "\n";

  cout << "  Written by Ville-Petteri Makinen\n";
  cout << "  http://www.iki.fi/~vpmakine\n";
  cout << "\n";
}

/*
 *
 */
static void
print_checklist() {
  cout << "\nAn error occurred, so please check that\n";
  cout << "  1. Input files are accessible.\n";
  cout << "  2. Input files are not empty.\n";
  cout << "  3. You are using the correct delimiter (default is tab).\n";
  cout << "  4. The file names are configured correctly.\n";
  cout << "  5. The edge variables are configured correctly.\n";
  cout << "  6. Every data file contains necessary variables.\n";
  cout << "  7. Every instruction in the configuration file is unique.\n";
  cout << "  8. Variable names in the input files are unique.\n";
  cout << "\n";
}
