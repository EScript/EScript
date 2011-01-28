#ifdef ES_BUILD_APPLICATION
#include <cstdlib>
#include <iostream>
#include <string>

#include "../EScript/EScript.h"
#include "../EScript/Parser/Parser.h"

#ifdef ES_DEBUG_MEMORY
#include "Utils/Debug.h"
#endif

/*! Command line application */
int main(int argc,char * argv[]) {

	using namespace EScript;

    EScript::init();

	#ifdef ES_DEBUG_MEMORY
	Tokenizer::identifyStaticToken(0); // init constants
    Debug::clearObjects();
	#endif
    // create script
    ERef<Script> s=new Script();
    // create runtime
    ERef<Runtime> rt=new Runtime();

    rt->initVariable("args",Array::create(argc,argv));

    std::string file="tests/test.escript";
    if(argc>1) {
        file=argv[1];
    }
//    try {
//        s->loadScript(file);
//    } catch (Object * e) {
//        std::cerr << "\n\a "<<e->toString() << "\n"<<std::endl;
//        return EXIT_FAILURE;
//    }
//
    ObjRef result;
//    try {
        result=rt->executeObj(s.get());
//    }catch (Exception * e) {
//        std::cout << "\nERROR \a";
//        if (e) std::cout << e->toString();
//    }catch (Object * o) {
//        std::cout << "\nCaught: ";
//        if (o) std::cout << o->toString();
//    }catch (...) {
//        std::cout << "\nCaught unknown C++ expception. ";
//    }
//
    std::cout << "\n --- \n\n\n";
//
//    try {
//        s->loadScript(file);
//    } catch (Object * e) {
//        std::cerr << "\n\a "<<e->toString() << "\n"<<std::endl;
//        return EXIT_FAILURE;
//    }
//
//    try {
//        result=rt->execute(s.get());
//    }catch (Exception * e) {
//        std::cout << "\nERROR \a";
//        if (e) std::cout << e->toString();
//    }catch (Object * o) {
//        std::cout << "\nCaught: ";
//        if (o) std::cout << o->toString();
//    }catch (...) {
//        std::cout << "\nCaught unknown C++ expception. ";
//    }
//
//    std::cout << "\n --- \n\n\n";

//    // --- Test NumberRef
//    std::cout << numberRefTest1<<"," <<numberRefTest2; // should be (18,16)

	std::string line;
	ERef<Parser> parser=new Parser;
	while(true) {
		std::cin >> line;
	    ObjRef statement;
//	    Object * statement;
	    try {
			statement=parser->parse(s->getRootBlock(),line.c_str());
		} catch (Object * e) {
			std::cerr << "\n\a "<<e->toString() << "\n"<<std::endl;
			continue;
		}catch(...){
			std::cout <<"ERROR\n";
			continue;
		}

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

		std::cout << result.toString()<<"\n";
//		std::cout << statement.get();
	}


    if (!result.isNull()) {
        std::cout << "\nResult: " << result.toString()<<"\n";
    }
    parser=NULL;
    s=NULL;
    rt=NULL;

    #ifdef ES_DEBUG_MEMORY
    Debug::showObjects();
    #endif
    return EXIT_SUCCESS;
}
#endif // ES_BUILD_APPLICATION
