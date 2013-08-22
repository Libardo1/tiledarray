# - Try to find MADNESS lib
#
# This module will search for MADNESS components in MADNESS_DIR and the default
# search path. This module will define:
#
#  MADNESS_FOUND                - System has MADNESS lib with correct version
#  MADNESS_INCLUDE_DIR          - The MADNESS include directory
#  MADNESS_INCLUDE_DIRS         - The MADNESS include directory
#  MADNESS_LIBRARY              - The MADNESS library
#  MADNESS_LIBRARIES            - The MADNESS libraries and their dependencies
#  MADNESS_${COMPONENT}_FOUND   - System has the specified MADNESS COMPONENT
#  MADNESS_${COMPONENT}_LIBRARY - The MADNESS COMPONENT library
#
#  Valid COMPONENTS are:
#    MADmra 
#    MADtinyxml 
#    MADmuparser
#    MADlinalg
#    MADtensor
#    MADmisc
#    MADworld
#

include(LibFindMacros)

# Dependencies
libfind_package(MADNESS LAPACK)

# Find the include dir
if (NOT MADNESS_FIND_QUIETLY)
  message(STATUS "Looking for madness_config.h")
endif()
find_path(MADNESS_INCLUDE_DIR
    NAMES madness_config.h 
    PATHS ${MADNESS_DIR})
if (NOT MADNESS_FIND_QUIETLY)
  if(MADNESS_PROCESS_INCLUDES)
    message(STATUS "Looking for madness_config.h - found")
  else()
    message(STATUS "Looking for madness_config.h - not found")
  endif()
endif()

# Remove duplicate libraries
list(REMOVE_DUPLICATES MADNESS_FIND_COMPONENTS)

# Add any missing dependencies to the commponent list

# Add missing dependencies for MADmra
list(FIND MADNESS_FIND_COMPONENTS MADmra _lib_find)
if(NOT _lib_find EQUAL -1)
  foreach(_libdep MADtinyxml MADmuparser MADlinalg)
    list(FIND MADNESS_FIND_COMPONENTS ${_libdep} _lib_find)
    if(_lib_find EQUAL -1)
      list(APPEND MADNESS_FIND_COMPONENTS ${_libdep})
     endif()
  endforeach()
endif()

# Add missing dependencies for MADlinalg
list(FIND MADNESS_FIND_COMPONENTS MADlinalg _lib_find)
if(NOT _lib_find EQUAL -1)
  list(FIND MADNESS_FIND_COMPONENTS MADtensor _lib_find)
  if(_lib_find EQUAL -1)
    list(APPEND MADNESS_FIND_COMPONENTS MADtensor)
   endif()
endif()

# Add missing dependencies for MADtensor
list(FIND MADNESS_FIND_COMPONENTS MADtensor _lib_find)
if(NOT _lib_find EQUAL -1)
  list(FIND MADNESS_FIND_COMPONENTS MADmisc _lib_find)
  if(_lib_find EQUAL -1)
    list(APPEND MADNESS_FIND_COMPONENTS MADmisc)
   endif()
endif()

# Add missing dependencies for MADmisc
list(FIND MADNESS_FIND_COMPONENTS MADmisc _lib_find)
if(NOT _lib_find EQUAL -1)
  list(FIND MADNESS_FIND_COMPONENTS MADworld _lib_find)
  if(_lib_find EQUAL -1)
    list(APPEND MADNESS_FIND_COMPONENTS MADworld)
  endif()
endif() 

# Finally the library itself
foreach (COMPONENT ${MADNESS_FIND_COMPONENTS})

  # Find the component library.
  if (NOT MADNESS_FIND_QUIETLY)
    message(STATUS "Looking for ${COMPONENT}")
  endif()
  set(MADNESS_${COMPONENT}_LIBRARY MADNESS_${COMPONENT}-NOTFOUND)
  find_library(MADNESS_${COMPONENT}_LIBRARY 
      NAMES ${COMPONENT}
      PATHS ${MADNESS_DIR})
  
  # Set result status variables
  if (MADNESS_${COMPONENT}_LIBRARY)
    set(MADNESS_${COMPONENT}_FOUND TRUE)
  else()
    set(MADNESS_${COMPONENT}_FOUND FALSE)
  endif()
  
  # Print the result of the search
  if (NOT MADNESS_FIND_QUIETLY)
    if(MADNESS_${COMPONENT}_FOUND)
      message(STATUS "Looking for ${COMPONENT} - found")
    else()
      message(STATUS "Looking for ${COMPONENT} - not found")
    endif()
  endif()
endforeach()

# Add the found libraries to MADNESS_PROCESS_LIBS so that they are in the proper order
foreach(COMPONENT MADmra MADtinyxml MADmuparser MADlinalg MADtensor MADmisc MADworld)
  if(MADNESS_${COMPONENT}_FOUND)
    list(APPEND MADNESS_LIBRARY ${MADNESS_${COMPONENT}_LIBRARY})
  endif()
endforeach()

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries this lib depends on.
set(MADNESS_PROCESS_INCLUDES MADNESS_INCLUDE_DIR)
set(MADNESS_PROCESS_LIBS MADNESS_LIBRARY LAPACK_LIBRARIES)
libfind_process(MADNESS)
