/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "promela.y"


// This is based on the original Yacc grammar of SPIN (spin.y):

/* Copyright (c) 1989-2003 by Lucent Technologies, Bell Laboratories.     */
/* All Rights Reserved.  This software is for educational purposes only.  */
/* No guarantee whatsoever is expressed or implied by the distribution of */
/* this code.  Permission is given to distribute this code provided that  */
/* this introductory message is not removed and no monies are exchanged.  */
/* Software written by Gerard J. Holzmann.  For tool documentation see:   */
/*             http://spinroot.com/                                       */
/* Send all bug-reports and/or questions to: bugs@spinroot.com            */

#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

#include "symbols.hpp"
#include "ast.hpp"

#include "y.tab.hpp"

#define YYDEBUG 1

#ifdef CPP
extern "C" 
#endif

int yylex(YYSTYPE * yylval_param, symTable** globalSymTab);

extern int nbrLines;

int yyerror (symTable** globalSymTab, stmnt** program, const char* msg){
	fprintf(stderr, "Syntax error on line %d: '%s'.\n", nbrLines, msg);
	exit(1);
}

std::string nameSpace = "global";
symbol::Type declType = symbol::T_NA;
tdefSymNode* typeDef = nullptr;
mtypedefSymNode* mtypeDef = nullptr;

symTable* currentSymTab = nullptr;
symTable* savedSymTab = nullptr;

std::list<varSymNode*> declSyms;
std::list<varSymNode*> typeLst;
std::list<std::string> params;
std::list<variantQuantifier*> variants;

std::map<std::string, stmntLabel*> labelsMap;

int mtypeId = 1;
bool inInline = false;


#line 129 "y.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "y.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_CONST = 3,                      /* CONST  */
  YYSYMBOL_IF = 4,                         /* IF  */
  YYSYMBOL_DO = 5,                         /* DO  */
  YYSYMBOL_AC = 6,                         /* AC  */
  YYSYMBOL_TYPE = 7,                       /* TYPE  */
  YYSYMBOL_NAME = 8,                       /* NAME  */
  YYSYMBOL_UNAME = 9,                      /* UNAME  */
  YYSYMBOL_PNAME = 10,                     /* PNAME  */
  YYSYMBOL_INAME = 11,                     /* INAME  */
  YYSYMBOL_VNAME = 12,                     /* VNAME  */
  YYSYMBOL_BASE = 13,                      /* BASE  */
  YYSYMBOL_STRING = 14,                    /* STRING  */
  YYSYMBOL_REAL = 15,                      /* REAL  */
  YYSYMBOL_TRUE = 16,                      /* TRUE  */
  YYSYMBOL_FALSE = 17,                     /* FALSE  */
  YYSYMBOL_SKIP = 18,                      /* SKIP  */
  YYSYMBOL_ASSERT = 19,                    /* ASSERT  */
  YYSYMBOL_PRINT = 20,                     /* PRINT  */
  YYSYMBOL_PRINTM = 21,                    /* PRINTM  */
  YYSYMBOL_C_CODE = 22,                    /* C_CODE  */
  YYSYMBOL_C_DECL = 23,                    /* C_DECL  */
  YYSYMBOL_C_EXPR = 24,                    /* C_EXPR  */
  YYSYMBOL_C_STATE = 25,                   /* C_STATE  */
  YYSYMBOL_C_TRACK = 26,                   /* C_TRACK  */
  YYSYMBOL_RUN = 27,                       /* RUN  */
  YYSYMBOL_LEN = 28,                       /* LEN  */
  YYSYMBOL_ENABLED = 29,                   /* ENABLED  */
  YYSYMBOL_EVAL = 30,                      /* EVAL  */
  YYSYMBOL_PC_VAL = 31,                    /* PC_VAL  */
  YYSYMBOL_TYPEDEF = 32,                   /* TYPEDEF  */
  YYSYMBOL_MTYPE = 33,                     /* MTYPE  */
  YYSYMBOL_INLINE = 34,                    /* INLINE  */
  YYSYMBOL_LABEL = 35,                     /* LABEL  */
  YYSYMBOL_OF = 36,                        /* OF  */
  YYSYMBOL_GOTO = 37,                      /* GOTO  */
  YYSYMBOL_BREAK = 38,                     /* BREAK  */
  YYSYMBOL_ELSE = 39,                      /* ELSE  */
  YYSYMBOL_SEMI = 40,                      /* SEMI  */
  YYSYMBOL_FI = 41,                        /* FI  */
  YYSYMBOL_OD = 42,                        /* OD  */
  YYSYMBOL_CA = 43,                        /* CA  */
  YYSYMBOL_SEP = 44,                       /* SEP  */
  YYSYMBOL_ATOMIC = 45,                    /* ATOMIC  */
  YYSYMBOL_NON_ATOMIC = 46,                /* NON_ATOMIC  */
  YYSYMBOL_D_STEP = 47,                    /* D_STEP  */
  YYSYMBOL_UNLESS = 48,                    /* UNLESS  */
  YYSYMBOL_TIMEOUT = 49,                   /* TIMEOUT  */
  YYSYMBOL_NONPROGRESS = 50,               /* NONPROGRESS  */
  YYSYMBOL_ACTIVE = 51,                    /* ACTIVE  */
  YYSYMBOL_PROCTYPE = 52,                  /* PROCTYPE  */
  YYSYMBOL_D_PROCTYPE = 53,                /* D_PROCTYPE  */
  YYSYMBOL_HIDDEN = 54,                    /* HIDDEN  */
  YYSYMBOL_SHOW = 55,                      /* SHOW  */
  YYSYMBOL_ISLOCAL = 56,                   /* ISLOCAL  */
  YYSYMBOL_PRIORITY = 57,                  /* PRIORITY  */
  YYSYMBOL_PROVIDED = 58,                  /* PROVIDED  */
  YYSYMBOL_FULL = 59,                      /* FULL  */
  YYSYMBOL_EMPTY = 60,                     /* EMPTY  */
  YYSYMBOL_NFULL = 61,                     /* NFULL  */
  YYSYMBOL_NEMPTY = 62,                    /* NEMPTY  */
  YYSYMBOL_XU = 63,                        /* XU  */
  YYSYMBOL_CLAIM = 64,                     /* CLAIM  */
  YYSYMBOL_TRACE = 65,                     /* TRACE  */
  YYSYMBOL_INIT = 66,                      /* INIT  */
  YYSYMBOL_WHILE = 67,                     /* WHILE  */
  YYSYMBOL_WHEN = 68,                      /* WHEN  */
  YYSYMBOL_WAIT = 69,                      /* WAIT  */
  YYSYMBOL_RESET = 70,                     /* RESET  */
  YYSYMBOL_SPEC = 71,                      /* SPEC  */
  YYSYMBOL_EVENTUALLY = 72,                /* EVENTUALLY  */
  YYSYMBOL_ALWAYS = 73,                    /* ALWAYS  */
  YYSYMBOL_GLOBALLY = 74,                  /* GLOBALLY  */
  YYSYMBOL_FINALLY = 75,                   /* FINALLY  */
  YYSYMBOL_UNTIL = 76,                     /* UNTIL  */
  YYSYMBOL_NEXT = 77,                      /* NEXT  */
  YYSYMBOL_LTL = 78,                       /* LTL  */
  YYSYMBOL_BLTL = 79,                      /* BLTL  */
  YYSYMBOL_K = 80,                         /* K  */
  YYSYMBOL_FMULTILTL = 81,                 /* FMULTILTL  */
  YYSYMBOL_ASGN = 82,                      /* ASGN  */
  YYSYMBOL_SND = 83,                       /* SND  */
  YYSYMBOL_O_SND = 84,                     /* O_SND  */
  YYSYMBOL_RCV = 85,                       /* RCV  */
  YYSYMBOL_R_RCV = 86,                     /* R_RCV  */
  YYSYMBOL_OR = 87,                        /* OR  */
  YYSYMBOL_AND = 88,                       /* AND  */
  YYSYMBOL_89_ = 89,                       /* '|'  */
  YYSYMBOL_90_ = 90,                       /* '^'  */
  YYSYMBOL_91_ = 91,                       /* '&'  */
  YYSYMBOL_EQ = 92,                        /* EQ  */
  YYSYMBOL_NE = 93,                        /* NE  */
  YYSYMBOL_GT = 94,                        /* GT  */
  YYSYMBOL_LT = 95,                        /* LT  */
  YYSYMBOL_GE = 96,                        /* GE  */
  YYSYMBOL_LE = 97,                        /* LE  */
  YYSYMBOL_LSHIFT = 98,                    /* LSHIFT  */
  YYSYMBOL_RSHIFT = 99,                    /* RSHIFT  */
  YYSYMBOL_100_ = 100,                     /* '+'  */
  YYSYMBOL_101_ = 101,                     /* '-'  */
  YYSYMBOL_102_ = 102,                     /* '*'  */
  YYSYMBOL_103_ = 103,                     /* '/'  */
  YYSYMBOL_104_ = 104,                     /* '%'  */
  YYSYMBOL_INCR = 105,                     /* INCR  */
  YYSYMBOL_DECR = 106,                     /* DECR  */
  YYSYMBOL_107_ = 107,                     /* '~'  */
  YYSYMBOL_UMIN = 108,                     /* UMIN  */
  YYSYMBOL_NEG = 109,                      /* NEG  */
  YYSYMBOL_COUNT = 110,                    /* COUNT  */
  YYSYMBOL_CONTEXT = 111,                  /* CONTEXT  */
  YYSYMBOL_DOT = 112,                      /* DOT  */
  YYSYMBOL_IMPLIES = 113,                  /* IMPLIES  */
  YYSYMBOL_114_ = 114,                     /* '('  */
  YYSYMBOL_115_ = 115,                     /* ')'  */
  YYSYMBOL_116_ = 116,                     /* '['  */
  YYSYMBOL_117_ = 117,                     /* ']'  */
  YYSYMBOL_118_ = 118,                     /* '{'  */
  YYSYMBOL_119_ = 119,                     /* '}'  */
  YYSYMBOL_120_ = 120,                     /* ':'  */
  YYSYMBOL_121_ = 121,                     /* ','  */
  YYSYMBOL_122_ = 122,                     /* '.'  */
  YYSYMBOL_123_ = 123,                     /* '@'  */
  YYSYMBOL_YYACCEPT = 124,                 /* $accept  */
  YYSYMBOL_start_parsing = 125,            /* start_parsing  */
  YYSYMBOL_126_1 = 126,                    /* $@1  */
  YYSYMBOL_127_2 = 127,                    /* $@2  */
  YYSYMBOL_program = 128,                  /* program  */
  YYSYMBOL_units = 129,                    /* units  */
  YYSYMBOL_unit = 130,                     /* unit  */
  YYSYMBOL_proc = 131,                     /* proc  */
  YYSYMBOL_132_3 = 132,                    /* $@3  */
  YYSYMBOL_133_4 = 133,                    /* $@4  */
  YYSYMBOL_proctype = 134,                 /* proctype  */
  YYSYMBOL_inst = 135,                     /* inst  */
  YYSYMBOL_init = 136,                     /* init  */
  YYSYMBOL_137_5 = 137,                    /* $@5  */
  YYSYMBOL_claim = 138,                    /* claim  */
  YYSYMBOL_139_6 = 139,                    /* $@6  */
  YYSYMBOL_events = 140,                   /* events  */
  YYSYMBOL_utypedef = 141,                 /* utypedef  */
  YYSYMBOL_mtypedef = 142,                 /* mtypedef  */
  YYSYMBOL_143_7 = 143,                    /* $@7  */
  YYSYMBOL_ns = 144,                       /* ns  */
  YYSYMBOL_145_8 = 145,                    /* $@8  */
  YYSYMBOL_146_9 = 146,                    /* $@9  */
  YYSYMBOL_147_10 = 147,                   /* $@10  */
  YYSYMBOL_c_fcts = 148,                   /* c_fcts  */
  YYSYMBOL_cstate = 149,                   /* cstate  */
  YYSYMBOL_ccode = 150,                    /* ccode  */
  YYSYMBOL_cexpr = 151,                    /* cexpr  */
  YYSYMBOL_body = 152,                     /* body  */
  YYSYMBOL_153_11 = 153,                   /* $@11  */
  YYSYMBOL_sequence = 154,                 /* sequence  */
  YYSYMBOL_step = 155,                     /* step  */
  YYSYMBOL_vis = 156,                      /* vis  */
  YYSYMBOL_asgn = 157,                     /* asgn  */
  YYSYMBOL_one_decl = 158,                 /* one_decl  */
  YYSYMBOL_159_12 = 159,                   /* $@12  */
  YYSYMBOL_160_13 = 160,                   /* $@13  */
  YYSYMBOL_decl_lst = 161,                 /* decl_lst  */
  YYSYMBOL_decl = 162,                     /* decl  */
  YYSYMBOL_var_list = 163,                 /* var_list  */
  YYSYMBOL_ivar = 164,                     /* ivar  */
  YYSYMBOL_param_list = 165,               /* param_list  */
  YYSYMBOL_ch_init = 166,                  /* ch_init  */
  YYSYMBOL_basetype = 167,                 /* basetype  */
  YYSYMBOL_typ_list = 168,                 /* typ_list  */
  YYSYMBOL_vardcl = 169,                   /* vardcl  */
  YYSYMBOL_varref = 170,                   /* varref  */
  YYSYMBOL_pfld = 171,                     /* pfld  */
  YYSYMBOL_cmpnd = 172,                    /* cmpnd  */
  YYSYMBOL_sfld = 173,                     /* sfld  */
  YYSYMBOL_stmnt = 174,                    /* stmnt  */
  YYSYMBOL_Special = 175,                  /* Special  */
  YYSYMBOL_Stmnt = 176,                    /* Stmnt  */
  YYSYMBOL_options = 177,                  /* options  */
  YYSYMBOL_option = 178,                   /* option  */
  YYSYMBOL_real_expr = 179,                /* real_expr  */
  YYSYMBOL_OS = 180,                       /* OS  */
  YYSYMBOL_MS = 181,                       /* MS  */
  YYSYMBOL_aname = 182,                    /* aname  */
  YYSYMBOL_expr = 183,                     /* expr  */
  YYSYMBOL_Opt_priority = 184,             /* Opt_priority  */
  YYSYMBOL_full_expr = 185,                /* full_expr  */
  YYSYMBOL_Opt_enabler = 186,              /* Opt_enabler  */
  YYSYMBOL_Expr = 187,                     /* Expr  */
  YYSYMBOL_Probe = 188,                    /* Probe  */
  YYSYMBOL_args = 189,                     /* args  */
  YYSYMBOL_prargs = 190,                   /* prargs  */
  YYSYMBOL_margs = 191,                    /* margs  */
  YYSYMBOL_arg = 192,                      /* arg  */
  YYSYMBOL_rarg = 193,                     /* rarg  */
  YYSYMBOL_rargs = 194,                    /* rargs  */
  YYSYMBOL_nlst = 195,                     /* nlst  */
  YYSYMBOL_props = 196,                    /* props  */
  YYSYMBOL_prop = 197,                     /* prop  */
  YYSYMBOL_ltl_expr = 198,                 /* ltl_expr  */
  YYSYMBOL_bltl_expr = 199,                /* bltl_expr  */
  YYSYMBOL_k_steps = 200,                  /* k_steps  */
  YYSYMBOL_variant_quants = 201,           /* variant_quants  */
  YYSYMBOL_variant_quant = 202,            /* variant_quant  */
  YYSYMBOL_variant_expr = 203              /* variant_expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2147

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  124
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  80
/* YYNRULES -- Number of rules.  */
#define YYNRULES  249
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  495

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   359


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   104,    91,     2,
     114,   115,   102,   100,   121,   101,   122,   103,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   120,     2,
       2,     2,     2,     2,   123,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   116,     2,   117,    90,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   118,    89,   119,   107,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    92,    93,    94,    95,    96,    97,
      98,    99,   105,   106,   108,   109,   110,   111,   112,   113
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   150,   150,   151,   150,   175,   178,   179,   182,   183,
     184,   185,   186,   199,   200,   201,   202,   203,   209,   217,
     207,   232,   233,   236,   237,   238,   239,   253,   252,   269,
     268,   278,   281,   296,   296,   315,   316,   318,   315,   333,
     334,   337,   338,   339,   340,   343,   344,   347,   350,   350,
     360,   361,   362,   365,   370,   371,   372,   373,   381,   382,
     383,   384,   387,   388,   394,   394,   395,   395,   398,   399,
     403,   404,   411,   412,   415,   420,   425,   430,   431,   432,
     436,   439,   440,   444,   459,   476,   477,   478,   481,   484,
     485,   488,   489,   492,   493,   497,   498,   501,   502,   503,
     504,   505,   506,   507,   508,   512,   513,   514,   515,   516,
     517,   518,   519,   520,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   539,   542,   543,   546,   547,   551,   552,
     553,   554,   555,   556,   557,   560,   561,   564,   565,   568,
     569,   572,   573,   574,   575,   576,   577,   578,   579,   580,
     581,   582,   583,   584,   585,   586,   587,   588,   589,   590,
     591,   592,   593,   601,   602,   603,   608,   609,   610,   611,
     612,   615,   616,   617,   618,   619,   620,   621,   622,   623,
     624,   625,   630,   631,   632,   635,   636,   639,   640,   643,
     644,   649,   650,   651,   652,   653,   654,   655,   656,   657,
     663,   664,   665,   666,   670,   671,   675,   676,   680,   681,
     684,   685,   688,   689,   690,   691,   695,   696,   697,   698,
     701,   702,   703,   707,   708,   709,   712,   719,   727,   738,
     739,   740,   741,   742,   745,   746,   747,   748,   749,   752,
     753,   754,   755,   756,   757,   760,   761,   764,   765,   768
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "CONST", "IF", "DO",
  "AC", "TYPE", "NAME", "UNAME", "PNAME", "INAME", "VNAME", "BASE",
  "STRING", "REAL", "TRUE", "FALSE", "SKIP", "ASSERT", "PRINT", "PRINTM",
  "C_CODE", "C_DECL", "C_EXPR", "C_STATE", "C_TRACK", "RUN", "LEN",
  "ENABLED", "EVAL", "PC_VAL", "TYPEDEF", "MTYPE", "INLINE", "LABEL", "OF",
  "GOTO", "BREAK", "ELSE", "SEMI", "FI", "OD", "CA", "SEP", "ATOMIC",
  "NON_ATOMIC", "D_STEP", "UNLESS", "TIMEOUT", "NONPROGRESS", "ACTIVE",
  "PROCTYPE", "D_PROCTYPE", "HIDDEN", "SHOW", "ISLOCAL", "PRIORITY",
  "PROVIDED", "FULL", "EMPTY", "NFULL", "NEMPTY", "XU", "CLAIM", "TRACE",
  "INIT", "WHILE", "WHEN", "WAIT", "RESET", "SPEC", "EVENTUALLY", "ALWAYS",
  "GLOBALLY", "FINALLY", "UNTIL", "NEXT", "LTL", "BLTL", "K", "FMULTILTL",
  "ASGN", "SND", "O_SND", "RCV", "R_RCV", "OR", "AND", "'|'", "'^'", "'&'",
  "EQ", "NE", "GT", "LT", "GE", "LE", "LSHIFT", "RSHIFT", "'+'", "'-'",
  "'*'", "'/'", "'%'", "INCR", "DECR", "'~'", "UMIN", "NEG", "COUNT",
  "CONTEXT", "DOT", "IMPLIES", "'('", "')'", "'['", "']'", "'{'", "'}'",
  "':'", "','", "'.'", "'@'", "$accept", "start_parsing", "$@1", "$@2",
  "program", "units", "unit", "proc", "$@3", "$@4", "proctype", "inst",
  "init", "$@5", "claim", "$@6", "events", "utypedef", "mtypedef", "$@7",
  "ns", "$@8", "$@9", "$@10", "c_fcts", "cstate", "ccode", "cexpr", "body",
  "$@11", "sequence", "step", "vis", "asgn", "one_decl", "$@12", "$@13",
  "decl_lst", "decl", "var_list", "ivar", "param_list", "ch_init",
  "basetype", "typ_list", "vardcl", "varref", "pfld", "cmpnd", "sfld",
  "stmnt", "Special", "Stmnt", "options", "option", "real_expr", "OS",
  "MS", "aname", "expr", "Opt_priority", "full_expr", "Opt_enabler",
  "Expr", "Probe", "args", "prargs", "margs", "arg", "rarg", "rargs",
  "nlst", "props", "prop", "ltl_expr", "bltl_expr", "k_steps",
  "variant_quants", "variant_quant", "variant_expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-390)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-137)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -390,    38,  1484,  -390,  -390,  -390,    13,    32,    44,  -390,
    -390,   -60,  -390,  -390,  -390,  -390,   -26,  -390,  -390,  1449,
    -390,  -390,    80,  -390,  -390,  -390,  -390,  -390,  -390,  -390,
    -390,  -390,   131,  -390,    85,   127,   -13,   143,    39,   -26,
    -390,  -390,    97,    91,  -390,  -390,  -390,   153,    11,  -390,
     152,   157,    28,  -390,    68,    78,  -390,  1016,   203,   -26,
     204,   208,   211,  -390,    91,  -390,  -390,  -390,   219,   219,
    -390,  -390,   168,   188,   111,   118,  -390,  -390,  -390,   189,
     189,   189,   -30,   134,  -390,  -390,  -390,  1258,   137,   140,
    -390,    90,   141,   142,   144,   226,  -390,  -390,   117,   139,
    -390,  -390,   169,   170,   179,   180,   313,   313,  1321,  1321,
    1321,  1321,   181,   174,  1258,  1016,  -390,  -390,   435,  -390,
    -390,   266,   175,  -390,   250,  -390,  -390,  2005,  -390,   103,
    -390,  -390,  -390,  -390,  -390,   182,   183,   121,  -390,   185,
     184,   -48,  -390,   186,   221,  -390,  -390,    28,  -390,   296,
     668,   264,   189,   267,   263,  -390,  1321,   784,  1321,   192,
     -49,  -390,   297,    18,  -390,  -390,   198,    14,  1321,  1321,
    -390,  1016,  1016,    14,    14,    14,    14,  1321,    14,   -58,
    1321,   -58,  1321,   -58,   -58,   -58,   -58,  1321,   305,  1286,
     -57,   435,    -9,  -390,   199,   900,  1258,  1321,  1321,    72,
      73,  -390,  -390,  1321,    14,    14,   311,    14,  -390,  1132,
    1321,  1258,  1258,  1321,  1321,  1321,  1321,  1321,  1321,  1321,
    1321,  1321,  1321,  1321,  1321,  1321,  1321,  1321,  1321,  1321,
    1258,  1258,  1321,  1321,   202,   202,   205,   196,   -11,   314,
     321,   322,   219,  1195,  -390,   210,   217,    10,   552,  -390,
    -390,  -390,  -390,  1767,   -29,  -390,  -390,  -390,  1500,   220,
    -390,   218,   222,   215,   224,   228,  1321,   232,  1860,  1889,
     435,   435,   238,   240,   241,   242,  1351,   194,  1321,  1321,
     -58,   -58,  1918,   175,  1321,  -390,  -390,   207,  -390,  -390,
    -390,  -390,  1465,  -390,  -390,  -390,  -390,   244,    20,   356,
      20,    20,  -390,   -73,  -390,    20,    20,  -390,  1798,   245,
    -390,  -390,  -390,  -390,  2034,   548,   272,   779,  -390,   895,
    1009,  1316,   430,   430,   309,   309,   309,   309,   100,   100,
     -25,   -25,   -58,   -58,   -58,   -58,   548,   272,   779,  -390,
    2034,   246,   247,   353,  -390,  -390,  1321,   121,  -390,   252,
    -390,     9,   251,  -390,  -390,   367,  -390,  2034,   296,  -390,
    -390,  -390,    10,   120,  -390,  -390,  1132,  1321,  -390,  1321,
     258,  -390,  -390,   259,  -390,  -390,  -390,   256,   257,  -390,
    -390,  -390,  -390,  1321,  1321,  1321,  1321,  1321,  1321,   664,
     779,  -390,  -390,  1535,  -390,  1321,  1321,   283,  -390,   268,
     261,    20,    20,   285,   274,    27,  -390,  -390,  -390,   262,
     275,  -390,  -390,  -390,  -390,  -390,  -390,   276,  -390,   -26,
     138,    10,    10,    10,    10,  1016,  -390,  -390,  -390,    97,
    -390,  -390,  1569,  1602,  1635,  1668,  1701,  1734,  1321,   282,
    1947,  -390,  -390,  -390,   284,  -390,  -390,  -390,    14,   390,
     286,  -390,    97,   365,  -390,  -390,   122,   122,  -390,  -390,
     552,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  1976,  -390,
    -390,  -390,  -390,  -390,  1321,   345,   287,  -390,  -390,  1829,
     292,   -26,   172,  -390,  1258,  -390,  -390,  -390,   294,   298,
     304,   172,  -390,  -390,  -390
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,    23,     1,    45,    46,     0,     0,     0,    35,
      17,    24,    59,    60,    61,    29,     0,    27,     3,     5,
       6,     8,     0,     9,    10,    11,    13,    14,    16,    15,
      40,    39,     0,    12,     0,     0,     0,     0,     0,     0,
      48,    31,   185,   223,     7,    21,    22,     0,    62,    66,
      41,    42,    58,    36,     0,     0,    30,    58,     0,     0,
       0,     0,     0,     4,   223,    18,    63,    33,     0,     0,
      43,    44,     0,    68,     0,     0,    25,    26,   173,     0,
       0,     0,    89,     0,   174,   175,   199,     0,     0,     0,
      47,     0,     0,     0,     0,     0,   102,   118,     0,     0,
     176,   177,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    58,   112,   172,    58,    50,
      53,   171,    93,    88,    56,    95,    96,   187,   117,   188,
     191,   183,   184,   186,    28,     0,     0,     0,   225,     0,
       0,    85,    65,    72,    74,    67,    64,    58,    32,    77,
      58,     0,   124,     0,     0,   123,     0,    58,   204,    89,
     171,   111,     0,     0,   139,   140,     0,     0,     0,     0,
     103,    58,    58,     0,     0,     0,     0,     0,     0,   229,
       0,   230,     0,   231,   163,   162,   161,     0,     0,     0,
       0,    58,   137,    52,     0,    58,     0,     0,     0,     0,
       0,   106,   107,     0,     0,     0,     0,     0,    91,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   245,    58,     0,
       0,     0,     0,     0,    69,    78,     0,     0,   135,    99,
     125,   100,   101,     0,    89,    55,    54,   104,   210,     0,
     205,     0,     0,   206,     0,     0,   204,     0,     0,     0,
      58,    58,     0,     0,     0,     0,     0,     0,     0,     0,
     234,   235,     0,    93,     0,   141,   192,     0,    49,   138,
      51,   105,   210,    98,   208,   116,   214,     0,     0,     0,
       0,     0,   212,   216,    97,     0,     0,   113,     0,     0,
     182,   181,    94,    57,   232,   157,   198,   156,   197,   149,
     148,   147,   154,   155,   150,   151,   152,   153,   158,   159,
     142,   143,   144,   145,   146,   233,   196,   195,   194,   193,
       0,   183,   184,     0,   248,   247,     0,     0,    71,     0,
     220,     0,     0,    86,    73,     0,    76,    75,    77,    37,
     134,   133,     0,     0,   126,    90,     0,     0,   122,     0,
       0,   110,   109,     0,   166,   167,   178,     0,     0,   200,
     202,   201,   203,     0,     0,     0,     0,     0,     0,   157,
     156,   160,    92,     0,   121,     0,     0,     0,   215,     0,
       0,     0,     0,     0,     0,     0,   170,   226,   227,     0,
     183,   246,    19,   221,    34,   222,    87,     0,    79,     0,
       0,     0,     0,     0,     0,    58,   211,   207,   108,   185,
     119,   120,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   114,   219,   168,     0,   217,   115,   169,     0,     0,
       0,   228,   185,     0,    38,   128,   129,   130,   131,   132,
     135,   165,   243,   244,   239,   240,   241,   242,     0,   209,
     213,   218,   180,   179,     0,   189,     0,   127,   164,     0,
       0,     0,     0,   249,     0,    20,    81,    82,    83,     0,
       0,     0,    80,   190,    84
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -390,  -390,  -390,  -390,  -390,  -390,   402,  -390,  -390,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,  -390,
    -390,  -390,  -390,  -390,  -390,  -390,    47,  -390,   -39,  -390,
    -111,  -106,    51,  -390,     5,  -390,  -390,  -141,  -390,   -64,
    -390,    67,  -390,  -390,   -65,  -390,   -56,   260,   223,   145,
    -155,  -390,  -390,   -16,  -390,  -225,  -175,  -390,  -390,    49,
    -389,   -84,  -390,   -85,  -390,   163,  -390,   249,  -187,  -390,
    -192,  -390,   385,  -390,  -223,   227,   343,   114,  -390,   230
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     2,    43,    18,    19,    20,    21,   139,   452,
      47,    22,    23,    42,    24,    39,    25,    26,    27,   140,
      28,    37,    75,   419,    29,    30,   116,   117,    41,    57,
     118,   119,    72,    67,   120,    68,    69,    74,   349,   142,
     143,   246,   356,   488,   489,   144,   160,   122,   123,   208,
     124,   125,   126,   151,   152,   363,   194,   195,   166,   127,
      59,   128,   481,   129,   130,   259,   370,   293,   260,   303,
     304,   351,    63,    64,   131,   132,   180,   236,   237,   344
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      56,   121,   257,   161,   191,   145,   244,    33,   307,   341,
     294,   294,   193,   360,   155,   155,   287,   413,   210,   -64,
     134,   264,   159,   296,    33,   361,   159,    34,   159,   190,
     230,   231,  -136,  -136,  -136,  -136,   261,   262,     3,   248,
     461,   401,    54,    12,    13,    14,    35,    55,   402,    31,
     297,   210,    36,    32,   313,   229,    38,    73,   286,   121,
     270,   271,   121,   475,   153,   154,    31,   203,   240,   204,
      32,   205,   241,   364,   206,   296,   296,   226,   227,   228,
     159,   159,    12,    13,    14,   193,   156,   156,   229,   290,
     157,   366,    40,    66,   121,   377,   378,   348,   164,    50,
     165,   121,   297,   297,   -70,    52,   397,   265,   399,   400,
    -136,   267,   291,   403,   404,   121,   121,   272,   273,   274,
     275,   299,   277,   410,   362,   113,   316,   318,   414,   113,
     415,   113,    45,    46,   300,   121,   250,   420,    48,   121,
      49,    51,   193,   302,   302,   337,   339,   448,   309,   310,
     449,    53,    73,   121,    58,   179,   181,   183,   184,   185,
     186,    65,   256,   189,   193,   193,    70,   298,   305,    60,
      61,    71,    62,   299,   299,   146,   210,    49,   354,   486,
     426,   487,   427,   113,   113,    76,   300,   300,   301,   306,
     230,   231,   121,   234,   235,    77,   456,   457,   458,   459,
     224,   225,   226,   227,   228,   253,   133,   258,   439,   444,
     445,   257,   135,   229,   121,   121,   136,   268,   269,   137,
     421,   422,   423,   424,   423,   424,   276,   141,   147,   280,
     148,   281,   149,   150,   170,   171,   282,   425,   421,   422,
     423,   424,   302,    73,   302,   302,   292,   292,   158,   302,
     302,   162,   308,   455,   163,   167,   168,   172,   169,   314,
     315,   317,   319,   320,   321,   322,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   332,   333,   334,   335,   336,
     338,   340,   340,   173,   174,   477,   383,   384,   385,   386,
     387,   388,   357,   175,   176,   187,   188,   207,   209,   238,
     232,   233,   239,   243,   245,   249,   252,   242,   156,   251,
     121,   263,   266,   159,   460,   258,    78,   347,   288,   311,
     343,   159,   350,   346,   352,   353,   394,   389,   390,    84,
      85,   358,   359,   393,   301,   368,   369,    90,   306,   371,
      91,    92,    93,   372,    94,   302,   302,   374,   196,   197,
     198,   199,   200,   379,   193,   380,   381,   382,   396,   398,
     231,   409,   100,   101,   406,   407,   408,   412,   416,   121,
     417,   201,   202,   428,   429,   430,   431,   441,   443,   446,
     454,   450,   203,   442,   204,   210,   205,   106,   107,   206,
     108,   447,   472,   453,   451,   340,   109,   469,   473,   471,
     490,   476,   474,   480,   121,   482,   484,   222,   223,   224,
     225,   226,   227,   228,   110,   491,   258,   492,   258,   493,
     111,    44,   229,   112,   113,   418,   494,   177,   392,   373,
     312,   178,   432,   433,   434,   435,   436,   437,    78,    79,
      80,    81,   485,    82,   258,   440,    83,   295,   283,   138,
     182,    84,    85,    86,    87,    88,    89,     4,     5,    90,
     342,   411,    91,    92,    93,   345,    94,     0,     0,     0,
       0,     0,    95,    96,    97,   192,     0,     0,     0,     0,
      98,     0,    99,     0,   100,   101,     0,   468,     0,    12,
      13,    14,     0,     0,   102,   103,   104,   105,     0,     0,
       0,     0,     0,     0,     0,     0,   210,     0,     0,   106,
     107,     0,   108,     0,     0,     0,     0,     0,   109,     0,
       0,     0,     0,   479,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,     0,   110,     0,     0,     0,
       0,     0,   111,   229,     0,   112,   113,     0,     0,   114,
       0,     0,     0,   115,  -135,    78,    79,    80,    81,   -58,
      82,   -58,     0,    83,     0,     0,     0,     0,    84,    85,
      86,    87,    88,    89,     4,     5,    90,     0,     0,    91,
      92,    93,     0,    94,     0,     0,     0,     0,     0,    95,
      96,    97,   192,     0,     0,     0,     0,    98,     0,    99,
       0,   100,   101,     0,     0,     0,    12,    13,    14,     0,
       0,   102,   103,   104,   105,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   210,     0,   106,   107,     0,   108,
       0,     0,     0,     0,     0,   109,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   110,     0,     0,     0,     0,     0,   111,
       0,   229,   112,   113,     0,     0,   114,     0,     0,     0,
     115,    78,    79,    80,    81,     0,    82,     0,     0,    83,
       0,     0,     0,     0,    84,    85,    86,    87,    88,    89,
       4,     5,    90,     0,     0,    91,    92,    93,     0,    94,
       0,     0,     0,     0,     0,    95,    96,    97,     0,     0,
       0,     0,     0,    98,     0,    99,     0,   100,   101,     0,
       0,     0,    12,    13,    14,     0,     0,   102,   103,   104,
     105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     210,     0,   106,   107,     0,   108,     0,     0,     0,     0,
       0,   109,   279,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   110,
       0,     0,     0,     0,     0,   111,     0,   229,   112,   113,
       0,     0,   114,     0,   247,     0,   115,    78,    79,    80,
      81,     0,   254,     0,     0,    83,     0,     0,     0,     0,
      84,    85,    86,    87,    88,    89,     4,     5,    90,     0,
       0,    91,    92,    93,     0,    94,     0,     0,     0,     0,
       0,    95,    96,    97,     0,     0,     0,     0,     0,    98,
       0,    99,     0,   100,   101,     0,     0,     0,    12,    13,
      14,     0,     0,   102,   103,   104,   105,   255,     0,     0,
       0,     0,     0,     0,     0,   210,     0,     0,   106,   107,
       0,   108,     0,     0,     0,     0,     0,   109,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,     0,   110,     0,     0,     0,     0,
       0,   111,   229,     0,   112,   113,     0,     0,   114,     0,
       0,     0,   115,    78,    79,    80,    81,     0,    82,     0,
       0,    83,     0,     0,     0,     0,    84,    85,    86,    87,
      88,    89,     4,     5,    90,     0,     0,    91,    92,    93,
       0,    94,     0,     0,     0,     0,     0,    95,    96,    97,
     289,     0,     0,     0,     0,    98,     0,    99,     0,   100,
     101,     0,     0,     0,    12,    13,    14,     0,     0,   102,
     103,   104,   105,     0,     0,     0,     0,     0,     0,     0,
       0,   210,     0,     0,   106,   107,     0,   108,     0,     0,
       0,     0,     0,   109,     0,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
       0,   110,     0,     0,     0,     0,     0,   111,   229,     0,
     112,   113,     0,     0,   114,     0,     0,     0,   115,    78,
      79,    80,    81,     0,    82,     0,     0,    83,     0,     0,
       0,     0,    84,    85,    86,    87,    88,    89,     4,     5,
      90,     0,     0,    91,    92,    93,     0,    94,     0,     0,
       0,     0,     0,    95,    96,    97,     0,     0,     0,     0,
       0,    98,     0,    99,     0,   100,   101,     0,     0,     0,
      12,    13,    14,     0,     0,   102,   103,   104,   105,     0,
       0,     0,     0,     0,     0,   210,     0,     0,     0,     0,
     106,   107,     0,   108,     0,     0,     0,     0,     0,   109,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,     0,     0,     0,   110,     0,     0,
       0,     0,   229,   111,     0,     0,   112,   113,     0,     0,
     114,     0,     0,     0,   115,    78,    79,    80,    81,     0,
     254,     0,     0,    83,     0,     0,     0,     0,    84,    85,
      86,    87,    88,    89,     4,     5,    90,     0,     0,    91,
      92,    93,     0,    94,     0,     0,     0,     0,     0,    95,
      96,    97,     0,     0,     0,     0,     0,    98,     0,    99,
       0,   100,   101,     0,     0,     0,     0,     0,     0,     0,
       0,   102,   103,   104,   105,     0,     0,     0,    78,     0,
       0,     0,     0,   159,     0,     0,   106,   107,     0,   108,
       0,    84,    85,     0,     0,   109,     0,     0,     0,    90,
       0,     0,    91,    92,    93,     0,    94,     0,     0,     0,
       0,     0,     0,   110,     0,     0,     0,     0,     0,   111,
       0,     0,   112,   113,   100,   101,   114,     0,     0,     0,
     115,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    78,     0,     0,     0,     0,   159,     0,     0,   106,
     107,     0,   108,     0,    84,    85,    86,     0,   109,     0,
       0,     0,    90,     0,     0,    91,    92,    93,     0,    94,
       0,     0,     0,     0,     0,     0,   110,     0,     0,     0,
       0,     0,   111,     0,     0,   112,   113,   100,   101,   177,
       0,   355,     0,     0,     0,     0,     0,   102,   103,   104,
     105,     0,     0,     0,    78,     0,   284,     0,     0,   159,
       0,     0,   106,   107,     0,   108,     0,    84,    85,     0,
       0,   109,     0,     0,     0,    90,     0,     0,    91,    92,
      93,     0,    94,     0,     0,     0,     0,     0,     0,   110,
       0,     0,   210,     0,     0,   111,     0,     0,   112,   113,
     100,   101,   114,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   284,   210,     0,     0,   106,   107,     0,   108,   229,
       0,   285,     0,     0,   109,     0,     0,     0,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,     0,   110,     0,     0,     0,     0,   210,   111,   229,
       0,   112,   113,     0,     0,   177,     0,     0,   278,   279,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   -58,     0,   -58,     0,
       0,     0,     0,     0,   229,     0,   285,     0,     0,     0,
       0,     4,     5,     0,     6,     7,     0,     0,     0,     0,
       0,     8,     0,     9,     0,     0,     0,     0,     0,    10,
       0,   -58,     0,   -58,     0,     0,     0,     0,     0,     0,
      11,   -23,   -23,    12,    13,    14,     4,     5,     0,     6,
       7,     0,     0,    15,    16,    17,     8,     0,     9,     0,
       0,     0,     0,     0,    10,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    11,     0,     0,    12,    13,
      14,   210,     0,     0,     0,     0,     0,     0,    15,    16,
      17,     0,   278,   279,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
       0,     0,     0,     0,     0,     0,   210,     0,   229,   395,
       0,     0,     0,     0,     0,     0,   367,   278,   279,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,     0,     0,     0,     0,     0,
       0,   210,     0,   229,     0,     0,     0,     0,     0,     0,
       0,   367,   278,   279,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
       0,     0,     0,     0,     0,   210,     0,     0,   229,     0,
       0,     0,     0,     0,     0,   438,   278,   279,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,     0,     0,     0,     0,   210,     0,
       0,     0,   229,     0,     0,     0,     0,     0,   462,   278,
     279,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,     0,     0,     0,
       0,   210,     0,     0,     0,   229,     0,     0,     0,     0,
       0,   463,   278,   279,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
       0,     0,     0,     0,   210,     0,     0,     0,   229,     0,
       0,     0,     0,     0,   464,   278,   279,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,     0,     0,     0,     0,   210,     0,     0,
       0,   229,     0,     0,     0,     0,     0,   465,   278,   279,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,     0,     0,     0,     0,
     210,     0,     0,     0,   229,     0,     0,     0,     0,     0,
     466,   278,   279,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,     0,
       0,     0,     0,   210,     0,     0,     0,   229,     0,     0,
       0,     0,     0,   467,   278,   279,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,     0,     0,   210,     0,     0,     0,     0,     0,
     229,     0,     0,     0,   365,   278,   279,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,     0,     0,   210,     0,     0,     0,     0,
       0,   229,     0,     0,     0,   405,   278,   279,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,     0,     0,   210,     0,     0,     0,
       0,     0,   229,     0,     0,     0,   483,   278,   279,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   210,     0,     0,     0,     0,
       0,     0,     0,   229,     0,   375,   278,   279,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   210,     0,     0,     0,     0,     0,
       0,     0,   229,     0,   376,   278,   279,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   210,     0,     0,     0,     0,     0,     0,
       0,   229,     0,   391,   278,   279,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   210,     0,     0,     0,     0,     0,     0,     0,
     229,     0,   470,   278,   279,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   210,     0,     0,     0,     0,     0,     0,     0,   229,
       0,   478,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     210,     0,     0,     0,     0,     0,     0,     0,   229,     0,
       0,   278,   279,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,     0,
       0,     0,     0,     0,     0,     0,     0,   229
};

static const yytype_int16 yycheck[] =
{
      39,    57,   157,    87,   115,    69,   147,     2,   200,   232,
     197,   198,   118,     3,    44,    44,   191,     8,    76,     8,
      59,     3,     8,     3,    19,    15,     8,    14,     8,   114,
      87,    88,    41,    42,    43,    44,    85,    86,     0,   150,
     429,   114,     3,    54,    55,    56,    14,     8,   121,     2,
      30,    76,     8,     2,   209,   113,   116,    52,   115,   115,
     171,   172,   118,   452,    80,    81,    19,   116,   116,   118,
      19,   120,   120,   248,   123,     3,     3,   102,   103,   104,
       8,     8,    54,    55,    56,   191,   116,   116,   113,   195,
     120,   120,   118,    82,   150,   270,   271,   238,     8,    14,
      10,   157,    30,    30,   115,   118,   298,   163,   300,   301,
     119,   167,   196,   305,   306,   171,   172,   173,   174,   175,
     176,   101,   178,   346,   114,   111,   211,   212,   119,   111,
     121,   111,    52,    53,   114,   191,   152,   362,     7,   195,
       9,    14,   248,   199,   200,   230,   231,   120,   204,   205,
     123,     8,   147,   209,    57,   106,   107,   108,   109,   110,
     111,     8,   157,   114,   270,   271,    14,    95,    95,    78,
      79,    14,    81,   101,   101,     7,    76,     9,   242,     7,
     367,     9,   369,   111,   111,   117,   114,   114,   116,   116,
      87,    88,   248,    72,    73,   117,   421,   422,   423,   424,
     100,   101,   102,   103,   104,   156,     3,   158,   395,   401,
     402,   366,     8,   113,   270,   271,     8,   168,   169,     8,
     100,   101,   102,   103,   102,   103,   177,     8,    40,   180,
     119,   182,   114,    44,     8,   118,   187,   117,   100,   101,
     102,   103,   298,   238,   300,   301,   197,   198,   114,   305,
     306,   114,   203,   115,   114,   114,   114,   118,   114,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   114,   114,   460,    92,    93,    94,    95,
      96,    97,   243,   114,   114,   114,   122,   122,    48,   114,
     118,   118,   118,    82,     8,    41,    43,   121,   116,    42,
     366,    14,   114,     8,   425,   266,     3,   121,   119,     8,
     118,     8,     8,   118,     3,     3,   119,   278,   279,    16,
      17,   121,   115,   284,   116,   115,   121,    24,   116,   115,
      27,    28,    29,   115,    31,   401,   402,   115,    82,    83,
      84,    85,    86,   115,   460,   115,   115,   115,   114,     3,
      88,     8,    49,    50,   119,   119,   119,   115,   117,   425,
       3,   105,   106,   115,   115,   119,   119,    94,   117,    94,
     419,   119,   116,   115,   118,    76,   120,    74,    75,   123,
      77,   117,   448,   117,   119,   346,    83,   115,     8,   115,
     484,    36,   116,    58,   460,   118,   114,    98,    99,   100,
     101,   102,   103,   104,   101,   121,   367,   119,   369,   115,
     107,    19,   113,   110,   111,   358,   491,   114,   283,   266,
     207,   118,   383,   384,   385,   386,   387,   388,     3,     4,
       5,     6,   481,     8,   395,   396,    11,   198,   188,    64,
     107,    16,    17,    18,    19,    20,    21,    22,    23,    24,
     233,   347,    27,    28,    29,   235,    31,    -1,    -1,    -1,
      -1,    -1,    37,    38,    39,    40,    -1,    -1,    -1,    -1,
      45,    -1,    47,    -1,    49,    50,    -1,   438,    -1,    54,
      55,    56,    -1,    -1,    59,    60,    61,    62,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    74,
      75,    -1,    77,    -1,    -1,    -1,    -1,    -1,    83,    -1,
      -1,    -1,    -1,   474,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,    -1,   101,    -1,    -1,    -1,
      -1,    -1,   107,   113,    -1,   110,   111,    -1,    -1,   114,
      -1,    -1,    -1,   118,   119,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    -1,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    27,
      28,    29,    -1,    31,    -1,    -1,    -1,    -1,    -1,    37,
      38,    39,    40,    -1,    -1,    -1,    -1,    45,    -1,    47,
      -1,    49,    50,    -1,    -1,    -1,    54,    55,    56,    -1,
      -1,    59,    60,    61,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    76,    -1,    74,    75,    -1,    77,
      -1,    -1,    -1,    -1,    -1,    83,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   101,    -1,    -1,    -1,    -1,    -1,   107,
      -1,   113,   110,   111,    -1,    -1,   114,    -1,    -1,    -1,
     118,     3,     4,     5,     6,    -1,     8,    -1,    -1,    11,
      -1,    -1,    -1,    -1,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    -1,    -1,    27,    28,    29,    -1,    31,
      -1,    -1,    -1,    -1,    -1,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    45,    -1,    47,    -1,    49,    50,    -1,
      -1,    -1,    54,    55,    56,    -1,    -1,    59,    60,    61,
      62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    74,    75,    -1,    77,    -1,    -1,    -1,    -1,
      -1,    83,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   101,
      -1,    -1,    -1,    -1,    -1,   107,    -1,   113,   110,   111,
      -1,    -1,   114,    -1,   116,    -1,   118,     3,     4,     5,
       6,    -1,     8,    -1,    -1,    11,    -1,    -1,    -1,    -1,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
      -1,    27,    28,    29,    -1,    31,    -1,    -1,    -1,    -1,
      -1,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,    45,
      -1,    47,    -1,    49,    50,    -1,    -1,    -1,    54,    55,
      56,    -1,    -1,    59,    60,    61,    62,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    74,    75,
      -1,    77,    -1,    -1,    -1,    -1,    -1,    83,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,    -1,   101,    -1,    -1,    -1,    -1,
      -1,   107,   113,    -1,   110,   111,    -1,    -1,   114,    -1,
      -1,    -1,   118,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    11,    -1,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    -1,    -1,    27,    28,    29,
      -1,    31,    -1,    -1,    -1,    -1,    -1,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,    45,    -1,    47,    -1,    49,
      50,    -1,    -1,    -1,    54,    55,    56,    -1,    -1,    59,
      60,    61,    62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    74,    75,    -1,    77,    -1,    -1,
      -1,    -1,    -1,    83,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
      -1,   101,    -1,    -1,    -1,    -1,    -1,   107,   113,    -1,
     110,   111,    -1,    -1,   114,    -1,    -1,    -1,   118,     3,
       4,     5,     6,    -1,     8,    -1,    -1,    11,    -1,    -1,
      -1,    -1,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    -1,    -1,    27,    28,    29,    -1,    31,    -1,    -1,
      -1,    -1,    -1,    37,    38,    39,    -1,    -1,    -1,    -1,
      -1,    45,    -1,    47,    -1,    49,    50,    -1,    -1,    -1,
      54,    55,    56,    -1,    -1,    59,    60,    61,    62,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,
      74,    75,    -1,    77,    -1,    -1,    -1,    -1,    -1,    83,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,    -1,    -1,    -1,   101,    -1,    -1,
      -1,    -1,   113,   107,    -1,    -1,   110,   111,    -1,    -1,
     114,    -1,    -1,    -1,   118,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    11,    -1,    -1,    -1,    -1,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    -1,    -1,    27,
      28,    29,    -1,    31,    -1,    -1,    -1,    -1,    -1,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    45,    -1,    47,
      -1,    49,    50,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    59,    60,    61,    62,    -1,    -1,    -1,     3,    -1,
      -1,    -1,    -1,     8,    -1,    -1,    74,    75,    -1,    77,
      -1,    16,    17,    -1,    -1,    83,    -1,    -1,    -1,    24,
      -1,    -1,    27,    28,    29,    -1,    31,    -1,    -1,    -1,
      -1,    -1,    -1,   101,    -1,    -1,    -1,    -1,    -1,   107,
      -1,    -1,   110,   111,    49,    50,   114,    -1,    -1,    -1,
     118,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,    -1,    -1,    -1,    -1,     8,    -1,    -1,    74,
      75,    -1,    77,    -1,    16,    17,    18,    -1,    83,    -1,
      -1,    -1,    24,    -1,    -1,    27,    28,    29,    -1,    31,
      -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,
      -1,    -1,   107,    -1,    -1,   110,   111,    49,    50,   114,
      -1,   116,    -1,    -1,    -1,    -1,    -1,    59,    60,    61,
      62,    -1,    -1,    -1,     3,    -1,    40,    -1,    -1,     8,
      -1,    -1,    74,    75,    -1,    77,    -1,    16,    17,    -1,
      -1,    83,    -1,    -1,    -1,    24,    -1,    -1,    27,    28,
      29,    -1,    31,    -1,    -1,    -1,    -1,    -1,    -1,   101,
      -1,    -1,    76,    -1,    -1,   107,    -1,    -1,   110,   111,
      49,    50,   114,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,    40,    76,    -1,    -1,    74,    75,    -1,    77,   113,
      -1,   115,    -1,    -1,    83,    -1,    -1,    -1,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,    -1,   101,    -1,    -1,    -1,    -1,    76,   107,   113,
      -1,   110,   111,    -1,    -1,   114,    -1,    -1,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,     7,    -1,     9,    -1,
      -1,    -1,    -1,    -1,   113,    -1,   115,    -1,    -1,    -1,
      -1,    22,    23,    -1,    25,    26,    -1,    -1,    -1,    -1,
      -1,    32,    -1,    34,    -1,    -1,    -1,    -1,    -1,    40,
      -1,     7,    -1,     9,    -1,    -1,    -1,    -1,    -1,    -1,
      51,    52,    53,    54,    55,    56,    22,    23,    -1,    25,
      26,    -1,    -1,    64,    65,    66,    32,    -1,    34,    -1,
      -1,    -1,    -1,    -1,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,
      56,    76,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,
      66,    -1,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,   113,   114,
      -1,    -1,    -1,    -1,    -1,    -1,   121,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    -1,   113,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   121,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,   113,    -1,
      -1,    -1,    -1,    -1,    -1,   120,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,    -1,    -1,    -1,    -1,    76,    -1,
      -1,    -1,   113,    -1,    -1,    -1,    -1,    -1,   119,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,   113,    -1,    -1,    -1,    -1,
      -1,   119,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
      -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,   113,    -1,
      -1,    -1,    -1,    -1,   119,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,   113,    -1,    -1,    -1,    -1,    -1,   119,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    -1,   113,    -1,    -1,    -1,    -1,    -1,
     119,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,   113,    -1,    -1,
      -1,    -1,    -1,   119,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,
     113,    -1,    -1,    -1,   117,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,    -1,    -1,    76,    -1,    -1,    -1,    -1,
      -1,   113,    -1,    -1,    -1,   117,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,    -1,    -1,    76,    -1,    -1,    -1,
      -1,    -1,   113,    -1,    -1,    -1,   117,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   113,    -1,   115,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,    76,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   113,    -1,   115,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   113,    -1,   115,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     113,    -1,   115,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,
      -1,   115,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   113,    -1,
      -1,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   113
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   125,   126,     0,    22,    23,    25,    26,    32,    34,
      40,    51,    54,    55,    56,    64,    65,    66,   128,   129,
     130,   131,   135,   136,   138,   140,   141,   142,   144,   148,
     149,   150,   156,   158,    14,    14,     8,   145,   116,   139,
     118,   152,   137,   127,   130,    52,    53,   134,     7,     9,
      14,    14,   118,     8,     3,     8,   152,   153,    57,   184,
      78,    79,    81,   196,   197,     8,    82,   157,   159,   160,
      14,    14,   156,   158,   161,   146,   117,   117,     3,     4,
       5,     6,     8,    11,    16,    17,    18,    19,    20,    21,
      24,    27,    28,    29,    31,    37,    38,    39,    45,    47,
      49,    50,    59,    60,    61,    62,    74,    75,    77,    83,
     101,   107,   110,   111,   114,   118,   150,   151,   154,   155,
     158,   170,   171,   172,   174,   175,   176,   183,   185,   187,
     188,   198,   199,     3,   152,     8,     8,     8,   196,   132,
     143,     8,   163,   164,   169,   163,     7,    40,   119,   114,
      44,   177,   178,   177,   177,    44,   116,   120,   114,     8,
     170,   185,   114,   114,     8,    10,   182,   114,   114,   114,
       8,   118,   118,   114,   114,   114,   114,   114,   118,   183,
     200,   183,   200,   183,   183,   183,   183,   114,   122,   183,
     187,   154,    40,   155,   180,   181,    82,    83,    84,    85,
      86,   105,   106,   116,   118,   120,   123,   122,   173,    48,
      76,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   113,
      87,    88,   118,   118,    72,    73,   201,   202,   114,   118,
     116,   120,   121,    82,   161,     8,   165,   116,   154,    41,
     177,    42,    43,   183,     8,    63,   158,   174,   183,   189,
     192,    85,    86,    14,     3,   170,   114,   170,   183,   183,
     154,   154,   170,   170,   170,   170,   183,   170,    87,    88,
     183,   183,   183,   171,    40,   115,   115,   180,   119,    40,
     155,   185,   183,   191,   192,   191,     3,    30,    95,   101,
     114,   116,   170,   193,   194,    95,   116,   194,   183,   170,
     170,     8,   172,   174,   183,   183,   187,   183,   187,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   187,   183,   187,
     183,   198,   199,   118,   203,   203,   118,   121,   161,   162,
       8,   195,     3,     3,   163,   116,   166,   183,   121,   115,
       3,    15,   114,   179,   180,   117,   120,   121,   115,   121,
     190,   115,   115,   189,   115,   115,   115,   180,   180,   115,
     115,   115,   115,    92,    93,    94,    95,    96,    97,   183,
     183,   115,   173,   183,   119,   114,   114,   194,     3,   194,
     194,   114,   121,   194,   194,   117,   119,   119,   119,     8,
     198,   201,   115,     8,   119,   121,   117,     3,   165,   147,
     179,   100,   101,   102,   103,   117,   192,   192,   115,   115,
     119,   119,   183,   183,   183,   183,   183,   183,   120,   192,
     183,    94,   115,   117,   194,   194,    94,   117,   120,   123,
     119,   119,   133,   117,   152,   115,   179,   179,   179,   179,
     154,   184,   119,   119,   119,   119,   119,   119,   183,   115,
     115,   115,   170,     8,   116,   184,    36,   180,   115,   183,
      58,   186,   118,   117,   114,   152,     7,     9,   167,   168,
     185,   121,   119,   115,   168
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,   124,   126,   127,   125,   128,   129,   129,   130,   130,
     130,   130,   130,   130,   130,   130,   130,   130,   132,   133,
     131,   134,   134,   135,   135,   135,   135,   137,   136,   139,
     138,   140,   141,   143,   142,   145,   146,   147,   144,   148,
     148,   149,   149,   149,   149,   150,   150,   151,   153,   152,
     154,   154,   154,   155,   155,   155,   155,   155,   156,   156,
     156,   156,   157,   157,   159,   158,   160,   158,   161,   161,
     162,   162,   163,   163,   164,   164,   164,   165,   165,   165,
     166,   167,   167,   168,   168,   169,   169,   169,   170,   171,
     171,   172,   172,   173,   173,   174,   174,   175,   175,   175,
     175,   175,   175,   175,   175,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   177,   177,   178,   178,   179,   179,
     179,   179,   179,   179,   179,   180,   180,   181,   181,   182,
     182,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   184,   184,   185,   185,   186,
     186,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     188,   188,   188,   188,   189,   189,   190,   190,   191,   191,
     192,   192,   193,   193,   193,   193,   194,   194,   194,   194,
     195,   195,   195,   196,   196,   196,   197,   197,   197,   198,
     198,   198,   198,   198,   199,   199,   199,   199,   199,   200,
     200,   200,   200,   200,   200,   201,   201,   202,   202,   203
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,     4,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     0,
      11,     1,     1,     0,     1,     4,     4,     0,     4,     0,
       3,     2,     5,     0,     7,     0,     0,     0,     9,     1,
       1,     3,     3,     4,     4,     1,     1,     1,     0,     5,
       1,     3,     2,     1,     3,     3,     1,     3,     0,     1,
       1,     1,     0,     1,     0,     4,     0,     4,     1,     3,
       0,     1,     1,     3,     1,     3,     3,     0,     1,     3,
       7,     1,     1,     1,     3,     1,     3,     4,     1,     1,
       4,     2,     4,     0,     2,     1,     1,     3,     3,     3,
       3,     3,     1,     2,     3,     3,     2,     2,     5,     4,
       4,     2,     1,     3,     5,     5,     3,     1,     1,     5,
       5,     4,     4,     2,     1,     2,     3,     6,     3,     3,
       3,     3,     3,     1,     1,     0,     1,     1,     2,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       4,     2,     2,     2,     7,     6,     4,     4,     5,     5,
       4,     1,     1,     1,     1,     1,     1,     1,     4,     6,
       6,     3,     3,     1,     1,     0,     2,     1,     1,     0,
       4,     1,     3,     3,     3,     3,     3,     3,     3,     1,
       4,     4,     4,     4,     0,     1,     0,     2,     1,     4,
       1,     3,     1,     4,     1,     2,     1,     3,     4,     3,
       1,     2,     2,     0,     1,     2,     5,     5,     6,     2,
       2,     2,     3,     3,     3,     3,     2,     3,     3,     5,
       5,     5,     5,     5,     5,     1,     3,     2,     2,     6
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (globalSymTab, program, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, globalSymTab, program); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, symTable** globalSymTab, stmnt** program)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (globalSymTab);
  YY_USE (program);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, symTable** globalSymTab, stmnt** program)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, globalSymTab, program);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, symTable** globalSymTab, stmnt** program)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], globalSymTab, program);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, globalSymTab, program); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, symTable** globalSymTab, stmnt** program)
{
  YY_USE (yyvaluep);
  YY_USE (globalSymTab);
  YY_USE (program);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (symTable** globalSymTab, stmnt** program)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, globalSymTab);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 150 "promela.y"
                        { *globalSymTab = new symTable("global"); symTable::addPredefinedSym(*globalSymTab); currentSymTab = *globalSymTab; }
#line 2281 "y.tab.cpp"
    break;

  case 3: /* $@2: %empty  */
#line 151 "promela.y"
                                                {
								std::list<symTable*> sysTables;
								auto sysSyms = (*globalSymTab)->getSymbols<sysSymNode*>();
								
								for(auto sym : sysSyms) {
									(*globalSymTab)->remove(sym);
									//dynamic_cast<sysSymNode*>(sym)->setDef(*globalSymTab);
								}

								auto sysTable = new symTable("system");

								for(auto sym : sysSyms) {
									sysTable->insert(sym); 
							  	}

								sysTable->addNextSymTab(*globalSymTab);

								currentSymTab = *globalSymTab; 
							}
#line 2305 "y.tab.cpp"
    break;

  case 5: /* program: units  */
#line 175 "promela.y"
                                                                                { /*DBUG("REDUCE: units -> program\n")*/}
#line 2311 "y.tab.cpp"
    break;

  case 6: /* units: unit  */
#line 178 "promela.y"
                                                                                { /*DBUG("REDUCE: unit -> units\n")*/ }
#line 2317 "y.tab.cpp"
    break;

  case 7: /* units: units unit  */
#line 179 "promela.y"
                                                                                { /*DBUG("REDUCE: units unit -> units\n")*/ }
#line 2323 "y.tab.cpp"
    break;

  case 8: /* unit: proc  */
#line 182 "promela.y"
                                                        { /*DBUG("REDUCE: proc -> unit\n")*/ *program = stmnt::merge(*program, (yyvsp[0].pStmntVal)); }
#line 2329 "y.tab.cpp"
    break;

  case 9: /* unit: init  */
#line 183 "promela.y"
                                                                { /*DBUG("REDUCE: init -> unit\n")*/ *program = stmnt::merge(*program, (yyvsp[0].pStmntVal)); }
#line 2335 "y.tab.cpp"
    break;

  case 10: /* unit: claim  */
#line 184 "promela.y"
                                                                { /*DBUG("REDUCE: never -> unit\n")*/ *program = stmnt::merge(*program, (yyvsp[0].pStmntVal)); }
#line 2341 "y.tab.cpp"
    break;

  case 11: /* unit: events  */
#line 185 "promela.y"
                                                                { assert(false); std::cout << "The 'events' construct is currently not supported."; }
#line 2347 "y.tab.cpp"
    break;

  case 12: /* unit: one_decl  */
#line 186 "promela.y"
                                                                { 
													/*DBUG("REDUCE: one_decl -> unit\n")*/
													stmnt* decl = nullptr;
													if (declSyms.front()->getType() == symbol::T_CHAN) 
														decl = new chanDecl(declSyms, nbrLines);
													else {
														assert(declSyms.front()->getType() != symbol::T_MTYPE_DEF && declSyms.front()->getType() != symbol::T_TDEF);
														decl = new varDecl(declSyms, nbrLines);
													}
													assert(decl);
													declSyms.clear();
													*program = stmnt::merge(*program, decl);
												}
#line 2365 "y.tab.cpp"
    break;

  case 13: /* unit: utypedef  */
#line 199 "promela.y"
                                                                { /*DBUG("REDUCE: utype -> unit\n")*/ *program = stmnt::merge(*program, (yyvsp[0].pStmntVal)); }
#line 2371 "y.tab.cpp"
    break;

  case 14: /* unit: mtypedef  */
#line 200 "promela.y"
                                                                                        { /*DBUG("REDUCE: mtype -> unit\n")*/ *program = stmnt::merge(*program, (yyvsp[0].pStmntVal)); }
#line 2377 "y.tab.cpp"
    break;

  case 15: /* unit: c_fcts  */
#line 201 "promela.y"
                                                                { std::cout << "Embedded C code is not supported."; 						}
#line 2383 "y.tab.cpp"
    break;

  case 16: /* unit: ns  */
#line 202 "promela.y"
                                                                { /*DBUG("REDUCE: ns -> unit\n")*/ *program = stmnt::merge(*program, (yyvsp[0].pStmntVal)); 	}
#line 2389 "y.tab.cpp"
    break;

  case 18: /* $@3: %empty  */
#line 209 "promela.y"
                                                                                                { 
													nameSpace = (yyvsp[0].sVal); savedSymTab = currentSymTab; 
													currentSymTab = currentSymTab->createSubTable(nameSpace); 
													auto predef = new pidSymNode(0, "_pid");
													predef->setMask(symbol::READ_ACCESS | symbol::PREDEFINED); 
													currentSymTab->insert(predef);
												}
#line 2401 "y.tab.cpp"
    break;

  case 19: /* $@4: %empty  */
#line 217 "promela.y"
                  { currentSymTab = savedSymTab; }
#line 2407 "y.tab.cpp"
    break;

  case 20: /* proc: inst proctype NAME $@3 '(' decl ')' $@4 Opt_priority Opt_enabler body  */
#line 220 "promela.y"
                                                                                        {	
		  											/*DBUG("REDUCE: inst proctype NAME ( decl ) prio ena body -> proc\n")*/
													auto procNbLine = (yyvsp[0].pStmntVal)->getLineNb();
		  											ptypeSymNode* proc = new ptypeSymNode((yyvsp[-8].sVal), (yyvsp[-10].pConstExprVal), declSyms, (yyvsp[0].pStmntVal), procNbLine);
		  											declSyms.clear();
		  											(yyval.pStmntVal) = new procDecl(proc, procNbLine);
		  											(*globalSymTab)->insert(proc);
		  											nameSpace = "global";
		  											free((yyvsp[-8].sVal));
		  										}
#line 2422 "y.tab.cpp"
    break;

  case 21: /* proctype: PROCTYPE  */
#line 232 "promela.y"
                                                                                { /*DBUG("REDUCE: -> PROCTYPE proctype\n")*/ }
#line 2428 "y.tab.cpp"
    break;

  case 22: /* proctype: D_PROCTYPE  */
#line 233 "promela.y"
                                                                                { std::cout << "Deterministic proctypes are not supported (only useful for simulation)."; }
#line 2434 "y.tab.cpp"
    break;

  case 23: /* inst: %empty  */
#line 236 "promela.y"
                                                                        { /*DBUG("REDUCE: void -> inst\n")*/ (yyval.pConstExprVal) = new exprConst(0, nbrLines); 	}
#line 2440 "y.tab.cpp"
    break;

  case 24: /* inst: ACTIVE  */
#line 237 "promela.y"
                                                                                        { /*DBUG("REDUCE: ACTIVE -> inst\n")*/ (yyval.pConstExprVal) = new exprConst(1, nbrLines); }
#line 2446 "y.tab.cpp"
    break;

  case 25: /* inst: ACTIVE '[' CONST ']'  */
#line 238 "promela.y"
                                                                        { /*DBUG("REDUCE: ACTIVE [ CONST ] -> inst \n")*/ (yyval.pConstExprVal) = new exprConst((yyvsp[-1].iVal), nbrLines); }
#line 2452 "y.tab.cpp"
    break;

  case 26: /* inst: ACTIVE '[' NAME ']'  */
#line 239 "promela.y"
                                                                        { 
													/*DBUG("REDUCE: ACTIVE [ NAME ] -> inst\n")*/
													varSymNode* var = *globalSymTab? static_cast<varSymNode*>((*globalSymTab)->lookup((yyvsp[-1].sVal))) : nullptr;
													if(var == nullptr) std::cout << "The variable "<<(yyvsp[-1].sVal)<<" does not exist.";
													else if(var->getType() != symbol::T_INT && var->getType() != symbol::T_BYTE && var->getType() != symbol::T_SHORT) std::cout << "The variable "<<(yyvsp[-1].sVal)<<" is not of type int, short or bit.";
													else if(var->getInitExpr() == nullptr || var->getInitExpr()->getType() != astNode::E_EXPR_CONST) std::cout << "The variable "<<(yyvsp[-1].sVal)<<" does not have a constant value.";
													else {
														(yyval.pConstExprVal) = new exprConst(static_cast<exprConst*>(var->getInitExpr())->getCstValue(), nbrLines);
													}
													free((yyvsp[-1].sVal));											
												}
#line 2468 "y.tab.cpp"
    break;

  case 27: /* $@5: %empty  */
#line 253 "promela.y"
                { nameSpace = "init"; }
#line 2474 "y.tab.cpp"
    break;

  case 28: /* init: INIT $@5 Opt_priority body  */
#line 255 "promela.y"
                                                                                        {	
													/*DBUG("REDUCE: INIT Opt_priority body -> init\n")*/
													if(*globalSymTab && (*globalSymTab)->lookup("init") != nullptr) 
														std::cout << "This is the second init process; only one is allowed.";
													else {
														initSymNode* init = new initSymNode(nbrLines, (yyvsp[0].pStmntVal));
														(yyval.pStmntVal) = new initDecl(init, nbrLines);
														(*globalSymTab)->insert(init);
													}
													nameSpace = "global";
												}
#line 2490 "y.tab.cpp"
    break;

  case 29: /* $@6: %empty  */
#line 269 "promela.y"
                { nameSpace = "never"; }
#line 2496 "y.tab.cpp"
    break;

  case 30: /* claim: CLAIM $@6 body  */
#line 270 "promela.y"
                                                                                        {
													/*DBUG("REDUCE: CLAIM body -> claim\n")*/
													neverSymNode* never = new neverSymNode(nbrLines, (yyvsp[0].pStmntVal));
													(yyval.pStmntVal) = new neverDecl(never, nbrLines);
													(*globalSymTab)->insert(never);
													nameSpace = "global";
												}
#line 2508 "y.tab.cpp"
    break;

  case 31: /* events: TRACE body  */
#line 278 "promela.y"
                                                                        { std::cout << "Event sequences (traces) are not supported."; }
#line 2514 "y.tab.cpp"
    break;

  case 32: /* utypedef: TYPEDEF NAME '{' decl_lst '}'  */
#line 281 "promela.y"
                                                        {	
													/*DBUG("REDUCE: TYPEDEF NAME '{' decl_lst '}' -> utype\n")*/

													for(auto declSym : declSyms)
														(*globalSymTab)->remove(declSym->getName());

													tdefSymNode* tdef = new tdefSymNode((yyvsp[-3].sVal), *globalSymTab, declSyms, nbrLines);
													declSyms.clear();

													(yyval.pStmntVal) = new tdefDecl(tdef, nbrLines);
													(*globalSymTab)->insert(tdef);
													free((yyvsp[-3].sVal));  
												}
#line 2532 "y.tab.cpp"
    break;

  case 33: /* $@7: %empty  */
#line 296 "promela.y"
                                                                        {	mtypeDef = new mtypedefSymNode(nbrLines);	}
#line 2538 "y.tab.cpp"
    break;

  case 34: /* mtypedef: vis TYPE asgn $@7 '{' nlst '}'  */
#line 297 "promela.y"
                                                                                {
													assert(mtypeDef->getMTypeList().size() != 0);
													(*globalSymTab)->insert(mtypeDef);

													/*DBUG("REDUCE: vis TYPE asgn { nlst } -> one_decl\n")*/
													if((yyvsp[-5].iType) != symbol::T_MTYPE) {
														std::cout <<  "This syntax only works for MTYPEs definition.";
														exit(1);
													}
													(yyval.pStmntVal) = new mtypeDecl(mtypeDef, nbrLines);
													// The mtype values are added in the nlst rule.
												}
#line 2555 "y.tab.cpp"
    break;

  case 35: /* $@8: %empty  */
#line 315 "promela.y"
                         { inInline = true; }
#line 2561 "y.tab.cpp"
    break;

  case 36: /* $@9: %empty  */
#line 316 "promela.y"
                       { nameSpace = (yyvsp[0].sVal); savedSymTab = currentSymTab; currentSymTab = currentSymTab->createSubTable(nameSpace); }
#line 2567 "y.tab.cpp"
    break;

  case 37: /* $@10: %empty  */
#line 318 "promela.y"
                  { for(std::string it : params) 
		  		currentSymTab->insert(varSymNode::createSymbol(symbol::T_NA, nbrLines, it));
		    currentSymTab = savedSymTab;
		  }
#line 2576 "y.tab.cpp"
    break;

  case 38: /* ns: INLINE $@8 NAME $@9 '(' param_list ')' $@10 body  */
#line 322 "promela.y"
                                                                                        {
													/*DBUG("REDUCE: INLINE nm ( param_list ) body -> ns\n")*/
													auto sym = new inlineSymNode((yyvsp[-6].sVal), params, (yyvsp[0].pStmntVal), nbrLines);
													params.clear();
		  											(yyval.pStmntVal) = new inlineDecl(sym, nbrLines);
													(*globalSymTab)->insert(sym);
													inInline = false;
													free((yyvsp[-6].sVal));
												}
#line 2590 "y.tab.cpp"
    break;

  case 48: /* $@11: %empty  */
#line 350 "promela.y"
                                                                                { 
													savedSymTab = currentSymTab; 
													if(!(currentSymTab = (*globalSymTab)->getSubSymTab(nameSpace)))
														currentSymTab = savedSymTab->createSubTable(nameSpace); 
													nameSpace = "";
												}
#line 2601 "y.tab.cpp"
    break;

  case 49: /* body: '{' $@11 sequence OS '}'  */
#line 357 "promela.y"
                                                                                        { /*DBUG("REDUCE: '{' sequence OS '}' -> body\n")*/ (yyval.pStmntVal) = (yyvsp[-2].pStmntVal); (yyval.pStmntVal)->setLocalSymTab(currentSymTab); currentSymTab->setBlock((yyvsp[-2].pStmntVal)); currentSymTab = savedSymTab; }
#line 2607 "y.tab.cpp"
    break;

  case 50: /* sequence: step  */
#line 360 "promela.y"
                                                                                { /*DBUG("REDUCE: step -> sequence\n")*/ (yyval.pStmntVal) = (yyvsp[0].pStmntVal);  }
#line 2613 "y.tab.cpp"
    break;

  case 51: /* sequence: sequence MS step  */
#line 361 "promela.y"
                                                                                { /*DBUG("REDUCE: sequence MS step -> sequence\n")*/ (yyval.pStmntVal) = stmnt::merge((yyvsp[-2].pStmntVal), (yyvsp[0].pStmntVal)); }
#line 2619 "y.tab.cpp"
    break;

  case 52: /* sequence: sequence step  */
#line 362 "promela.y"
                                                                                { /*DBUG("REDUCE: sequence step -> sequence\n")*/ (yyval.pStmntVal) = stmnt::merge((yyvsp[-1].pStmntVal), (yyvsp[0].pStmntVal)); }
#line 2625 "y.tab.cpp"
    break;

  case 53: /* step: one_decl  */
#line 365 "promela.y"
                                                                                { 
													assert(declSyms.front()->getType() != symbol::T_MTYPE_DEF); 
												 	(yyval.pStmntVal) = new varDecl(static_cast<std::list<varSymNode*>>(declSyms), nbrLines);
												 	declSyms.clear();
												}
#line 2635 "y.tab.cpp"
    break;

  case 54: /* step: NAME ':' one_decl  */
#line 370 "promela.y"
                                                                                { std::cout << "Declarations with labels are not suported."; }
#line 2641 "y.tab.cpp"
    break;

  case 55: /* step: NAME ':' XU  */
#line 371 "promela.y"
                                                                                { std::cout << "Channel assertions are currently not supported."; }
#line 2647 "y.tab.cpp"
    break;

  case 56: /* step: stmnt  */
#line 372 "promela.y"
                                                                                        { /*DBUG("REDUCE: stmnt -> step\n")*/ (yyval.pStmntVal) = (yyvsp[0].pStmntVal); }
#line 2653 "y.tab.cpp"
    break;

  case 57: /* step: stmnt UNLESS stmnt  */
#line 373 "promela.y"
                                                                        { std::cout << "Unless statements are currently not supported."; }
#line 2659 "y.tab.cpp"
    break;

  case 59: /* vis: HIDDEN  */
#line 382 "promela.y"
                                                                                        { std::cout << "The 'hidden' keyword is not supported."; }
#line 2665 "y.tab.cpp"
    break;

  case 60: /* vis: SHOW  */
#line 383 "promela.y"
                                                                                        { std::cout << "The 'show' keyword is not supported."; }
#line 2671 "y.tab.cpp"
    break;

  case 61: /* vis: ISLOCAL  */
#line 384 "promela.y"
                                                                                        { std::cout << "The 'local' keyword is not supported."; }
#line 2677 "y.tab.cpp"
    break;

  case 64: /* $@12: %empty  */
#line 394 "promela.y"
                   { declType = (yyvsp[0].iType); }
#line 2683 "y.tab.cpp"
    break;

  case 65: /* one_decl: vis TYPE $@12 var_list  */
#line 394 "promela.y"
                                                { /*DBUG("REDUCE: vis TYPE var_list -> one_decl\n")*/ }
#line 2689 "y.tab.cpp"
    break;

  case 66: /* $@13: %empty  */
#line 395 "promela.y"
                            { declType = symbol::T_UTYPE; typeDef = *globalSymTab? static_cast<tdefSymNode*>((*globalSymTab)->lookup((yyvsp[0].sVal))) : nullptr; assert(typeDef); }
#line 2695 "y.tab.cpp"
    break;

  case 67: /* one_decl: vis UNAME $@13 var_list  */
#line 395 "promela.y"
                                                                                                                                                                                                        { /*DBUG("REDUCE: vis UNAME var_list -> one_decl\n")*/ free((yyvsp[-2].sVal)); }
#line 2701 "y.tab.cpp"
    break;

  case 68: /* decl_lst: one_decl  */
#line 398 "promela.y"
                                                                                { /*DBUG("REDUCE: one_decl -> decl_list\n")*/ }
#line 2707 "y.tab.cpp"
    break;

  case 69: /* decl_lst: one_decl SEMI decl_lst  */
#line 399 "promela.y"
                                                                        { /*DBUG("REDUCE: one_decl SEMI decl_list -> decl_lst\n")*/ }
#line 2713 "y.tab.cpp"
    break;

  case 70: /* decl: %empty  */
#line 403 "promela.y"
                                                                        { /*DBUG("REDUCE: void -> decl\n")*/ }
#line 2719 "y.tab.cpp"
    break;

  case 71: /* decl: decl_lst  */
#line 404 "promela.y"
                                                                                        { /*DBUG("REDUCE: decl_list -> decl\n")*/ }
#line 2725 "y.tab.cpp"
    break;

  case 72: /* var_list: ivar  */
#line 411 "promela.y"
                                                                                { /*DBUG("REDUCE: ivar -> var_list\n")*/ currentSymTab->insert((yyvsp[0].pVarSymVal)); declSyms.push_front((yyvsp[0].pVarSymVal)); }
#line 2731 "y.tab.cpp"
    break;

  case 73: /* var_list: ivar ',' var_list  */
#line 412 "promela.y"
                                                                                { /*DBUG("REDUCE: ivar , var_list -> var_list\n")*/ currentSymTab->insert((yyvsp[-2].pVarSymVal)); declSyms.push_front((yyvsp[-2].pVarSymVal)); }
#line 2737 "y.tab.cpp"
    break;

  case 74: /* ivar: vardcl  */
#line 415 "promela.y"
                                                                                { 
												  /*DBUG("REDUCE: var_decl -> ivar\n")*/ (yyval.pVarSymVal) = varSymNode::createSymbol(declType, nbrLines, (yyvsp[0].pDataVal).sVal, (yyvsp[0].pDataVal).iVal); 
												  if(declType == symbol::T_UTYPE) { assert(typeDef); static_cast<utypeSymNode*>((yyval.pVarSymVal))->setUType(typeDef); }
												  if((yyvsp[0].pDataVal).sVal) free((yyvsp[0].pDataVal).sVal);
												}
#line 2747 "y.tab.cpp"
    break;

  case 75: /* ivar: vardcl ASGN expr  */
#line 420 "promela.y"
                                                                                { /*DBUG("REDUCE: var_decl ASGN expr -> ivar\n")*/ 
												  (yyval.pVarSymVal) = varSymNode::createSymbol(declType, nbrLines, (yyvsp[-2].pDataVal).sVal, (yyvsp[-2].pDataVal).iVal, (yyvsp[0].pExprVal)); 
												  if(declType == symbol::T_UTYPE) { assert(typeDef); static_cast<utypeSymNode*>((yyval.pVarSymVal))->setUType(typeDef); }
												  if((yyvsp[-2].pDataVal).sVal) free((yyvsp[-2].pDataVal).sVal);
												}
#line 2757 "y.tab.cpp"
    break;

  case 76: /* ivar: vardcl ASGN ch_init  */
#line 425 "promela.y"
                                                                        { /*DBUG("REDUCE: var_decl ASGN ch_init -> ivar\n")*/ (yyval.pVarSymVal) = new chanSymNode(nbrLines, (yyvsp[-2].pDataVal).sVal, (yyvsp[-2].pDataVal).iVal, (yyvsp[0].pDataVal).iVal, typeLst);	
												  typeLst.clear(); if((yyvsp[-2].pDataVal).sVal) free((yyvsp[-2].pDataVal).sVal); //double free???if($3.sVal) free($3.sVal); 
												}
#line 2765 "y.tab.cpp"
    break;

  case 77: /* param_list: %empty  */
#line 430 "promela.y"
                                                                                        { }
#line 2771 "y.tab.cpp"
    break;

  case 78: /* param_list: NAME  */
#line 431 "promela.y"
                                                                                        { params.push_front(std::string((yyvsp[0].sVal))); free((yyvsp[0].sVal)); }
#line 2777 "y.tab.cpp"
    break;

  case 79: /* param_list: NAME ',' param_list  */
#line 432 "promela.y"
                                                            { params.push_front(std::string((yyvsp[-2].sVal))); free((yyvsp[-2].sVal)); }
#line 2783 "y.tab.cpp"
    break;

  case 80: /* ch_init: '[' CONST ']' OF '{' typ_list '}'  */
#line 436 "promela.y"
                                                        { /*DBUG("REDUCE: [ CONST ] OF { typ_list } -> ch_init\n")*/ (yyval.pDataVal).iVal = (yyvsp[-5].iVal); }
#line 2789 "y.tab.cpp"
    break;

  case 81: /* basetype: TYPE  */
#line 439 "promela.y"
                                                                                { (yyval.pDataVal).sVal = nullptr; (yyval.pDataVal).iType = (yyvsp[0].iType); }
#line 2795 "y.tab.cpp"
    break;

  case 82: /* basetype: UNAME  */
#line 440 "promela.y"
                                                                                        { (yyval.pDataVal).sVal = (yyvsp[0].sVal); (yyval.pDataVal).iType = symbol::T_UTYPE; }
#line 2801 "y.tab.cpp"
    break;

  case 83: /* typ_list: basetype  */
#line 444 "promela.y"
                                                                                {	/*DBUG("REDUCE: basetype -> typ_list\n")*/
													varSymNode* typ = nullptr;
													if((yyvsp[0].pDataVal).iType != symbol::T_UTYPE && (yyvsp[0].pDataVal).iType != symbol::T_NA) {
														typ = varSymNode::createSymbol((yyvsp[0].pDataVal).iType, nbrLines);
													} else {
														tdefSymNode* pType = *globalSymTab ? dynamic_cast<tdefSymNode*>((*globalSymTab)->lookup((yyvsp[0].pDataVal).sVal)) : nullptr;
														typ = new utypeSymNode(pType, nbrLines);
														if(typ == nullptr) {
															std::cout << "The type "<<(yyvsp[0].pDataVal).sVal<<" was not declared in a typedef.\n";
															assert(false);
														}
													}
													typeLst.push_back(typ);
													if((yyvsp[0].pDataVal).sVal) free((yyvsp[0].pDataVal).sVal);
												}
#line 2821 "y.tab.cpp"
    break;

  case 84: /* typ_list: basetype ',' typ_list  */
#line 459 "promela.y"
                                                                        {	/*DBUG("REDUCE: basetype , typ_list -> typ_list\n")*/
													varSymNode* typ = nullptr;
													if((yyvsp[-2].pDataVal).iType != symbol::T_UTYPE && (yyvsp[-2].pDataVal).iType != symbol::T_NA) {
														typ = varSymNode::createSymbol((yyvsp[-2].pDataVal).iType, nbrLines);
													} else {
														tdefSymNode* pType = *globalSymTab ? dynamic_cast<tdefSymNode*>((*globalSymTab)->lookup((yyvsp[-2].pDataVal).sVal)) : nullptr;
														typ = new utypeSymNode(pType, nbrLines);
														if(typ == nullptr) {
															std::cout << "The type "<<(yyvsp[-2].pDataVal).sVal<<" was not declared in a typedef.\n";
															assert(false);
														}
													}
													typeLst.push_front(typ);
													if((yyvsp[-2].pDataVal).sVal) free((yyvsp[-2].pDataVal).sVal);
												}
#line 2841 "y.tab.cpp"
    break;

  case 85: /* vardcl: NAME  */
#line 476 "promela.y"
                                                                                { /*/*DBUG("REDUCE: NAME -> vardcl\n"*)*/ (yyval.pDataVal).sVal = (yyvsp[0].sVal); (yyval.pDataVal).iVal = 1; }
#line 2847 "y.tab.cpp"
    break;

  case 86: /* vardcl: NAME ':' CONST  */
#line 477 "promela.y"
                                                                                { std::cout << "The 'unsigned' data type is not supported."; }
#line 2853 "y.tab.cpp"
    break;

  case 87: /* vardcl: NAME '[' CONST ']'  */
#line 478 "promela.y"
                                                                        { /*DBUG("REDUCE: NAME [ CONST ] -> vardcl\n")*/ (yyval.pDataVal).sVal = (yyvsp[-3].sVal); (yyval.pDataVal).iVal = (yyvsp[-1].iVal); }
#line 2859 "y.tab.cpp"
    break;

  case 88: /* varref: cmpnd  */
#line 481 "promela.y"
                                                                                { /*DBUG("REDUCE: cmpnd -> varref\n")*/ (yyval.pExprVarRefVal) = (yyvsp[0].pExprVarRefVal); symbol* sym = nullptr; if(!inInline) sym = (yyval.pExprVarRefVal)->resolve(currentSymTab); assert(sym || inInline); }
#line 2865 "y.tab.cpp"
    break;

  case 89: /* pfld: NAME  */
#line 484 "promela.y"
                                                                                { /*DBUG("REDUCE: NAME -> pfld\n")*/ (yyval.pExprVarRefNameVal) = new exprVarRefName((yyvsp[0].sVal), nbrLines); free((yyvsp[0].sVal)); }
#line 2871 "y.tab.cpp"
    break;

  case 90: /* pfld: NAME '[' expr ']'  */
#line 485 "promela.y"
                                                                                { /*DBUG("REDUCE: NAME [ expr ] -> pfld\n")*/ (yyval.pExprVarRefNameVal) = new exprVarRefName((yyvsp[-3].sVal), (yyvsp[-1].pExprVal), nbrLines); free((yyvsp[-3].sVal)); }
#line 2877 "y.tab.cpp"
    break;

  case 91: /* cmpnd: pfld sfld  */
#line 488 "promela.y"
                                                                                { /*DBUG("REDUCE: pfld sfld -> cmpnd\n")*/ (yyval.pExprVarRefVal) = new exprVarRef(nbrLines, (yyvsp[-1].pExprVarRefNameVal), (yyvsp[0].pExprVarRefVal)); }
#line 2883 "y.tab.cpp"
    break;

  case 92: /* cmpnd: CONTEXT '.' pfld sfld  */
#line 489 "promela.y"
                                                                        { /*DBUG("REDUCE: CONTEX . pfld sfld -> cmpnd\n")*/ (yyval.pExprVarRefVal) = new exprVarRef(nbrLines, (yyvsp[-1].pExprVarRefNameVal), (yyvsp[0].pExprVarRefVal)); }
#line 2889 "y.tab.cpp"
    break;

  case 93: /* sfld: %empty  */
#line 492 "promela.y"
                                                                                        { /*DBUG("REDUCE: void -> sfld\n")*/ (yyval.pExprVarRefVal) = nullptr; }
#line 2895 "y.tab.cpp"
    break;

  case 94: /* sfld: '.' cmpnd  */
#line 493 "promela.y"
                                                                        { /*DBUG("REDUCE: . cmpnd -> sfld\n")*/ (yyval.pExprVarRefVal) = (yyvsp[0].pExprVarRefVal);   }
#line 2901 "y.tab.cpp"
    break;

  case 95: /* stmnt: Special  */
#line 497 "promela.y"
                                                                                { /*DBUG("REDUCE: special -> stmnt\n")*/ (yyval.pStmntVal) = (yyvsp[0].pStmntVal); }
#line 2907 "y.tab.cpp"
    break;

  case 96: /* stmnt: Stmnt  */
#line 498 "promela.y"
                                                                                        { /*DBUG("REDUCE: Stmnt -> stmnt\n")*/ (yyval.pStmntVal) = (yyvsp[0].pStmntVal); }
#line 2913 "y.tab.cpp"
    break;

  case 97: /* Special: varref RCV rargs  */
#line 501 "promela.y"
                                                                        { (yyval.pStmntVal) = new stmntChanRecv((yyvsp[-2].pExprVarRefVal), (yyvsp[0].pExprRArgListVal), nbrLines); }
#line 2919 "y.tab.cpp"
    break;

  case 98: /* Special: varref SND margs  */
#line 502 "promela.y"
                                                                                { (yyval.pStmntVal) = new stmntChanSnd((yyvsp[-2].pExprVarRefVal), (yyvsp[0].pExprArgListVal), nbrLines); }
#line 2925 "y.tab.cpp"
    break;

  case 99: /* Special: IF options FI  */
#line 503 "promela.y"
                                                                                { (yyval.pStmntVal) = new stmntIf((yyvsp[-1].pStmntOptVal), (yyvsp[-2].iVal)); }
#line 2931 "y.tab.cpp"
    break;

  case 100: /* Special: DO options OD  */
#line 504 "promela.y"
                                                                                { (yyval.pStmntVal) = new stmntDo((yyvsp[-1].pStmntOptVal), (yyvsp[-2].iVal)); }
#line 2937 "y.tab.cpp"
    break;

  case 101: /* Special: AC options CA  */
#line 505 "promela.y"
                                                                                { (yyval.pStmntVal) = new stmntIf((yyvsp[-1].pStmntOptVal), (yyvsp[-2].iVal)); }
#line 2943 "y.tab.cpp"
    break;

  case 102: /* Special: BREAK  */
#line 506 "promela.y"
                                                                                        { (yyval.pStmntVal) = new stmntBreak(nbrLines); }
#line 2949 "y.tab.cpp"
    break;

  case 103: /* Special: GOTO NAME  */
#line 507 "promela.y"
                                                                                        { (yyval.pStmntVal) = new stmntGoto((yyvsp[0].sVal), nbrLines); free((yyvsp[0].sVal)); }
#line 2955 "y.tab.cpp"
    break;

  case 104: /* Special: NAME ':' stmnt  */
#line 508 "promela.y"
                                                                                { if((yyvsp[0].pStmntVal)->getType() == astNode::E_STMNT_LABEL && static_cast<stmntLabel*>((yyvsp[0].pStmntVal))->getLabelled()->getType() == astNode::E_STMNT_LABEL) 
													std::cout << "Only two labels per state are supported."; 
												  (yyval.pStmntVal) = new stmntLabel((yyvsp[-2].sVal), (yyvsp[0].pStmntVal), nbrLines); assert(labelsMap.find((yyvsp[-2].sVal)) == labelsMap.end()); labelsMap[(yyvsp[-2].sVal)] = dynamic_cast<stmntLabel*>((yyval.pStmntVal)); free((yyvsp[-2].sVal)); }
#line 2963 "y.tab.cpp"
    break;

  case 105: /* Stmnt: varref ASGN full_expr  */
#line 512 "promela.y"
                                                                { (yyval.pStmntVal) = new stmntAsgn((yyvsp[-2].pExprVarRefVal), (yyvsp[0].pExprVal), nbrLines); }
#line 2969 "y.tab.cpp"
    break;

  case 106: /* Stmnt: varref INCR  */
#line 513 "promela.y"
                                                                                { (yyval.pStmntVal) = new stmntIncr((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 2975 "y.tab.cpp"
    break;

  case 107: /* Stmnt: varref DECR  */
#line 514 "promela.y"
                                                                                { (yyval.pStmntVal) = new stmntDecr((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 2981 "y.tab.cpp"
    break;

  case 108: /* Stmnt: PRINT '(' STRING prargs ')'  */
#line 515 "promela.y"
                                                                { (yyval.pStmntVal) = new stmntPrint((yyvsp[-2].sVal), (yyvsp[-1].pExprArgListVal), nbrLines); }
#line 2987 "y.tab.cpp"
    break;

  case 109: /* Stmnt: PRINTM '(' varref ')'  */
#line 516 "promela.y"
                                                                        { (yyval.pStmntVal) = new stmntPrintm((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 2993 "y.tab.cpp"
    break;

  case 110: /* Stmnt: PRINTM '(' CONST ')'  */
#line 517 "promela.y"
                                                                        { (yyval.pStmntVal) = new stmntPrintm((yyvsp[-1].iVal), nbrLines); }
#line 2999 "y.tab.cpp"
    break;

  case 111: /* Stmnt: ASSERT full_expr  */
#line 518 "promela.y"
                                                                                { (yyval.pStmntVal) = new stmntAssert((yyvsp[0].pExprVal), nbrLines); }
#line 3005 "y.tab.cpp"
    break;

  case 112: /* Stmnt: ccode  */
#line 519 "promela.y"
                                                                                        { std::cout << "Embedded C code is not supported."; }
#line 3011 "y.tab.cpp"
    break;

  case 113: /* Stmnt: varref R_RCV rargs  */
#line 520 "promela.y"
                                                                        { std::cout << "Sorted send and random receive are not supported."; }
#line 3017 "y.tab.cpp"
    break;

  case 114: /* Stmnt: varref RCV LT rargs GT  */
#line 521 "promela.y"
                                                                        { std::cout << "Channel poll operations are not supported."; }
#line 3023 "y.tab.cpp"
    break;

  case 115: /* Stmnt: varref R_RCV LT rargs GT  */
#line 522 "promela.y"
                                                                        { std::cout << "Channel poll operations are not supported."; }
#line 3029 "y.tab.cpp"
    break;

  case 116: /* Stmnt: varref O_SND margs  */
#line 523 "promela.y"
                                                                        { std::cout << "Sorted send and random receive are not supported."; }
#line 3035 "y.tab.cpp"
    break;

  case 117: /* Stmnt: full_expr  */
#line 524 "promela.y"
                                                                                        { (yyval.pStmntVal) = new stmntExpr((yyvsp[0].pExprVal), nbrLines); }
#line 3041 "y.tab.cpp"
    break;

  case 118: /* Stmnt: ELSE  */
#line 525 "promela.y"
                                                                                        { (yyval.pStmntVal) = new stmntElse(nbrLines); }
#line 3047 "y.tab.cpp"
    break;

  case 119: /* Stmnt: ATOMIC '{' sequence OS '}'  */
#line 526 "promela.y"
                                                                { (yyval.pStmntVal) = new stmntAtomic((yyvsp[-2].pStmntVal), nbrLines); }
#line 3053 "y.tab.cpp"
    break;

  case 120: /* Stmnt: D_STEP '{' sequence OS '}'  */
#line 527 "promela.y"
                                                                { (yyval.pStmntVal) = new stmntDStep((yyvsp[-2].pStmntVal), nbrLines); }
#line 3059 "y.tab.cpp"
    break;

  case 121: /* Stmnt: '{' sequence OS '}'  */
#line 528 "promela.y"
                                                                        { (yyval.pStmntVal) = new stmntSeq((yyvsp[-2].pStmntVal), nbrLines); }
#line 3065 "y.tab.cpp"
    break;

  case 122: /* Stmnt: INAME '(' args ')'  */
#line 529 "promela.y"
                                                                        { 
													(yyval.pStmntVal) = new stmntCall((yyvsp[-3].sVal), (yyvsp[-1].pExprArgListVal), nbrLines); 
													auto fctSym = (*globalSymTab)->lookup((yyvsp[-3].sVal));
													std::cout << "Inline call "<< (yyvsp[-3].sVal) <<" at line "<< nbrLines <<"\n";
													assert(fctSym->getType() == symbol::T_INLINE);
													assert(dynamic_cast<inlineSymNode*>(fctSym) != nullptr);
													if((yyvsp[-1].pExprArgListVal))
														assert(dynamic_cast<inlineSymNode*>(fctSym)->getParams().size() == (yyvsp[-1].pExprArgListVal)->getSize());
													free((yyvsp[-3].sVal)); 
												}
#line 3080 "y.tab.cpp"
    break;

  case 123: /* Stmnt: NAME SEP  */
#line 539 "promela.y"
                                                                                        { (yyval.pStmntVal) = new stmntAction((yyvsp[-1].sVal), nbrLines); }
#line 3086 "y.tab.cpp"
    break;

  case 124: /* options: option  */
#line 542 "promela.y"
                                                                                { (yyval.pStmntOptVal) = new stmntOpt((yyvsp[0].pStmntVal), nbrLines); }
#line 3092 "y.tab.cpp"
    break;

  case 125: /* options: option options  */
#line 543 "promela.y"
                                                                                { (yyval.pStmntOptVal) = new stmntOpt((yyvsp[-1].pStmntVal), (yyvsp[0].pStmntOptVal), nbrLines); }
#line 3098 "y.tab.cpp"
    break;

  case 126: /* option: SEP sequence OS  */
#line 546 "promela.y"
                                                                        { (yyval.pStmntVal) = (yyvsp[-1].pStmntVal); }
#line 3104 "y.tab.cpp"
    break;

  case 127: /* option: SEP '[' real_expr ']' sequence OS  */
#line 547 "promela.y"
                                                                { (yyval.pStmntVal) = (yyvsp[-1].pStmntVal); (yyvsp[-1].pStmntVal)->setProb((yyvsp[-3].rVal)); }
#line 3110 "y.tab.cpp"
    break;

  case 128: /* real_expr: '(' real_expr ')'  */
#line 551 "promela.y"
                                                                                        { (yyval.rVal) = (yyvsp[-1].rVal); }
#line 3116 "y.tab.cpp"
    break;

  case 129: /* real_expr: real_expr '+' real_expr  */
#line 552 "promela.y"
                                                                                        { (yyval.rVal) = (yyvsp[-2].rVal) + (yyvsp[0].rVal); }
#line 3122 "y.tab.cpp"
    break;

  case 130: /* real_expr: real_expr '-' real_expr  */
#line 553 "promela.y"
                                                                                        { (yyval.rVal) = (yyvsp[-2].rVal) - (yyvsp[0].rVal); }
#line 3128 "y.tab.cpp"
    break;

  case 131: /* real_expr: real_expr '*' real_expr  */
#line 554 "promela.y"
                                                                    { (yyval.rVal) = (yyvsp[-2].rVal) * (yyvsp[0].rVal); }
#line 3134 "y.tab.cpp"
    break;

  case 132: /* real_expr: real_expr '/' real_expr  */
#line 555 "promela.y"
                                                                    { (yyval.rVal) = (yyvsp[-2].rVal) / (yyvsp[0].rVal); }
#line 3140 "y.tab.cpp"
    break;

  case 133: /* real_expr: REAL  */
#line 556 "promela.y"
                                                                                                        { (yyval.rVal) = (yyvsp[0].rVal);}
#line 3146 "y.tab.cpp"
    break;

  case 134: /* real_expr: CONST  */
#line 557 "promela.y"
                                                                                                        { (yyval.rVal) = (yyvsp[0].iVal);}
#line 3152 "y.tab.cpp"
    break;

  case 136: /* OS: SEMI  */
#line 561 "promela.y"
                                        { /* redundant semi at end of sequence */ }
#line 3158 "y.tab.cpp"
    break;

  case 137: /* MS: SEMI  */
#line 564 "promela.y"
                                        { /* at least one semi-colon */ }
#line 3164 "y.tab.cpp"
    break;

  case 138: /* MS: MS SEMI  */
#line 565 "promela.y"
                                        { /* but more are okay too   */ }
#line 3170 "y.tab.cpp"
    break;

  case 139: /* aname: NAME  */
#line 568 "promela.y"
                                                                                { (yyval.sVal) = (yyvsp[0].sVal); }
#line 3176 "y.tab.cpp"
    break;

  case 140: /* aname: PNAME  */
#line 569 "promela.y"
                                                                                        { (yyval.sVal) = (yyvsp[0].sVal); }
#line 3182 "y.tab.cpp"
    break;

  case 141: /* expr: '(' expr ')'  */
#line 572 "promela.y"
                                                                        { (yyval.pExprVal) = new exprPar		((yyvsp[-1].pExprVal), nbrLines); }
#line 3188 "y.tab.cpp"
    break;

  case 142: /* expr: expr '+' expr  */
#line 573 "promela.y"
                                                                                { (yyval.pExprVal) = new exprPlus		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3194 "y.tab.cpp"
    break;

  case 143: /* expr: expr '-' expr  */
#line 574 "promela.y"
                                                                                { (yyval.pExprVal) = new exprMinus	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3200 "y.tab.cpp"
    break;

  case 144: /* expr: expr '*' expr  */
#line 575 "promela.y"
                                                                                { (yyval.pExprVal) = new exprTimes	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3206 "y.tab.cpp"
    break;

  case 145: /* expr: expr '/' expr  */
#line 576 "promela.y"
                                                                                { (yyval.pExprVal) = new exprDiv		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3212 "y.tab.cpp"
    break;

  case 146: /* expr: expr '%' expr  */
#line 577 "promela.y"
                                                                                { (yyval.pExprVal) = new exprMod		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3218 "y.tab.cpp"
    break;

  case 147: /* expr: expr '&' expr  */
#line 578 "promela.y"
                                                                                { (yyval.pExprVal) = new exprBitwAnd	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3224 "y.tab.cpp"
    break;

  case 148: /* expr: expr '^' expr  */
#line 579 "promela.y"
                                                                                { (yyval.pExprVal) = new exprBitwXor	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3230 "y.tab.cpp"
    break;

  case 149: /* expr: expr '|' expr  */
#line 580 "promela.y"
                                                                                { (yyval.pExprVal) = new exprBitwOr	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3236 "y.tab.cpp"
    break;

  case 150: /* expr: expr GT expr  */
#line 581 "promela.y"
                                                                                { (yyval.pExprVal) = new exprGT		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3242 "y.tab.cpp"
    break;

  case 151: /* expr: expr LT expr  */
#line 582 "promela.y"
                                                                                { (yyval.pExprVal) = new exprLT		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3248 "y.tab.cpp"
    break;

  case 152: /* expr: expr GE expr  */
#line 583 "promela.y"
                                                                                { (yyval.pExprVal) = new exprGE		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3254 "y.tab.cpp"
    break;

  case 153: /* expr: expr LE expr  */
#line 584 "promela.y"
                                                                                { (yyval.pExprVal) = new exprLE		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3260 "y.tab.cpp"
    break;

  case 154: /* expr: expr EQ expr  */
#line 585 "promela.y"
                                                                                { (yyval.pExprVal) = new exprEQ		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3266 "y.tab.cpp"
    break;

  case 155: /* expr: expr NE expr  */
#line 586 "promela.y"
                                                                                { (yyval.pExprVal) = new exprNE		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3272 "y.tab.cpp"
    break;

  case 156: /* expr: expr AND expr  */
#line 587 "promela.y"
                                                                                { (yyval.pExprVal) = new exprAnd		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3278 "y.tab.cpp"
    break;

  case 157: /* expr: expr OR expr  */
#line 588 "promela.y"
                                                                                { (yyval.pExprVal) = new exprOr		((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3284 "y.tab.cpp"
    break;

  case 158: /* expr: expr LSHIFT expr  */
#line 589 "promela.y"
                                                                                { (yyval.pExprVal) = new exprLShift	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3290 "y.tab.cpp"
    break;

  case 159: /* expr: expr RSHIFT expr  */
#line 590 "promela.y"
                                                                                { (yyval.pExprVal) = new exprRShift	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3296 "y.tab.cpp"
    break;

  case 160: /* expr: COUNT '(' expr ')'  */
#line 591 "promela.y"
                                                                        { (yyval.pExprVal) = new exprCount	((yyvsp[-1].pExprVal), nbrLines); }
#line 3302 "y.tab.cpp"
    break;

  case 161: /* expr: '~' expr  */
#line 592 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprBitwNeg	((yyvsp[0].pExprVal), nbrLines); }
#line 3308 "y.tab.cpp"
    break;

  case 162: /* expr: '-' expr  */
#line 593 "promela.y"
                                                                        { 	if((yyvsp[0].pExprVal)->getType() != astNode::E_EXPR_CONST) 
														(yyval.pExprVal) = new exprUMin((yyvsp[0].pExprVal), nbrLines);
													else {
														exprConst* tmp = dynamic_cast<exprConst*>((yyvsp[0].pExprVal));
														(yyval.pExprVal) = new exprConst(- tmp->getCstValue(), nbrLines);
														delete tmp;
													}
												}
#line 3321 "y.tab.cpp"
    break;

  case 163: /* expr: SND expr  */
#line 601 "promela.y"
                                                                        { (yyval.pExprVal) = new exprNeg	((yyvsp[0].pExprVal), nbrLines); }
#line 3327 "y.tab.cpp"
    break;

  case 164: /* expr: '(' expr SEMI expr ':' expr ')'  */
#line 602 "promela.y"
                                                                { (yyval.pExprVal) = new exprCond	((yyvsp[-5].pExprVal), (yyvsp[-3].pExprVal), (yyvsp[-1].pExprVal), nbrLines); }
#line 3333 "y.tab.cpp"
    break;

  case 165: /* expr: RUN aname '(' args ')' Opt_priority  */
#line 603 "promela.y"
                                                        { auto run = new exprRun ((yyvsp[-4].sVal), (yyvsp[-2].pExprArgListVal), nbrLines);
												  (yyval.pExprVal) = run;
												  auto procSym = run->resolve(*globalSymTab); 
												  assert(procSym); free((yyvsp[-4].sVal)); 
												}
#line 3343 "y.tab.cpp"
    break;

  case 166: /* expr: LEN '(' varref ')'  */
#line 608 "promela.y"
                                                                        { (yyval.pExprVal) = new exprLen	((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 3349 "y.tab.cpp"
    break;

  case 167: /* expr: ENABLED '(' expr ')'  */
#line 609 "promela.y"
                                                                        { std::cout << "The enabled keyword is not supported."; }
#line 3355 "y.tab.cpp"
    break;

  case 168: /* expr: varref RCV '[' rargs ']'  */
#line 610 "promela.y"
                                                                        { std::cout << "Construct not supported."; /* Unclear */ }
#line 3361 "y.tab.cpp"
    break;

  case 169: /* expr: varref R_RCV '[' rargs ']'  */
#line 611 "promela.y"
                                                                { std::cout << "Sorted send and random receive are not supported."; }
#line 3367 "y.tab.cpp"
    break;

  case 170: /* expr: varref '{' varref '}'  */
#line 612 "promela.y"
                                                                        { (yyval.pExprVal) = new exprProjVar((yyvsp[-3].pExprVarRefVal), (yyvsp[-1].pExprVarRefVal), nbrLines); 
													assert((yyvsp[-3].pExprVarRefVal)->getFinalSymbol()->getType() != symbol::T_VARIANT && (yyvsp[-1].pExprVarRefVal)->getFinalSymbol()->getType() == symbol::T_VARIANT) ; 
												}
#line 3375 "y.tab.cpp"
    break;

  case 171: /* expr: varref  */
#line 615 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprVar	((yyvsp[0].pExprVarRefVal), nbrLines); }
#line 3381 "y.tab.cpp"
    break;

  case 172: /* expr: cexpr  */
#line 616 "promela.y"
                                                                                        { std::cout << "Embedded C code is not supported."; }
#line 3387 "y.tab.cpp"
    break;

  case 173: /* expr: CONST  */
#line 617 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprConst((yyvsp[0].iVal), nbrLines); }
#line 3393 "y.tab.cpp"
    break;

  case 174: /* expr: TRUE  */
#line 618 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprTrue	(nbrLines); }
#line 3399 "y.tab.cpp"
    break;

  case 175: /* expr: FALSE  */
#line 619 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprFalse(nbrLines); }
#line 3405 "y.tab.cpp"
    break;

  case 176: /* expr: TIMEOUT  */
#line 620 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprTimeout(nbrLines); }
#line 3411 "y.tab.cpp"
    break;

  case 177: /* expr: NONPROGRESS  */
#line 621 "promela.y"
                                                                                { std::cout << "The 'np_' variable is not supported."; }
#line 3417 "y.tab.cpp"
    break;

  case 178: /* expr: PC_VAL '(' expr ')'  */
#line 622 "promela.y"
                                                                        { std::cout << "The 'pc_value()' construct is not supported."; }
#line 3423 "y.tab.cpp"
    break;

  case 179: /* expr: varref '[' expr ']' '@' NAME  */
#line 623 "promela.y"
                                                                { std::cout << "Construct not supported."; /* Unclear */ }
#line 3429 "y.tab.cpp"
    break;

  case 180: /* expr: varref '[' expr ']' ':' varref  */
#line 624 "promela.y"
                                                                { std::cout << "Construct not supported."; /* Unclear */ }
#line 3435 "y.tab.cpp"
    break;

  case 181: /* expr: varref '@' NAME  */
#line 625 "promela.y"
                                                                                { (yyval.pExprVal) = new exprRemoteRef((yyvsp[-2].pExprVarRefVal) , (yyvsp[0].sVal), labelsMap[(yyvsp[0].sVal)]->getLineNb(), nbrLines); 
													assert(labelsMap.find((yyvsp[0].sVal)) != labelsMap.end()); 
													assert((yyvsp[-2].pExprVarRefVal)->getFinalSymbol()->getType() == symbol::T_PTYPE);
													free((yyvsp[0].sVal));
												}
#line 3445 "y.tab.cpp"
    break;

  case 182: /* expr: varref ':' varref  */
#line 630 "promela.y"
                                                                                { assert((yyvsp[-2].pExprVarRefVal)->getFinalSymbol()->getType() == symbol::T_PTYPE); (yyvsp[-2].pExprVarRefVal)->appendVarRef((yyvsp[0].pExprVarRefVal)); (yyval.pExprVal) = (yyvsp[-2].pExprVarRefVal); }
#line 3451 "y.tab.cpp"
    break;

  case 183: /* expr: ltl_expr  */
#line 631 "promela.y"
                                                                                        { (yyval.pExprVal) = (yyvsp[0].pExprVal); }
#line 3457 "y.tab.cpp"
    break;

  case 184: /* expr: bltl_expr  */
#line 632 "promela.y"
                                                                                        { (yyval.pExprVal) = (yyvsp[0].pExprVal); }
#line 3463 "y.tab.cpp"
    break;

  case 186: /* Opt_priority: PRIORITY CONST  */
#line 636 "promela.y"
                                                                                { assert(false); std::cout << "The 'priority' construct is related to simulation and not supported."; }
#line 3469 "y.tab.cpp"
    break;

  case 187: /* full_expr: expr  */
#line 639 "promela.y"
                                                                                { (yyval.pExprVal) = (yyvsp[0].pExprVal); }
#line 3475 "y.tab.cpp"
    break;

  case 188: /* full_expr: Expr  */
#line 640 "promela.y"
                                                                                        { (yyval.pExprVal) = (yyvsp[0].pExprVal); }
#line 3481 "y.tab.cpp"
    break;

  case 190: /* Opt_enabler: PROVIDED '(' full_expr ')'  */
#line 644 "promela.y"
                                                                { assert(false); std::cout << "The 'provided' construct is currently not supported."; }
#line 3487 "y.tab.cpp"
    break;

  case 191: /* Expr: Probe  */
#line 649 "promela.y"
                                                                                { (yyval.pExprVal) = (yyvsp[0].pExprVal); }
#line 3493 "y.tab.cpp"
    break;

  case 192: /* Expr: '(' Expr ')'  */
#line 650 "promela.y"
                                                                                { (yyval.pExprVal) = new exprPar	((yyvsp[-1].pExprVal), nbrLines); }
#line 3499 "y.tab.cpp"
    break;

  case 193: /* Expr: Expr AND Expr  */
#line 651 "promela.y"
                                                                                { (yyval.pExprVal) = new exprAnd	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3505 "y.tab.cpp"
    break;

  case 194: /* Expr: Expr AND expr  */
#line 652 "promela.y"
                                                                                { (yyval.pExprVal) = new exprAnd	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3511 "y.tab.cpp"
    break;

  case 195: /* Expr: Expr OR Expr  */
#line 653 "promela.y"
                                                                                { (yyval.pExprVal) = new exprOr	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3517 "y.tab.cpp"
    break;

  case 196: /* Expr: Expr OR expr  */
#line 654 "promela.y"
                                                                                { (yyval.pExprVal) = new exprOr	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3523 "y.tab.cpp"
    break;

  case 197: /* Expr: expr AND Expr  */
#line 655 "promela.y"
                                                                                { (yyval.pExprVal) = new exprAnd	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3529 "y.tab.cpp"
    break;

  case 198: /* Expr: expr OR Expr  */
#line 656 "promela.y"
                                                                                { (yyval.pExprVal) = new exprOr	((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3535 "y.tab.cpp"
    break;

  case 199: /* Expr: SKIP  */
#line 657 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprSkip	(nbrLines); }
#line 3541 "y.tab.cpp"
    break;

  case 200: /* Probe: FULL '(' varref ')'  */
#line 663 "promela.y"
                                                                { (yyval.pExprVal) = new exprFull	((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 3547 "y.tab.cpp"
    break;

  case 201: /* Probe: NFULL '(' varref ')'  */
#line 664 "promela.y"
                                                                        { (yyval.pExprVal) = new exprNFull((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 3553 "y.tab.cpp"
    break;

  case 202: /* Probe: EMPTY '(' varref ')'  */
#line 665 "promela.y"
                                                                        { (yyval.pExprVal) = new exprEmpty((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 3559 "y.tab.cpp"
    break;

  case 203: /* Probe: NEMPTY '(' varref ')'  */
#line 666 "promela.y"
                                                                        { (yyval.pExprVal) = new exprNEmpty((yyvsp[-1].pExprVarRefVal), nbrLines); }
#line 3565 "y.tab.cpp"
    break;

  case 204: /* args: %empty  */
#line 670 "promela.y"
                                                { (yyval.pExprArgListVal) = nullptr; }
#line 3571 "y.tab.cpp"
    break;

  case 205: /* args: arg  */
#line 671 "promela.y"
                                                                                        { (yyval.pExprArgListVal) = (yyvsp[0].pExprArgListVal); }
#line 3577 "y.tab.cpp"
    break;

  case 206: /* prargs: %empty  */
#line 675 "promela.y"
                                                                        { (yyval.pExprArgListVal) = nullptr; }
#line 3583 "y.tab.cpp"
    break;

  case 207: /* prargs: ',' arg  */
#line 676 "promela.y"
                                                                                        { (yyval.pExprArgListVal) = (yyvsp[0].pExprArgListVal); }
#line 3589 "y.tab.cpp"
    break;

  case 208: /* margs: arg  */
#line 680 "promela.y"
                                                                                { (yyval.pExprArgListVal) = (yyvsp[0].pExprArgListVal); }
#line 3595 "y.tab.cpp"
    break;

  case 209: /* margs: expr '(' arg ')'  */
#line 681 "promela.y"
                                                                                { assert(false); }
#line 3601 "y.tab.cpp"
    break;

  case 210: /* arg: expr  */
#line 684 "promela.y"
                                                                                { (yyval.pExprArgListVal) = new exprArgList(new exprArg((yyvsp[0].pExprVal), nbrLines), nbrLines); }
#line 3607 "y.tab.cpp"
    break;

  case 211: /* arg: expr ',' arg  */
#line 685 "promela.y"
                                                                                { (yyval.pExprArgListVal) = new exprArgList(new exprArg((yyvsp[-2].pExprVal), nbrLines), (yyvsp[0].pExprArgListVal), nbrLines); }
#line 3613 "y.tab.cpp"
    break;

  case 212: /* rarg: varref  */
#line 688 "promela.y"
                                                                                { (yyval.pExprRArgVal) = new exprRArgVar((yyvsp[0].pExprVarRefVal), nbrLines); }
#line 3619 "y.tab.cpp"
    break;

  case 213: /* rarg: EVAL '(' expr ')'  */
#line 689 "promela.y"
                                                                                { (yyval.pExprRArgVal) = new exprRArgEval((yyvsp[-1].pExprVal), nbrLines); }
#line 3625 "y.tab.cpp"
    break;

  case 214: /* rarg: CONST  */
#line 690 "promela.y"
                                                                                        { (yyval.pExprRArgVal) = new exprRArgConst(new exprConst((yyvsp[0].iVal), nbrLines), nbrLines); }
#line 3631 "y.tab.cpp"
    break;

  case 215: /* rarg: '-' CONST  */
#line 691 "promela.y"
                                                                        { (yyval.pExprRArgVal) = new exprRArgConst(new exprConst(-(yyvsp[0].iVal), nbrLines), nbrLines); }
#line 3637 "y.tab.cpp"
    break;

  case 216: /* rargs: rarg  */
#line 695 "promela.y"
                                                                                { (yyval.pExprRArgListVal) = new exprRArgList((yyvsp[0].pExprRArgVal), nbrLines); }
#line 3643 "y.tab.cpp"
    break;

  case 217: /* rargs: rarg ',' rargs  */
#line 696 "promela.y"
                                                                                { (yyval.pExprRArgListVal) = new exprRArgList((yyvsp[-2].pExprRArgVal), (yyvsp[0].pExprRArgListVal), nbrLines); }
#line 3649 "y.tab.cpp"
    break;

  case 218: /* rargs: rarg '(' rargs ')'  */
#line 697 "promela.y"
                                                                        { (yyval.pExprRArgListVal) = new exprRArgList((yyvsp[-3].pExprRArgVal), (yyvsp[-1].pExprRArgListVal), nbrLines); }
#line 3655 "y.tab.cpp"
    break;

  case 219: /* rargs: '(' rargs ')'  */
#line 698 "promela.y"
                                                                                { (yyval.pExprRArgListVal) = (yyvsp[-1].pExprRArgListVal); }
#line 3661 "y.tab.cpp"
    break;

  case 220: /* nlst: NAME  */
#line 701 "promela.y"
                                                                                { /*DBUG("REDUCE: NAME -> nlst\n")*/ cmtypeSymNode* sym = new cmtypeSymNode(nbrLines, mtypeDef, (yyvsp[0].sVal), mtypeId++); (*globalSymTab)->insert(sym); free((yyvsp[0].sVal)); }
#line 3667 "y.tab.cpp"
    break;

  case 221: /* nlst: nlst NAME  */
#line 702 "promela.y"
                                                                                        { /*DBUG("REDUCE: nlst NAME -> NAME\n")*/ cmtypeSymNode* sym = new cmtypeSymNode(nbrLines, mtypeDef, (yyvsp[0].sVal), mtypeId++); (*globalSymTab)->insert(sym); free((yyvsp[0].sVal)); }
#line 3673 "y.tab.cpp"
    break;

  case 222: /* nlst: nlst ','  */
#line 703 "promela.y"
                                                                { /*DBUG("REDUCE: nlst , -> nlst\n")*/ }
#line 3679 "y.tab.cpp"
    break;

  case 226: /* prop: LTL NAME '{' ltl_expr '}'  */
#line 712 "promela.y"
                                                                                {	/*DBUG("REDUCE: one_decl -> unit\n")*/
															auto sym = new ltlSymNode((yyvsp[-3].sVal), (yyvsp[-1].pExprVal), nbrLines);
															(*globalSymTab)->insert(sym);
															stmnt* decl = new ltlDecl(sym, nbrLines);
															assert(decl);
															*program = stmnt::merge(*program, decl);
														}
#line 3691 "y.tab.cpp"
    break;

  case 227: /* prop: BLTL NAME '{' bltl_expr '}'  */
#line 719 "promela.y"
                                                                                {
															/*DBUG("REDUCE: one_decl -> unit\n")*/
															auto sym = new bltlSymNode((yyvsp[-3].sVal), (yyvsp[-1].pExprVal), nbrLines);
															(*globalSymTab)->insert(sym);
															stmnt* decl = new bltlDecl(sym, nbrLines);
															assert(decl);
															*program = stmnt::merge(*program, decl);
														}
#line 3704 "y.tab.cpp"
    break;

  case 228: /* prop: FMULTILTL NAME variant_quants '{' ltl_expr '}'  */
#line 727 "promela.y"
                                                                 { 	/*DBUG("REDUCE: one_decl -> unit\n")*/
															auto sym = new fMultiLTLSymNode((yyvsp[-4].sVal), variants, (yyvsp[-1].pExprVal), nbrLines);
															(*globalSymTab)->insert(sym);
															variants.clear();
															stmnt* decl = new fMultiLTLDecl(sym, nbrLines);
															assert(decl);
															*program = stmnt::merge(*program, decl);
														}
#line 3717 "y.tab.cpp"
    break;

  case 229: /* ltl_expr: GLOBALLY expr  */
#line 738 "promela.y"
                                                                                { (yyval.pExprVal) = new exprGlobally((yyvsp[0].pExprVal), nbrLines); 	}
#line 3723 "y.tab.cpp"
    break;

  case 230: /* ltl_expr: FINALLY expr  */
#line 739 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprFinally((yyvsp[0].pExprVal), nbrLines); 	}
#line 3729 "y.tab.cpp"
    break;

  case 231: /* ltl_expr: NEXT expr  */
#line 740 "promela.y"
                                                                                                { (yyval.pExprVal) = new exprNext((yyvsp[0].pExprVal), nbrLines); 		}
#line 3735 "y.tab.cpp"
    break;

  case 232: /* ltl_expr: expr UNTIL expr  */
#line 741 "promela.y"
                                                                                                { (yyval.pExprVal) = new exprUntil((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3741 "y.tab.cpp"
    break;

  case 233: /* ltl_expr: expr IMPLIES expr  */
#line 742 "promela.y"
                                                                                                { (yyval.pExprVal) = new exprImplies((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3747 "y.tab.cpp"
    break;

  case 234: /* bltl_expr: GLOBALLY k_steps expr  */
#line 745 "promela.y"
                                                                        { (yyval.pExprVal) = new exprBoundedGlobally((yyvsp[-1].pExprVal), (yyvsp[0].pExprVal), nbrLines); 	}
#line 3753 "y.tab.cpp"
    break;

  case 235: /* bltl_expr: FINALLY k_steps expr  */
#line 746 "promela.y"
                                                                                { (yyval.pExprVal) = new exprBoundedFinally((yyvsp[-1].pExprVal), (yyvsp[0].pExprVal), nbrLines); 	}
#line 3759 "y.tab.cpp"
    break;

  case 236: /* bltl_expr: NEXT expr  */
#line 747 "promela.y"
                                                                                        { (yyval.pExprVal) = new exprNext((yyvsp[0].pExprVal), nbrLines); 		}
#line 3765 "y.tab.cpp"
    break;

  case 237: /* bltl_expr: expr UNTIL expr  */
#line 748 "promela.y"
                                                                                                { (yyval.pExprVal) = new exprUntil((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3771 "y.tab.cpp"
    break;

  case 238: /* bltl_expr: expr IMPLIES expr  */
#line 749 "promela.y"
                                                                                                { (yyval.pExprVal) = new exprImplies((yyvsp[-2].pExprVal), (yyvsp[0].pExprVal), nbrLines); }
#line 3777 "y.tab.cpp"
    break;

  case 239: /* k_steps: '{' varref GT expr '}'  */
#line 752 "promela.y"
                                                                        { (yyval.pExprVal) = new exprGT ((yyvsp[-3].pExprVarRefVal), (yyvsp[-1].pExprVal), nbrLines); }
#line 3783 "y.tab.cpp"
    break;

  case 240: /* k_steps: '{' varref LT expr '}'  */
#line 753 "promela.y"
                                                                                { (yyval.pExprVal) = new exprLT ((yyvsp[-3].pExprVarRefVal), (yyvsp[-1].pExprVal), nbrLines); }
#line 3789 "y.tab.cpp"
    break;

  case 241: /* k_steps: '{' varref GE expr '}'  */
#line 754 "promela.y"
                                                                                { (yyval.pExprVal) = new exprGE ((yyvsp[-3].pExprVarRefVal), (yyvsp[-1].pExprVal), nbrLines); }
#line 3795 "y.tab.cpp"
    break;

  case 242: /* k_steps: '{' varref LE expr '}'  */
#line 755 "promela.y"
                                                                                { (yyval.pExprVal) = new exprLE ((yyvsp[-3].pExprVarRefVal), (yyvsp[-1].pExprVal), nbrLines); }
#line 3801 "y.tab.cpp"
    break;

  case 243: /* k_steps: '{' varref EQ expr '}'  */
#line 756 "promela.y"
                                                                                { (yyval.pExprVal) = new exprEQ ((yyvsp[-3].pExprVarRefVal), (yyvsp[-1].pExprVal), nbrLines); }
#line 3807 "y.tab.cpp"
    break;

  case 244: /* k_steps: '{' varref NE expr '}'  */
#line 757 "promela.y"
                                                                                { (yyval.pExprVal) = new exprNE ((yyvsp[-3].pExprVarRefVal), (yyvsp[-1].pExprVal), nbrLines); }
#line 3813 "y.tab.cpp"
    break;

  case 245: /* variant_quants: variant_quant  */
#line 760 "promela.y"
                                                                                { variants.push_front((yyvsp[0].pVarQuantVal)); }
#line 3819 "y.tab.cpp"
    break;

  case 246: /* variant_quants: variant_quant ',' variant_quants  */
#line 761 "promela.y"
                                                                                        { variants.push_front((yyvsp[-2].pVarQuantVal)); }
#line 3825 "y.tab.cpp"
    break;

  case 247: /* variant_quant: ALWAYS variant_expr  */
#line 764 "promela.y"
                                                                        { (yyval.pVarQuantVal) = new exprAlways((yyvsp[0].pExprVarRefNameVal), nbrLines); }
#line 3831 "y.tab.cpp"
    break;

  case 248: /* variant_quant: EVENTUALLY variant_expr  */
#line 765 "promela.y"
                                                                                        { (yyval.pVarQuantVal) = new exprEventually((yyvsp[0].pExprVarRefNameVal), nbrLines); }
#line 3837 "y.tab.cpp"
    break;

  case 249: /* variant_expr: '{' NAME '}' '[' expr ']'  */
#line 768 "promela.y"
                                                        { auto sym = new variantSymNode(nbrLines, (yyvsp[-4].sVal), (yyvsp[-1].pExprVal)); (*globalSymTab)->insert(sym); (yyval.pExprVarRefNameVal) = new exprVarRefName((yyvsp[-4].sVal), sym, nbrLines); }
#line 3843 "y.tab.cpp"
    break;


#line 3847 "y.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (globalSymTab, program, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, globalSymTab, program);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, globalSymTab, program);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (globalSymTab, program, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, globalSymTab, program);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, globalSymTab, program);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 774 "promela.y"

