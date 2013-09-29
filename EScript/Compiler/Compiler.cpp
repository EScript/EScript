// Compiler.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Compiler.h"
#include "Parser.h"
#include "CompilerContext.h"
#include "../Consts.h"
#include "AST/AnnotatedStatement.h"
#include "AST/Block.h"
#include "AST/ConditionalExpr.h"
#include "AST/ControlStatements.h"
#include "AST/FunctionCallExpr.h"
#include "AST/GetAttributeExpr.h"
#include "AST/IfStatement.h"
#include "AST/LogicOpExpr.h"
#include "AST/LoopStatement.h"
#include "AST/SetAttributeExpr.h"
#include "AST/SwitchCaseStatement.h"
#include "AST/TryCatchStatement.h"
#include "AST/UserFunctionExpr.h"
#include "AST/ValueExpr.h"
#include "../Objects/Callables/UserFunction.h"

#include <functional>
#include <map>
#include <stdexcept>

#if !defined(_MSC_VER) and !defined(UNUSED_ATTRIBUTE)
#define UNUSED_ATTRIBUTE __attribute__ ((unused))
#else
#define UNUSED_ATTRIBUTE
#endif

namespace EScript{

typedef std::function<void (CompilerContext &, EPtr<AST::ASTNode>)> handler_t;
typedef std::map<internalTypeId_t, handler_t> handlerRegistry_t;
static bool initHandler(handlerRegistry_t &);
static handlerRegistry_t handlerRegistry;
static bool _handlerInitialized UNUSED_ATTRIBUTE = initHandler(handlerRegistry);


//! (ctor)
Compiler::Compiler(Logger * _logger) : logger(_logger ? _logger : new StdLogger(std::cout)) {
}

UserFunction * Compiler::compile(const CodeFragment & code){

	// prepare container function
	ERef<UserFunction> fun = new UserFunction;
	fun->setCode(code);

	// parse and build syntax tree
	Parser p(getLogger());
	ERef<AST::Block> block = p.parse(code);
	block->convertToExpression();

	// outerBlock is used to add a return statement: {return {block}}
	ERef<AST::Block> outerBlock(AST::Block::createBlockStatement());
	outerBlock->addStatement(new AST::ReturnStatement(block.get()));
		
	// compile and create instructions
	CompilerContext ctxt(*this,fun->getInstructionBlock(),code);
	
	// this would make 'this' and 'thisFn' available. This is not intended here. (not sure though...)
	//ctxt.pushSetting_basicLocalVars(); // make 'this' and parameters available

	ctxt.addExpression(outerBlock.get());
	Compiler::finalizeInstructions(fun->getInstructionBlock());

	return fun.detachAndDecrease();
}

//! (internal)
void Compiler::compileASTNode(CompilerContext & ctxt,EPtr<AST::ASTNode> node)const{
	if(node->getLine()>=0)
		ctxt.setLine(node->getLine());

	const AST::ASTNode::nodeType_t typeId = node->getNodeType();
	handlerRegistry_t::iterator it = handlerRegistry.find(typeId);
	if(it==handlerRegistry.end()){
		std::cout << reinterpret_cast<void*>(typeId)<<"\n";
		throwError(ctxt,"ASTNode can't be compiled.");
	}
	it->second(ctxt,node);
}

void Compiler::addExpression(CompilerContext & ctxt,EPtr<AST::ASTNode> expression)const{
	compileASTNode(ctxt,expression);
	if( !expression->isExpression()){ // make sure that something is added to the stack
		ctxt.addInstruction(Instruction::createPushVoid());
	}
}

//! (internal)
void Compiler::addStatement(CompilerContext & ctxt,EPtr<AST::ASTNode> statement)const{
	compileASTNode(ctxt,statement);
	if(statement->isExpression()){ // if something is added to the stack, remove it.
		ctxt.addInstruction(Instruction::createPop());
	}
}


//! (static)
void Compiler::finalizeInstructions( InstructionBlock & instructionBlock ){

	std::vector<Instruction> & instructions = instructionBlock._accessInstructions();

//	if(instructionBlock.hasJumpMarkers()){
		std::map<uint32_t,uint32_t> markerToPosition;

		{ // pass 1: remove setMarker-instructions and store position
			std::vector<Instruction> tmp;
			for(const auto & instruction : instructions) {
				if(instruction.getType() == Instruction::I_SET_MARKER) {
					markerToPosition[instruction.getValue_uint32()] = tmp.size();
				} else {
					tmp.push_back(instruction);
				}
			}
			tmp.swap(instructions);
//			instructionBlock.clearMarkerNames();
		}

		{ // pass 2: adapt jump instructions
			for(auto & instruction : instructions) {
				if( instruction.getType() == Instruction::I_JMP
						|| instruction.getType() == Instruction::I_JMP_IF_SET
						|| instruction.getType() == Instruction::I_JMP_ON_TRUE
						|| instruction.getType() == Instruction::I_JMP_ON_FALSE
						|| instruction.getType() == Instruction::I_SET_EXCEPTION_HANDLER){
					const uint32_t markerId = instruction.getValue_uint32();

					// is name of a marker (and not already a jump position)
					if(markerId>=Instruction::JMP_TO_MARKER_OFFSET){
						instruction.setValue_uint32(markerToPosition[markerId]);
					}
				}
			}

		}

//	}
}

void Compiler::throwError(CompilerContext & ctxt,const std::string & msg)const{
	std::ostringstream os;
	os << "Compiler: " << msg;
	Exception * e = new Exception(os.str(),ctxt.getCurrentLine());
	e->setFilename(ctxt.getCode().getFilename());
	throw e;
}


// ------------------------------------------------------------------


//! (static)
bool initHandler(handlerRegistry_t & m){
	using namespace AST;

	// \note  the redundant assignment to 'id2' is a workaround to a strange linker error ("undefined reference EScript::_TypeIds::TYPE_NUMBER")
	#define ADD_HANDLER( _id, _type, _block) \
	{ \
		struct _handler { \
			void operator()(CompilerContext & ctxt,EPtr<ASTNode> obj){ \
				_type * self = obj.toType<_type>(); \
				if(!self) throw std::invalid_argument("Compiler: Wrong type!"); \
				do _block while(false); \
			} \
		}; \
		const internalTypeId_t id2 = _id; \
		m[id2] = _handler(); \
	}
	// ------------------------
	// values

	// Bool
	ADD_HANDLER( ASTNode::TYPE_VALUE_BOOL, AST::BoolValueExpr, {
		ctxt.addInstruction(Instruction::createPushBool(self->getValue()));
	})
	// Identifier
	ADD_HANDLER( ASTNode::TYPE_VALUE_IDENTIFIER, AST::IdentifierValueExpr, {
		ctxt.addInstruction(Instruction::createPushId(self->getValue()));
	})
	// Number
	ADD_HANDLER( ASTNode::TYPE_VALUE_FLOATING_POINT, AST::NumberValueExpr, {
		ctxt.addInstruction(Instruction::createPushNumber(self->getValue()));
	})

	// String
	ADD_HANDLER( ASTNode::TYPE_VALUE_STRING, AST::StringValueExpr, {
		ctxt.addInstruction(Instruction::createPushString(ctxt.declareString(self->getValue())));
	})
	// Void
	ADD_HANDLER( ASTNode::TYPE_VALUE_VOID, AST::VoidValueExpr, {
		ctxt.addInstruction(Instruction::createPushVoid());
	})


	// ------------------------

	// @( [annotations] ) [statement]
	ADD_HANDLER( ASTNode::TYPE_ANNOTATED_STATEMENT, AnnotatedStatement, {
				
		const uint32_t skipMarker = ctxt.createMarker();
		ctxt.addInstruction(Instruction::createPushId( ctxt.createOnceStatementMarker() ));
		ctxt.addInstruction(Instruction::createSysCall( Consts::SYS_CALL_ONCE,0 )); // directly pops the id from the stack
		ctxt.addInstruction(Instruction::createJmpOnTrue( skipMarker ));
		ctxt.addStatement(self->getStatement());
		ctxt.addInstruction(Instruction::createSetMarker( skipMarker ));
	})
	
	// break
	ADD_HANDLER( ASTNode::TYPE_BREAK_STATEMENT, BreakStatement, {
		const uint32_t target = ctxt.getCurrentMarker(CompilerContext::BREAK_MARKER);
		if(target==Instruction::INVALID_JUMP_ADDRESS){
			ctxt.getCompiler().throwError(ctxt,"'break' outside a loop.");
		}
		std::vector<size_t> variablesToReset;
		ctxt.collectLocalVariables(CompilerContext::BREAK_MARKER,variablesToReset);
		for(const auto & var : variablesToReset) {
			ctxt.addInstruction(Instruction::createResetLocalVariable(var));
		}
		ctxt.addInstruction(Instruction::createJmp(target));
	})

	// Block
	ADD_HANDLER( ASTNode::TYPE_BLOCK_EXPRESSION, Block, {
//				std::cout << " TYPE_BLOCK_EXPRESSION "<<self->getLine()<<"\n";
		if(self->hasLocalVars())
			ctxt.pushSetting_localVars(self->getVars());

		if(self->getStatements().empty()){
			ctxt.addInstruction(Instruction::createPushVoid());
		}else{
			for( Block::cStatementCursor c = self->getStatements().begin();  c != self->getStatements().end(); ++c) {
				if(c+1 == self->getStatements().end()){ // last statemenet ? --> keep the result
					ctxt.addExpression( *c );
				}else{
					ctxt.addStatement(*c);
				}
			}
		}
		if(self->hasLocalVars()){
			for(const auto & localVar : self->getVars()) {
				ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(localVar)));
			}
			ctxt.popSetting();
		}
	})

	// BlockStatement
	ADD_HANDLER( ASTNode::TYPE_BLOCK_STATEMENT, Block, {
//								std::cout << " TYPE_BLOCK_STATEMENT "<<self->getLine()<<"\n";

		if(self->hasLocalVars())
			ctxt.pushSetting_localVars(self->getVars());

		for( AST::Block::cStatementCursor c = self->getStatements().begin();  c != self->getStatements().end(); ++c) {
			ctxt.addStatement(*c);
		}
		if(self->hasLocalVars()){
			for(const auto & localVar : self->getVars()) {
				ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(localVar)));
			}
			ctxt.popSetting();
		}
	})

	// ConditionalExpr
	ADD_HANDLER( ASTNode::TYPE_CONDITIONAL_EXPRESSION, ConditionalExpr, {
		if(self->getCondition().isNull()){
			if(self->getElseAction().isNotNull()){
				ctxt.addExpression(self->getElseAction());
			}
		}else{
			const uint32_t elseMarker = ctxt.createMarker();

			ctxt.addExpression(self->getCondition());
			ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));

			ctxt.addExpression( self->getAction() );

			if(self->getElseAction().isNotNull()){
				const uint32_t endMarker = ctxt.createMarker();
				ctxt.addInstruction(Instruction::createJmp(endMarker));
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
				ctxt.addExpression( self->getElseAction() );
				ctxt.addInstruction(Instruction::createSetMarker(endMarker));
			}else{
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
			}
		}
	})
	// ContinueStatement
	ADD_HANDLER( ASTNode::TYPE_CONTINUE_STATEMENT, ContinueStatement, {
		const uint32_t target = ctxt.getCurrentMarker(CompilerContext::CONTINUE_MARKER);
		if(target==Instruction::INVALID_JUMP_ADDRESS){
			ctxt.getCompiler().throwError(ctxt,"'continue' outside a loop.");
		}
		std::vector<size_t> variablesToReset;
		ctxt.collectLocalVariables(CompilerContext::CONTINUE_MARKER,variablesToReset);
		for(const auto & var : variablesToReset) {
			ctxt.addInstruction(Instruction::createResetLocalVariable(var));
		}
		ctxt.addInstruction(Instruction::createJmp(target));
	})
	// ExitStatement
	ADD_HANDLER( ASTNode::TYPE_EXIT_STATEMENT, ExitStatement, {
		if(self->getValueExpression().isNotNull())
			ctxt.addExpression(self->getValueExpression());
		ctxt.addInstruction(Instruction::createSysCall(Consts::SYS_CALL_EXIT,
														self->getValueExpression().isNotNull() ? 1 : 0));
	})
	// FunctionCallExpr
	ADD_HANDLER( ASTNode::TYPE_FUNCTION_CALL_EXPRESSION, FunctionCallExpr, {
		ctxt.setLine(self->getLine());

		if(!self->isSysCall()){
			do{
				GetAttributeExpr * gAttr = self->getGetFunctionExpression().toType<GetAttributeExpr>();

				// getAttributeExpression (...)
				if( gAttr ){
					const StringId & attrId = gAttr->getAttrId();

					if(gAttr->getObjectExpression()==nullptr){ // singleIdentifier (...)
						const int localVarIndex = ctxt.getCurrentVarIndex(attrId);
						if(localVarIndex>=0){
							if( !self->isConstructorCall() ){ // constructor calls don't need a caller
								ctxt.addInstruction(Instruction::createPushVoid());
							}
							ctxt.addInstruction(Instruction::createGetLocalVariable(localVarIndex));
						}else{
							if( self->isConstructorCall() ){ // constructor calls don't need a caller
								ctxt.addInstruction(Instruction::createGetVariable(attrId));
							}else{
								ctxt.addInstruction(Instruction::createFindVariable(attrId));
							}
						}
						break;
					} // getAttributeExpression.identifier (...)
					else if(GetAttributeExpr * gAttrGAttr = gAttr->getObjectExpression().toType<GetAttributeExpr>() ){
						ctxt.addExpression(gAttrGAttr);
						if( !self->isConstructorCall() ){ // constructor calls don't need a caller
							ctxt.addInstruction(Instruction::createDup());
						}
						ctxt.addInstruction(Instruction::createGetAttribute(attrId));
						break;
					} // somethingElse.identifier (...) e.g. foo().bla(), 7.bla()
					else{
						ctxt.addExpression(gAttr->getObjectExpression());
						if( !self->isConstructorCall() ){ // constructor calls don't need a caller
							ctxt.addInstruction(Instruction::createDup());
						}
						ctxt.addInstruction(Instruction::createGetAttribute(attrId));
						break;
					}
				}else{
					if( !self->isConstructorCall() ){ // constructor calls don't need a caller
						ctxt.addInstruction(Instruction::createPushVoid());
					}
					if(self->getGetFunctionExpression().isNull()){
						throw std::runtime_error("Compiler: Empty function call.");
					}
					ctxt.addExpression(self->getGetFunctionExpression());
					break;
				}

			}while(false);
		}
		for(const auto & param : self->getParams()) {
			if( param.isNull() ){
				// push undefined to be able to distinguish 'someFun(void,2);' from 'someFun(,2);'
				ctxt.addInstruction(Instruction::createPushUndefined());
			}else{
				ctxt.addExpression(param);
			}
		}
		uint32_t paramCount = self->getParams().size();
		if(self->hasExpandingParameters()){
			// store param count on stack
			ctxt.addInstruction(Instruction::createPushUInt(static_cast<uint32_t>(paramCount)));
			// call EXPAND_PARAMETERS before calling the function
			for(auto it=self->getExpandingParameters().begin();it!=self->getExpandingParameters().end();++it){
				// number of stack entries have to be stored to get to the next expanding parameter
				const uint32_t next = (it+1 == self->getExpandingParameters().end()) ? paramCount : *(it+1);
				ctxt.addInstruction(Instruction::createPushUInt(next - *it -1));
			}
			ctxt.addInstruction(Instruction::createSysCall(Consts::SYS_CALL_EXPAND_PARAMS_ON_STACK,
															static_cast<uint32_t>(self->getExpandingParameters().size()+1)));
			paramCount = Consts::DYNAMIC_PARAMETER_COUNT;
		}

		if( self->isSysCall()){
			ctxt.addInstruction(Instruction::createSysCall(self->getSysCallId(),paramCount));
		}else if( self->isConstructorCall()){
			ctxt.addInstruction(Instruction::createCreateInstance(paramCount));
		}else{
			ctxt.addInstruction(Instruction::createCall(paramCount));
		}
	})

	// GetAttributeExpr
	ADD_HANDLER( ASTNode::TYPE_GET_ATTRIBUTE_EXPRESSION, GetAttributeExpr, {
		if(self->getObjectExpression().isNotNull()){
			ctxt.addExpression(self->getObjectExpression());
			ctxt.addInstruction(Instruction::createGetAttribute(self->getAttrId()));
		}else{
			const int localVarIndex = ctxt.getCurrentVarIndex(self->getAttrId());
			if(localVarIndex>=0){
				ctxt.addInstruction(Instruction::createGetLocalVariable(localVarIndex));
			}else{
				ctxt.addInstruction(Instruction::createGetVariable(self->getAttrId()));
			}
		}

	})

	// IfStatement
	ADD_HANDLER( ASTNode::TYPE_IF_STATEMENT, IfStatement, {
		if(self->getCondition().isNull()){
			if(self->getElseAction().isNotNull()){
				ctxt.addStatement(self->getElseAction());
			}
		}else{
			const uint32_t elseMarker = ctxt.createMarker();

			ctxt.addExpression(self->getCondition());
			ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));
			if(self->getAction().isNotNull()){
				ctxt.addStatement(self->getAction());
			}

			if(self->getElseAction().isNotNull()){
				const uint32_t endMarker = ctxt.createMarker();
				ctxt.addInstruction(Instruction::createJmp(endMarker));
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
				ctxt.addStatement(self->getElseAction());
				ctxt.addInstruction(Instruction::createSetMarker(endMarker));
			}else{
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
			}
		}
	})


	// LogicOpExpr
	ADD_HANDLER( ASTNode::TYPE_LOGIC_OP_EXPRESSION, LogicOpExpr, {
		switch(self->getOperator()){
			case LogicOpExpr::NOT:{
				ctxt.addExpression(self->getLeft());
				ctxt.addInstruction(Instruction::createNot());
				break;
			}
			case LogicOpExpr::OR:{
				const uint32_t marker = ctxt.createMarker();
				const uint32_t endMarker = ctxt.createMarker();
				ctxt.addExpression(self->getLeft());
				ctxt.addInstruction(Instruction::createJmpOnTrue(marker));
				ctxt.addExpression(self->getRight());
				ctxt.addInstruction(Instruction::createJmpOnTrue(marker));
				ctxt.addInstruction(Instruction::createPushBool(false));
				ctxt.addInstruction(Instruction::createJmp(endMarker));
				ctxt.addInstruction(Instruction::createSetMarker(marker));
				ctxt.addInstruction(Instruction::createPushBool(true));
				ctxt.addInstruction(Instruction::createSetMarker(endMarker));
				break;
			}
			default:
			case LogicOpExpr::AND:{
				const uint32_t marker = ctxt.createMarker();
				const uint32_t endMarker = ctxt.createMarker();
				ctxt.addExpression(self->getLeft());
				ctxt.addInstruction(Instruction::createJmpOnFalse(marker));
				ctxt.addExpression(self->getRight());
				ctxt.addInstruction(Instruction::createJmpOnFalse(marker));
				ctxt.addInstruction(Instruction::createPushBool(true));
				ctxt.addInstruction(Instruction::createJmp(endMarker));
				ctxt.addInstruction(Instruction::createSetMarker(marker));
				ctxt.addInstruction(Instruction::createPushBool(false));
				ctxt.addInstruction(Instruction::createSetMarker(endMarker));
				break;
			}
		}
	})

	// LoopStatement
	ADD_HANDLER( ASTNode::TYPE_LOOP_STATEMENT, LoopStatement, {
		const uint32_t loopBegin = ctxt.createMarker();
		const uint32_t loopEndMarker = ctxt.createMarker();
		const uint32_t loopContinueMarker = ctxt.createMarker();
		const uint32_t loopElseMarker = ctxt.createMarker();

		if(self->getInitStatement().isNotNull()){
			ctxt.addStatement(self->getInitStatement());
		}
		ctxt.addInstruction(Instruction::createSetMarker(loopBegin));

		if(self->getPreConditionExpression().isNotNull()){
			ctxt.addExpression(self->getPreConditionExpression());
			ctxt.addInstruction(Instruction::createJmpOnFalse(loopElseMarker));
		}
		ctxt.pushSetting_marker( CompilerContext::BREAK_MARKER ,loopEndMarker);
		ctxt.pushSetting_marker( CompilerContext::CONTINUE_MARKER ,loopContinueMarker);
		if(self->getAction().isNotNull()) {
			ctxt.addStatement(self->getAction());
		}
		ctxt.popSetting();
		ctxt.popSetting();

		if(self->getPostConditionExpression().isNotNull()){ // increaseStmt is ignored!
			ctxt.addInstruction(Instruction::createSetMarker(loopContinueMarker));
			ctxt.addExpression(self->getPostConditionExpression());
			ctxt.addInstruction(Instruction::createJmpOnTrue(loopBegin));
		}else{
			ctxt.addInstruction(Instruction::createSetMarker(loopContinueMarker));
			if(self->getIncreaseStatement().isNotNull()){
				ctxt.addStatement(self->getIncreaseStatement());
			}
			ctxt.addInstruction(Instruction::createJmp(loopBegin));
		}
		ctxt.addInstruction(Instruction::createSetMarker(loopElseMarker));
		if(self->getElseAction().isNotNull()){
			ctxt.addStatement(self->getElseAction());
		}

		ctxt.addInstruction(Instruction::createSetMarker(loopEndMarker));
	})

	// ReturnStatement
	ADD_HANDLER( ASTNode::TYPE_RETURN_STATEMENT, ReturnStatement, {
		if(self->getValueExpression().isNotNull()){
			ctxt.addExpression(self->getValueExpression());
			ctxt.addInstruction(Instruction::createAssignLocal(Consts::LOCAL_VAR_INDEX_internalResult));
		}
		ctxt.addInstruction(Instruction::createJmp(Instruction::INVALID_JUMP_ADDRESS));
	})

	// SetAttributeExpr
	ADD_HANDLER( ASTNode::TYPE_SET_ATTRIBUTE_EXPRESSION, SetAttributeExpr, {
		ctxt.addExpression(self->getValueExpression());

		ctxt.setLine(self->getLine());
		ctxt.addInstruction(Instruction::createDup());

		const StringId & attrId = self->getAttrId();
		if(self->isAssignment()){
			// no object given: a = ...
			if(self->getObjectExpression().isNull()){
				// local variable: var a = ...
				if(ctxt.getCurrentVarIndex(attrId)>=0){
					ctxt.addInstruction(Instruction::createAssignLocal(ctxt.getCurrentVarIndex(attrId)));
				}else{
					ctxt.addInstruction(Instruction::createAssignVariable(attrId));
				}
			}else{ // object.a =
				ctxt.addExpression(self->getObjectExpression());
				ctxt.addInstruction(Instruction::createAssignAttribute(attrId));
			}

		}else{
				ctxt.addExpression(self->getObjectExpression());
				ctxt.addInstruction(Instruction::createPushUInt(static_cast<uint32_t>(self->getAttributeProperties())));
				ctxt.addInstruction(Instruction::createSetAttribute(attrId));
		}
	})
	typedef std::deque<std::pair<size_t,uint32_t>> caseMarkerQueue_t; // macros don't like nested template parameters...
	ADD_HANDLER( ASTNode::TYPE_SWITCH_STATEMENT, SwitchCaseStatement, {
		const auto endMarker = ctxt.createMarker();
		const Block* block = self->getBlock();

		// ---------------
		// jmp dispatcher
		ctxt.addExpression(self->getDecisionExpression());

		if(block->hasLocalVars())
			ctxt.pushSetting_localVars(block->getVars());

		// add comparisons
		auto defaultMarker = endMarker;
		caseMarkerQueue_t caseMarkers; // stmtIndex -> marker
		for(const auto & posAndExpression: self->getCaseInfos()){
			const auto caseMarker = ctxt.createMarker();
			caseMarkers.emplace_back(posAndExpression.first,caseMarker);

			if(posAndExpression.second.isNotNull()){
				ctxt.addExpression(posAndExpression.second); // push case expression
				ctxt.addInstruction(Instruction::createSysCall(Consts::SYS_CALL_CASE_TEST,1));

				ctxt.addInstruction(Instruction::createJmpOnTrue(caseMarker));
			}else{ // default:
				defaultMarker = caseMarker;
			}
		}
		// finally jump to the default case
		ctxt.addInstruction(Instruction::createPop());	// pop decision var
		ctxt.addInstruction(Instruction::createJmp(defaultMarker));

		// ---------------
		// cases block
		ctxt.pushSetting_marker( CompilerContext::BREAK_MARKER ,endMarker);


		size_t stmtIdx = 0;
		for( const auto & stmt : block->getStatements()){
			while(!caseMarkers.empty() && stmtIdx==caseMarkers.front().first){
				ctxt.addInstruction(Instruction::createSetMarker(caseMarkers.front().second));
				caseMarkers.pop_front();
			}
			ctxt.addStatement(stmt);
			++stmtIdx;
		}
		while(!caseMarkers.empty() ){ // add remaining case markers (after the last instruction)
			ctxt.addInstruction(Instruction::createSetMarker(caseMarkers.front().second));
			caseMarkers.pop_front();
		}

		// ---------------
		// after the cases block
		ctxt.addInstruction(Instruction::createSetMarker(endMarker));
		if(block->hasLocalVars()){
			for(const auto & localVar : block->getVars()) {
				ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(localVar)));
			}
			ctxt.popSetting(); // localVars
		}
		ctxt.popSetting(); // BREAK_MARKER
	})

	// ReturnStatement
	ADD_HANDLER( ASTNode::TYPE_THROW_STATEMENT, ThrowStatement, {
		if(self->getValueExpression().isNotNull())
			ctxt.addExpression(self->getValueExpression());
		ctxt.addInstruction(Instruction::createSysCall(Consts::SYS_CALL_THROW,
								self->getValueExpression().isNotNull() ? 1 : 0));
	})

	// TryCatchStatement
	ADD_HANDLER( ASTNode::TYPE_TRY_CATCH_STATEMENT, TryCatchStatement, {
		const uint32_t catchMarker = ctxt.createMarker();
		const uint32_t endMarker = ctxt.createMarker();

		// try
		// ------
		ctxt.pushSetting_marker(CompilerContext::EXCEPTION_MARKER,catchMarker);
		ctxt.addInstruction(Instruction::createSetExceptionHandler(catchMarker));

		// collect all variables that are declared inside the try-block (excluding nested try-blocks)
		std::vector<size_t> collectedVariableIndices;
		ctxt.pushLocalVarsCollector(&collectedVariableIndices);
		ctxt.addStatement(self->getTryBlock());
		ctxt.popLocalVarsCollector();

		ctxt.popSetting(); // restore previous EXCEPTION_MARKER

		// try block without exception --> reset catchMarker and jump to endMarker
		ctxt.addInstruction(Instruction::createSetExceptionHandler(ctxt.getCurrentMarker(CompilerContext::EXCEPTION_MARKER)));
		ctxt.addInstruction(Instruction::createJmp(endMarker));

		// catch
		// ------
		const StringId & exceptionVariableName = self->getExceptionVariableName();

		ctxt.addInstruction(Instruction::createSetMarker(catchMarker));
		// reset catchMarker
		ctxt.addInstruction(Instruction::createSetExceptionHandler(ctxt.getCurrentMarker(CompilerContext::EXCEPTION_MARKER)));

		// clear all variables defined inside try block
		for(const auto & localVar : collectedVariableIndices) {
			ctxt.addInstruction(Instruction::createResetLocalVariable(localVar));
		}

		// define exception variable
		if(!exceptionVariableName.empty()){
			std::set<StringId> varSet;
			varSet.insert(exceptionVariableName);
			ctxt.pushSetting_localVars(varSet);
			// load exception-variable with exception object ( exceptionVariableName = __result )
			ctxt.addInstruction(Instruction::createGetLocalVariable(Consts::LOCAL_VAR_INDEX_internalResult));
			ctxt.addInstruction(Instruction::createAssignLocal(ctxt.getCurrentVarIndex(exceptionVariableName)));
		}

		// clear the exception-variable
		ctxt.addInstruction(Instruction::createResetLocalVariable(Consts::LOCAL_VAR_INDEX_internalResult));

		// execute catch block
		ctxt.addStatement(self->getCatchBlock());
		// pop exception variable
		if(!exceptionVariableName.empty()){
			ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(exceptionVariableName)));
			ctxt.popSetting(); // variable
		}
		// end:
		ctxt.addInstruction(Instruction::createSetMarker(endMarker));
	})

	// user function
	ADD_HANDLER( ASTNode::TYPE_USER_FUNCTION_EXPRESSION, UserFunctionExpr, {

		ERef<UserFunction> fun = new UserFunction;
		fun->setCode(self->getCode());
		fun->setLine(self->getLine());

		CompilerContext ctxt2(ctxt.getCompiler(),fun->getInstructionBlock(),self->getCode());
		ctxt2.setLine(self->getLine()); // set the line of all initializations to the line of the function declaration

		{	// init parameter counts
			int minParamValueCount = 0;
			for(const auto & param : self->getParamList()) {
				if(param.isMultiParam() || param.getDefaultValueExpression() != nullptr)
					break;
				++minParamValueCount;
			}

			int maxParamValueCount = 0;
			size_t i = 0;
			for(const auto & param : self->getParamList()) {
				if(param.isMultiParam()){
					fun->setMultiParam(i);
					maxParamValueCount = -1;
					break;
				}else{
					++maxParamValueCount;
				}
				++i;
			}
			fun->setParameterCounts(self->getParamList().size(),minParamValueCount,maxParamValueCount);
		}

		// declare local variables
		for(const auto & param : self->getParamList())
			fun->getInstructionBlock().declareLocalVariable(param.getName());

		ctxt2.pushSetting_basicLocalVars(); // make 'this' and parameters available

		// default parameters
		for(const auto & param : self->getParamList()) {
			EPtr<AST::ASTNode> defaultExpr = param.getDefaultValueExpression();
			if(defaultExpr.isNotNull()){
				const int varIdx = ctxt2.getCurrentVarIndex(param.getName()); // \todo assert(varIdx>=0)

				const uint32_t parameterAvailableMarker = ctxt2.createMarker();
				ctxt2.addInstruction(Instruction::createPushUInt(varIdx));
				ctxt2.addInstruction(Instruction::createJmpIfSet(parameterAvailableMarker));

//					ctxt2.enableGlobalVarContext();				// \todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				ctxt2.addExpression(defaultExpr);
//					ctxt2.disableGlobalVarContext();			// \todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				ctxt2.addInstruction(Instruction::createAssignLocal(varIdx));

				ctxt2.addInstruction(Instruction::createSetMarker(parameterAvailableMarker));
			}
		}

		// parameter type checks
		for(const auto & param : self->getParamList()) {
			const ASTNode::refArray_t & typeExpressions = param.getTypeExpressions();
			if(typeExpressions.empty())
				continue;
			const int varIdx = ctxt2.getCurrentVarIndex(param.getName());	// \todo assert(varIdx>=0)
			// if the parameter has value constrains AND is a multi parameter, use a special system-call for this (instead of manually creating a foreach-loop here)
			// e.g. fn([Bool,Number] p*){...}
			if(param.isMultiParam()){
				for(const auto & typeExpr : typeExpressions) {
					ctxt2.addExpression(typeExpr);
				}
				ctxt2.addInstruction(Instruction::createGetLocalVariable(varIdx));
				ctxt2.addInstruction(Instruction::createSysCall(Consts::SYS_CALL_TEST_ARRAY_PARAMETER_CONSTRAINTS,typeExpressions.size()+1 ));
				ctxt2.addInstruction(Instruction::createPop());

			}else{
				std::vector<uint32_t> constrainOkMarkers; // each constrain gets its own ok-marker
				for(const auto & typeExpr : typeExpressions) {
					const uint32_t constrainOkMarker = ctxt2.createMarker();
					constrainOkMarkers.push_back(constrainOkMarker);

					ctxt2.addExpression(typeExpr);
					ctxt2.addInstruction(Instruction::createDup()); // store the constraint for the error message
					ctxt2.addInstruction(Instruction::createCheckType(varIdx));
					ctxt2.addInstruction(Instruction::createJmpOnTrue(constrainOkMarker));
				}

				// all constraint-checks failed! -> stack contains all failed constraints
				ctxt2.addInstruction(Instruction::createGetLocalVariable(varIdx));
				ctxt2.addInstruction(Instruction::createSysCall(Consts::SYS_CALL_THROW_TYPE_EXCEPTION,constrainOkMarkers.size()+1 ));
				ctxt2.addInstruction(Instruction::createJmp( Instruction::INVALID_JUMP_ADDRESS ));

				// depending on which constraint-check succeeded, pop the constraint-values from the stack
				for(std::vector<uint32_t>::const_reverse_iterator cIt = constrainOkMarkers.rbegin();cIt!=constrainOkMarkers.rend();++cIt){
					ctxt2.addInstruction(Instruction::createSetMarker(*cIt));
					ctxt2.addInstruction(Instruction::createPop());
				}
			}
		}

		// add super-constructor parameters
		const ASTNode::refArray_t & superConstrParams = self->getSConstructorExpressions();
		for(const auto & superConstrParam : superConstrParams) {
			ctxt2.addExpression(superConstrParam);
		}

		// init 'this' (or create it if this is a constructor call)
		ctxt2.addInstruction(Instruction::createInitCaller(superConstrParams.size()));

		ctxt2.addStatement(self->getBlock());
		ctxt2.popSetting();
		Compiler::finalizeInstructions(fun->getInstructionBlock());

		ctxt.addInstruction(Instruction::createPushFunction(ctxt.registerInternalFunction(fun.get())));

	})

	// YieldStatement
	ADD_HANDLER( ASTNode::TYPE_YIELD_STATEMENT, YieldStatement, {
		if(self->getValueExpression().isNotNull()){
			ctxt.addExpression(self->getValueExpression());
		}else{
			ctxt.addInstruction(Instruction::createPushVoid());
		}
		ctxt.addInstruction(Instruction::createYield());
	})

	// ------------------------
	#undef ADD_HANDLER
	return true;
}

}
