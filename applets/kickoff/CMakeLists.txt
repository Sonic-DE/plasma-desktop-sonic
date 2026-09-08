set_source_files_properties(KickoffSingleton.qml ActionMenu.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

plasma_add_applet(org.kde.plasma.kickoff
QML_SOURCES
    config.qml
    main.xml
    AbstractKickoffItemDelegate.qml
    ActionMenu.qml
    ApplicationsPage.qml
    BasePage.qml
    ConfigGeneral.qml
    DropAreaGridView.qml
    DropAreaListView.qml
    EmptyPage.qml
    Footer.qml
    FullRepresentation.qml
    Header.qml
    HorizontalStackView.qml
    KickoffDropArea.qml
    KickoffGridDelegate.qml
    KickoffGridView.qml
    KickoffListDelegate.qml
    KickoffListView.qml
    KickoffSingleton.qml
    LeaveButtons.qml
    ListOfGridsView.qml
    ListOfGridsViewDelegate.qml
    NormalPage.qml
    PlacesPage.qml
    SectionView.qml
    VerticalStackView.qml
    main.qml
 RESOURCES
    main.xml
 GENERATE_APPLET_CLASS

 CPP_SOURCES
     ${CMAKE_SOURCE_DIR}/messagelogger.cpp
     liddetector.h
     liddetector.cpp
 )

target_link_libraries(org.kde.plasma.kickoff PRIVATE Qt::DBus)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set_source_files_properties(code/tools.js PROPERTIES QT_QML_SKIP_QMLDIR_ENTRY TRUE)
ecm_target_qml_sources(org.kde.plasma.kickoff SOURCES code/tools.js PATH code)
