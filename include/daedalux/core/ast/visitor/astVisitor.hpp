#ifndef AST_VISITOR_H
#define AST_VISITOR_H

class stmnt;
class stmntChanRecv;
class stmntChanSnd;
class stmntOpt;
class stmntIf;
class stmntDo;
class stmntBreak;
class stmntAction;
class stmntGoto;
class stmntLabel;
class stmntSeq;
class stmntFct;
class stmntCall;
class stmntAtomic;
class stmntDStep;
class stmntAsgn;
class stmntIncr;
class stmntDecr;
class stmntPrint;
class stmntPrintm;
class stmntAssert;
class stmntExpr;
class stmntElse;
class stmntWait;
class stmntWhen;
class varDecl;
class chanDecl;
class mtypeDecl;
class tdefDecl;
class inlineDecl;
class procDecl;
class initDecl;
class neverDecl;
class expr;
class exprCond;
class exprRArg;
class exprRArgVar;
class exprRArgEval;
class exprRArgConst;
class exprRArgList;
class exprArg;
class exprArgList;
class exprVarRefName;
class exprVarRef;
class exprVar;
class exprRun;
class exprConst;
class exprTimeout;
class exprSkip;
class exprTrue;
class exprFalse;
class exprPlus;
class exprMinus;
class exprTimes;
class exprDiv;
class exprMod;
class exprGT;
class exprLT;
class exprGE;
class exprLE;
class exprEQ;
class exprNE;
class exprAnd;
class exprOr;
class exprBitwAnd;
class exprBitwOr;
class exprBitwXor;
class exprLShift;
class exprRShift;
class exprPar;
class exprCount;
class exprUMin;
class exprNeg;
class exprBitwNeg;
class exprLen;
class exprFull;
class exprNFull;
class exprEmpty;
class exprNEmpty;
class exprRemoteRef;

class ASTConstVisitor {
public:
	virtual ~ASTConstVisitor() {}
	//virtual void visit(const stmnt* node) {}
	virtual void visit(const stmntChanRecv* node) {}
	virtual void visit(const stmntChanSnd* node) {}
	virtual void visit(const stmntOpt* node) {}
	virtual void visit(const stmntIf* node) {}
	virtual void visit(const stmntDo* node) {}
	virtual void visit(const stmntBreak* node) {}
	virtual void visit(const stmntAction* node) {}
	virtual void visit(const stmntGoto* node) {}
	virtual void visit(const stmntLabel* node) {}
	virtual void visit(const stmntSeq* node) {}
	virtual void visit(const stmntFct* node) {}
	virtual void visit(const stmntCall* node) {}
	virtual void visit(const stmntAtomic* node) {}
	virtual void visit(const stmntDStep* node) {}
	virtual void visit(const stmntAsgn* node) {}
	virtual void visit(const stmntIncr* node) {}
	virtual void visit(const stmntDecr* node) {}
	virtual void visit(const stmntPrint* node) {}
	virtual void visit(const stmntPrintm* node) {}
	virtual void visit(const stmntAssert* node) {}
	virtual void visit(const stmntExpr* node) {}
	virtual void visit(const stmntElse* node) {}
	virtual void visit(const stmntWait* node) {}
	virtual void visit(const stmntWhen* node) {}
	virtual void visit(const varDecl* node) {}
	virtual void visit(const chanDecl* node) {}
	virtual void visit(const tdefDecl* node) {}
	virtual void visit(const mtypeDecl* node) {}
	virtual void visit(const inlineDecl* node) {}
	virtual void visit(const procDecl* node) {}
	virtual void visit(const initDecl* node) {}
	virtual void visit(const neverDecl* node) {}
	//virtual void visit(const expr* node) {}
	virtual void visit(const exprCond* node) {}
	virtual void visit(const exprRArg* node) {}
	virtual void visit(const exprRArgVar* node) {}
	virtual void visit(const exprRArgEval* node) {}
	virtual void visit(const exprRArgConst* node) {}
	virtual void visit(const exprRArgList* node) {}
	virtual void visit(const exprArg* node) {}
	virtual void visit(const exprArgList* node) {}
	virtual void visit(const exprVarRefName* node) {}
	virtual void visit(const exprVarRef* node) {}
	virtual void visit(const exprVar* node) {}
	virtual void visit(const exprRun* node) {}
	virtual void visit(const exprConst* node) {}
	virtual void visit(const exprTimeout* node) {}
	virtual void visit(const exprSkip* node) {}
	virtual void visit(const exprTrue* node) {}
	virtual void visit(const exprFalse* node) {}
	virtual void visit(const exprPlus* node) {}
	virtual void visit(const exprMinus* node) {}
	virtual void visit(const exprTimes* node) {}
	virtual void visit(const exprDiv* node) {}
	virtual void visit(const exprMod* node) {}
	virtual void visit(const exprGT* node) {}
	virtual void visit(const exprLT* node) {}
	virtual void visit(const exprGE* node) {}
	virtual void visit(const exprLE* node) {}
	virtual void visit(const exprEQ* node) {}
	virtual void visit(const exprNE* node) {}
	virtual void visit(const exprAnd* node) {}
	virtual void visit(const exprOr* node) {}
	virtual void visit(const exprBitwAnd* node) {}
	virtual void visit(const exprBitwOr* node) {}
	virtual void visit(const exprBitwXor* node) {}
	virtual void visit(const exprLShift* node) {}
	virtual void visit(const exprRShift* node) {}
	virtual void visit(const exprPar* node) {}
	virtual void visit(const exprCount* node) {}
	virtual void visit(const exprUMin* node) {}
	virtual void visit(const exprNeg* node) {}
	virtual void visit(const exprBitwNeg* node) {}
	virtual void visit(const exprLen* node) {}
	virtual void visit(const exprFull* node) {}
	virtual void visit(const exprNFull* node) {}
	virtual void visit(const exprEmpty* node) {}
	virtual void visit(const exprNEmpty* node) {}
	virtual void visit(const exprRemoteRef* node) {}


};

class ASTVisitor {
public:
	virtual ~ASTVisitor() {}
	//virtual void visit(stmnt* node) {}
	virtual void visit(stmntChanRecv* node) {}
	virtual void visit(stmntChanSnd* node) {}
	virtual void visit(stmntOpt* node) {}
	virtual void visit(stmntIf* node) {}
	virtual void visit(stmntDo* node) {}
	virtual void visit(stmntBreak* node) {}
	virtual void visit(stmntAction* node) {}
	virtual void visit(stmntGoto* node) {}
	virtual void visit(stmntLabel* node) {}
	virtual void visit(stmntSeq* node) {}
	virtual void visit(stmntFct* node) {}
	virtual void visit(stmntCall* node) {}
	virtual void visit(stmntAtomic* node) {}
	virtual void visit(stmntDStep* node) {}
	virtual void visit(stmntAsgn* node) {}
	virtual void visit(stmntIncr* node) {}
	virtual void visit(stmntDecr* node) {}
	virtual void visit(stmntPrint* node) {}
	virtual void visit(stmntPrintm* node) {}
	virtual void visit(stmntAssert* node) {}
	virtual void visit(stmntExpr* node) {}
	virtual void visit(stmntElse* node) {}
	virtual void visit(stmntWait* node) {}
	virtual void visit(stmntWhen* node) {}
	virtual void visit(varDecl* node) {}
	virtual void visit(chanDecl* node) {}
	virtual void visit(tdefDecl* node) {}
	virtual void visit(mtypeDecl* node) {}
	virtual void visit(inlineDecl* node) {}
	virtual void visit(procDecl* node) {}
	virtual void visit(initDecl* node) {}
	virtual void visit(neverDecl* node) {}
	//virtual void visit(expr* node) {}
	virtual void visit(exprCond* node) {}
	virtual void visit(exprRArg* node) {}
	virtual void visit(exprRArgVar* node) {}
	virtual void visit(exprRArgEval* node) {}
	virtual void visit(exprRArgConst* node) {}
	virtual void visit(exprRArgList* node) {}
	virtual void visit(exprArg* node) {}
	virtual void visit(exprArgList* node) {}
	virtual void visit(exprVarRefName* node) {}
	virtual void visit(exprVarRef* node) {}
	virtual void visit(exprVar* node) {}
	virtual void visit(exprRun* node) {}
	virtual void visit(exprConst* node) {}
	virtual void visit(exprTimeout* node) {}
	virtual void visit(exprSkip* node) {}
	virtual void visit(exprTrue* node) {}
	virtual void visit(exprFalse* node) {}
	virtual void visit(exprPlus* node) {}
	virtual void visit(exprMinus* node) {}
	virtual void visit(exprTimes* node) {}
	virtual void visit(exprDiv* node) {}
	virtual void visit(exprMod* node) {}
	virtual void visit(exprGT* node) {}
	virtual void visit(exprLT* node) {}
	virtual void visit(exprGE* node) {}
	virtual void visit(exprLE* node) {}
	virtual void visit(exprEQ* node) {}
	virtual void visit(exprNE* node) {}
	virtual void visit(exprAnd* node) {}
	virtual void visit(exprOr* node) {}
	virtual void visit(exprBitwAnd* node) {}
	virtual void visit(exprBitwOr* node) {}
	virtual void visit(exprBitwXor* node) {}
	virtual void visit(exprLShift* node) {}
	virtual void visit(exprRShift* node) {}
	virtual void visit(exprPar* node) {}
	virtual void visit(exprCount* node) {}
	virtual void visit(exprUMin* node) {}
	virtual void visit(exprNeg* node) {}
	virtual void visit(exprBitwNeg* node) {}
	virtual void visit(exprLen* node) {}
	virtual void visit(exprFull* node) {}
	virtual void visit(exprNFull* node) {}
	virtual void visit(exprEmpty* node) {}
	virtual void visit(exprNEmpty* node) {}
	virtual void visit(exprRemoteRef* node) {}
	
};


#endif
