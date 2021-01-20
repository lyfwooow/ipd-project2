cmake_minimum_required(VERSION 3.3)
project(prj06 C CXX)
include(.ipd/cmake/CMakeLists.txt)

# C source files common to multiple targets.
set(COMMON_C
    src/ballot.c
    src/ballot_box.c
    src/helpers.c
    src/libvc.c)

# We want to compile versions of the code with different values for
# MAX_CANDIDATES compiled in. This CMake function adds two targets (the
# count program and the tests) with MAX_CANDIDATES defined to to the
# value of its argument.
function(add_project_targets max)
    add_c_program(irv-${max}
            src/irv.c
            ASAN
            UBSAN
            ${COMMON_C}
            DEFINES MAX_CANDIDATES=${max})

    add_c_test_program(test_ballot-${max}
            test/test_ballot.c
            ASAN
            UBSAN
            ${COMMON_C}
            DEFINES MAX_CANDIDATES=${max})

    add_c_test_program(test_ballot_box-${max}
            test/test_ballot_box.c
            ASAN
            UBSAN
            ${COMMON_C}
            DEFINES MAX_CANDIDATES=${max})

    # Make test programs depend on main `irv` program so they can
    # run it and know it will be built:
    add_dependencies(test_ballot_box-${max} irv-${max})
    add_dependencies(test_ballot-${max} irv-${max})
endfunction(add_project_targets)

# Here are four sizes you might want to use. If you want to write tests
# that only work at particular sizes, you can check MAX_CANDIDATES in
# the relevant test cases and return early if it’s too small.
add_project_targets(3)
add_project_targets(7)
add_project_targets(15)
add_project_targets(31)
