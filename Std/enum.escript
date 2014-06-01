// JSONDataStore.escript
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2014 Claudius Jähn <ClaudiusJ@live.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

/*!
\code

	static State = enum( $NEW, $RUNNING, $FINISHED );

	var f = fn(State currentState){
		out( "Current state is "+currentState._printableName ); 
		out( "Current state value is "+currentState.value ); 
		switch(currentState){
			case State.NEW:
				//...
			case State.RUNNING:
				//...
			case State.FINISHED:
				//...
		}
	};
	
	f(State.NEW);

\endcode

*/
static enum = fn(valueIds...){
	var e = new Type;
	e._printableName ::=  $Enum;
	foreach(valueIds as var index, var id){
		var enumValue = new e;
		enumValue._printableName := id;
		enumValue.value := index;
		e.setAttribute(id,enumValue,EScript.ATTR_TYPE_ATTR_BIT);
	}
	e._constructor ::= void; // prevent instantiation
	return e;
};

module.on('./StdNamespace', fn(StdNamespace){
	StdNamespace.enum := enum;
});

return enum;