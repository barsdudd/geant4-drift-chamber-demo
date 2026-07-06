cmake_minimum_required(VERSION 3.16...3.27)
project(DetectorDemo)

# --- Find the ROOT package ---
find_package(ROOT REQUIRED)

# Find the Geant4 package (require the UI and visualization modules)
find_package(Geant4 REQUIRED ui_all vis_all)

# --- Add the ROOT include directories ---
include_directories(${ROOT_INCLUDE_DIRS})

# Load Geant4's standard include settings and compile flags
include(${Geant4_USE_FILE})

# Collect all .cc and .hh files in the current directory
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)

# Define the executable
add_executable(DetectorDemo ${sources} ${headers})

# --- Link the ROOT libraries in addition to Geant4 ---
target_link_libraries(DetectorDemo ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# Copy vis.mac into the build directory so it can be loaded at runtime
file(COPY vis.mac DESTINATION ${PROJECT_BINARY_DIR})
