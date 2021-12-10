cmake_minimum_required(VERSION 3.5)
project (compiler)

aux_source_directory(src SOURCES)

set(SYNTAX_SOURCES
	src/defs.cpp
	src/utils.cpp
	src/syntax.cpp
	src/syntaxtest.cpp
)

# set(GRAMMAR_SOURCES
# 	src/defs.cpp
# 	src/grammar.cpp
# 	src/lexical.cpp
# 	src/lexicaltest.cpp
# )

include_directories(include)
add_executable(syntax ${SYNTAX_SOURCES})

# include_directories(include)
# add_executable(lexical ${GRAMMAR_SOURCES})

# target_include_directories(main
#     PRIVATE
#         ${PROJECT_SOURCE_DIR}/include
# )
