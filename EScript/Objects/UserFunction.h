#ifndef USERFUNCTION_H
#define USERFUNCTION_H

#include "../Object.h"
#include <vector>

namespace EScript {

class Block;

/*! [UserFunction]  ---|> [Object]  */
class UserFunction : public Object {
		ES_PROVIDES_TYPE_NAME(UserFunction)
	public:

		/*! [Parameter] */
		class Parameter {
			private:
				identifierId name;
				ObjRef defaultValueExpressionRef;
				ObjRef typeRef;
				bool multiParam;
			public:
				Parameter(identifierId  name,Object * defaultValueExpression=NULL,Object * type=NULL);
				~Parameter();
				std::string toString()const;

				inline identifierId getName()const              {   return name;    }
				inline Object * getType()const                  {   return typeRef.get();   }

				inline void setMultiParam(bool b)               {   multiParam=b;   }
				inline bool isMultiParam()const                 {   return multiParam;  }

				inline Object * getDefaultValueExpression()const {
					return defaultValueExpressionRef.get();
				}
				inline void setDefaultValueExpression(Object * newDefaultExpression) {
					defaultValueExpressionRef=newDefaultExpression;
				}

		};

		typedef std::vector<Parameter *> parameterList_t;

		UserFunction(parameterList_t * params,Block * block,bool lambda);
		UserFunction(parameterList_t * params,Block * block,bool lambda,const std::vector<ObjRef> & _sConstrExpressions);
		virtual ~UserFunction();

		void setBlock(Block * block);
		inline Block * getBlock()const             		{	return blockRef.get(); }
		inline parameterList_t * getParamList()const 	{	return params; }
		inline bool isLambda()const                 	{	return lambda; }
        std::string getFilename()const;
        int getLine()const;

		inline std::vector<ObjRef> & getSConstructorExpressions()	{
			return sConstrExpressions;
		}

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual std::string toDbgString()const;

	private:
		ERef<Block> blockRef;
		parameterList_t * params;
		std::vector<ObjRef> sConstrExpressions;
		bool lambda;
};
}

#endif // USERFUNCTION_H
