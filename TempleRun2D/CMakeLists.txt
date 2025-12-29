# CMakeList.txt : CMake project for TempleRun2D, include source and define
# project specific logic here.
#

# Add source to this project's executable.

find_package (SDL3 REQUIRED)
find_package (SDL3_image REQUIRED)
find_package (SDL3_mixer REQUIRED)
find_package (glm REQUIRED)

add_executable (TempleRun2D "TempleRun2D.cpp" "TempleRun2D.h"   "GameState.cpp" "entities/UIButton.h" "screens.cpp" "utilities.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TempleRun2D PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

target_link_libraries(TempleRun2D PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer glm::glm)
