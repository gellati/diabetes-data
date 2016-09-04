/* file: scriptum.h
  Scriptum PostScript output library (originally part of CraneFoot) 
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

#ifndef scriptum_INCLUDED
#define scriptum_INCLUDED

#include <string>

namespace scriptum {

  class PostScript {
  private:
    void* buffer;
  public:

    /* Create an empty object. */
    PostScript();

     /* Copy constructor. This is added only for implicit use and should not
       be called explicitly. It does not create a true copy but transfers
       the data structures from the argument to the returned object. */
    PostScript(const PostScript&);

    /* Create a new postscript object with the output file indicated by
       the argument. */
    PostScript(const std::string&);

    /* Free resources and close output file. */
    ~PostScript();

    /* Transfer the contents of the argument to the calling object. Note that
       this does not correspond to duplicating the argument. */
    void operator=(const PostScript&);

    /* Return the value of a parameter with the given name. If name is
       unknown, an empty string is returned. */
    std::string operator[](const std::string&);

    /* Append PostScript code to the output file. */
    bool append(const std::string&);

    /* Assign a value (second argument) to a parameter (first argument).
       BackgroundColor        six digit integer RRGGBB
       Creator                anything
       DocumentMode           'normal' or 'encaps'
       FontSize               integer
       ForegroundColor        six digit integer RRGGBB
       PageOrientation        'portrait' or 'landscape'
       PageSize               'a0', 'a1', 'a2', 'a3', 'a4', 'a5',
                              'letter' or width,height
       VerboseMode            'true' or 'false' */
    bool assign(const std::string&, const std::string&);

    /* Close the output file. No more code can be appended to it.*/
    bool close();

    /* Page height (cm). */
    float height();

    /* Start a new page. Does not work for encapulated PostScript. */
    bool new_page();

    /* Return current PostScript code length. */
    unsigned long size() const;

    /* Page width (cm). */
    float width();

    /* Version identification. */
    static std::string version();

    /* Number of supported symbol patterns. */
    static unsigned int pattern_count();

    /* Number of supported symbol shapes. */
    static unsigned int shape_count();
  };
};

#endif /* scriptum_INCLUDED */
