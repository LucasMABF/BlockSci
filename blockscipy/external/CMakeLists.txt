find_path(DATE_INCLUDE_DIR date/date.h REQUIRED)
add_library(date INTERFACE)
target_include_directories(date SYSTEM INTERFACE ${DATE_INCLUDE_DIR})
