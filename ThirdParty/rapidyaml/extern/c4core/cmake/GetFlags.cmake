
function(_intersperse_with_flag outvar flag)
    if(MSVC AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC") # it may be clang as well
        set(flagchar "/")
    else()
        set(flagchar "-")
    endif()
    set(out)
    foreach(i ${ARGN})
        if(NOT "${i}" STREQUAL "")
            set(out "${out} ${flagchar}${flag} ${i}")
        endif()
    endforeach()
    set(${outvar} ${out} PARENT_SCOPE)
endfunction()

function(get_define_flags outvar)
    _intersperse_with_flag(out D ${ARGN})
    set(${outvar} ${out} PARENT_SCOPE)
endfunction()

function(get_include_flags outvar)
    _intersperse_with_flag(out I ${ARGN})
    set(${outvar} ${out} PARENT_SCOPE)
endfunction()
