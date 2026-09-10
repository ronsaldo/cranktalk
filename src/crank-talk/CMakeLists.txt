set(CrankTalk_Sources
    analysis-evaluation.c
    array.c
    association.c
    behavior.c
    context.c
    dictionary.c
    dynarray.c
    environment.c
    errors.c
    eval.c
    function.c
    gc.c
    heap.c
    hir.c
    message.c
    numbers.c
    object-model.c
    ordered-collection.c
    parse-tree.c
    parser.c
    scanner.c
    set.c
    source-position.c
    stack-frame.c
    string.c
)

add_library(CrankTalk ${CrankTalk_Sources})
target_link_libraries(CrankTalk m)

add_executable(crank-talk main.c)
target_link_libraries(crank-talk CrankTalk)
