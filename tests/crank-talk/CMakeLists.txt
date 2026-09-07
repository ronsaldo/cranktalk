set(CrankTalkTest_Sources
    analysis-evaluation.c
    main.c
    parser.c
    scanner.c
    string.c
)

add_executable(CrankTalkTests ${CrankTalkTest_Sources})
target_link_libraries(CrankTalkTests CrankTalk)
add_test(NAME CrankTalkTests COMMAND CrankTalkTests)