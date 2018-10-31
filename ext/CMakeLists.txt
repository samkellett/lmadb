function(add_interface_library name)
	add_library(${name} INTERFACE)
	add_library(${name}::${name} ALIAS ${name})

	target_include_directories(${name}
    	INTERFACE
        	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${name}/include>
	)
endfunction()

# in-tree dependencies that provide their own cmake targets.
add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_subdirectory(googletest EXCLUDE_FROM_ALL)

# dependencies that don't and require us to provide a shim.
add_interface_library(cppcoro)
