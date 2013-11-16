// ObjectSerialization.escript
// This file is part of the EScript StdLib library.
// See copyright notice in basics.escript
// ------------------------------------------------------

loadOnce(__DIR__+"/basics.escript");

/*! System for serializing EScript Objects into JSON-formatted strings.
	
	Example:
	
	// create two objects. someObject stores the same dataObject two times
	var dataObject = new ExtObject( { $data : 1 } );
	var someObject = new ExtObject( { $data1 : dataObject, $data2 : dataObject } );
	
	var ctxt = new ObjectSerialization.Context;
	var mySerializedObject = ctxt.serialize(someObject);

	// ... store and load mySerializedObject
	var ctxt = new ObjectSerialization.Context;
	var someObject = ctxt.createFromString( mySerializedObject);
	
	if( someObject.data1 == someObject.data2)
		out("someObject restored!");
	
*/

/*
   \todo  support Maps containing '##TYPE##' ##REF## or ##ID##
   \todo  support bound parameters: [1,2,3] => fn(a,b,c){ ... };

*/
static TypeRegistry;
static TypeHandler;
static GenericTypeHandler;
static Context;
static defaultRegistry;

// ---------------------------------------------------------------------------

/*! A TypeRegistry contains the de-/serialization functions. 
	If a TypeRegistry references a base registry, this registry can specialize on the handling of some types. For all 
	others, the base registry is used.
	\note If you use multiple TypeRegistry-objects, always use the same name for a Type. */

{
	var T = TypeRegistry = new Type;

	T._printableName @(override) ::= $TypeRegistry;

	T.registeredTypes @(private,init) := Map; // Map: type's name ---> TypeHandler
	T.baseRegistry @(private) := void; //  void or TypeRegistry

	//! (ctor)
	T._constructor ::= fn( [TypeRegistry,void] base = defaultRegistry){
		this.baseRegistry = base;
	};
	
	/*! Create and register a TypeHandler for the given Type.
		The TypeHandler is returned and should then be initialized.
		\see CommonSerializers for examples	*/
	T.registerType ::= fn(Type type, String typeName){
		var handler = new GenericTypeHandler(type,typeName);
		this.registerTypeHandler(handler);
		return handler;
	};

	//! (internal)
	T.registerTypeHandler ::= fn(TypeHandler typeHandler){
		this.registeredTypes[ typeHandler.getHandledTypeName() ] = typeHandler;
		this.registeredTypes[ typeHandler.getHandledType().toString() ] = typeHandler;
//		outln("Registering: ",typeHandler.getHandledTypeName()," : ",typeHandler.getHandledType().toString()," : ",typeHandler);
	};

	T.getTypeHandler ::= fn([String,Type] nameOrType){
		var handler = this.registeredTypes[nameOrType];
		return (!handler && this.baseRegistry) ? this.baseRegistry.getTypeHandler(nameOrType) : handler;
	};
}


// ---------------------------------------------------------------------------

/*! A Context is used for one de-/serialization process during which 
	referenced objects get context-unique identifiers.	*/
{
	var T = Context = new Type;
		
	T._printableName @(override) ::= $Context;

	T.objRegistry_NrToObjId @(private,init) := Map;
	T.objRegistry_ObjIdToObj @(private,init) := Map;
	T.typeRegistry := void;
	T.id @(private,init) := fn(){	return (time()%1000000).format(0,false); };
	T.counter @(private) := 0;
	T.globalObjCounter ::= 0;	// (static) used to easily generate unique object ids during one execution process

	//! (ctor)
	T._constructor ::= fn(TypeRegistry typeRegistry = defaultRegistry){
		this.typeRegistry = typeRegistry;
	};

	//! Helper
	T.getAttributeDescription ::= fn(obj){
		var m = new Map;
		foreach(obj._getAttributes() as var key,var value){
			if(!key.beginsWith("__"))
				m[key] =  this.createDescription(value);
		}
		return m;
	};

	//! Helper
	T.applyAttributesFromDescription ::= fn(obj,Map d){
		foreach(d as var key,var subDescription){
			if(key.beginsWith("__") || (key.beginsWith("##")&&key.endsWith("##")) )
				continue;
			obj.setAttribute(key,this.createObject(subDescription),EScript.ATTR_NORMAL_ATTRIBUTE);
		}
	};

	/*! Create a description for the given object.
		\note Use this function inside TypeHandlers to serialize objects / attributes; 
				From outside, use .serialize(...) to create a String. */
	T.createDescription ::= fn(obj){
		if(void===obj) // special case
			return void;

		for(var type = obj.getType(); type ; type = type.getBaseType() ){
			var typeHandler = this.typeRegistry.getTypeHandler(type);
			if(typeHandler){
				return typeHandler.createDescription(this,obj);	
			}
		}
		Runtime.warn("Can't serialize "+obj.toString()+" of type '"+obj.getType()+"'");
		return obj.toString();
	};
	
	/*! Create an Object from the given String */
	T.createFromString ::= fn(String s){	return this.createObject(parseJSON(s));	};

	/*! Create an Object from the given description. */
	T.createObject ::= fn(description){
		// special cases
		if(description---|>String){
			if(description.beginsWith("##REF(")){
				var refId = description.substr(6,-3);
				var obj = this.findObject(refId);
				if(void==obj)
					Runtime.warn("Unknown reference '"+refId+"'.");
				return obj;
			}
			return description;
		}else if( void==description || description---|>Number || description---|>Bool ){
			return description;
		}else if(description---|>Array){
			var arr=[];
			foreach(description as var subDescription)
				arr+=this.createObject(subDescription);
			return arr;
		}
		if(!(description---|>Map)){
			Runtime.warn("Unknown value");
			return void;
		}
		{
			var refId = description['##REF##'];  // for backward compatibility
			if(refId){
				var obj = this.findObject(refId);
				if(void==obj)
					Runtime.warn("Unknown reference '"+refId+"'.");
				return obj;
			}
		}
		var objId = description['##ID##'];
		if(objId){
			var obj = this.findObject(objId);
			if(obj)
				return obj;
		}
		var typeName = description['##TYPE##'];
		if(!typeName){ // special case: ordinary map
			var m = new Map;
			foreach(description as var key,var subDescription)
				m[key] = this.createObject(subDescription);
			return m;
		}else{
			var typeHandler = this.typeRegistry.getTypeHandler(typeName);
			if(!typeHandler){
				Runtime.warn("No factory for type '"+typeName+"'");
				return void;
			}
			var obj = typeHandler.createObject(this,description);
			if(objId){
				this.registerObject(obj,objId);
		//		out("registering Obj ",objId,"\n");
			}
			return obj;
		}
	};
	//! Find object's id or return false
	T.findObjectId ::= fn(obj){
		return obj.isSet( $__ObjectSerialization_objNr) ? 
				this.objRegistry_NrToObjId[obj.__ObjectSerialization_objNr] : false;
	};
	//! Find object by id
	T.findObject ::= fn(String objId){
		return this.objRegistry_ObjIdToObj[objId];
	};
	
	//! (internal)
	T.registerObjectIfNecessary  ::= fn(obj,Map description){
		var objId = description['##ID##'];
		if(objId && !this.findObject(objId)){
			this.registerObject(obj,objId);
		}
	};

	static globalObjCounter = 0;
	//! (internal)
	T.registerObject ::= fn(obj,_id = false){
		// The unique objNr is needed to be able to handle one object within several contexts.
		if(!obj.isSet($__ObjectSerialization_objNr))
			obj.__ObjectSerialization_objNr := ++globalObjCounter;
		
		var objNr = obj.__ObjectSerialization_objNr;
		var objId = _id ? _id : this.id+"."+ (++this.counter).format(0,false);
		this.objRegistry_NrToObjId[objNr] = objId;
		this.objRegistry_ObjIdToObj[objId] = obj;
		return objId;
	};
	T.serialize ::= fn(obj){	return toJSON(createDescription(obj));	};
}

// ---------------------------------------------------------------------------

/*! (internal) Manages the (de-)serialization process of objects of one specific type. 
	\note For almost all cases, use the specialized ObjectSerialization.GenericTypeHandler instead.
*/

{
	var T = TypeHandler = new Type;
	T._printableName @(override) ::= $TypeHandler;

	T.type @(private) := void;
	T.typeName @(private) := "";

	//! (ctor)
	T._constructor ::= 	fn(Type _type,String _typeName){
		type = _type;
		typeName = _typeName;
	};
	
	//! ---o
	T.createDescription ::= fn(Context ctxt,obj){	Std.ABSTRACT_METHOD();	};

	//! ---o
	T.createObject ::= 		fn(Context ctxt,Map description){	Std.ABSTRACT_METHOD();	};

	T.getHandledType ::= 			fn(){	return type;};
	T.getHandledTypeName ::= 		fn(){	return typeName;};
}

// ------------

//! GenericTypeHandler ---|> TypeHandler
{
	var T = GenericTypeHandler = new Type(TypeHandler);
	T._printableName @(override) ::= $GenericTypeHandler;
	
	T.trackIdentity @(private) := false;

	T.addInitializer ::=			fn(fun){	doInitializeObject += fun;	return this;	};
	T.addDescriber ::=				fn(fun){	doDescribeObject += fun;		return this;	};
	
	//! ---|> TypeHandler
	T.createDescription @(override) ::= fn(Context ctxt,obj){
		// object already serialized in this context? ---> return a reference.
		var id = ctxt.findObjectId(obj);
		if(id)
//			return {'##REF##' : id};
			return "##REF("+id+")##";
		
		// Should the object be referenced? ---> store it in the context's registry with a context wide id.
		var description = new Map;
		if(this.trackIdentity)
			description['##ID##'] = ctxt.registerObject(obj);
		
		description['##TYPE##'] = getHandledTypeName();
		
		this.doDescribeObject(ctxt,obj,description);
		
		return description;
	};

	//! ---|> TypeHandler
	T.createObject @(override) ::= 		fn(Context ctxt,Map description){	
		var obj = this.objectFactory(ctxt, this.getHandledType(), description);
		ctxt.registerObjectIfNecessary(obj,description); // if trackIdentity??
		this.doInitializeObject(ctxt, obj, description);
		return obj;
	};
	
	T.doDescribeObject @(private,init) := Std.require('Std/MultiProcedure');
	T.doInitializeObject @(private,init) := Std.require('Std/MultiProcedure');

	/*! Use IdentityTracking to identify Objects that are referenced multiple times
		in one serialization process.	*/
	T.enableIdentityTracking ::=	fn(){	this.trackIdentity = true;	return this;	};
	
	T.getDescribers  ::= 			fn(){	return this.doDescribeObject;	};
	T.getFactory  ::= 				fn(){	return this.objectFactory;		};
	T.getInitializers  ::= 			fn(){	return this.doInitializeObject;	};
	T.getIdentityTracking  ::= 		fn(){	return this.trackIdentity;	};
	
	/*!	Init the describers, factory, initializers and trackIdentity-marker from
		another TypeHandler. This can e.g. be used to build a handler for an inheriting type
		based on the handler of the type's base type.	*/
	T.initFrom ::= fn(GenericTypeHandler other){
		this.trackIdentity = other.getIdentityTracking();
		this.doDescribeObject = other.getDescribers().clone();
		this.doInitializeObject = other.getInitializers().clone();
		this.setFactory(other.getFactory());
		return this;
	};

	//! ---o
	T.objectFactory @(private) ::= fn(ctxt,Type actualType,Map description){
		return new actualType;
	};

	/*! Set a factory method(ctxt,Type type,Map description) to create the object.
		Per default, a default constructor is used ( "new type" ).	*/
	T.setFactory  ::= fn(fun){	this.objectFactory@(override,private) := fun;	return this;	};
}

// -------------------------------------------------------------------------------------------------------------------------
// Supported types

//! The default registry
defaultRegistry = new TypeRegistry(void); // create TypeRegistry without base


// Builtin EScript Types
// ----------------------

// Types that can directly be expressed in JSON (Array,Bool,Map,Number)-

{	// Array
	var th = new TypeHandler(Array,"Array");
	th.createDescription @(override) :=	fn(ctxt,Array obj){
		var description = [];
		foreach(obj as var value)
			description += ctxt.createDescription(value);
		return description;
	};
	defaultRegistry.registerTypeHandler(th);
}
{	// Bool (directly expressed by its description)
	var th = new TypeHandler(Bool,"Bool");
	th.createDescription @(override) :=	fn(ctxt,Bool obj){return obj;};
	defaultRegistry.registerTypeHandler(th);
}
{	// Number (directly expressed by its description)
	var th = new TypeHandler(Number,"Number");
	th.createDescription @(override) :=	fn(ctxt,Number obj){return obj;};
	defaultRegistry.registerTypeHandler(th);
}
{	// String (with special case for reserved values)
	var th = new TypeHandler(String,"String");
	th.createDescription @(override) := fn(ctxt,String obj){
		if(obj.beginsWith("##")){
			return {	'##TYPE##':"String", 'str' : obj	};
		}else{
			return obj;
		}
	};
	// only called for the special case having an explicit ##TYPE## field
	th.createObject @(override) := fn(Context ctxt,Map description){	
		return description['str'];
	};
	defaultRegistry.registerTypeHandler(th);
}
{	// Map (with special case for reserved keys)
	var th = new TypeHandler(Map,"Map");
	th.createDescription @(override) := fn(ctxt,Map obj){
		var containsReservedKeys = false;
		var description = new Map;
		foreach(obj as var key,var value){
			description[key] = ctxt.createDescription(value);
			containsReservedKeys |= key.beginsWith("##");
		}
		if(containsReservedKeys){
			return {
				'##TYPE##' : "Map",
				'entries' : description
			};
		}else{
			return description;
		}
	};
	// only called for the special case having an explicit ##TYPE## field
	th.createObject @(override) := fn(Context ctxt,Map description){
		var m = new Map;
		var entries = description['entries'];
		if(entries)
			foreach(entries as var key,var subDescription)
				m[key] = ctxt.createObject(subDescription);
		return m;
	};
	defaultRegistry.registerTypeHandler(th);
}

// -----

// ExtObject
defaultRegistry.registerType(ExtObject,"ExtObject")
	.enableIdentityTracking()
	.addDescriber(fn(ctxt,ExtObject obj,Map d){
		var attr = ctxt.getAttributeDescription(obj);
		if(!attr.empty())
			d['attr'] = attr;
	})
	.addInitializer(fn(ctxt,ExtObject obj,Map d){
		var attr = d['attr'];
		if(attr)
			ctxt.applyAttributesFromDescription(obj,attr);
	});

// Identifier
defaultRegistry.registerType(Identifier,"Identifier")
	.addDescriber(fn(ctxt,Identifier obj,Map d){	d['name'] = obj.toString();	})
	.setFactory(fn(ctxt,Type actualType,Map d){		return new Identifier(d['name']);	});


// Delegate
defaultRegistry.registerType(Delegate,"Delegate")
	.addDescriber(fn(ctxt,Delegate obj,Map d){	
		d['obj'] = ctxt.createDescription(obj.getObject());
		d['fun'] = ctxt.createDescription(obj.getFunction());
	})
	.setFactory(fn(ctxt,Type actualType,Map d){		
		return new Delegate( ctxt.createObject(d['obj']), ctxt.createObject(d['fun']) );
	});

// UserFunction
defaultRegistry.registerType(UserFunction,"UserFunction")
	.enableIdentityTracking()
	.addDescriber(fn(ctxt,UserFunction obj,Map d){
		if(obj.usesStaticData())
			Runtime.warn("Serializing UserFunction that relies on static data! "+obj.toString());

		var attr = ctxt.getAttributeDescription(obj);
		if(!attr.empty())
			d['attr'] = attr;
		d['code'] = obj.getCode();
	})
	.setFactory(fn(ctxt,Type actualType,Map d){		return eval("("+d['code']+");");	})
	.addInitializer(fn(ctxt,UserFunction obj,Map d){
		var attr = d['attr'];
		if(attr)
			ctxt.applyAttributesFromDescription(obj,attr);
	});

// -------------------------------------------------------------------------------
var ObjectSerialization = new Namespace;
Std.ObjectSerialization := ObjectSerialization;
ObjectSerialization.Context := Context;
ObjectSerialization.TypeRegistry := TypeRegistry;
ObjectSerialization.defaultRegistry := defaultRegistry;

ObjectSerialization.serialize := fn(obj){
	return (new Context).serialize(obj);
};
ObjectSerialization.create := fn([String,Map] stringOrDescription){
	return stringOrDescription---|>String ? 
				(new Context).createFromString(stringOrDescription) :
				(new Context).createObject(stringOrDescription);
};
ObjectSerialization.registerType := defaultRegistry->defaultRegistry.registerType;

return ObjectSerialization;
