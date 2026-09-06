set(CrankTalk_Sources
    array.c
    association.c
    behavior.c
    context.c
    dictionary.c
    dynarray.c
    errors.c
    function.c
    gc.c
    heap.c
    object-model.c
    ordered-collection.c
    parse-tree.c
    scanner.c
    set.c
    stack-frame.c
    string.c
)

add_library(CrankTalk ${CrankTalk_Sources})

add_executable(crank-talk main.c)
target_link_libraries(crank-talk CrankTalk)
