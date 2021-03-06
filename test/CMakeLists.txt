find_program(XMLLINT xmllint)
if (XMLLINT)
	file(GLOB SCXML_FILES samples/uscxml/*.scxml)
	
endif()

add_executable(test-predicates src/test-predicates.cpp)
target_link_libraries(test-predicates uscxml)
add_test(test-predicates ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-predicates ${CMAKE_SOURCE_DIR}/test/samples/uscxml/test-predicates.scxml)
set_target_properties(test-predicates PROPERTIES FOLDER "Tests")

add_executable(test-execution src/test-execution.cpp)
target_link_libraries(test-execution uscxml)
add_test(test-execution ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-execution ${CMAKE_SOURCE_DIR}/test/samples/uscxml/test-execution.scxml)
set_target_properties(test-execution PROPERTIES FOLDER "Tests")

# add_executable(test-apache-commons src/test-apache-commons.cpp)
# target_link_libraries(test-apache-commons uscxml)
# add_test(test-apache-commons ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-apache-commons ${CMAKE_SOURCE_DIR}/test/samples/apache)
# set_target_properties(test-apache-commons PROPERTIES FOLDER "Tests")

# if (V8_FOUND)
# 	add_executable(test-ecmascript-v8 src/test-ecmascript-v8.cpp)
# 	target_link_libraries(test-ecmascript-v8 uscxml)
# 	add_test(test-ecmascript-v8 ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-ecmascript-v8 ${CMAKE_SOURCE_DIR}/test/samples/uscxml/test-ecmascript.scxml)
# 	set_target_properties(test-ecmascript-v8 PROPERTIES FOLDER "Tests")
# endif()

if (SWI_FOUND)
	add_executable(test-prolog-swi src/test-prolog-swi.cpp)
	target_link_libraries(test-prolog-swi uscxml)
	add_test(test-prolog-swi ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-prolog-swi ${CMAKE_SOURCE_DIR}/test/samples/uscxml/test-prolog.scxml)
	set_target_properties(test-prolog-swi PROPERTIES FOLDER "Tests")
endif()

if (OPENSCENEGRAPH_FOUND)
	add_executable(test-osg src/test-osg.cpp)
	target_link_libraries(test-osg uscxml)
	set_target_properties(test-osg PROPERTIES FOLDER "Tests")
endif()

add_executable(test-communication src/test-communication.cpp)
target_link_libraries(test-communication uscxml)
add_test(test-communication ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-communication ${CMAKE_SOURCE_DIR}/test/samples/uscxml/test-communication.scxml)
set_target_properties(test-communication PROPERTIES FOLDER "Tests")

add_executable(test-eventdelay src/test-eventdelay.cpp)
target_link_libraries(test-eventdelay uscxml)
add_test(test-eventdelay ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-eventdelay)
set_target_properties(test-eventdelay PROPERTIES FOLDER "Tests")

add_executable(test-completion src/test-completion.cpp)
target_link_libraries(test-completion uscxml)
add_test(test-completion ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-completion ${CMAKE_SOURCE_DIR}/test/samples/uscxml/test-donedata.scxml)
set_target_properties(test-completion PROPERTIES FOLDER "Tests")

if (NOT WIN32)
	add_executable(test-arabica-events src/test-arabica-events.cpp)
	target_link_libraries(test-arabica-events uscxml)
	add_test(test-arabica-events ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-arabica-events ${CMAKE_SOURCE_DIR}/test/samples/uscxml/arabica/test-arabica-events.xml)
	set_target_properties(test-arabica-events PROPERTIES FOLDER "Tests")
endif()

add_executable(test-url src/test-url.cpp)
target_link_libraries(test-url uscxml)
add_test(test-url ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test-url)
set_target_properties(test-url PROPERTIES FOLDER "Tests")

add_executable(scxml-test-framework-client 
	${PROJECT_SOURCE_DIR}/contrib/src/jsmn/jsmn.c
	src/scxml-test-framework-client.cpp)
target_link_libraries(scxml-test-framework-client uscxml)
set_target_properties(scxml-test-framework-client PROPERTIES FOLDER "Tests")

