cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(qqsp VERSION 1.9.0)
include(FeatureSummary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Multimedia Widgets)
find_package(Qsp REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_INCLUDE_CURRENT_DIR True)

add_executable(qqsp)
target_sources(qqsp PRIVATE
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
	videoframeprocessor.cpp
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
	videoframeprocessor.h
	videolabel.h
	mainwindow.ui
	optionsdialog.ui
	qqsp.qrc
)

target_link_libraries(qqsp PRIVATE Qsp::qsp Qt5::Multimedia Qt5::Widgets)

find_package(Qt5 5.15 COMPONENTS WebEngine WebEngineWidgets)
if(Qt5_FOUND)
	target_sources(qqsp PRIVATE
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

	target_compile_definitions(qqsp PRIVATE _WEBBOX _WEBBOX_COMMON)
	target_link_libraries(qqsp PRIVATE Qt5::WebEngine Qt5::WebEngineWidgets)
endif()



feature_summary(FATAL_ON_MISSING_REQUIRED_PACKAGES WHAT ALL)
