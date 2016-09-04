/* file: utilities.cpp
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

#include <algorithm> 
#include "utilities.h"

using namespace std;

struct FloatInt {
  float f;
  int i;
};

class CompareFloatInt {
public:
  bool operator()(const FloatInt& x1, const FloatInt& x2) const {
    return (x1.f < x2.f);
  };
};

/*
 * Return element indices in the order of ascending values.
 */
vector<int>
sort(vector<float>& v) {
  unsigned int k;
  vector<int> indices(v.size(), -1);
  vector<FloatInt> values(v.size());

  for(k = 0; k < v.size(); k++) {
    values[k].f = v[k];
    values[k].i = k;
  }
  std::sort(values.begin(), values.end(), CompareFloatInt());  
  for(k = 0; k < v.size(); k++)
    indices[k] = values[k].i;

  return indices;
}

/*
 *
 */
unsigned int
get_color(float value) {
  unsigned int color = 0;
  float phi;
  float base = value;
  float red, gre, blu;
  if(base > 1.0) base = 1.0;
  if(base < 0.0) base = 0.0;

  base = pow(base, 1.2);
  phi = (0.18 + 0.85*M_PI*base);
  red = 1.2*fabs(pow(sin(phi + M_PI/2), 2));
  gre = fabs(pow(sin(phi + M_PI/4), 2));
  blu = fabs(pow(sin(phi - M_PI/12), 4));

  phi = sqrt(0.4 + base);
  red /= phi;
  gre /= phi;
  blu /= phi;
  if(red > 1.0) red = 1.0;
  if(gre > 1.0) gre = 1.0;
  if(blu > 1.0) blu = 1.0;

  red = floor(99.9*red);
  gre = floor(99.9*gre);
  blu = floor(99.9*blu);
  color += (unsigned int)(red*10000);
  color += (unsigned int)(gre*100);
  color += (unsigned int)(blu);

  return color;
}

/*
 *
 */
vector<float>
get_rgb(int code) {
  vector<float> rgb;
  if(code < 0) code = 0;
  if(code > 999999) code = 999999;
  rgb.push_back(((code/10000)%100)/99.0);
  rgb.push_back(((code/100)%100)/99.0);
  rgb.push_back((code%100)/99.0);
  return rgb;
}

/*
 *
 */
string
clarify(unsigned long n) {
  int exponent = 0;
  float size = 1.0*n;
  char buf[32];

  if(size >= 1000.0) {
    while(size >= 100.0) {
      size /= 1024.0;
      exponent++;
    }
    if(size < 10.0) sprintf(buf, "%4.2f?", size);
    else if(size < 100.0) sprintf(buf, "%4.1f?", size);
    switch(exponent) {
    case 1: buf[4] = 'K'; break;
    case 2: buf[4] = 'M'; break;
    case 3: buf[4] = 'G'; break;
    case 4: buf[4] = 'T'; break;
    case 5: buf[4] = 'P'; break;
    }
  }
  else
    sprintf(buf, "%4.0f ", size);

  return string(buf);
}

/*
 *
 */
string
pacify(string s) {
  char c = '\0';
  unsigned int i;
  unsigned int len = s.length();
  for(i = 0; i < len; i++) {
    c = s[i];
    if(!isprint(c)) c = '?';
    if(strchr("()\\", c) != NULL) c = ' ';
    s[i] = c;
  } 
  return s;
}

/*
 *
 */
string
pacify(string s, unsigned int n) {
  char c = '\0';
  unsigned int i;
  unsigned int len = s.length();
  if(len > n) len = n;
  for(i = 0; i < len; i++) {
    c = s[i];
    if(!isprint(c)) c = '?';
    if(strchr("()\\", c) != NULL) c = ' ';
    s[i] = c;
  } 
  return s;
}
