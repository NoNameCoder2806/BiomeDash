# CMakeList.txt : CMake project for BiomeDash, include source and define
# project specific logic here.
#

# Add source to this project's executable.

find_package (SDL3 REQUIRED)
find_package (SDL3_image REQUIRED)
find_package (SDL3_mixer REQUIRED)
find_package (SDL3_ttf REQUIRED)
find_package (glm REQUIRED)

add_executable (BiomeDash "BiomeDash.cpp" "BiomeDash.h"   "GameState.cpp" "ui/UIButton.h" "screens.cpp" "utilities.cpp" "collision.cpp" "gamelogic.cpp" "UIState.h" "entities/Transition.h" "ui/UILabel.h"  "ui/AnimatedUIButton.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BiomeDash PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

target_link_libraries(BiomeDash PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_mixer::SDL3_mixer SDL3_ttf::SDL3_ttf glm::glm)
