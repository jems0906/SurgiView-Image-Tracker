cmake_minimum_required(VERSION 3.21)

project(SurgiViewImageTracker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(SURGIVIEW_BUILD_TESTS "Build unit tests" ON)
option(SURGIVIEW_ENABLE_DCMTK "Enable real DICOM decoding through DCMTK" OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Multimedia Test)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(SurgiViewImageTracker
    src/main.cpp
    src/app/SurgiViewController.cpp
    src/app/FrameImageProvider.cpp
    src/imaging/DicomSeriesLoader.cpp
    src/imaging/DicomDecoder.cpp
    src/tracking/InstrumentTracker.cpp
    src/measurement/DepthMeasurement.cpp
    src/recording/PlaybackRecorder.cpp
)

target_include_directories(SurgiViewImageTracker PRIVATE src)

target_link_libraries(SurgiViewImageTracker
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Multimedia
)

target_compile_definitions(SurgiViewImageTracker PRIVATE SURGIVIEW_HAS_DCMTK=0)

if(SURGIVIEW_ENABLE_DCMTK)
    find_package(DCMTK REQUIRED)
    target_compile_definitions(SurgiViewImageTracker PRIVATE SURGIVIEW_HAS_DCMTK=1)

    if(TARGET DCMTK::DCMTK)
        target_link_libraries(SurgiViewImageTracker PRIVATE DCMTK::DCMTK)
    elseif(DCMTK_LIBRARIES)
        target_include_directories(SurgiViewImageTracker PRIVATE ${DCMTK_INCLUDE_DIRS})
        target_link_libraries(SurgiViewImageTracker PRIVATE ${DCMTK_LIBRARIES})
    else()
        message(FATAL_ERROR "DCMTK found but no linkable target or libraries were provided")
    endif()
endif()

qt_add_qml_module(SurgiViewImageTracker
    URI SurgiView
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

if(SURGIVIEW_BUILD_TESTS)
    enable_testing()

    qt_add_executable(SurgiViewDepthMeasurementTests
        tests/test_depthmeasurement.cpp
        src/measurement/DepthMeasurement.cpp
    )

    target_include_directories(SurgiViewDepthMeasurementTests PRIVATE src)

    target_link_libraries(SurgiViewDepthMeasurementTests
        PRIVATE
            Qt6::Core
            Qt6::Gui
            Qt6::Test
    )

    qt_add_executable(SurgiViewTrackerTests
        tests/test_tracker.cpp
        src/tracking/InstrumentTracker.cpp
        src/measurement/DepthMeasurement.cpp
    )

    target_include_directories(SurgiViewTrackerTests PRIVATE src)

    target_link_libraries(SurgiViewTrackerTests
        PRIVATE
            Qt6::Core
            Qt6::Gui
            Qt6::Test
    )

    add_test(NAME SurgiViewDepthMeasurementTests COMMAND SurgiViewDepthMeasurementTests)
    add_test(NAME SurgiViewTrackerTests COMMAND SurgiViewTrackerTests)
endif()
