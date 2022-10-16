cmake_minimum_required(VERSION 3.10)

project(mutantgenerator LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

#set(ASAN -fsanitize=address -fno-omit-frame-pointer -static-libasan)

set(Parser "y.tab.cpp")
set(Lexer "lex.yy.cpp")

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/parser/${Parser}
    COMMAND yacc -y -d -o ${CMAKE_SOURCE_DIR}/parser/${Parser} ${CMAKE_SOURCE_DIR}/parser/promela.y
    DEPENDS ${CMAKE_SOURCE_DIR}/parser/promela.y
    COMMENT "running yacc"
)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/parser/${Lexer}
    COMMAND flex -o ${CMAKE_SOURCE_DIR}/parser/${Lexer} ${CMAKE_SOURCE_DIR}/parser/promela.l
    DEPENDS ${CMAKE_SOURCE_DIR}/parser/promela.l
    COMMENT "running flex"
)
add_compile_options(
	-Wall
	-Wextra
	-pedantic
	-ggdb
	-O0
	${ASAN}
)
add_link_options(
	${ASAN}
)

add_executable(deadalux
    main.cpp
	./symbol/symbol.cpp
	./symbol/symTable.cpp
	./symbol/typedef/procSymNode.cpp
	./symbol/typedef/tdefSymNode.cpp
	./symbol/typedef/mtypedefSymNode.cpp
	./symbol/typedef/inlineSymNode.cpp
	./symbol/vardef/bitSymNode.cpp
	./symbol/vardef/boolSymNode.cpp
	./symbol/vardef/byteSymNode.cpp
	./symbol/vardef/shortSymNode.cpp
	./symbol/vardef/intSymNode.cpp
	./symbol/vardef/unsgnSymNode.cpp
	./symbol/vardef/utypeSymNode.cpp
	./symbol/vardef/mtypeSymNode.cpp
	./symbol/vardef/cidSymNode.cpp
	./symbol/vardef/chanSymNode.cpp
	./symbol/vardef/varSymNode.cpp
	./symbol/vardef/naSymNode.cpp
	./symbol/vardef/pidSymNode.cpp
	./symbol/vardef/variantSymNode.cpp
	./symbol/logic/temporalSymNode.cpp

	./ast/astNode.cpp
	./ast/stmnt/stmnt.cpp
	./ast/stmnt/chanStmnt.cpp
	./ast/stmnt/flowStmnt.cpp
	./ast/stmnt/stdlibStmnt.cpp
	./ast/stmnt/clockStmnt.cpp
	./ast/stmnt/decl.cpp
	./ast/stmnt/logicDecl.cpp
	./ast/expr/expr.cpp
	./ast/expr/unaryExpr.cpp
	./ast/expr/binaryExpr.cpp
	./ast/expr/varExpr.cpp
	./ast/expr/constExpr.cpp
	./ast/expr/argExpr.cpp
	./ast/expr/temporalExpr.cpp
	
	./automata/ASTtoFSM.cpp
	./automata/fsm.cpp
	./automata/fsmNode.cpp
	./automata/fsmEdge.cpp
	
	./semantic/state.cpp
	./semantic/programState.cpp
	./semantic/compositeState.cpp
	./semantic/process.cpp
	
	./semantic/variable/variable.cpp
	./semantic/variable/primitiveVariable.cpp
	./semantic/variable/utypeVariable.cpp
	./semantic/variable/boolVariable.cpp
	./semantic/variable/mtypeVariable.cpp
	./semantic/variable/channel.cpp
	
	./semantic/payload.cpp
	./semantic/transition.cpp
	./semantic/programTransition.cpp
	./semantic/rendezVousTransition.cpp
	./semantic/compositeTransition.cpp

${CMAKE_SOURCE_DIR}/parser/${Parser}
${CMAKE_SOURCE_DIR}/parser/${Lexer}
    
	)

target_include_directories(deadalux
    PUBLIC
    ./automata
    
    ./symbol
    ./symbol/typedef
    ./symbol/vardef
    ./symbol/logic
    ./symbol/visitor
    
    ./ast
    ./ast/stmnt
    ./ast/expr
    ./ast/visitor
    
    ./semantic
    ./semantic/variable
    
    ./parser

    ./libs/cudd/cplusplus

    ${CMAKE_CURRENT_BINARY_DIR}
	)


target_link_directories(deadalux
	PUBLIC

	./libs/cudd/cplusplus/.libs
	./libs/cudd/cudd/.libs
	)

target_link_libraries(deadalux libobj.a libcudd.a)



