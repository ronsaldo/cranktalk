set(CrankTalk_Sources
    array.c
    context.c
    dynarray.c
    errors.c
    heap.c
    object-model.c
    set.c
    string.c
)

add_library(CrankTalk ${CrankTalk_Sources})

add_executable(crank-talk main.c)
target_link_libraries(crank-talk CrankTalk)
