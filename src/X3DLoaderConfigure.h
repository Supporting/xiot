/*=========================================================================

/// This file is part of the x3dLoader library
/// Copyright (C) 2008 Kristian Sons (x3dLoader@actor3d.com)

/// This project has been funded with support from EDF R&D
/// Christophe MOUTON

/// TODO: Link

/// This library is free software; you can redistribute it and/or
/// modify it under the terms of the GNU Lesser General Public
/// License as published by the Free Software Foundation; either
/// version 2.1 of the License, or (at your option) any later version.

/// This library is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
/// Lesser General Public License for more details.

/// You should have received a copy of the GNU Lesser General Public
/// License along with this library; if not, write to the Free Software
/// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

=========================================================================*/
#ifndef __x3dexporterConfigure_h
#define __x3dexporterConfigure_h

#if defined(_MSC_VER) 
# pragma warning (disable: 4275) /* non-DLL-interface base class used */
# pragma warning (disable: 4251) /* needs to have dll-interface to be used by clients */
/* No warning for safe windows only functions */
# define _CRT_SECURE_NO_WARNINGS
#endif

// Setup export/import macro for DLL.  The symbol
// "x3dexporter_EXPORTS" is defined by CMake when building source
// files for a shared library named "x3dexporter".  For these
// sources we should export if building a shared library.  For other
// sources we should import if using a shared library.
#if defined(_WIN32)
# if defined(x3dLoader_EXPORTS)
#  define X3DLOADER_EXPORT __declspec(dllexport)
# else
#  define X3DLOADER_EXPORT __declspec(dllimport)
# endif
#else
# define X3DLOADER_EXPORT
#endif

#endif // __x3dexporterConfigure_h
