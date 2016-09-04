/* file: table.h
  Tablet text import library (originally part of CraneFoot)
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
    Makinen V-P, Parkkonen M, Wessman M, Groop P-H, Kanninen T and Kaski K,
    High-throughput pedigree drawing,
    Eur J Hum Gen, 13, 987-989, 2005.

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

#ifndef table_INCLUDED
#define table_INCLUDED

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "tablet.h"

#define VERSION    "1.2.1"
#define BUF_LIMIT  67108864

using namespace std;
using namespace tablet;

typedef struct {
  int rank;
  int n_keys;
  int n_fields;
  unsigned int size;
  char* key_0;
  char* key_1;
  char* key_2;
  char* key_3;
  char* line;
  int* keycols;
} row_t;

class RowObject: public Row {
public:
  RowObject();
  void fill(const void*);
};

class TableBuffer {
public:
  char separator;
  int n_rows;
  int n_cols;
  unsigned int id_code;
  unsigned int buf_size;
  char* buf;
  int* keycols;
  int* rank2locus;
  row_t* rows;
  char* data_source;
public:
  TableBuffer(unsigned int id) {
    separator = '\0';
    id_code = id;
    n_rows = 0;
    n_cols = 0;
    buf_size = 0;
    buf = NULL;
    data_source = NULL;
    keycols = NULL;
    rank2locus = NULL;
    rows = NULL;
  };
  ~TableBuffer() {
    if(buf != NULL) free(buf);
    if(keycols != NULL) free(keycols);
    if(rank2locus != NULL) free(rank2locus);
    if(rows != NULL) free(rows);
    if(data_source != NULL) free(data_source);
  };
};

extern int rowcmp(const void*, const void*);

#endif /* table_INCLUDED */




