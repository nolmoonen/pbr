add_executable(embedfile embedfile.c)

# list of resources to be added to executable
set(EMBEDDED_RESOURCES "" PARENT_SCOPE)

# {name} name of the created .c-file
# {path} relative path from top-level CMakeList.txt file
function(embed name path)
    add_custom_command(OUTPUT ${name}.c COMMAND embedfile ${name} ../${path} DEPENDS ${path})
    set(EMBEDDED_RESOURCES ${EMBEDDED_RESOURCES} ${name}.c PARENT_SCOPE)
endfunction()