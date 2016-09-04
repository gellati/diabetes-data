/* file: table.cpp
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

#include "table.h"

#define IOBUFCAP 524288

static unsigned int ID = 1;

extern char*        read_buffer(FILE*, unsigned int);
extern char*        trim_buffer(char*, int*);
extern char*        tidy_buffer(char*, int*, char);
extern unsigned int finish_buffer(char*, row_t*, unsigned int, char);

/*
 *
 */
Table::Table() {
  TableBuffer* tb = new TableBuffer(ID++);
  buffer = tb;
}

/*
 *
 */
Table::Table(const Table& t) {
  Table* pt = (Table*)(&t);
  buffer = pt->buffer;
  pt->buffer = new TableBuffer(ID++);
}

/*
 *
 */
Table::Table(const string& fname, const char sep) {
  int i;
  FILE* input;
  TableBuffer* tb = new TableBuffer(ID++);
  buffer = tb;

  /* Default state. */
  tb->data_source = (char*)malloc(fname.length() + 1);
  strcpy(tb->data_source, fname.c_str());

  /* Import data. */
  if((input = fopen(fname.c_str(), "r")) == NULL) return;

  /* Prepare buffer. */
  tb->buf = read_buffer(input, IOBUFCAP);
  fclose(input);

  if(sep != '\0') {
    tb->separator = sep;
    tb->buf = tidy_buffer(tb->buf, &(tb->n_rows), tb->separator);
  }
  else {
    tb->buf = trim_buffer(tb->buf, &(tb->n_rows));
    tb->separator = '\t';
  }
  if(tb->n_rows < 1) return;
  
  /* Create database. */
  tb->rank2locus = (int*)malloc((tb->n_rows)*sizeof(int));
  tb->rows = (row_t*)malloc((tb->n_rows)*sizeof(row_t));
  tb->n_cols = finish_buffer(tb->buf, tb->rows, tb->n_rows, tb->separator);
  for(i = 0; i < tb->n_rows; i++) {
    tb->rank2locus[i] = i;
    tb->buf_size += (tb->rows)[i].size;
  }
}

/*
 *
 */
Table::~Table() {
  TableBuffer* tb = (TableBuffer*)buffer;
  delete tb;
};

/*
 *
 */
int
Table::strcmp(const string& s1, const string& s2) {
  bool flag1 = (s1 == "");
  bool flag2 = (s2 == "");
  if(flag1 && flag2) return 0;
  if(flag1) return -1; 
  if(flag2) return 1;
  float f1 = atof(s1.c_str());
  float f2 = atof(s2.c_str());
  if(f1 < f2) return -1;
  if(f1 > f2) return 1;
  if(s1 < s2) return -1;
  if(s1 > s2) return 1;
  return 0;
}

/*
 *
 */
string
Table::version() {
  string s = "";
  s += VERSION;
  s += "\t";
  s += __DATE__;
  return s;
}
