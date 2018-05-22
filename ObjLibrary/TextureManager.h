//
//  TextureManager.h
//
//  A global service to handle Textures.
//
//  This file is part of the ObjLibrary, by Richard Hamilton,
//    which is copyright Hamilton 2009-2015.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.  If you are in a position of
//    authority, you may forbid others ffrom using them in areas
//    that fall under your authority.  For example, a professor
//    could forbid students from using them for a class project,
//    or an employer could forbid employees using for a company
//    project.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <iostream>

class Texture;



namespace TextureManager
{

//
//  getCount
//
//  Purpose: To deterine the number of textures loaded.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The number of textures loaded.
//  Side Effect: N/A
//

unsigned int getCount ();

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified index.
//  Parameter(s):
//    <1> index: Which texture
//  Precondition(s):
//    <1> index < getCount()
//  Returns: A reference to the texture with index index.
//  Side Effect: N/A
//

const Texture& get (unsigned int index);

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified name.  Name comparisons are
//           case-insensitive.
//  Parameter(s):
//    <1> a_name: The name of the texture
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> a_name != NULL
//    <2> logfile != ""
//  Returns: The Texture with name a_name.  If there is no
//           texture loaded with name a_name, and a_name ends in
//           ".bmp" (case-insensitive), a new Texture loaded
//           from a file named a_name is returned.  Otherwise an
//           all-white dummy texture is returned.
//  Side Effect: If the texture has not been loaded, and a
//               suitable file exists, the texture is loaded
//               and error messages may be generated.  If a
//               logfile or logging stream is specified, any
//               loading errors are written to that file or
//               stream.  Otherwise, any loading errors are
//               written to the standard error stream.  If the
//               dummy texture is returned, an error message is
//               always printed.  If the texture has already
//               been loaded, no error messages will be printed.
//

const Texture& get (const char* a_name);
const Texture& get (const char* a_name,
                    const std::string& logfile);
const Texture& get (const char* a_name,
                    std::ostream& r_logstream);

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified name.  Name comparisons are
//           case-insensitive.
//  Parameter(s):
//    <1> name: The name of the texture
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> logfile != ""
//  Returns: The Texture with name name.  If there is no texture
//           loaded with name name, and name ends in ".bmp"
//           (case-insensitive), a new Texture loaded from a
//           file named name is returned.  Otherwise an
//           all-white dummy texture is returned.
//  Side Effect: If the texture has not been loaded, and a
//               suitable file exists, the texture is loaded
//               and error messages may be generated.  If a
//               logfile or logging stream is specified, any
//               loading errors are written to that file or
//               stream.  Otherwise, any loading errors are
//               written to the standard error stream.  If the
//               dummy texture is returned, an error message is
//               always printed.  If the texture has already
//               been loaded, no error messages will be printed.
//

const Texture& get (const std::string& name);
const Texture& get (const std::string& name,
                    const std::string& logfile);
const Texture& get (const std::string& name,
                    std::ostream& r_logstream);

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified name.  Name comparisons are
//           case-insensitive.
//  Parameter(s):
//    <1> a_name: The name of the texture
//    <2> wrap: The behaviour of the texture outside of the
//              range [0, 1)
//    <3> mag_filter: The magnification filter
//    <4> min_filter: The minification filter
//    <5> logfile: The file to write loading errors to
//    <5> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> a_name != NULL
//    <2> wrap == GL_REPEAT || wrap == GL_CLAMP
//    <3> mag_filter == GL_NEAREST ||
//        mag_filter == GL_LINEAR
//    <4> min_filter == GL_NEAREST ||
//        min_filter == GL_LINEAR ||
//        min_filter == GL_NEAREST_MIPMAP_NEAREST ||
//        min_filter == GL_NEAREST_MIPMAP_LINEAR ||
//        min_filter == GL_LINEAR_MIPMAP_NEAREST ||
//        min_filter == GL_LINEAR_MIPMAP_LINEAR
//    <5> logfile != ""
//  Returns: The Texture with name a_name.  If there is no
//           texture loaded with name a_name, and a_name ends in
//           ".bmp" (case-insensitive), a new Texture loaded
//           from a file named a_name and with wrapping mode
//           wrap, magnification filter mag_filter, and
//           minification filter min_filter is returned.
//           Otherwise an all-white dummy texture is returned.
//           If the texture was already loaded, the wrapping
//           mode and magnification and minification filters
//           are ignored.
//  Side Effect: If the texture has not been loaded, and a
//               suitable file exists, the texture is loaded
//               and error messages may be generated.  If the
//               texture is loaded, it is set to have a wrapping
//               mode of wrap and magnification and minification
//               filters of mag_filter and min_filter.  If a
//               logfile or logging stream is specified, any
//               loading errors are written to that file or
//               stream.  Otherwise, any loading errors are
//               written to the standard error stream.  If the
//               dummy texture is returned, an error message is
//               always printed.  If the texture has already
//               been loaded, no error messages will be printed.
//

const Texture& get (const char* a_name,
                    unsigned int wrap,
                    unsigned int mag_filter,
                    unsigned int min_filter);
const Texture& get (const char* a_name,
                    unsigned int wrap,
                    unsigned int mag_filter,
                    unsigned int min_filter,
                    const std::string& logfile);
const Texture& get (const char* a_name,
                    unsigned int wrap,
                    unsigned int mag_filter,
                    unsigned int min_filter,
                    std::ostream& r_logstream);

//
//  get
//
//  Purpose: To retrieve a reference to the texture with the
//           specified name.  Name comparisons are
//           case-insensitive.
//  Parameter(s):
//    <1> name: The name of the texture
//    <2> wrap: The behaviour of the texture outside of the
//              range [0, 1)
//    <3> mag_filter: The magnification filter
//    <4> min_filter: The minification filter
//    <5> logfile: The file to write loading errors to
//    <5> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> wrap == GL_REPEAT || wrap == GL_CLAMP
//    <2> mag_filter == GL_NEAREST ||
//        mag_filter == GL_LINEAR
//    <3> min_filter == GL_NEAREST ||
//        min_filter == GL_LINEAR ||
//        min_filter == GL_NEAREST_MIPMAP_NEAREST ||
//        min_filter == GL_NEAREST_MIPMAP_LINEAR ||
//        min_filter == GL_LINEAR_MIPMAP_NEAREST ||
//        min_filter == GL_LINEAR_MIPMAP_LINEAR
//    <4> logfile != ""
//  Returns: The Texture with name name.  If there is no texture
//           loaded with name name, and name ends in ".bmp"
//           (case-insensitive), a new Texture loaded from a
//           file named name and with wrapping mode wrap,
//           magnification filter mag_filter, and minification
//           filter min_filter is returned.  Otherwise an
//           all-white dummy texture is returned.  If the
//           texture was already loaded, the wrapping mode and
//           magnification and minification filters are ignored.
//  Side Effect: If the texture has not been loaded, and a
//               suitable file exists, the texture is loaded
//               and error messages may be generated.  If the
//               texture is loaded, it is set to have a wrapping
//               mode of wrap and magnification and minification
//               filters of mag_filter and min_filter.  If a
//               logfile or logging stream is specified, any
//               loading errors are written to that file or
//               stream.  Otherwise, any loading errors are
//               written to the standard error stream.  If the
//               dummy texture is returned, an error message is
//               always printed.  If the texture has already
//               been loaded, no error messages will be printed.
//

const Texture& get (const std::string& name,
                    unsigned int wrap,
                    unsigned int mag_filter,
                    unsigned int min_filter);
const Texture& get (const std::string& name,
                    unsigned int wrap,
                    unsigned int mag_filter,
                    unsigned int min_filter,
                    const std::string& logfile);
const Texture& get (const std::string& name,
                    unsigned int wrap,
                    unsigned int mag_filter,
                    unsigned int min_filter,
                    std::ostream& r_logstream);

//
//  activate
//
//  Purpose: To activate the texture with the specified index.
//  Parameter(s):
//    <1> index: Which texture
//  Precondition(s):
//    <1> index < getCount()
//  Returns: The texture with index index is activated.
//  Side Effect: N/A
//

void activate (unsigned int index);

//
//  activate
//
//  Purpose: To activate the texture with the specified name.
//            Name comparisons are case-insensitive.
//  Parameter(s):
//    <1> a_name: The name of the texture
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> a_name != NULL
//    <2> logfile != ""
//  Returns: N/A
//  Side Effect: The Texture with name a_name is activated.  If
//               there is no texture loaded with name a_name,
//               and a_name ends in ".bmp" (case-insensitive), a
//               new Texture loaded from a file named a_name is
//               activated.  Otherwise an all-white dummy
//               texture is activated.  This function may
//               generate error messages.  If a logfile or
//               logging stream is specified, any loading errors
//               are written to that file or stream.  Otherwise,
//               any loading errors are written to the standard
//               error stream.  If the dummy texture is
//               activated, an error message is always printed.
//               If the texture has already been loaded, no
//               error messages will be printed.
//

void activate (const char* a_name);
void activate (const char* a_name,
               const std::string& logfile);
void activate (const char* a_name,
               std::ostream& r_logstream);

//
//  activate
//
//  Purpose: To activate the texture with the specified name.
//            Name comparisons are case-insensitive.
//  Parameter(s):
//    <1> name: The name of the texture
//    <2> logfile: The file to write loading errors to
//    <2> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> logfile != ""
//  Returns: N/A
//  Side Effect: The Texture with name name is activated.  If
//               there is no texture loaded with name name, and
//               name ends in ".bmp" (case-insensitive), a new
//               Texture loaded from a file named name is
//               activated.  Otherwise an all-white dummy
//               texture is activated.  This function may
//               generate error messages.  If a logfile or
//               logging stream is specified, any loading errors
//               are written to that file or stream.  Otherwise,
//               any loading errors are written to the standard
//               error stream.  If the dummy texture is
//               activated, an error message is always printed.
//               If the texture has already been loaded, no
//               error messages will be printed.
//

void activate (const std::string& name);
void activate (const std::string& name,
               const std::string& logfile);
void activate (const std::string& name,
               std::ostream& r_logstream);

//
//  activate
//
//  Purpose: To activate the texture with the specified name.
//            Name comparisons are case-insensitive.
//  Parameter(s):
//    <1> a_name: The name of the texture
//    <2> wrap: The behaviour of the texture outside of the
//              range [0, 1)
//    <3> mag_filter: The magnification filter
//    <4> min_filter: The minification filter
//    <5> logfile: The file to write loading errors to
//    <5> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> a_name != NULL
//    <2> wrap == GL_REPEAT || wrap == GL_CLAMP
//    <3> mag_filter == GL_NEAREST ||
//        mag_filter == GL_LINEAR
//    <4> min_filter == GL_NEAREST ||
//        min_filter == GL_LINEAR ||
//        min_filter == GL_NEAREST_MIPMAP_NEAREST ||
//        min_filter == GL_NEAREST_MIPMAP_LINEAR ||
//        min_filter == GL_LINEAR_MIPMAP_NEAREST ||
//        min_filter == GL_LINEAR_MIPMAP_LINEAR
//    <5> logfile != ""
//  Returns: N/A
//  Side Effect: The Texture with name a_name is activated.  If
//               there is no texture loaded with name a_name,
//               and a_name ends in ".bmp" (case-insensitive), a
//               new Texture loaded from a file named a_name and
//               with wrapping mode wrap, magnification filter
//               mag_filter, and minification filter min_filter
//               is activated.  Otherwise an all-white dummy
//               texture is activated.  This function may
//               generate error messages.  If a logfile or
//               logging stream is specified, any loading errors
//               are written to that file or stream.  Otherwise,
//               any loading errors are written to the standard
//               error stream.  If the dummy texture is
//               activated, an error message is always printed.
//               If the texture has already been loaded, no
//               error messages will be printed and the wrapping
//               mode and magnification and minification filters
//               are ignored.
//

void activate (const char* a_name,
               unsigned int wrap,
               unsigned int mag_filter,
               unsigned int min_filter);
void activate (const char* a_name,
               unsigned int wrap,
               unsigned int mag_filter,
               unsigned int min_filter,
               const std::string& logfile);
void activate (const char* a_name,
               unsigned int wrap,
               unsigned int mag_filter,
               unsigned int min_filter,
               std::ostream& r_logstream);

//
//  activate
//
//  Purpose: To activate the texture with the specified name.
//            Name comparisons are case-insensitive.
//  Parameter(s):
//    <1> name: The name of the texture
//    <2> wrap: The behaviour of the texture outside of the
//              range [0, 1)
//    <3> mag_filter: The magnification filter
//    <4> min_filter: The minification filter
//    <5> logfile: The file to write loading errors to
//    <5> r_logstream: The stream to write loading errors to
//  Precondition(s):
//    <1> wrap == GL_REPEAT || wrap == GL_CLAMP
//    <2> mag_filter == GL_NEAREST ||
//        mag_filter == GL_LINEAR
//    <3> min_filter == GL_NEAREST ||
//        min_filter == GL_LINEAR ||
//        min_filter == GL_NEAREST_MIPMAP_NEAREST ||
//        min_filter == GL_NEAREST_MIPMAP_LINEAR ||
//        min_filter == GL_LINEAR_MIPMAP_NEAREST ||
//        min_filter == GL_LINEAR_MIPMAP_LINEAR
//    <4> logfile != ""
//  Returns: N/A
//  Side Effect: The Texture with name name is activated.  If
//               there is no texture loaded with name name, and
//               name ends in ".bmp" (case-insensitive), a new
//               Texture loaded from a file named name and with
//               wrapping mode wrap, magnification filter
//               mag_filter, and minification filter min_filter
//               is activated.  Otherwise an all-white dummy
//               texture is activated.  This function may
//               generate error messages.  If a logfile or
//               logging stream is specified, any loading errors
//               are written to that file or stream.  Otherwise,
//               any loading errors are written to the standard
//               error stream.  If the dummy texture is
//               activated, an error message is always printed.
//               If the texture has already been loaded, no
//               error messages will be printed and the wrapping
//               mode and magnification and minification filters
//               are ignored.
//

void activate (const std::string& name,
               unsigned int wrap,
               unsigned int mag_filter,
               unsigned int min_filter);
void activate (const std::string& name,
               unsigned int wrap,
               unsigned int mag_filter,
               unsigned int min_filter,
               const std::string& logfile);
void activate (const std::string& name,
               unsigned int wrap,
               unsigned int mag_filter,
               unsigned int min_filter,
               std::ostream& r_logstream);

//
//  isLoaded
//
//  Purpose: To determine if a texture with the specified name
//           exists.  Name comparisons are case-insensitive.
//  Parameter(s):
//    <1> a_name: The name of the texture
//  Precondition(s):
//    <1> a_name != NULL
//  Returns: Whether there is a texture with name a_name.
//  Side Effect: N/A
//

bool isLoaded (const char* a_name);

//
//  isLoaded
//
//  Purpose: To determine if a texture with the specified name
//           exists.  Name comparisons are case-insensitive.
//  Parameter(s):
//    <1> name: The name of the texture
//  Precondition(s): N/A
//  Returns: Whether there is a texture with name name.
//  Side Effect: N/A
//

bool isLoaded (const std::string& name);

//
//  isDummyTexture
//
//  Purpose: To determine if the specified Texture is the dummy
//           texture returned when a get command fails.
//  Parameter(s):
//    <1> texture: The texture
//  Precondition(s): N/A
//  Returns: Whether texture is the dummy Texture returned by
//           the get function.
//  Side Effect: N/A
//

bool isDummyTexture (const Texture& texture);

//
//  add
//
//  Purpose: To add the texture with the specified OpenGL
//           texture name to the texture manager.
//  Parameter(s):
//    <1> opengl_name: The OpenGL texture name for the texture
//    <2> a_name: The name of the texture to add
//  Precondition(s):
//    <1> opengl_name != 0
//    <2> a_name != NULL
//    <3> !isLoaded(a_name)
//  Returns: The texture with name opengl_name.
//  Side Effect: The texture with OpenGL texture name
//               opengl_name is added to the texture manager
//               under the name a_name.
//

const Texture& add (unsigned int opengl_name,
                    const char* a_name);

//
//  add
//
//  Purpose: To add the texture with the specified OpenGL
//           texture name to the texture manager.
//  Parameter(s):
//    <1> opengl_name: The OpenGL texture name for the texture
//    <2> name: The name of the texture
//  Precondition(s):
//    <1> opengl_name != 0
//    <2> !isLoaded(name)
//  Returns: The texture with name opengl_name.
//  Side Effect: The texture with OpenGL texture name
//               opengl_name is added to the texture manager
//               under the name name.
//

const Texture& add (unsigned int opengl_name,
                    const std::string& name);

//
//  add
//
//  Purpose: To add the specified Texture to the texture manager
//           with the specified name.
//  Parameter(s):
//    <1> texture: The Texture
//    <2> a_name: The name of the texture to add
//  Precondition(s):
//    <1> texture.isSet()
//    <2> a_name != NULL
//    <3> !isLoaded(a_name)
//  Returns: tetxure.
//  Side Effect: Texture texture is added to the texture manager
//               under the name a_name.
//

const Texture& add (const Texture& texture,
                    const char* a_name);

//
//  add
//
//  Purpose: To add the specified Texture to the texture manager
//           with the specified name.
//  Parameter(s):
//    <1> texture: The Texture
//    <2> name: The name of the texture
//  Precondition(s):
//    <1> texture.isSet()
//    <2> !isLoaded(name)
//  Returns: tetxure.
//  Side Effect: Texture texture is added to the texture manager
//               under the name name.
//

const Texture& add (const Texture& texture,
                    const std::string& name);

//
//  unloadAll
//
//  Purpose: To remove all textures from the texture manager.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: tetxure.
//  Side Effect: All textures are removed from the texture
//               manager.
//

void unloadAll ();



};	// end of namespace TextureManager

#endif
