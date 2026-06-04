if(NOT DEFINED CASE_FILE)
    message(FATAL_ERROR "CASE_FILE is required")
endif()

if(NOT EXISTS "${CASE_FILE}")
    message(FATAL_ERROR "case file not found: ${CASE_FILE}")
endif()

if(NOT DEFINED MIN_CASES)
    set(MIN_CASES 1)
endif()

file(READ "${CASE_FILE}" case_text)
string(REGEX MATCHALL "(^|\n)case=" case_headers "${case_text}")
list(LENGTH case_headers case_count)

if(case_count LESS MIN_CASES)
    message(FATAL_ERROR "expected at least ${MIN_CASES} case(s), found ${case_count}: ${CASE_FILE}")
endif()

if(NOT case_text MATCHES "\ninput=" OR NOT case_text MATCHES "\noutput=")
    message(FATAL_ERROR "case file must contain input= and output= blocks: ${CASE_FILE}")
endif()

message(STATUS "VPL cases OK: ${case_count} case(s) in ${CASE_FILE}")
