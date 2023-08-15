set(files "574;575;576;624;625;626;627;628;629;630;641;642;643;644;645;646;647;648;671;672;673;674;675;676;677")
list(TRANSFORM files PREPEND ${CMAKE_SOURCE_DIR}/data/unpacked/21Mg_)
list(TRANSFORM files APPEND .root)

foreach(file ${files})
  if(NOT EXISTS ${file})
    message(FATAL_ERROR
            "Could not find file ${file}. "
            "Has something changed? "
            "Try deleting CMakeCache.txt and re-running.")
  endif()
  # check whether the unpacked file has already been sorted - if it has, we don't want to do it again
  string(REPLACE unpacked ${sorted_data_dir} mfile ${file})
  string(REPLACE .root m.root mfile ${mfile})
  if(EXISTS ${mfile})
    list(REMOVE_ITEM files ${file})
  endif()
endforeach()

if(files) # any unpacked files still need to be sorted?
  file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data/${sorted_data_dir})
  message(STATUS "Sorting unpacked files in ${CMAKE_SOURCE_DIR}/data/unpacked and saving the resulting matched files to ${CMAKE_SOURCE_DIR}/data/${sorted_data_dir}")

  set(SORT_CMD "Sorter -Q -s ${CMAKE_SOURCE_DIR}/setup/setup.json -m ${CMAKE_SOURCE_DIR}/setup/matcher.json -o ${CMAKE_SOURCE_DIR}/data/${sorted_data_dir}/ 2>&1 >/dev/null")
  string(REPLACE ";" " " files "${files}")
  message(STATUS "Sorting using GNU parallel...")
  execute_process(COMMAND bash -c "parallel ${SORT_CMD} ::: ${files}" COMMAND_ECHO STDOUT)
  message(STATUS "Sorting complete.")
endif()
unset(files)

set(FILES_SORTED YES CACHE INTERNAL "Unpacked data files have all been sorted.")