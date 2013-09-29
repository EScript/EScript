// ASTNode.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef ES_ASTNODE_H
#define ES_ASTNODE_H

#include "../../Utils/EReferenceCounter.h"
#include "../../Utils/TypeNameMacro.h"
#include "../../Utils/ObjRef.h"
#include "../../Utils/StringId.h"
#include <vector>

namespace EScript {

namespace AST {

//! [ASTNode]  ---|> [EReferenceCounter]
class ASTNode : public EReferenceCounter<ASTNode> {
		ES_PROVIDES_TYPE_NAME(ASTNode)
	public:
		typedef std::vector<ERef<ASTNode>> refArray_t;
		typedef EPtr<ASTNode> ptr_t;
		typedef ERef<ASTNode> ref_t;

		typedef uint8_t nodeType_t;

		static const nodeType_t TYPE_ANNOTATED_STATEMENT		= 0x00;
		static const nodeType_t TYPE_BLOCK_EXPRESSION			= 0x01;
		static const nodeType_t TYPE_BLOCK_STATEMENT			= 0x02;
		static const nodeType_t TYPE_BREAK_STATEMENT			= 0x03;
		static const nodeType_t TYPE_CONDITIONAL_EXPRESSION		= 0x04;
		static const nodeType_t TYPE_CONTINUE_STATEMENT			= 0x05;
		static const nodeType_t TYPE_EXIT_STATEMENT				= 0x06;
		static const nodeType_t TYPE_FOREACH_STATEMENT			= 0x07;
		static const nodeType_t TYPE_FUNCTION_CALL_EXPRESSION	= 0x08;
		static const nodeType_t TYPE_GET_ATTRIBUTE_EXPRESSION	= 0x09;
		static const nodeType_t TYPE_IF_STATEMENT				= 0x0a;
		static const nodeType_t TYPE_LOGIC_OP_EXPRESSION		= 0x0b;
		static const nodeType_t TYPE_LOOP_STATEMENT				= 0x0c;
		static const nodeType_t TYPE_RETURN_STATEMENT			= 0x0d;
		static const nodeType_t TYPE_SET_ATTRIBUTE_EXPRESSION	= 0x0e;
		static const nodeType_t TYPE_SWITCH_STATEMENT			= 0x0f;
		static const nodeType_t TYPE_THROW_STATEMENT			= 0x10;
		static const nodeType_t TYPE_TRY_CATCH_STATEMENT		= 0x11;
		static const nodeType_t TYPE_USER_FUNCTION_EXPRESSION	= 0x12;
		static const nodeType_t TYPE_VALUE_BOOL					= 0x13;
		static const nodeType_t TYPE_VALUE_FLOATING_POINT		= 0x14;
		static const nodeType_t TYPE_VALUE_IDENTIFIER			= 0x15;
		static const nodeType_t TYPE_VALUE_STRING				= 0x16;
		static const nodeType_t TYPE_VALUE_VOID					= 0x17;
		static const nodeType_t TYPE_YIELD_STATEMENT			= 0x18;

		virtual ~ASTNode(){}

		nodeType_t getNodeType()const		{	return typeId;	}
		int getLine()const					{	return line;	}
		bool isExpression()const			{	return isExpr;	}
		void setLine(int l)					{	line = l;	}

	protected:
		ASTNode(nodeType_t _typeId, bool _isExpression, int _line=-1) :
				typeId(_typeId),line(_line),isExpr(_isExpression){}
		void convert(nodeType_t _typeId, bool _isExpression){
			typeId = _typeId;
			isExpr = _isExpression;
		}
	private:
		nodeType_t typeId;
		int line;
		bool isExpr;

};

}
}

#endif // ES_ASTNODE_H
