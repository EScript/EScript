#ifndef SCRIPT_H
#define SCRIPT_H

#include "../Object.h"

#include "Exception.h"
#include "../Statements/Block.h"

namespace EScript {

/*! [Script] ---|> [Object] */
class Script : public Object {
        ES_PROVIDES_TYPE_NAME(Script)
    public:
        static void init();
        static ERef<Namespace> SGLOBALS;

        // ---
        Script();
        virtual ~Script();
        Block * getRootBlock()const;
        void loadScript(std::string filename) throw (Exception *);

        /// ---|> [Object]
        virtual Object * execute(Runtime & rt);

    protected:
        ERef<Block> rootBlock;
};
}

#endif // SCRIPT_H
