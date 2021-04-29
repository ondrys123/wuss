set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Project 3rd party libraries are added here
Include(FetchContent)

# Add Catch2
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.4
)

FetchContent_MakeAvailable(Catch2)

# Find Boost
find_package(Boost
    REQUIRED
    COMPONENTS program_options
)
