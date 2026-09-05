set(CrankTalkTest_Sources
    main.c
    scanner.c
    string.c
)

add_executable(CrankTalkTests ${CrankTalkTest_Sources})
target_link_libraries(CrankTalkTests CrankTalk)
add_test(NAME CrankTalkTests COMMAND CrankTalkTests)