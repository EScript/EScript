#ifndef FORCONTROL_H
#define FORCONTROL_H

#include "../Object.h"

namespace EScript {

/*! [ForControl]  ---|> [Object]    */
class ForControl : public Object {
        ES_PROVIDES_TYPE_NAME(ForControl)
    public:
        ForControl(Object * initExpression,Object * condExpression,Object * incrExpression,Object * action);
        virtual ~ForControl();

        /// ---|> [Object]
        virtual std::string toString()const;
        virtual Object * execute(Runtime & rt);

    private:
        ObjRef initExpressionRef;
        ObjRef condExpressionRef;
        ObjRef incrExpressionRef;
        ObjRef actionRef;
};
}

#endif // FORCONTROL_H
