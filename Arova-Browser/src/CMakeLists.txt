set(ARORA_SOURCES
    main.cpp
    aboutdialog.cpp
    acceptlanguagedialog.cpp
    autofilldialog.cpp
    autofillmanager.cpp
    autosaver.cpp
    browserapplication.cpp
    browsermainwindow.cpp
    clearbutton.cpp
    clearprivatedata.cpp
    downloadmanager.cpp
    modelmenu.cpp
    modeltoolbar.cpp
    plaintexteditsearch.cpp
    searchbar.cpp
    searchbutton.cpp
    searchlineedit.cpp
    settings.cpp
    sourcehighlighter.cpp
    sourceviewer.cpp
    tabbar.cpp
    tabwidget.cpp
    toolbarsearch.cpp
    webactionmapper.cpp
    webpage.cpp
    webview.cpp
    webviewsearch.cpp
)

set(ARORA_HEADERS
    aboutdialog.h
    acceptlanguagedialog.h
    autofilldialog.h
    autofillmanager.h
    autosaver.h
    browserapplication.h
    browsermainwindow.h
    clearbutton.h
    clearprivatedata.h
    downloadmanager.h
    modelmenu.h
    modeltoolbar.h
    plaintexteditsearch.h
    searchbar.h
    searchbutton.h
    searchlineedit.h
    settings.h
    sourcehighlighter.h
    sourceviewer.h
    tabbar.h
    tabwidget.h
    toolbarsearch.h
    webactionmapper.h
    webpage.h
    webview.h
    webviewsearch.h
)

set(ARORA_FORMS
    aboutdialog.ui
    autofilldialog.ui
    acceptlanguagedialog.ui
    downloaditem.ui
    downloads.ui
    searchbanner.ui
    settings.ui
)

set(ARORA_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/data/data.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/data/graphics/graphics.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/data/searchengines/searchengines.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/htmls/htmls.qrc
)

qt_add_executable(arova ${ARORA_SOURCES} ${ARORA_HEADERS} ${ARORA_FORMS} ${ARORA_RESOURCES})

target_include_directories(arova PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/xbel
    ${CMAKE_CURRENT_SOURCE_DIR}/history
    ${CMAKE_CURRENT_SOURCE_DIR}/locationbar
    ${CMAKE_CURRENT_SOURCE_DIR}/network
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/networkcookiejar
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/clicktoflash
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/useragent
)

target_link_libraries(arova PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::WebEngineWidgets
)

target_compile_definitions(arova PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_STRICT_ITERATORS
)

# Sub-module sources
target_sources(arova PRIVATE
    # adblock
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockblockednetworkreply.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblocknetwork.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockpage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockrule.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockschemeaccesshandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblocksubscription.cpp
)

target_sources(arova PRIVATE
    # adblock headers
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockblockednetworkreply.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblocknetwork.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockpage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockrule.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockschemeaccesshandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblocksubscription.h
)

target_sources(arova PRIVATE
    # bookmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/addbookmarkdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/bookmarksdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/bookmarksmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/bookmarksmenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/bookmarksmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/bookmarkstoolbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/bookmarknode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/xbel/xbelreader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/xbel/xbelwriter.cpp
)

target_sources(arova PRIVATE
    # history
    ${CMAKE_CURRENT_SOURCE_DIR}/history/history.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/historycompleter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/history/historymanager.cpp
)

target_sources(arova PRIVATE
    # locationbar
    ${CMAKE_CURRENT_SOURCE_DIR}/locationbar/locationbar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/locationbar/locationbarsiteicon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/locationbar/privacyindicator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/locationbar/autocompleteprovider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/locationbar/suggestionpopup.cpp
)

target_sources(arova PRIVATE
    # network
    ${CMAKE_CURRENT_SOURCE_DIR}/network/fileaccesshandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/networkaccessmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/networkdiskcache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/networkproxyfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/schemeaccesshandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/cookiedialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/cookieexceptionsdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/cookieexceptionsmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/cookiejar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/cookiemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/networkcookiejar/networkcookiejar.cpp
)

target_sources(arova PRIVATE
    # opensearch
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchengine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchenginedelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchengineaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchenginemodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchreader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchwriter.cpp
)

target_sources(arova PRIVATE
    # qwebplugins
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/arovawebplugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/webpluginfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/clicktoflash/clicktoflash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/clicktoflash/clicktoflashplugin.cpp
)

target_sources(arova PRIVATE
    # utils
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/editlistview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/edittableview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/edittreeview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/languagemanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/lineedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/networkaccessmanagerproxy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/singleapplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/squeezelabel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/treesortfilterproxymodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/webpageproxy.cpp
)

target_sources(arova PRIVATE
    # useragent
    ${CMAKE_CURRENT_SOURCE_DIR}/useragent/useragentmenu.cpp
)

if(APPLE)
    set_target_properties(arova PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info_mac.plist
        OUTPUT_NAME Arova
    )
endif()

if(WIN32)
    target_sources(arova PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utils/explorerstyle.cpp)
    target_link_libraries(arova PRIVATE user32 advapi32 gdi32)
endif()

# Forms from submodules
target_sources(arova PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/adblock/adblockdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/addbookmarkdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/bookmarks/bookmarksdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/history/history.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/opensearch/opensearchdialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/network/passworddialog.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/network/proxy.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/cookies.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/network/cookiejar/cookiesexceptions.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/clicktoflash/clicktoflash.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/clicktoflash/clicktoflashsettings.ui
)

# Resources from submodules
target_sources(arova PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/qwebplugins/clicktoflash/clicktoflash.qrc
)

if(UNIX AND NOT APPLE)
    install(TARGETS arova RUNTIME DESTINATION bin)
endif()

# Deploy the translations next to the executable so they are found at runtime
file(GLOB ARORA_TRANSLATIONS ${CMAKE_CURRENT_SOURCE_DIR}/locale/*.qm)
add_custom_command(TARGET arova POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:arova>/locale"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARORA_TRANSLATIONS} "$<TARGET_FILE_DIR:arova>/locale/"
    COMMENT "Copying translations"
)

install(FILES ${ARORA_TRANSLATIONS} DESTINATION share/arova/locale)
