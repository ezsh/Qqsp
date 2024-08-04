cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(qqsp VERSION 1.9.0)
include(FeatureSummary)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES off)

find_package(Qt6 6.7 COMPONENTS LinguistTools WebEngineCore WebEngineWidgets)
find_package(Qt6 6.7 REQUIRED COMPONENTS Core Gui Multimedia MultimediaWidgets Widgets)
find_package(Qsp REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR True)

add_executable(Qqsp)
target_sources(Qqsp PRIVATE
	main.cpp
	mainwindow.cpp
	qsptextbox.cpp
	qsplistbox.cpp
	qspinputbox.cpp
	qspimgcanvas.cpp
	qspmsgdlg.cpp
	qspinputdlg.cpp
	qspstr.cpp
	callbacks_gui.cpp
	comtools.cpp
	optionsdialog.cpp
	# videoframeprocessor.cpp
	videolabel.cpp
	mainwindow.h
	qsptextbox.h
	qsplistbox.h
	qspinputbox.h
	qspimgcanvas.h
	qspmsgdlg.h
	qspinputdlg.h
	qspstr.h
	callbacks_gui.h
	comtools.h
	optionsdialog.h
	# videoframeprocessor.h
	videolabel.h
	mainwindow.ui
	optionsdialog.ui
	qqsp.qrc
)

target_link_libraries(Qqsp PRIVATE Qsp::qsp Qt::Multimedia Qt::MultimediaWidgets Qt::Widgets)

if(Qt6WebEngineWidgets_FOUND)
	target_sources(Qqsp PRIVATE
		qspwebbox.cpp
		qspwebengineurlrequestinterceptor.cpp
		qspwebengineurlschemehandler.cpp
		qspexecwebengineurlschemehandler.cpp
		url_schemes.cpp
		qspwebchannel.cpp
		qspwebbox.h
		qspwebengineurlrequestinterceptor.h
		qspwebengineurlschemehandler.h
		qspexecwebengineurlschemehandler.h
		url_schemes.h
		qspwebchannel.h
	)

    target_compile_definitions(Qqsp PRIVATE _WEBBOX _WEBBOX_COMMON)
    target_link_libraries(Qqsp PRIVATE Qt::WebEngineCore Qt::WebEngineWidgets)
endif()

if (Qt6LinguistTools_FOUND)
	qt_add_translations(Qqsp
		TS_FILES
		    Qqsp.en.ts Qqsp.ru.ts Qqsp.uk.ts
			qt_uk.ts qtbase_uk.ts
	)
endif()

install(TARGETS Qqsp DESTINATION "${CMAKE_INSTALL_BINDIR}")
if (LINUX)
	install(FILES Qqsp.desktop DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")
	install(FILES qsp.mime DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/mime/packages" RENAME qsp.xml)
endif()

feature_summary(FATAL_ON_MISSING_REQUIRED_PACKAGES WHAT ALL)
