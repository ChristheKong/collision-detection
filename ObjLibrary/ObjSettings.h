//
//  ObjSettings.h
//
//  A module to encapsulate the general settings for the OBJ
//    library.
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

#ifndef OBJ_SETTINGS_H
#define OBJ_SETTINGS_H



//
//  An OBJ file often references one or more MTL material files
//    which in term reference one or more textures.  It is
//    possible to for the path from the OBJ file to be
//    implicitly prepended to the other files.  This is called
//    called path propagation and can be enabled or disabled as
//    through the use of a macro.
//
//  For example, consider the file "barrel.obj" in the "models"
//    subfolder.  The OBJ file references a material library
//    named "barrel.mtl" which in turn references an image named
//    "barrel.bmp".  The barrel is loaded with the filename
//    "models/barrel.obj".  If path propagation is disabled, the
//    material library and image will be loaded with the
//    filenames "barrel.mtl" and "barrel.bmp".  If, in contrast,
//    path propagation is enabled, the filenames used are
//    "models/barrel.mtl" and "models/barrel.bmp".
//
//  WARNING: Enabling and disabling path propagation after
//           models have been loaded can lead to unexpected
//           behaviour.
//
//  To enable path propogation, define the macro
//    OBJ_LIBRARY_PATH_PROPAGATION.
//
#define OBJ_LIBRARY_PATH_PROPAGATION



//
//  By default, the ObjLibrary only loads textures of type
//    ".bmp".  However, it can also load textures of type
//    ".png".  To do this, you will need the following
//    libraries:
//      1. ZLib 1.1.3 by Jean-loup Gailly and Mark Adlerun
//      2. LibPNG 1.0.2 by Guy Eric Schalnat, Group 42, Inc.,
//         Andreas Dilger, Glenn Randers-Pehrson
//      3. (unnamed) PNG loader library by Ben Wyatt
//
//  The assumed organization of files is as follows:
//    glpng.c
//    glpng.h
//    ObjLibrary
//      ObjLibrary library files
//    png
//      png library files
//    zlib
//      zlib library files
//
//  It would also be possible to use the other libraries as .lib
//    files, but those often lead to portability issues.
//
//  To enable loading of .png files, define the macro
//    OBJ_LIBRARY_LOAD_PNG_TEXTURES.
//
//#define OBJ_LIBRARY_LOAD_PNG_TEXTURES



//
//  By default, models are drawn in fixed-pipeline mode.  There
//    is a more powerful and faster implementation available if
//    your program is running shaders.  For this, you will need:
//
//  TODO: Finish this
//
//  The assumed organization of files is as follows:
//    glew.c
//    glew.h
//    ObjLibrary
//      ObjLibrary library files
//    wglew.c
//
//  To enable shader-based display, define the macro
//    OBJ_LIBRARY_SHADER_DISPLAY.
//
//  #define OBJ_LIBRARY_SHADER_DISPLAY





#endif
