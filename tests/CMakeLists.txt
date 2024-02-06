create_test_sourcelist(Tests
    CommonCxxTests.cpp
    baseTest.cpp
    priorityTest.cpp
)

add_executable(CommonCxxTests ${Tests})
target_link_libraries(CommonCxxTests ${PROJECT_NAME})
target_include_directories(CommonCxxTests PUBLIC ../include)

set(TestsToRun ${Tests})
remove(TestsToRun CommonCxxTests.cpp)

foreach(test ${TestsToRun})
    get_filename_component(TName ${test} NAME_WE)
    add_test(NAME ${TName} COMMAND CommonCxxTests ${TName})
endforeach()

