if(NOT DEFINED RUNNER)
    message(FATAL_ERROR "RUNNER is required")
endif()

if(NOT EXISTS "${RUNNER}")
    message(FATAL_ERROR "runner not found: ${RUNNER}")
endif()

set(input "parse LEFT\nparse XYZ\nparse DROP\nend\n")
set(expected "parse(LEFT)=LEFT\nparse(XYZ)=NONE\nparse(DROP)=DROP\n")
set(input_file "${RUNNER}.smoke.in")
file(WRITE "${input_file}" "${input}")

execute_process(
    COMMAND "${RUNNER}"
    INPUT_FILE "${input_file}"
    OUTPUT_VARIABLE actual
    ERROR_VARIABLE stderr
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "case runner exited with ${result}\n${stderr}")
endif()

if(NOT actual STREQUAL expected)
    message(FATAL_ERROR "unexpected smoke output\nactual:\n${actual}\nexpected:\n${expected}")
endif()

message(STATUS "case runner smoke OK: ${RUNNER}")
