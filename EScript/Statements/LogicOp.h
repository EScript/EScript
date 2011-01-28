#ifndef LOGICOP_H
#define LOGICOP_H

#include "../Object.h"

namespace EScript {

/*! [LogicOp]  ---|> [Object]   */
class LogicOp : public Object {
        ES_PROVIDES_TYPE_NAME(LogicOp)
    public:
		enum opType_T{
			OR,AND,NOT
		};

        LogicOp(Object * left,Object * right,opType_T op);
        virtual ~LogicOp();

        inline Object * getLeft()const  {   return leftRef.get();   }
        inline Object * getRight()const {   return rightRef.get();  }
        inline int getOperator()const   {   return op;  }

        /// ---|> [Object]
        virtual std::string toString()const;
        Object * execute(Runtime & rt);

    private:
        ObjRef leftRef;
        ObjRef rightRef;
        opType_T op;
};
}

#endif // LOGICOP_H
