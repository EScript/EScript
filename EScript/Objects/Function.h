#ifndef LIBFUNCTION_H
#define LIBFUNCTION_H

#include "../Object.h"
#include "../Runtime/Runtime.h"

namespace EScript {

/*! [Function] ---|> [Object]   */
class Function : public ExtObject {
        ES_PROVIDES_TYPE_NAME(Function)
    public:
        typedef Object * ( * functionPtr)(Runtime & runtime,Object * caller, const ParameterValues & parameter);
        // ---
        Function(functionPtr fnptr);
        virtual ~Function();
        inline functionPtr getFnPtr()const	{	return fnptr;	}

        /// ---|> [Object]
        virtual Object * clone()const 		{	return new Function(fnptr);	}

    private:
        functionPtr fnptr;
};

}
#endif // LIBFUNCTION_H
