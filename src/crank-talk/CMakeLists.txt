set(CrankTalk_Sources
    analysis-build.c
    analysis-evaluation.c
    array.c
    association.c
    behavior.c
    bytecode.c
    bytes.c
    context.c
    control-flow.c
    dictionary.c
    dynarray.c
    environment.c
    errors.c
    eval.c
    function.c
    gc.c
    gui-sdl2.c
    gui.c
    heap.c
    hir.c
    io.c
    macros.c
    message.c
    numbers.c
    object-model.c
    ordered-collection.c
    parse-tree.c
    parser.c
    primitives.c
    random.c
    scanner.c
    script.c
    set.c
    sound-sdl2.c
    sound.c
    source-position.c
    stack-frame.c
    stb-impls.c
    string.c
    time.c
)

add_library(CrankTalk ${CrankTalk_Sources})
target_link_libraries(CrankTalk ${CrankTalk_DEP_LIBS})

add_executable(crank-talk main.c)
target_link_libraries(crank-talk CrankTalk)
