set(CrankTalk_Sources
    heap.c
    object-model.c
)

add_library(CrankTalk ${CrankTalk_Sources})

add_executable(crank-talk main.c)
target_link_libraries(crank-talk CrankTalk)
