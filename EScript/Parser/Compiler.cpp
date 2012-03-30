// Compiler.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#include "Compiler.h"
#include "Parser.h"
#include "CompilerContext.h"
#include "../Consts.h"
#include "../Objects/typeIds.h"
#include "../Objects/AST/BlockStatement.h"
#include "../Objects/AST/ConditionalExpr.h"
#include "../Objects/AST/FunctionCallExpr.h"
#include "../Objects/AST/GetAttributeExpr.h"
#include "../Objects/AST/IfStatement.h"
#include "../Objects/AST/LogicOpExpr.h"
#include "../Objects/AST/LoopStatement.h"
#include "../Objects/AST/SetAttributeExpr.h"
#include "../Objects/AST/Statement.h"
#include "../Objects/AST/TryCatchStatement.h"
#include "../Objects/AST/UserFunctionExpr.h"
#include "../Objects/Callables/UserFunction.h"
#include "../Objects/Identifier.h"
#include "../Objects/Values/Bool.h"
#include "../Objects/Values/Number.h"
#include "../Objects/Values/String.h"
#include "../Objects/Values/Void.h"

#include <stdexcept>
#include <map>

#if !defined(_MSC_VER) and !defined(UNUSED_ATTRIBUTE)
#define UNUSED_ATTRIBUTE __attribute__ ((unused))
#else
#define UNUSED_ATTRIBUTE
#endif

namespace EScript{

// init handlerRegistry \todo Can't this be done more efficiently using c++11 functionals???
struct handler_t{ virtual void operator()(CompilerContext & ctxt,ObjPtr obj)=0; };
typedef std::map<internalTypeId_t,handler_t *> handlerRegistry_t;
static bool initHandler(handlerRegistry_t &);
static handlerRegistry_t handlerRegistry;
static bool _handlerInitialized UNUSED_ATTRIBUTE = initHandler(handlerRegistry);


//! (ctor)
Compiler::Compiler(Logger * _logger) : logger(_logger ? _logger : new StdLogger(std::cout)) {
}


void Compiler::compileExpression(CompilerContext & ctxt,ObjPtr expression)const{
	if(expression.isNull())
		return;
	const internalTypeId_t typeId = expression->_getInternalTypeId();

	handlerRegistry_t::iterator it = handlerRegistry.find(typeId);
	if(it==handlerRegistry.end()){
			std::cout << reinterpret_cast<void*>(typeId)<<"\n";
		throw std::invalid_argument("Expression can't be compiled.");
	}
	(*it->second)(ctxt,expression);
}


void Compiler::log(CompilerContext & ctxt,Logger::level_t messageLevel, const std::string & msg)const{
	std::ostringstream os;
	os << "[Compiler] " << msg ;
//	<< " (" << getCurrentFilename();
//	if(token!=NULL)
//		os << ':' << token->getLine();
//	os << ").";
	logger->log(messageLevel,os.str());
}

UserFunction * Compiler::compile(const StringData & code){
	static const StringId inline_id("[inline]");

	// prepare container function
	ERef<UserFunction> fun = new UserFunction(new UserFunction::parameterList_t,new AST::BlockStatement); // dummy parameters
	fun->setCodeString(String::create(code),0,code.str().length());

	// parse and build syntax tree
	ERef<AST::BlockStatement> block(new AST::BlockStatement);
	block->setFilename(inline_id);
//	ERef<UserFunctionExpr> fun = new UserFunction(block);

	Parser p(getLogger());
	p._produceBytecode = true;
	p.parse(block.get(),code);

	// compile and create instructions
	CompilerContext ctxt(*this,fun->getInstructions());
	ctxt.compile(block.get());
	Compiler::finalizeInstructions(fun->getInstructions());

	return fun.detachAndDecrease();
}
	
//! (static) 
void Compiler::finalizeInstructions( InstructionBlock & instructionBlock ){

	std::vector<Instruction> & instructions = instructionBlock._accessInstructions();
	
	if(instructionBlock.hasJumpMarkers()){
		std::map<uint32_t,uint32_t> markerToPosition;
	
		{ // pass 1: remove setMarker-instructions and store position
			std::vector<Instruction> tmp;
			for(std::vector<Instruction>::const_iterator it=instructions.begin();it!=instructions.end();++it){
				if( it->getType() == Instruction::I_SET_MARKER ){
					markerToPosition[it->getValue_uint32()] = tmp.size();
				}else{
					tmp.push_back(*it);
				}
			}
			tmp.swap(instructions);
//			instructionBlock.clearMarkerNames();
		}

		{ // pass 2: adapt jump instructions
			for(std::vector<Instruction>::iterator it=instructions.begin();it!=instructions.end();++it){
				if( it->getType() == Instruction::I_JMP 
						|| it->getType() == Instruction::I_JMP_IF_SET 
						|| it->getType() == Instruction::I_JMP_ON_TRUE 
						|| it->getType() == Instruction::I_JMP_ON_FALSE
						|| it->getType() == Instruction::I_SET_EXCEPTION_HANDLER){
					const uint32_t markerId = it->getValue_uint32();
					
					// is name of a marker (and not already a jump position)
					if(markerId>=Instruction::JMP_TO_MARKER_OFFSET){
						it->setValue_uint32(markerToPosition[markerId]);
					}
				}
			}
			
		}
		
	}
}

//! (internal)
void Compiler::compileStatement(CompilerContext & ctxt,const AST::Statement & statement)const{
	using AST::Statement;
	
	if(statement.getType() == Statement::TYPE_CONTINUE){
		const uint32_t target = ctxt.getCurrentMarker(CompilerContext::CONTINUE_MARKER);
		if(target==Instruction::INVALID_JUMP_ADDRESS){
			std::cout << "\nError: Continue outside a loop!\n"; //! \todo Compiler error
		}
		std::vector<size_t> variablesToReset;
		ctxt.collectLocalVariables(CompilerContext::CONTINUE_MARKER,variablesToReset);
		for(std::vector<size_t>::const_iterator it = variablesToReset.begin();it!=variablesToReset.end();++it){
			ctxt.addInstruction(Instruction::createResetLocalVariable(*it));
		}
		ctxt.addInstruction(Instruction::createJmp(target));
		
	}else if(statement.getType() == Statement::TYPE_BREAK){
		const uint32_t target = ctxt.getCurrentMarker(CompilerContext::BREAK_MARKER);
		if(target==Instruction::INVALID_JUMP_ADDRESS){
			std::cout << "\nError: Break outside a loop!\n"; //! \todo Compiler error
		}
		std::vector<size_t> variablesToReset;
		ctxt.collectLocalVariables(CompilerContext::BREAK_MARKER,variablesToReset);
		for(std::vector<size_t>::const_iterator it = variablesToReset.begin();it!=variablesToReset.end();++it){
			ctxt.addInstruction(Instruction::createResetLocalVariable(*it));
		}
		ctxt.addInstruction(Instruction::createJmp(target));
	}else if(statement.getType() == Statement::TYPE_RETURN){
		if(statement.getExpression().isNotNull()){
			ctxt.compile(statement.getExpression());
			ctxt.addInstruction(Instruction::createAssignLocal(Consts::LOCAL_VAR_INDEX_internalResult));
		}
		ctxt.addInstruction(Instruction::createJmp(Instruction::INVALID_JUMP_ADDRESS));
	
	}else if(statement.getExpression().isNotNull()){
		ctxt.setLine(statement.getLine());
		ctxt.compile(statement.getExpression());
		if(statement.getType() == Statement::TYPE_EXPRESSION)
			ctxt.addInstruction(Instruction::createPop());
	}
}



// ------------------------------------------------------------------


//! (static)
bool initHandler(handlerRegistry_t & m){
	// \note  the redundant assignment to 'id2' is a workaround to a strange linker error ("undefined reference EScript::_TypeIds::TYPE_NUMBER")
	#define ADD_HANDLER( _id, _type, _block) \
	{ \
		struct _handler : public handler_t{ \
			~_handler(){} \
			virtual void operator()(CompilerContext & ctxt,ObjPtr obj){ \
				_type * self = obj.toType<_type>(); \
				if(!self) throw std::invalid_argument("Wrong type!"); \
				do _block while(false); \
			} \
		}; \
		const internalTypeId_t id2 = _id; \
		m[id2] = new _handler(); \
	}
	// ------------------------
	// Simple types

	// Bool
	ADD_HANDLER( _TypeIds::TYPE_BOOL, Bool, {
		ctxt.addInstruction(Instruction::createPushBool(self->toBool()));
	})
	// Identifier
	ADD_HANDLER( _TypeIds::TYPE_IDENTIFIER, Identifier, {
		ctxt.addInstruction(Instruction::createPushId(self->getId()));
	})
	// Number
	ADD_HANDLER( _TypeIds::TYPE_NUMBER, Number, {
		ctxt.addInstruction(Instruction::createPushNumber(self->toDouble()));
	})

	// String
	ADD_HANDLER( _TypeIds::TYPE_STRING, String, {
		ctxt.addInstruction(Instruction::createPushString(ctxt.declareString(self->toString())));
	})
	// Void
	ADD_HANDLER( _TypeIds::TYPE_VOID, Void, {
		ctxt.addInstruction(Instruction::createPushVoid());
	})


	// ------------------------
	// AST
	using namespace AST;


	// BlockStatement
	ADD_HANDLER( _TypeIds::TYPE_BLOCK_STATEMENT, BlockStatement, {
		if(self->hasLocalVars())
			ctxt.pushSetting_localVars(*self->getVars());

		for ( BlockStatement::cStatementCursor c = self->getStatements().begin();  c != self->getStatements().end(); ++c) {
			ctxt.compile(*c);
		}
		if(self->hasLocalVars()){
			for(std::set<StringId>::const_iterator it = self->getVars()->begin();it!=self->getVars()->end();++it){
				ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(*it)));
			}
			ctxt.popSetting();
		}
	})

	// ConditionalExpr
	ADD_HANDLER( _TypeIds::TYPE_CONDITIONAL_EXPRESSION, ConditionalExpr, {
		if(self->getCondition().isNull()){
			if(self->getElseAction().isNotNull()){
				ctxt.compile(self->getElseAction());
			}
		}else{
			const uint32_t elseMarker = ctxt.createMarker();

			ctxt.compile(self->getCondition());
			ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));

			ctxt.compile( self->getAction() );

			if(self->getElseAction().isNotNull()){
				const uint32_t endMarker = ctxt.createMarker();
				ctxt.addInstruction(Instruction::createJmp(endMarker));
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
				ctxt.compile( self->getElseAction() );
				ctxt.addInstruction(Instruction::createSetMarker(endMarker));
			}else{
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
			}
		}
	})

	// FunctionCallExpr
	ADD_HANDLER( _TypeIds::TYPE_FUNCTION_CALL_EXPRESSION, FunctionCallExpr, {
		if(!self->isSysCall()){
			do{
				GetAttributeExpr * gAttr = self->getGetFunctionExpression().toType<GetAttributeExpr>();

				// getAttributeExpression (...)
				if( gAttr ){
					const StringId attrId = gAttr->getAttrId();

					if(gAttr->getObjectExpression()==NULL){ // singleIdentifier (...)
						const int localVarIndex = ctxt.getCurrentVarIndex(attrId);
						if(localVarIndex>=0){
							ctxt.addInstruction(Instruction::createPushVoid());
							ctxt.addInstruction(Instruction::createGetLocalVariable(localVarIndex));
						}else{
							ctxt.addInstruction(Instruction::createFindVariable(attrId));
						}
						break;
					} // getAttributeExpression.identifier (...)
					else if(GetAttributeExpr * gAttrGAttr = gAttr->getObjectExpression().toType<GetAttributeExpr>() ){
						ctxt.compile(gAttrGAttr);
						ctxt.addInstruction(Instruction::createDup());
						ctxt.addInstruction(Instruction::createGetAttribute(attrId));
						break;
					} // somethingElse.identifier (...) e.g. foo().bla(), 7.bla()
					else{
						ctxt.compile(gAttr->getObjectExpression());
						ctxt.addInstruction(Instruction::createDup());
						ctxt.addInstruction(Instruction::createGetAttribute(attrId));
						break;
					}
				}else{
					ctxt.addInstruction(Instruction::createPushVoid());
					ctxt.compile(self->getGetFunctionExpression());
					break;
				}

			}while(false);
		}
		for(std::vector<ObjRef>::const_iterator it=self->getParams().begin();it!=self->getParams().end();++it){
			if( it->isNull() ){
				/* \todo explicitly mark this as NULL/Undefined and not void! Perhaps: pushUndefined!?
					Then "(fn(a=1,b=2,c=3){ out(a+b+c);}) (,10);" should work.	*/
				ctxt.addInstruction(Instruction::createPushVoid());
			}else{
				ctxt.compile(*it);
			}
		}
		if( self->isSysCall()){
			ctxt.addInstruction(Instruction::createPushUInt(self->getSysCallId()));
			ctxt.addInstruction(Instruction::createSysCall(self->getParams().size()));
		}else if( self->isConstructorCall()){
			ctxt.addInstruction(Instruction::createCreateInstance(self->getParams().size()));
		}else{
			ctxt.addInstruction(Instruction::createCall(self->getParams().size()));
		}
	})

	// GetAttributeExpr
	ADD_HANDLER( _TypeIds::TYPE_GET_ATTRIBUTE_EXPRESSION, GetAttributeExpr, {
		if(self->getObjectExpression().isNotNull()){
			ctxt.compile(self->getObjectExpression());
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
	ADD_HANDLER( _TypeIds::TYPE_IF_STATEMENT, IfStatement, {
		if(self->getCondition().isNull()){
			if(self->getElseAction().isValid()){
				ctxt.compile(self->getElseAction());
			}
		}else{
			const uint32_t elseMarker = ctxt.createMarker();

			ctxt.compile(self->getCondition());
			ctxt.addInstruction(Instruction::createJmpOnFalse(elseMarker));
			if(self->getAction().isValid()){
				ctxt.compile(self->getAction());
			}

			if(self->getElseAction().isValid()){
				const uint32_t endMarker = ctxt.createMarker();
				ctxt.addInstruction(Instruction::createJmp(endMarker));
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
				ctxt.compile(self->getElseAction());
				ctxt.addInstruction(Instruction::createSetMarker(endMarker));
			}else{
				ctxt.addInstruction(Instruction::createSetMarker(elseMarker));
			}
		}
	})


	// IfStatement
	ADD_HANDLER( _TypeIds::TYPE_LOGIC_OP_EXPRESSION, LogicOpExpr, {
		switch(self->getOperator()){
			case LogicOpExpr::NOT:{
				ctxt.compile(self->getLeft());
				ctxt.addInstruction(Instruction::createNot());
				break;
			}
			case LogicOpExpr::OR:{
				const uint32_t marker = ctxt.createMarker();
				const uint32_t endMarker = ctxt.createMarker();
				ctxt.compile(self->getLeft());
				ctxt.addInstruction(Instruction::createJmpOnTrue(marker));
				ctxt.compile(self->getRight());
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
				ctxt.compile(self->getLeft());
				ctxt.addInstruction(Instruction::createJmpOnFalse(marker));
				ctxt.compile(self->getRight());
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

	// IfStatement
	ADD_HANDLER( _TypeIds::TYPE_LOOP_STATEMENT, LoopStatement, {
		const uint32_t loopBegin = ctxt.createMarker();
		const uint32_t loopEndMarker = ctxt.createMarker();
		const uint32_t loopContinueMarker = ctxt.createMarker();

		if(self->getInitStatement().isValid()){
			ctxt.setLine(self->getInitStatement().getLine());
			ctxt.compile(self->getInitStatement());
		}
		ctxt.addInstruction(Instruction::createSetMarker(loopBegin));

		if(self->getPreConditionExpression().isNotNull()){
			ctxt.compile(self->getPreConditionExpression());
			ctxt.addInstruction(Instruction::createJmpOnFalse(loopEndMarker));
		}
		ctxt.pushSetting_marker( CompilerContext::BREAK_MARKER ,loopEndMarker);
		ctxt.pushSetting_marker( CompilerContext::CONTINUE_MARKER ,loopContinueMarker);
		ctxt.compile(self->getAction());
		ctxt.popSetting();
		ctxt.popSetting();

		if(self->getPostConditionExpression().isNotNull()){ // increaseStmt is ignored!
			ctxt.addInstruction(Instruction::createSetMarker(loopContinueMarker));
			ctxt.compile(self->getPostConditionExpression());
			ctxt.addInstruction(Instruction::createJmpOnTrue(loopBegin));
		}else{
			ctxt.addInstruction(Instruction::createSetMarker(loopContinueMarker));
			if(self->getIncreaseStatement().isValid()){
				ctxt.compile(self->getIncreaseStatement());
			}
			ctxt.addInstruction(Instruction::createJmp(loopBegin));
		}
		ctxt.addInstruction(Instruction::createSetMarker(loopEndMarker));
	})


	// IfStatement
	ADD_HANDLER( _TypeIds::TYPE_SET_ATTRIBUTE_EXPRESSION, SetAttributeExpr, {
		ctxt.compile(self->getValueExpression());

		ctxt.setLine(self->getLine());
		ctxt.addInstruction(Instruction::createDup());

		const StringId attrId = self->getAttrId();
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
				ctxt.compile(self->getObjectExpression());
				ctxt.addInstruction(Instruction::createAssignAttribute(attrId));
			}

		}else{
				ctxt.compile(self->getObjectExpression());
				ctxt.addInstruction(Instruction::createPushUInt(static_cast<uint32_t>(self->getAttributeProperties())));
				ctxt.addInstruction(Instruction::createSetAttribute(attrId));
		}
	})

	// TryCatchStatement
	ADD_HANDLER( _TypeIds::TYPE_TRY_CATCH_STATEMENT, TryCatchStatement, {
		const uint32_t catchMarker = ctxt.createMarker();
		const uint32_t endMarker = ctxt.createMarker();

		// try
		// ------
		ctxt.pushSetting_marker(CompilerContext::EXCEPTION_MARKER,catchMarker);
		ctxt.addInstruction(Instruction::createSetExceptionHandler(catchMarker));

		// collect all variables that are declared inside the try-block (excluding nested try-blocks)
		std::vector<size_t> collectedVariableIndices;
		ctxt.pushLocalVarsCollector(&collectedVariableIndices);
		ctxt.compile(self->getTryBlock().get());
		ctxt.popLocalVarsCollector();

		ctxt.addInstruction(Instruction::createJmp(endMarker));
		ctxt.popSetting(); // EXCEPTION_MARKER

		// catch
		// ------
		const StringId exceptionVariableName = self->getExceptionVariableName();

		ctxt.addInstruction(Instruction::createSetMarker(catchMarker));
		// reset catchMarker
		ctxt.addInstruction(Instruction::createSetExceptionHandler(ctxt.getCurrentMarker(CompilerContext::EXCEPTION_MARKER)));

		// clear all variables defined inside try block
		for(std::vector<size_t>::const_iterator it = collectedVariableIndices.begin(); it!=collectedVariableIndices.end();++it){
			ctxt.addInstruction(Instruction::createResetLocalVariable(*it));
		}

		// define exception variable
		if(!exceptionVariableName.empty()){
			std::set<StringId> varSet;
			varSet.insert(exceptionVariableName);
			ctxt.pushSetting_localVars(varSet);
		}
		// load exception variable with exception object ( exceptionVariableName = __result )
		ctxt.addInstruction(Instruction::createGetLocalVariable(2));
		ctxt.addInstruction(Instruction::createAssignLocal(ctxt.getCurrentVarIndex(exceptionVariableName)));

		// execute catch block
		ctxt.compile(self->getCatchBlock().get());
		// pop exception variable
		if(!exceptionVariableName.empty()){
			ctxt.addInstruction(Instruction::createResetLocalVariable(ctxt.getCurrentVarIndex(exceptionVariableName)));
			ctxt.popSetting(); // variable
		}
		// end:
		ctxt.addInstruction(Instruction::createSetMarker(endMarker));
	})

	// ------------------------
	// Other objects
	ADD_HANDLER( _TypeIds::TYPE_USER_FUNCTION_EXPRESSION, UserFunctionExpr, {

		ERef<UserFunction> fun = new UserFunction(new UserFunction::parameterList_t,new AST::BlockStatement); // dummy parameters
		fun->setParameterCounts(self->getParamList().size(),self->getMinParamCount(),self->getMaxParamCount());
		//! \todo set code string
//		fun->setCodeString(String::create(code),0,code.str().length());

		CompilerContext ctxt2(ctxt.getCompiler(),fun->getInstructions());

		// declare a local variables for each parameter expression
		for(UserFunctionExpr::parameterList_t::const_iterator it = self->getParamList().begin();it!=self->getParamList().end();++it){
			fun->getInstructions().declareLocalVariable( it->getName() );
		}

		ctxt2.pushSetting_basicLocalVars(); // make 'this' and parameters available

		// default parameters
		for(UserFunctionExpr::parameterList_t::const_iterator it = self->getParamList().begin();it!=self->getParamList().end();++it){
			const UserFunctionExpr::Parameter & param = *it;
			ObjPtr defaultExpr = param.getDefaultValueExpression();
			if(defaultExpr.isNotNull()){
				const int varIdx = ctxt2.getCurrentVarIndex(param.getName()); // \todo assert(varIdx>=0)

				const uint32_t parameterAvailableMarker = ctxt2.createMarker();
				ctxt2.addInstruction(Instruction::createPushUInt(varIdx));
				ctxt2.addInstruction(Instruction::createJmpIfSet(parameterAvailableMarker));

//					ctxt2.enableGlobalVarContext();				// \todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				ctxt2.compile(defaultExpr);
//					ctxt2.disableGlobalVarContext();			// \todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				ctxt2.addInstruction(Instruction::createAssignLocal(varIdx));

				ctxt2.addInstruction(Instruction::createSetMarker(parameterAvailableMarker));
//
			}
		}

		// parameter type checks
		for(UserFunctionExpr::parameterList_t::const_iterator it = self->getParamList().begin();it!=self->getParamList().end();++it){
			const UserFunctionExpr::Parameter & param = *it;
			const std::vector<ObjRef> & typeExpressions = param.getTypeExpressions();
			if(typeExpressions.empty())
				continue;
			const int varIdx = ctxt2.getCurrentVarIndex(param.getName());	// \todo assert(varIdx>=0)
			const uint32_t typeOkMarker = ctxt2.createMarker();
			
			for(std::vector<ObjRef>::const_iterator it2 = typeExpressions.begin();it2!=typeExpressions.end();++it2){
				if(it2!=typeExpressions.begin())
					ctxt2.addInstruction(Instruction::createPop());

				ctxt2.compile( *it2 );
				ctxt2.addInstruction(Instruction::createCheckType(varIdx));
				ctxt2.addInstruction(Instruction::createJmpOnTrue(typeOkMarker));
			}
			// type check failed! -> topmost element on stack is the last checked type (used for the error message)
			ctxt2.addInstruction(Instruction::createGetLocalVariable(varIdx));
			ctxt2.addInstruction(Instruction::createPushUInt(Consts::SYS_CALL_THROW_TYPE_EXCEPTION));
			ctxt2.addInstruction(Instruction::createSysCall(2));
			ctxt2.addInstruction(Instruction::createJmp( Instruction::INVALID_JUMP_ADDRESS ));
			
			ctxt2.addInstruction(Instruction::createSetMarker(typeOkMarker));
		}

		// add super-constructor parameters
		const std::vector<ObjRef> & superConstrParams = self->getSConstructorExpressions();
		for(std::vector<ObjRef>::const_iterator it = superConstrParams.begin();it!=superConstrParams.end();++it)
			ctxt2.compile(*it);
		
		// init 'this' (or create it if this is a constructor call)
		ctxt2.addInstruction(Instruction::createInitCaller(superConstrParams.size()));
//		ctxt2.addInstruction(Instruction::createAssignLocal(Consts::LOCAL_VAR_INDEX_this)); 

		ctxt2.compile(self->getBlock());
		ctxt2.popSetting();
		Compiler::finalizeInstructions(fun->getInstructions());

		ctxt.addInstruction(Instruction::createPushFunction(ctxt.registerInternalFunction(fun.get())));

	})

	// ------------------------
	#undef ADD_HANDLER
	return true;
}



}
