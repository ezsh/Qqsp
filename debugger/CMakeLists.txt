target_sources(Qqsp PRIVATE
	debugwindow.cpp
	debugwindow.h
	expressionsmodel.cpp
	expressionsmodel.h
	expressionsview.cpp
	expressionsview.h

	debugwindow.ui
)

target_include_directories(Qqsp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
