/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_Y_TAB_HPP_INCLUDED
# define YY_YY_Y_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    CONST = 258,                   /* CONST  */
    IF = 259,                      /* IF  */
    DO = 260,                      /* DO  */
    AC = 261,                      /* AC  */
    TYPE = 262,                    /* TYPE  */
    NAME = 263,                    /* NAME  */
    UNAME = 264,                   /* UNAME  */
    PNAME = 265,                   /* PNAME  */
    INAME = 266,                   /* INAME  */
    VNAME = 267,                   /* VNAME  */
    BASE = 268,                    /* BASE  */
    STRING = 269,                  /* STRING  */
    REAL = 270,                    /* REAL  */
    TRUE = 271,                    /* TRUE  */
    FALSE = 272,                   /* FALSE  */
    SKIP = 273,                    /* SKIP  */
    ASSERT = 274,                  /* ASSERT  */
    PRINT = 275,                   /* PRINT  */
    PRINTM = 276,                  /* PRINTM  */
    C_CODE = 277,                  /* C_CODE  */
    C_DECL = 278,                  /* C_DECL  */
    C_EXPR = 279,                  /* C_EXPR  */
    C_STATE = 280,                 /* C_STATE  */
    C_TRACK = 281,                 /* C_TRACK  */
    RUN = 282,                     /* RUN  */
    LEN = 283,                     /* LEN  */
    ENABLED = 284,                 /* ENABLED  */
    EVAL = 285,                    /* EVAL  */
    PC_VAL = 286,                  /* PC_VAL  */
    TYPEDEF = 287,                 /* TYPEDEF  */
    MTYPE = 288,                   /* MTYPE  */
    INLINE = 289,                  /* INLINE  */
    LABEL = 290,                   /* LABEL  */
    OF = 291,                      /* OF  */
    GOTO = 292,                    /* GOTO  */
    BREAK = 293,                   /* BREAK  */
    ELSE = 294,                    /* ELSE  */
    SEMI = 295,                    /* SEMI  */
    FI = 296,                      /* FI  */
    OD = 297,                      /* OD  */
    CA = 298,                      /* CA  */
    SEP = 299,                     /* SEP  */
    ATOMIC = 300,                  /* ATOMIC  */
    NON_ATOMIC = 301,              /* NON_ATOMIC  */
    D_STEP = 302,                  /* D_STEP  */
    UNLESS = 303,                  /* UNLESS  */
    TIMEOUT = 304,                 /* TIMEOUT  */
    NONPROGRESS = 305,             /* NONPROGRESS  */
    ACTIVE = 306,                  /* ACTIVE  */
    PROCTYPE = 307,                /* PROCTYPE  */
    D_PROCTYPE = 308,              /* D_PROCTYPE  */
    HIDDEN = 309,                  /* HIDDEN  */
    SHOW = 310,                    /* SHOW  */
    ISLOCAL = 311,                 /* ISLOCAL  */
    PRIORITY = 312,                /* PRIORITY  */
    PROVIDED = 313,                /* PROVIDED  */
    FULL = 314,                    /* FULL  */
    EMPTY = 315,                   /* EMPTY  */
    NFULL = 316,                   /* NFULL  */
    NEMPTY = 317,                  /* NEMPTY  */
    XU = 318,                      /* XU  */
    CLAIM = 319,                   /* CLAIM  */
    TRACE = 320,                   /* TRACE  */
    INIT = 321,                    /* INIT  */
    WHILE = 322,                   /* WHILE  */
    WHEN = 323,                    /* WHEN  */
    WAIT = 324,                    /* WAIT  */
    RESET = 325,                   /* RESET  */
    SPEC = 326,                    /* SPEC  */
    EVENTUALLY = 327,              /* EVENTUALLY  */
    ALWAYS = 328,                  /* ALWAYS  */
    GLOBALLY = 329,                /* GLOBALLY  */
    FINALLY = 330,                 /* FINALLY  */
    UNTIL = 331,                   /* UNTIL  */
    NEXT = 332,                    /* NEXT  */
    LTL = 333,                     /* LTL  */
    BLTL = 334,                    /* BLTL  */
    K = 335,                       /* K  */
    FMULTILTL = 336,               /* FMULTILTL  */
    ASGN = 337,                    /* ASGN  */
    SND = 338,                     /* SND  */
    O_SND = 339,                   /* O_SND  */
    RCV = 340,                     /* RCV  */
    R_RCV = 341,                   /* R_RCV  */
    OR = 342,                      /* OR  */
    AND = 343,                     /* AND  */
    EQ = 344,                      /* EQ  */
    NE = 345,                      /* NE  */
    GT = 346,                      /* GT  */
    LT = 347,                      /* LT  */
    GE = 348,                      /* GE  */
    LE = 349,                      /* LE  */
    LSHIFT = 350,                  /* LSHIFT  */
    RSHIFT = 351,                  /* RSHIFT  */
    INCR = 352,                    /* INCR  */
    DECR = 353,                    /* DECR  */
    UMIN = 354,                    /* UMIN  */
    NEG = 355,                     /* NEG  */
    COUNT = 356,                   /* COUNT  */
    CONTEXT = 357,                 /* CONTEXT  */
    DOT = 358,                     /* DOT  */
    IMPLIES = 359                  /* IMPLIES  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define CONST 258
#define IF 259
#define DO 260
#define AC 261
#define TYPE 262
#define NAME 263
#define UNAME 264
#define PNAME 265
#define INAME 266
#define VNAME 267
#define BASE 268
#define STRING 269
#define REAL 270
#define TRUE 271
#define FALSE 272
#define SKIP 273
#define ASSERT 274
#define PRINT 275
#define PRINTM 276
#define C_CODE 277
#define C_DECL 278
#define C_EXPR 279
#define C_STATE 280
#define C_TRACK 281
#define RUN 282
#define LEN 283
#define ENABLED 284
#define EVAL 285
#define PC_VAL 286
#define TYPEDEF 287
#define MTYPE 288
#define INLINE 289
#define LABEL 290
#define OF 291
#define GOTO 292
#define BREAK 293
#define ELSE 294
#define SEMI 295
#define FI 296
#define OD 297
#define CA 298
#define SEP 299
#define ATOMIC 300
#define NON_ATOMIC 301
#define D_STEP 302
#define UNLESS 303
#define TIMEOUT 304
#define NONPROGRESS 305
#define ACTIVE 306
#define PROCTYPE 307
#define D_PROCTYPE 308
#define HIDDEN 309
#define SHOW 310
#define ISLOCAL 311
#define PRIORITY 312
#define PROVIDED 313
#define FULL 314
#define EMPTY 315
#define NFULL 316
#define NEMPTY 317
#define XU 318
#define CLAIM 319
#define TRACE 320
#define INIT 321
#define WHILE 322
#define WHEN 323
#define WAIT 324
#define RESET 325
#define SPEC 326
#define EVENTUALLY 327
#define ALWAYS 328
#define GLOBALLY 329
#define FINALLY 330
#define UNTIL 331
#define NEXT 332
#define LTL 333
#define BLTL 334
#define K 335
#define FMULTILTL 336
#define ASGN 337
#define SND 338
#define O_SND 339
#define RCV 340
#define R_RCV 341
#define OR 342
#define AND 343
#define EQ 344
#define NE 345
#define GT 346
#define LT 347
#define GE 348
#define LE 349
#define LSHIFT 350
#define RSHIFT 351
#define INCR 352
#define DECR 353
#define UMIN 354
#define NEG 355
#define COUNT 356
#define CONTEXT 357
#define DOT 358
#define IMPLIES 359

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 66 "promela.y"
 
	int       				iVal;
	char*    				sVal;
	double					rVal;
	class dataTuple			pDataVal;
	
	class stmnt*			pStmntVal;
	class stmntOpt*			pStmntOptVal;
	class expr*				pExprVal;
	class exprConst*		pConstExprVal;
	class exprVarRef*		pExprVarRefVal;
	class exprVarRefName*	pExprVarRefNameVal;
	class exprArgList*		pExprArgListVal;
	class exprRArgList*		pExprRArgListVal;
	class exprRArg*			pExprRArgVal;
	class variantQuantifier*pVarQuantVal;
	
	class symbol*			pSymTabVal;
	class varSymNode*		pVarSymVal;
	class tdefSymNode*		pTdefSymVal;
	class mtypedefSymNode*	pTypedefSymVal;
	
	enum symbol::Type   	iType;

#line 300 "y.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (symTable** globalSymTab, stmnt** program);


#endif /* !YY_YY_Y_TAB_HPP_INCLUDED  */
