cmake_minimum_required(VERSION 3.5)
project (compiler)

aux_source_directory(src SOURCES)

set(SYNTAX_DEMO_SOURCES
	src/defs.cpp
	src/utils.cpp
	src/syntax.cpp
	src/syntax_demo.cpp
)

set(LEXICAL_DEMO_SOURCES
	src/defs.cpp
	src/utils.cpp
	src/lexical.cpp
	src/lexical_demo.cpp
)

set(SYNTAX_TEST_SOURCES
	src/defs.cpp
	src/utils.cpp
	src/syntax.cpp
	src/lexical.cpp
	src/syntax_test.cpp
)

set(LEXICAL_TEST_SOURCES
	src/defs.cpp
	src/utils.cpp
	src/syntax.cpp
	src/lexical.cpp
	src/lexical_test.cpp
)


include_directories(include)
add_executable(syntax_demo ${SYNTAX_DEMO_SOURCES})

include_directories(include)
add_executable(lexical_demo ${LEXICAL_DEMO_SOURCES})

include_directories(include)
add_executable(syntax_test ${SYNTAX_TEST_SOURCES})

include_directories(include)
add_executable(lexical_test ${LEXICAL_TEST_SOURCES})
