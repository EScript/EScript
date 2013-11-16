// info.escript
// This file is part of the EScript StdLib library.
// See copyright notice in basics.escript
// ------------------------------------------------------

loadOnce(__DIR__ + "/basics.escript");

// -------------------------------------
// info

/*! info(...) is a global command that outputs infos for the given parameter.
	\code info( 1 );
	\code info( 1.getType() );
	\code info( 1.getType().getBaseType() );

	To add a custom information provider for a specific type, call
	\code info += [ MyType, fn(objOfMyType,Array results){ results += "AdditionalInfo: "+objOfMyType.getSpecificInfo(); }];

	To add an additional short info to a function, you can call
	\code myFunction._shortInfo := "Some short usage hint.";

	To get the info instead of directly printing it, use info.get( obj );
*/
Std.info := new ExtObject;

var info = Std.info;

info._shortInfo := "Call to print information on the given object.";
info.registry @(private) := new Map;
info._call := fn( caller, obj ){
	outln("-"*50,"\n",this.get(obj).implode("\n"));
};
info."+=" := fn(arr){
	this.registry[arr[0]] = arr[1];
	return this;
};
/* stereo
	mode: left, right, sideBySide, crossEye
	setRightEyeOffset
	enableRightRye
	enableLeftRye
	sideBySideEnabled = dataWrapper
	*/
info.get := fn(obj,type = void){
	var result = [];
	if(void===obj){ // special case!
		result += "This object is Void -- the empty object. ";
	}else{
		var handlers = [];
		if(!type)
			type = obj.getType();
		for(;type;type=type.getBaseType()){
			if(registry[type]){
				registry[type](obj,result);
				break;
			}
		}else{
			result += "???" + obj.toDbgString();
		}
	}
	return result;
};

info.getShortDescription := fn(obj){
	var shortInfo = getShortInfo(obj);
	if(shortInfo)
		return "(" + shortInfo + ") " +obj.toDbgString();
	return obj.toDbgString();
};

info.getShortInfo := fn(obj){
	return obj.isSet($_shortInfo) ? obj._shortInfo : false;
};

// -----------------------------------------------------------------------

info+=[Object,fn(obj,Array result){
	var str = obj.toDbgString();
	if(str.length()>70 )
		str = str.substr(0,60)+"... (shortend)";
	result += "Object: '" + str +"'";
	if(var shortInfo = getShortInfo(obj)){
		result	+= "Info: " +shortInfo;
	}
	var types = [];
	for(var type = obj.getType();type;type = type.getBaseType())
		types += type;
	result += "Types: " + types.implode(" ---|> ") ;

}];

info+=[Number,fn(obj,Array result){
	result += "This object is of the integral value-type Number.";
}];

info+=[Bool,fn(obj,Array result){
	result += "This object is of the integral value-type Bool.";
}];

info+=[String,fn(obj,Array result){
	result += "This object is of the integral value-type String. " +
		"The string's length is "+obj.length()+".";
}];

info+=[Type,fn(obj,Array result){
	result += "This object is a Type.";
	var types = [];
	for(var type = obj;type;type = type.getBaseType())
		types += type;
	result += "Inheritance: " + types.implode(" ---|> ") ;

	var attributes = obj._getAttributes();
	if(!attributes.empty()){
		result += "Attributes (" + attributes.count() + "):\n------------";
		foreach(attributes as var key,var value){
			var props = [];
			var properties = obj.getAttributeProperties(key);
			if( (properties&EScript.ATTR_TYPE_ATTR_BIT)>0 )
				props += "type";
			if( (properties&EScript.ATTR_PRIVATE_BIT)>0 )
				props += "private";
			if( (properties&EScript.ATTR_CONST_BIT)>0 )
				props += "const";
			if( (properties&EScript.ATTR_INIT_BIT)>0 )
				props += "init";
			if( (properties&EScript.ATTR_OVERRIDE_BIT)>0 )
				props += "override";
			if(!props.empty()){
				key += " @(" + props.implode(',') + ")";
			}
			result += "> " + key + (" "*(20-key.length()))+ " := " + this.getShortDescription(value);
		}
	}
	result += "(Hint: use .getBaseType() for inherited members)";
}];

info+=[ExtObject,fn(obj,Array result){
//	result += "This object internally is an ExtObject.";
	var attributes = obj._getAttributes();
	if(!attributes.empty()){
		result += "Attributes (" + attributes.count() + "):\n------------";
		foreach(attributes as var key,var value){
			var props = [];
			var properties = obj.getAttributeProperties(key);
			if( (properties&EScript.ATTR_TYPE_ATTR_BIT)>0 )
				props += "type";
			if( (properties&EScript.ATTR_PRIVATE_BIT)>0 )
				props += "private";
			if( (properties&EScript.ATTR_CONST_BIT)>0 )
				props += "const";
			if( (properties&EScript.ATTR_INIT_BIT)>0 )
				props += "init";
			if( (properties&EScript.ATTR_OVERRIDE_BIT)>0 )
				props += "override";
			if(!props.empty()){
				key += " @(" + props.implode(',') + ")";
			}
			result += "> " + key + (" "*(20-key.length()))+ " := " + this.getShortDescription(value);
		}
	}
	result += "(Hint: use .getType() for inherited members)";
}];

info+=[Collection,fn(obj,Array result){
	result += "This object is a Collection having "+obj.count()+" entries:";
	var i = 0;
	foreach(obj as var key,var value){
		result += " '"+key+"'" + "\t: "+this.getShortDescription(value);
		if(++i>10){
			result += "... (Hint:use print_r(obj) to see all entries)";
		}
	}
}];

info+=[UserFunction,fn(obj,Array result){
	result += "------------";
	result += "This object is an UserFunction expecting "+obj.getMinParamCount()+" to "+obj.getMaxParamCount()+" parameters.";
	result += "It is defined in the file '"+obj.getFilename()+"'.";
}];
//
//info += [Set,fn(obj,Array result){
//
//	result += "------------";
//	result += "Set-content: ";
//	result += "{";
//	foreach(obj as var entry){
//		result += "\t" + entry;
//	}
//	result += "}";
//}];

Std._registerModule('Std/info',info); // support loading with Std.requireModule and loadOnce.

return info;
