if(NOT EXISTS ${unpacked_data_dir})
  message(FATAL_ERROR
      "The path ${unpacked_data_dir} does not exist. "
      "Please provide the correct path to the unpacked (or 'mapped') data of the experiment, "
      "i.e. a directory with files named '21Mg_XXX.root', in the variable 'unpacked_data_dir' "
      "in the file ${CMAKE_PARENT_LIST_FILE}.")
endif()

list(APPEND misnamed_data_files Mg21_545.root Mg21_546.root Mg21_547.root)
list(APPEND correct_data_file_names 21Mg_545.root 21Mg_546.root 21Mg_547.root)
list(TRANSFORM misnamed_data_files PREPEND ${unpacked_data_dir}/)
list(TRANSFORM correct_data_file_names PREPEND ${unpacked_data_dir}/)

foreach(wrong correct IN ZIP_LISTS misnamed_data_files correct_data_file_names)
  execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${wrong} ${correct} COMMAND_ECHO STDOUT)
endforeach()
unset(misnamed_data_files)
unset(correct_data_file_names)

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/data COMMAND_ECHO STDOUT
                COMMAND ${CMAKE_COMMAND} -E create_symlink ${unpacked_data_dir} ${CMAKE_SOURCE_DIR}/data/unpacked COMMAND_ECHO STDOUT)

set(PATH_CONFIG_OK YES CACHE INTERNAL "Unpacked data dir found, necessary symlinks created and data dir in project created.")