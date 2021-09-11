cmake_minimum_required(VERSION 3.6.3)
project(XAPI3)

# set compile parameter
if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -fpermissive -rdynamic -std=c++11 -Wno-unused-parameter")
elseif (${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra -Wno-missing-field-initializers")
	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1 -D_UNICODE -DUNICODE)
endif()

# set output dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# for vim plugin - YCM
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# set use folder in vs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# thread
find_package(Threads)

# include and link directories
link_directories(
	${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

# add XAPI_CPP
file(GLOB_RECURSE XAPI_CPP_h ${CMAKE_CURRENT_LIST_DIR}/languages/CPP/XAPI_CPP/*.h)
file(GLOB_RECURSE XAPI_CPP_cpp ${CMAKE_CURRENT_LIST_DIR}/languages/CPP/XAPI_CPP/*.cpp)
list(FILTER XAPI_CPP_cpp EXCLUDE REGEX ".*dllmain.cpp")
if (WIN32)
	add_library(XAPI_CPP SHARED
		${XAPI_CPP_h}
		${XAPI_CPP_cpp}
		${CMAKE_CURRENT_LIST_DIR}/include/XApiC.h
		${CMAKE_CURRENT_LIST_DIR}/include/XApiCpp.h
		${CMAKE_CURRENT_LIST_DIR}/include/CrossPlatform.h
	)
	set_target_properties(XAPI_CPP
		PROPERTIES
		FOLDER "languages/CPP"
		OUTPUT_NAME XAPI_CPP_$(PlatformTarget)
	)
else()
	add_library(XAPI_CPP SHARED
		${XAPI_CPP_cpp}
	)
endif()
target_link_libraries(XAPI_CPP 
	${CMAKE_THREAD_LIBS_INIT}
	${CMAKE_DL_LIBS}
)
target_compile_definitions(XAPI_CPP
	PRIVATE BUILDING_DLL
	PUBLIC XAPI_CPP_EXPORTS
)

# get common files
set(common_h
	${CMAKE_CURRENT_LIST_DIR}/include/toolkit.h
	${CMAKE_CURRENT_LIST_DIR}/include/inirw.h
	${CMAKE_CURRENT_LIST_DIR}/include/ChinaStock.h
	${CMAKE_CURRENT_LIST_DIR}/include/IDGenerator.h
	${CMAKE_CURRENT_LIST_DIR}/include/ApiProcess.h
)
set(common_cpp
	${CMAKE_CURRENT_LIST_DIR}/include/inirw.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/ChinaStock.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/IDGenerator.cpp
)
set(common_quote_h
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager.h
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager1.h
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager2.h
	${CMAKE_CURRENT_LIST_DIR}/include/CSyntheticCalculate_DepthMarketDataNField.h
)
set(common_quote_cpp
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager1.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/CSubscribeManager2.cpp
)
file(GLOB synthetic_h ${CMAKE_CURRENT_LIST_DIR}/include/synthetic/*.h)

# functions
function(move_into_filter filter files)
	foreach(FILE ${files}) 
		# Get the directory of the source file
		get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
	
		# Remove common directory prefix to make the group
		string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
	
		# Make sure we are using windows slashes
		string(REPLACE "/" "\\" GROUP "${GROUP}")
	
		source_group("${filter}" FILES "${FILE}")
	endforeach()
endfunction(move_into_filter)

function(add_quote name folders)
	set(src_files "")
	set(h_files "")
	foreach(folder ${folders})
		file(GLOB tmp_h ${folder}/*.h)
		file(GLOB tmp_cpp ${folder}/*.cpp)
		list(APPEND h_files ${tmp_h})
		list(APPEND src_files ${tmp_cpp})
	endforeach(folder)
	
	list(FILTER src_files EXCLUDE REGEX ".*dllmain.cpp")
		
	if (WIN32)
		add_library(${name} SHARED
			${h_files}
			${src_files}
			${common_h}
			${common_cpp}
			${common_quote_h}
			${common_quote_cpp}
			${synthetic_h}
		)
		move_into_filter(common "${common_h}")
		move_into_filter(common "${common_cpp}")
		move_into_filter(common_quote "${common_quote_h}")
		move_into_filter(common_quote "${common_quote_cpp}")
		move_into_filter(synthetic "${synthetic_h}")
	else()
		add_library(${name} SHARED
			${src_files}
			${common_cpp}
			${common_quote_cpp}
		)
	endif()
	
	target_include_directories(${name} PRIVATE
		${folders}
	)
	target_compile_definitions(${name}
		PRIVATE USE_CMAKE BUILDING_DLL
	)
	if (WIN32)
		set_target_properties(${name}
			PROPERTIES
			FOLDER "apis"
			OUTPUT_NAME ${name}_$(PlatformTarget)
		)
	endif()
endfunction(add_quote)

function(add_trade name folders)
	set(src_files "")
	set(h_files "")
	foreach(folder ${folders})
		file(GLOB tmp_h ${folder}/*.h)
		file(GLOB tmp_cpp ${folder}/*.cpp)
		list(APPEND h_files ${tmp_h})
		list(APPEND src_files ${tmp_cpp})
	endforeach(folder)
	
	list(FILTER src_files EXCLUDE REGEX ".*dllmain.cpp")
	
	if (WIN32)
		add_library(${name} SHARED
			${h_files}
			${src_files}
			${common_h}
			${common_cpp}
			${synthetic_h}
		)
		move_into_filter(common "${common_h}")
		move_into_filter(common "${common_cpp}")
		move_into_filter(synthetic "${synthetic_h}")
	else()
		add_library(${name} SHARED
			${src_files}
			${common_cpp}
		)
	endif()
	
	target_include_directories(${name} PRIVATE
		${folders}
	)
	target_compile_definitions(${name}
		PRIVATE USE_CMAKE BUILDING_DLL
	)
	if (WIN32)
		set_target_properties(${name}
			PROPERTIES
			FOLDER "apis"
			OUTPUT_NAME ${name}_$(PlatformTarget)
		)
	endif()
endfunction(add_trade)

function(add_apis name cpp_files h_files)
	list(FILTER cpp_files EXCLUDE REGEX ".*dllmain.cpp")

	if (WIN32)
		add_library(${name} SHARED
			${h_files}
			${cpp_files}
			${common_h}
			${common_cpp}
			${common_quote_h}
			${common_quote_cpp}
			${synthetic_h}
		)
		move_into_filter(common "${common_h}")
		move_into_filter(common "${common_cpp}")
		move_into_filter(common_quote "${common_quote_h}")
		move_into_filter(common_quote "${common_quote_cpp}")
		move_into_filter(synthetic "${synthetic_h}")
	else()
		add_library(${name} SHARED
			${cpp_files}
			${common_cpp}
			${common_quote_cpp}
		)
	endif()
	
	target_compile_definitions(${name}
		PRIVATE USE_CMAKE BUILDING_DLL
	)
	if (WIN32)
		set_target_properties(${name}
			PROPERTIES
			FOLDER "apis"
			OUTPUT_NAME ${name}_$(PlatformTarget)
		)
	endif()
endfunction(add_apis)

# add CTP Quote
set(folder "")
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Quote)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP_SE_Quote)
add_quote(CTP_SE_Quote "${folder}")
if (WIN32)
	if (CMAKE_CL_64)
		ADD_CUSTOM_COMMAND(TARGET CTP_SE_Quote
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win64/thostmduserapi_se.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win64/thostmduserapi_se.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
		)
	else()
		ADD_CUSTOM_COMMAND(TARGET CTP_SE_Quote
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win32/thostmduserapi_se.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win32/thostmduserapi_se.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
		)
	endif()
else()
	ADD_CUSTOM_COMMAND(TARGET CTP_SE_Quote
		PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/linux64/libthostmduserapi_se.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
	)
endif()
target_link_libraries(CTP_SE_Quote
	thostmduserapi_se
)

# add CTP Trade
set(folder "")
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Trade)
list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP_SE_Trade)
add_trade(CTP_SE_Trade "${folder}")
if (WIN32)
	if (CMAKE_CL_64)
		ADD_CUSTOM_COMMAND(TARGET CTP_SE_Trade
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win64/thosttraderapi_se.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win64/thosttraderapi_se.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
		)
	else()
		ADD_CUSTOM_COMMAND(TARGET CTP_SE_Trade
			PRE_LINK
			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win32/thosttraderapi_se.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/win32/thosttraderapi_se.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
		)
	endif()
else()
	ADD_CUSTOM_COMMAND(TARGET CTP_SE_Trade
		PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTP_SE/linux64/libthosttraderapi_se.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
	)
endif()
target_link_libraries(CTP_SE_Trade
	thosttraderapi_se
)

## add XTP Quote
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/XTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/XTP/Quote)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/XTP_Quote)
#add_quote(XTP_Quote "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
#		ADD_CUSTOM_COMMAND(TARGET XTP_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win64/xtpquoteapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win64/xtpquoteapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET XTP_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win32/xtpquoteapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win32/xtpquoteapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#else()
#	ADD_CUSTOM_COMMAND(TARGET XTP_Quote
#		PRE_LINK
#		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/linux64/libxtpquoteapi.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
#	)
#endif()
#target_link_libraries(XTP_Quote
#	xtpquoteapi
#)
#target_compile_definitions(XTP_Quote
#	PRIVATE USE_CMAKE BUILDING_DLL ISLIB
#)

## add XTP Trade
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/XTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/XTP/Trade)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/XTP_Trade)
#add_trade(XTP_Trade "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
#		ADD_CUSTOM_COMMAND(TARGET XTP_Trade
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win64/xtptraderapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win64/xtptraderapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET XTP_Trade
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win32/xtptraderapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/win32/xtptraderapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#else()
#	ADD_CUSTOM_COMMAND(TARGET XTP_Trade
#		PRE_LINK
#		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/XTP/linux64/libxtptraderapi.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
#	)
#endif()
#target_link_libraries(XTP_Trade
#	xtptraderapi
#)

## add CTP Spot Trade
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Trade)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP_SOPT_SE_Trade)
#add_trade(CTP_SPOT_SE_Trade "${folder}")
#target_link_libraries(CTP_SPOT_SE_Trade
#	thosttraderapi_se
#)
#add_dependencies(CTP_SPOT_SE_Trade
#	CTP_SE_Trade
#)
#
## add CTPZQ Quote
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Quote)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTPZQ_Quote)
#add_quote(CTPZQ_Quote "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
#		ADD_CUSTOM_COMMAND(TARGET CTPZQ_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win64/zqthostmdapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win64/zqthostmdapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET CTPZQ_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win32/zqthostmdapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win32/zqthostmdapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(CTPZQ_Quote
#	zqthostmdapi
#)

## add CTPZQ Trade
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Trade)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTPZQ_Trade)
#add_trade(CTPZQ_Trade "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
#		ADD_CUSTOM_COMMAND(TARGET CTPZQ_Trade
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win64/zqthosttraderapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win64/zqthosttraderapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET CTPZQ_Trade
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win32/zqthosttraderapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/CTPZQ/win32/zqthosttraderapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(CTPZQ_Trade
#	zqthosttraderapi
#)


## add Kingstar Quote
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Quote)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/Kingstar_Quote)
#add_quote(Kingstar_Quote "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
##		ADD_CUSTOM_COMMAND(TARGET Kingstar_Quote
##			PRE_LINK
##			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win64/KSMarketDataAPI.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win64/KSMarketDataAPI.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
##		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET Kingstar_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win32/KSMarketDataAPI.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win32/KSMarketDataAPI.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(Kingstar_Quote
#	KSMarketDataAPI
#)

## add Kingstar Trade
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Trade)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/Kingstar_Trade)
#add_trade(Kingstar_Trade "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
##		ADD_CUSTOM_COMMAND(TARGET Kingstar_Trade
##			PRE_LINK
##			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win64/KSTradeAPI.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win64/KSTradeAPI.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
##		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET Kingstar_Trade
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win32/KSTradeAPI.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Kingstar/win32/KSTradeAPI.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(Kingstar_Trade
#	KSTradeAPI
#)

## add LTS Quote v2
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Quote_v2)
#add_quote(LTS_Quote_v2 "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
##		ADD_CUSTOM_COMMAND(TARGET LTS_Quote_v2
##			PRE_LINK
##			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win64/securitymduserapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win64/securitymduserapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
##		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET LTS_Quote_v2
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win32/securitymduserapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win32/securitymduserapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(LTS_Quote_v2
#	securitymduserapi
#)

## add LTS Query v2
#set(folder "")
## list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Query_v2)
#file(GLOB LTS_Query_cpp ${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Query_v2/*.cpp)
#file(GLOB LTS_Query_h ${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Query_v2/*.h)
#list(APPEND LTS_Query_cpp ${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Trade_v2/TypeConvert.cpp)
#list(APPEND LTS_Query_h ${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Trade_v2/TypeConvert.h)
#add_apis(LTS_Query_v2 "${LTS_Query_cpp}" "${LTS_Query_h}")
#if (WIN32)
#	if (CMAKE_CL_64)
##		ADD_CUSTOM_COMMAND(TARGET LTS_Query_v2
##			PRE_LINK
##			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win64/securityqueryapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win64/securityqueryapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
##		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET LTS_Query_v2
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win32/securityqueryapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win32/securityqueryapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(LTS_Query_v2
#	securityqueryapi
#)
#target_include_directories(LTS_Query_v2 PRIVATE
#	${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Query_v2
#)

## add LTS Trade v2
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/LTS_Trade_v2)
#add_trade(LTS_Trade_v2 "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
##		ADD_CUSTOM_COMMAND(TARGET LTS_Trade_v2
##			PRE_LINK
##			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win64/securitytraderapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win64/securitytraderapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
##		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET LTS_Trade_v2
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win32/securitytraderapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/LTS_v2/win32/securitytraderapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(LTS_Trade_v2
#	securitytraderapi
#)

## add Sgit Quote
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Quote)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/Sgit_Quote)
#add_quote(Sgit_Quote "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
##		ADD_CUSTOM_COMMAND(TARGET Sgit_Quote
##			PRE_LINK
##			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win64/sgitquotapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win64/sgitquotapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
##		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET Sgit_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win32/sgitquotapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win32/sgitquotapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(Sgit_Quote
#	sgitquotapi
#)

## add Sgit Trade
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/CTP/Trade)
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/Sgit_Trade)
#add_trade(Sgit_Trade "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
##		ADD_CUSTOM_COMMAND(TARGET Sgit_Trade
##			PRE_LINK
##			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win64/sgittradeapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
##			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win64/sgittradeapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
##		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET Sgit_Trade
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win32/sgittradeapi.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Sgit/win32/sgittradeapi.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(Sgit_Trade
#	sgittradeapi
#)

## add Wind Quote
#set(folder "")
#list(APPEND folder ${CMAKE_CURRENT_LIST_DIR}/apis/Wind_Quote)
#add_quote(Wind_Quote "${folder}")
#if (WIN32)
#	if (CMAKE_CL_64)
#		ADD_CUSTOM_COMMAND(TARGET Wind_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Wind/win64/WAPIWrapperCpp.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Wind/win64/WAPIWrapperCpp.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	else()
#		ADD_CUSTOM_COMMAND(TARGET Wind_Quote
#			PRE_LINK
#			COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Wind/win32/WAPIWrapperCpp.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)/
#			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Wind/win32/WAPIWrapperCpp.lib ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$(Configuration)/
#		)
#	endif()
#endif()
#target_link_libraries(Wind_Quote
#	WAPIWrapperCpp
#)

# REM_Trade
file(GLOB REM_TRADE_H ${CMAKE_CURRENT_LIST_DIR}/apis/REM_Trade/*.h)
file(GLOB REM_TRADE_CPP ${CMAKE_CURRENT_LIST_DIR}/apis/REM_Trade/*.cpp)
list(FILTER REM_TRADE_CPP EXCLUDE REGEX ".*dllmain.cpp")
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/REM/)
add_library(REM_Trade SHARED ${REM_TRADE_H} ${REM_TRADE_CPP})
ADD_CUSTOM_COMMAND(TARGET REM_Trade
	PRE_LINK
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/REM/linux64/libEESTraderApi.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)
target_link_libraries(REM_Trade
	EESTraderApi
)

# REM_Quote
file(GLOB REM_QUOTE_H ${CMAKE_CURRENT_LIST_DIR}/apis/REM_Quote/*.h)
file(GLOB REM_QUOTE_CPP ${CMAKE_CURRENT_LIST_DIR}/apis/REM_Quote/*.cpp)
list(FILTER REM_QUOTE_CPP EXCLUDE REGEX ".*dllmain.cpp")
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/REM/)
add_library(REM_Quote SHARED ${REM_QUOTE_H} ${REM_QUOTE_CPP})
ADD_CUSTOM_COMMAND(TARGET REM_Quote
	PRE_LINK
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/REM/linux64/libEESQuoteApi.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)
target_link_libraries(REM_Quote
	EESQuoteApi
)

# Tap_Trade
file(GLOB TAP_TRADE_H ${CMAKE_CURRENT_LIST_DIR}/apis/Tap_Trade/*.h)
file(GLOB TAP_TRADE_CPP ${CMAKE_CURRENT_LIST_DIR}/apis/Tap_Trade/*.cpp)
list(FILTER TAP_TRADE_CPP EXCLUDE REGEX ".*dllmain.cpp")
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/Tap/)
add_library(Tap_Trade SHARED ${TAP_TRADE_H} ${TAP_TRADE_CPP})
ADD_CUSTOM_COMMAND(TARGET Tap_Trade
	PRE_LINK
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Tap/linux64/libTapTradeAPI.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Tap/linux64/libTapDataCollectAPI.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)
target_link_libraries(Tap_Trade
	TapTradeAPI
)


# Tap_Quote
file(GLOB TAP_QUOTE_H ${CMAKE_CURRENT_LIST_DIR}/apis/Tap_Quote/*.h)
file(GLOB TAP_QUOTE_CPP ${CMAKE_CURRENT_LIST_DIR}/apis/Tap_Quote/*.cpp)
list(FILTER TAP_QUOTE_CPP EXCLUDE REGEX ".*dllmain.cpp")
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/Tap/)
add_library(Tap_Quote SHARED ${TAP_QUOTE_H} ${TAP_QUOTE_CPP})
ADD_CUSTOM_COMMAND(TARGET Tap_Quote
	PRE_LINK
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/include/Tap/linux64/libTapQuoteAPI.so ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)
target_link_libraries(Tap_Quote
	TapQuoteAPI
)

## add FakeAPI
#file(GLOB FAKE_API_H ${CMAKE_CURRENT_LIST_DIR}/tools/FakeAPI/*.h)
#file(GLOB FAKE_API_CPP ${CMAKE_CURRENT_LIST_DIR}/tools/FakeAPI/*.cpp)
#list(FILTER FAKE_API_CPP EXCLUDE REGEX ".*dllmain.cpp")
#if (WIN32)
#	add_library(FakeAPI SHARED
#		${FAKE_API_H}
#		${FAKE_API_CPP}
#		${CMAKE_CURRENT_LIST_DIR}/tools/FakeAPI/export.def
#	)
#else()
#	add_library(FakeAPI SHARED
#		${FAKE_API_CPP}
#	)
#endif()
#set_target_properties(FakeAPI
#	PROPERTIES
#	FOLDER "tools"
#)

## add Cpp Test
#file(GLOB CPP_TEST_H ${CMAKE_CURRENT_LIST_DIR}/tests/CPP/*.h)
#file(GLOB CPP_TEST_CPP ${CMAKE_CURRENT_LIST_DIR}/tests/CPP/*.cpp)
#if (WIN32)
#	add_executable(CPP_TEST
#		${CPP_TEST_H}
#		${CPP_TEST_CPP}
#	)
#	set_target_properties(CPP_TEST
#		PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(OutDir)"
#	)
#else()
#	add_executable(CPP_TEST
#		${CPP_TEST_CPP}
#	)
#endif()
#target_compile_definitions(CPP_TEST
#	PRIVATE USE_CMAKE
#)
#add_dependencies(CPP_TEST
#	XAPI_CPP
#	CTP_SE_Quote
#	CTP_SE_Trade
#)
#target_link_libraries(CPP_TEST
#	XAPI_CPP
#)
#set_target_properties(CPP_TEST
#	PROPERTIES
#	FOLDER "tests/CPP"
#)
 
# add cpp examples
add_executable(ExampleCTP ${CMAKE_CURRENT_LIST_DIR}/example/cpp/CTP/example_ctp.cpp)
target_link_libraries(ExampleCTP XAPI_CPP)
target_compile_definitions(ExampleCTP PRIVATE USE_CMAKE)
add_dependencies(ExampleCTP
	XAPI_CPP
	CTP_SE_Quote
	CTP_SE_Trade
)

#add_executable(ExampleXTP ${CMAKE_CURRENT_LIST_DIR}/example/cpp/XTP/example_xtp.cpp)
#target_link_libraries(ExampleXTP XAPI_CPP)
#target_compile_definitions(ExampleXTP PRIVATE USE_CMAKE)
#add_dependencies(ExampleXTP
#	XAPI_CPP
#	XTP_Quote
#	XTP_Trade
#)

set_target_properties(ExampleCTP # ExampleXTP
	PROPERTIES
	FOLDER "example/CPP"
)
