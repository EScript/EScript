#ifdef ES_BUILD_TEST_APPLICATION
#include <cstdlib>
#include <iostream>
#include <string>

#include "../EScript/EScript.h"

#ifdef ES_DEBUG_MEMORY
#include "../EScript/Parser/Tokenizer.h"
#include "../EScript/Utils/Debug.h"
#endif

using namespace EScript;


int main(int argc,char * argv[]) {

    EScript::init();

#ifdef ES_DEBUG_MEMORY
	Tokenizer::identifyStaticToken(0); // init constants
    Debug::clearObjects();
#endif
    ERef<Script> s=new Script();
    ERef<Runtime> rt=new Runtime();

    rt->initVariable("args",Array::create(argc,argv));

     // --- Test NumberRef
    int numberRefTest1=17;
    rt->initVariable("numberRefTest1",new NumberRef(numberRefTest1));
    float numberRefTest2=17;
    rt->initVariable("numberRefTest2",new NumberRef(numberRefTest2));


    std::string file="tests/test.escript";
    if(argc>1) {
        file=argv[1];
    }
    try {
        s->loadScript(file);
    } catch (Object * e) {
        std::cerr << "\n\a "<<e->toString() << "\n"<<std::endl;
        return EXIT_FAILURE;
    }

    ObjRef result;
    try {
        result=rt->executeObj(s.get());
    }catch (Exception * e) {
        std::cout << "\nERROR \a";
        if (e) std::cout << e->toString();
    }catch (Object * o) {
        std::cout << "\nCaught: ";
        if (o) std::cout << o->toString();
    }catch (...) {
        std::cout << "\nCaught unknown C++ expception. ";
    }

    std::cout << "\n --- \n\n\n";

//    // --- Test NumberRef
//    std::cout << numberRefTest1<<"," <<numberRefTest2; // should be (18,16)

    if (!result.isNull()) {
        std::cout << "\nResult: " << result.toString()<<"\n";
    }
    s=NULL;
    rt=NULL;

    #ifdef ES_DEBUG_MEMORY
//        std::cout << "\nGLOBALS: " << Script::SGLOBALS<<"\n";

        Debug::showObjects();
    #endif
//system("pause");
    return EXIT_SUCCESS;
}
#endif // ES_BUILD_TEST_APPLICATION
