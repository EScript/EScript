// test.cpp
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifdef ES_BUILD_TEST_APPLICATION
#include <cstdlib>
#include <iostream>
#include <string>

#include "../EScript/EScript.h"
#include "../EScript/Objects/ReferenceObject.h"

#ifdef ES_DEBUG_MEMORY
#include "../EScript/Compiler/Tokenizer.h"
#include "../EScript/Utils/Debug.h"
#endif

using namespace EScript;

// ---------------------------------------------------------
// test case for wrapped class

//! A simple test class with some data members
struct TestObject{
	int m1;
	float m2;
	explicit TestObject(int _m1,float _m2) : m1(_m1),m2(_m2){}
	bool operator==(const TestObject&other)const {	return m1==other.m1 && m2==other.m2;}
};

//! A EScript-container for the simple test class
struct E_TestObject : public ReferenceObject<TestObject>{
	ES_PROVIDES_TYPE_NAME(TestObject)
public:

	E_TestObject(int i=0,float f=0) :
			ReferenceObject<TestObject>(getTypeObject(),i,f){}
	virtual ~E_TestObject(){
//		std::cout << " ~TEST ";
	}
	TestObject & operator*(){				return ref();	}
	const TestObject & operator*()const{	return ref();	}

	static Type* getTypeObject(){
		static Type * typeObject = new Type(Object::getTypeObject()); // ---|> Object
		return typeObject;
	}
	//! (static)
	static void init(Namespace & ns){
		Type * typeObject = getTypeObject();
		declareConstant(&ns,getClassName(),typeObject);

		//! TestObject new TestObject([i [,j]])
		ES_CTOR(typeObject,0,2,new E_TestObject(parameter[0].to<int>(rt),parameter[1].to<float>(rt)))

		//! Number getM1()
		ES_MFUN(typeObject, const TestObject,"getM1",0,0, thisObj->m1)

		//! Number getM2()
		ES_MFUN(typeObject, const TestObject,"getM2",0,0, thisObj->m2)
		
		//! self setM1(Number)
		ES_MFUN(typeObject, TestObject,"setM1",1,1,( thisObj->m1=parameter[0].to<int>(rt),thisEObj))

		//! self setM2(Number)
		ES_MFUN(typeObject, TestObject,"setM2",1,1,( thisObj->m2=parameter[0].to<float>(rt),thisEObj))

	}
};

// define rule to convert E_TestObject to TestObject*
ES_CONV_EOBJ_TO_OBJ(E_TestObject, TestObject*,	&**eObj)

// ----------------------------------------------------------------------------

int main(int argc,char * argv[]) {
	EScript::init();

	// --- Init the TestObejct-Type
	E_TestObject::init(*EScript::getSGlobals());

#ifdef ES_DEBUG_MEMORY
	Tokenizer::identifyStaticToken(StringId()); // init constants
	Debug::clearObjects();
#endif

	ERef<Runtime> rt(new Runtime);

	declareConstant(rt->getGlobals(),"args",Array::create(argc,argv));

	// --- Load and execute script
	std::string file= argc>1 ? argv[1] : "tests/test.escript";
	std::pair<bool,ObjRef> result = EScript::loadAndExecute(*rt.get(),file);

	// --- output result
	if(result.second.isNotNull()) {
		std::cout << "\n\n --- "<<"\nResult: " << result.second.toString()<<"\n";
	}

	// --- cleanup
	result.second = nullptr;
	rt = nullptr;

#ifdef ES_DEBUG_MEMORY
	Debug::showObjects();
#endif
	return result.first ? EXIT_SUCCESS : EXIT_FAILURE;
}
#endif // ES_BUILD_TEST_APPLICATION
