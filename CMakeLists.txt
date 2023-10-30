cmake_minimum_required(VERSION 3.15)

project(daedalux LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

#set(ASAN -fsanitize=address -fno-omit-frame-pointer -static-libasan)
# enable_testing()

# add_subdirectory(tests)

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
	-Wno-unused-parameter
	-pedantic
	#-O2
	-ggdb
	-g
	
	${ASAN}
)
add_link_options(
	${ASAN}
)

add_executable(daedalux
    # main.cpp
	main_cli.cpp

	./cli/CLI11.hpp
	./cli/modelchecking_subcommand.cpp
	./cli/mutant_subcommand.cpp
	./cli/promela_loader.cpp

	./symbol/symbol.cpp
	./symbol/symTable.cpp
	./symbol/typedef/ptypeSymNode.cpp
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
	./symbol/vardef/sysSymNode.cpp
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
	
	./automata/astToFsm.cpp
	./automata/fsm.cpp
	./automata/fsmNode.cpp
	./automata/fsmEdge.cpp
	
	./semantic/state.cpp
	./semantic/stateDecorator.cpp
	./semantic/programState.cpp
	./semantic/featuredStateDecorator.cpp
	# ./semantic/timedStateDecorator.cpp

	./semantic/compositeState.cpp
	./semantic/thread.cpp
	./semantic/process.cpp
	./semantic/never.cpp
	
	./semantic/variable/variable.cpp
	./semantic/variable/variableDecorator.cpp
	./semantic/variable/primitiveVariable.cpp
	./semantic/variable/utypeVariable.cpp
	./semantic/variable/boolVariable.cpp
	./semantic/variable/mtypeVariable.cpp
	./semantic/variable/channel.cpp
	
	./semantic/payload.cpp
	./semantic/transition.cpp
	./semantic/programTransition.cpp
	./semantic/processTransition.cpp
	./semantic/compositeTransition.cpp
	./semantic/featuredProgramTransition.cpp
	# ./semantic/timedProgramTransition.cpp
	./semantic/neverTransition.cpp
	./semantic/deleteTransVisitor.cpp
	./semantic/initState.cpp

	./parser/${Parser}
	./parser/${Lexer}
	
	./features/tvl.cpp
	./features/expToADD.cpp
	./features/ADDutils.cpp
	
	./algorithms/explore.cpp
	./algorithms/elementStack.cpp
	./algorithms/reachabilityRelation.cpp

	
	./visualizers/stateToGraphViz.cpp
	./visualizers/trace.cpp
	./visualizers/traceReport.cpp
	
	./logic/ltl.cpp
    
	)

target_include_directories(daedalux
    PUBLIC
    ./automata

	./cli
    
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
    
    ./features
    
    ./algorithms
    
    ./visualizers
    
    ./libs/cudd
    ./libs/cudd/st
    ./libs/cudd/mtr
    ./libs/cudd/epd
    ./libs/cudd/cudd
    ./libs/cudd/cplusplus
    
    ./logic

    ${CMAKE_CURRENT_BINARY_DIR}
	)


target_link_directories(daedalux
	PUBLIC

	./libs/cudd/cplusplus/.libs
	./libs/cudd/cudd/.libs
	)

target_link_libraries(daedalux libobj.a libcudd.a)



