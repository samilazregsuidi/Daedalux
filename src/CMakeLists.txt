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


# Create a library from the source files
add_library(daedalux_lib STATIC
	core/ast/astNode.cpp
	core/ast/stmnt/stmnt.cpp
	core/ast/expr/expr.cpp

	core/symbol/symbol.cpp
	core/symbol/symTable.cpp

	core/automata/astToFsm.cpp
	core/automata/fsm.cpp
	core/automata/fsmNode.cpp
	core/automata/fsmEdge.cpp

	core/semantic/payload.cpp
	core/semantic/variable/variable.cpp
	core/semantic/variable/variableDecorator.cpp
	core/semantic/variable/state/state.cpp
	core/semantic/variable/state/stateDecorator.cpp
	core/semantic/variable/state/compositeState.cpp
	core/semantic/variable/state/thread.cpp
	core/semantic/variable/transition/transition.cpp
	core/semantic/variable/transition/compositeTransition.cpp
	core/semantic/variable/transition/rendezVousTransition.cpp
    	core/semantic/variable/transition/deleteTransVisitor.cpp
	core/semantic/variable/state/initState.cpp

	core/logic/ltl.cpp

	main_cli.cpp

	cli/CLI11.hpp
	cli/modelchecking_subcommand.cpp
	cli/mutant_subcommand.cpp

	promela/parser/promela_loader.cpp

	promela/symbol/typedef/ptypeSymNode.cpp
	promela/symbol/typedef/tdefSymNode.cpp
	promela/symbol/typedef/mtypedefSymNode.cpp
	promela/symbol/typedef/inlineSymNode.cpp
	promela/symbol/vardef/bitSymNode.cpp
	promela/symbol/vardef/boolSymNode.cpp
	promela/symbol/vardef/byteSymNode.cpp
	promela/symbol/vardef/shortSymNode.cpp
	promela/symbol/vardef/intSymNode.cpp
	promela/symbol/vardef/unsgnSymNode.cpp
	promela/symbol/vardef/utypeSymNode.cpp
	promela/symbol/vardef/mtypeSymNode.cpp
	promela/symbol/vardef/cidSymNode.cpp
	promela/symbol/vardef/chanSymNode.cpp
	promela/symbol/vardef/varSymNode.cpp
	promela/symbol/vardef/naSymNode.cpp
	promela/symbol/vardef/pidSymNode.cpp
	promela/symbol/vardef/sysSymNode.cpp
	promela/symbol/vardef/variantSymNode.cpp
	promela/symbol/logic/temporalSymNode.cpp

	promela/ast/stmnt/chanStmnt.cpp
	promela/ast/stmnt/flowStmnt.cpp
	promela/ast/stmnt/stdlibStmnt.cpp
	promela/ast/stmnt/clockStmnt.cpp
	promela/ast/stmnt/decl.cpp
	promela/ast/stmnt/logicDecl.cpp

	promela/ast/expr/unaryExpr.cpp
	promela/ast/expr/binaryExpr.cpp
	promela/ast/expr/varExpr.cpp
	promela/ast/expr/constExpr.cpp
	promela/ast/expr/argExpr.cpp
	promela/ast/expr/temporalExpr.cpp	
	
	promela/semantic/variable/primitiveVariable.cpp
	promela/semantic/variable/utypeVariable.cpp
	promela/semantic/variable/boolVariable.cpp
	promela/semantic/variable/mtypeVariable.cpp
	promela/semantic/variable/channel.cpp
	promela/semantic/variable/state/process.cpp
	promela/semantic/variable/state/never.cpp
	promela/semantic/variable/state/programState.cpp	
	promela/semantic/variable/transition/neverTransition.cpp	
	promela/semantic/variable/transition/processTransition.cpp
	promela/semantic/variable/transition/progTransition.cpp

	promela/parser/${Parser}
	promela/parser/${Lexer}

	feature/tvl.cpp
	feature/expToADD.cpp
	feature/ADDutils.cpp
	feature/semantic/variable/state/featuredStateDecorator.cpp
	feature/semantic/variable/transition/featuredTransition.cpp

	algorithm/explore.cpp
	algorithm/elementStack.cpp
	algorithm/reachabilityRelation.cpp

	visualizer/stateToGraphViz.cpp
	visualizer/trace.cpp
	visualizer/traceReport.cpp
)

target_include_directories(daedalux_lib
PUBLIC
    
    cli
    
    core/symbol
    core/symbol/typedef
    core/symbol/vardef
    core/symbol/logic
    core/symbol/visitor
    
    core/ast
    core/ast/stmnt
    core/ast/expr
    core/ast/visitor
    
    core/logic
    core/logic/specification
    
    core/automata
    
    core/semantic
    core/semantic/variable
    core/semantic/variable/state
    core/semantic/variable/transition
    
    promela
    promela/parser
    
    promela/symbol
    promela/symbol/typedef
    promela/symbol/vardef
    promela/symbol/logic
    
    promela/ast
    promela/ast/stmnt
    promela/ast/expr
    
    promela/semantic
    promela/semantic/variable
    promela/semantic/variable/state
    promela/semantic/variable/transition
    
    feature
    feature/semantic/
    feature/semantic/variable
    feature/semantic/variable/state
    feature/semantic/variable/transition
    
    algorithm
    
    visualizer
    
    libs/cudd
    libs/cudd/st
    libs/cudd/mtr
    libs/cudd/epd
    libs/cudd/cudd
    libs/cudd/cplusplus

    ${CMAKE_CURRENT_BINARY_DIR}
)


target_link_directories(daedalux_lib
	PUBLIC
	libs/cudd/cplusplus/.libs
	libs/cudd/cudd/.libs
	)

target_link_libraries(daedalux_lib libobj.a libcudd.a)


add_executable(daedalux
    # main.cpp
    
    	core/ast/astNode.cpp
	core/ast/stmnt/stmnt.cpp
	core/ast/expr/expr.cpp
	
	core/symbol/symbol.cpp
	core/symbol/symTable.cpp
	
	core/automata/astToFsm.cpp
	core/automata/fsm.cpp
	core/automata/fsmNode.cpp
	core/automata/fsmEdge.cpp
	
	core/semantic/payload.cpp
	core/semantic/variable/variable.cpp
	core/semantic/variable/variableDecorator.cpp
	core/semantic/variable/state/state.cpp
	core/semantic/variable/state/stateDecorator.cpp
	core/semantic/variable/state/compositeState.cpp
	core/semantic/variable/state/thread.cpp
	core/semantic/variable/transition/transition.cpp
	core/semantic/variable/transition/compositeTransition.cpp
	core/semantic/variable/transition/rendezVousTransition.cpp
    	core/semantic/variable/transition/deleteTransVisitor.cpp
	core/semantic/variable/state/initState.cpp
	
	core/logic/ltl.cpp
    
	main_cli.cpp

	cli/CLI11.hpp
	cli/modelchecking_subcommand.cpp
	cli/mutant_subcommand.cpp
	
	promela/parser/promela_loader.cpp
	
	promela/symbol/typedef/ptypeSymNode.cpp
	promela/symbol/typedef/tdefSymNode.cpp
	promela/symbol/typedef/mtypedefSymNode.cpp
	promela/symbol/typedef/inlineSymNode.cpp
	promela/symbol/vardef/bitSymNode.cpp
	promela/symbol/vardef/boolSymNode.cpp
	promela/symbol/vardef/byteSymNode.cpp
	promela/symbol/vardef/shortSymNode.cpp
	promela/symbol/vardef/intSymNode.cpp
	promela/symbol/vardef/unsgnSymNode.cpp
	promela/symbol/vardef/utypeSymNode.cpp
	promela/symbol/vardef/mtypeSymNode.cpp
	promela/symbol/vardef/cidSymNode.cpp
	promela/symbol/vardef/chanSymNode.cpp
	promela/symbol/vardef/varSymNode.cpp
	promela/symbol/vardef/naSymNode.cpp
	promela/symbol/vardef/pidSymNode.cpp
	promela/symbol/vardef/sysSymNode.cpp
	promela/symbol/vardef/variantSymNode.cpp
	promela/symbol/logic/temporalSymNode.cpp

	promela/ast/stmnt/chanStmnt.cpp
	promela/ast/stmnt/flowStmnt.cpp
	promela/ast/stmnt/stdlibStmnt.cpp
	promela/ast/stmnt/clockStmnt.cpp
	promela/ast/stmnt/decl.cpp
	promela/ast/stmnt/logicDecl.cpp
	
	promela/ast/expr/unaryExpr.cpp
	promela/ast/expr/binaryExpr.cpp
	promela/ast/expr/varExpr.cpp
	promela/ast/expr/constExpr.cpp
	promela/ast/expr/argExpr.cpp
	promela/ast/expr/temporalExpr.cpp	
	
	promela/semantic/variable/primitiveVariable.cpp
	promela/semantic/variable/utypeVariable.cpp
	promela/semantic/variable/boolVariable.cpp
	promela/semantic/variable/mtypeVariable.cpp
	promela/semantic/variable/channel.cpp
	promela/semantic/variable/state/process.cpp
	promela/semantic/variable/state/never.cpp
	promela/semantic/variable/state/programState.cpp	
	promela/semantic/variable/transition/neverTransition.cpp	
	promela/semantic/variable/transition/processTransition.cpp
	promela/semantic/variable/transition/progTransition.cpp

	promela/parser/${Parser}
	promela/parser/${Lexer}
	
	feature/tvl.cpp
	feature/expToADD.cpp
	feature/ADDutils.cpp
	feature/semantic/variable/state/featuredStateDecorator.cpp
	feature/semantic/variable/transition/featuredTransition.cpp
	
	algorithm/explore.cpp
	algorithm/elementStack.cpp
	algorithm/reachabilityRelation.cpp

	visualizer/stateToGraphViz.cpp
	visualizer/trace.cpp
	visualizer/traceReport.cpp
    
	)

target_include_directories(daedalux
    PUBLIC
    
    cli
    
    core/symbol
    core/symbol/typedef
    core/symbol/vardef
    core/symbol/logic
    core/symbol/visitor
    
    core/ast
    core/ast/stmnt
    core/ast/expr
    core/ast/visitor
    
    core/logic
    core/logic/specification
    
    core/automata
    
    core/semantic
    core/semantic/variable
    core/semantic/variable/state
    core/semantic/variable/transition
    
    promela
    promela/parser
    
    promela/symbol
    promela/symbol/typedef
    promela/symbol/vardef
    promela/symbol/logic
    
    promela/ast
    promela/ast/stmnt
    promela/ast/expr
    
    promela/semantic
    promela/semantic/variable
    promela/semantic/variable/state
    promela/semantic/variable/transition
    
    feature
    feature/semantic/
    feature/semantic/variable
    feature/semantic/variable/state
    feature/semantic/variable/transition
    
    algorithm
    
    visualizer
    
    libs/cudd
    libs/cudd/st
    libs/cudd/mtr
    libs/cudd/epd
    libs/cudd/cudd
    libs/cudd/cplusplus

    ${CMAKE_CURRENT_BINARY_DIR}
)


target_link_directories(daedalux
	PUBLIC
	libs/cudd/cplusplus/.libs
	libs/cudd/cudd/.libs
	)

target_link_libraries(daedalux libobj.a libcudd.a)
