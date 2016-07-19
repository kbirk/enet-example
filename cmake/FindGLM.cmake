# Finds GLM

find_path(GLM_INCLUDE_DIR glm/glm.hpp
	HINTS ${GLM_INCLUDEDIR} ${GLM_INCLUDE_DIRS}
	PATH_SUFFIXES glm)

set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR)
