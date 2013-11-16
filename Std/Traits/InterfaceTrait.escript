// InterfaceTrait.escript
// This file is part of the EScript StdLib library.
// See copyright notice in ../basics.escript
// ------------------------------------------------------

loadOnce(__DIR__+"/TraitBasics.escript");
loadOnce(__DIR__+"/../MultiProcedure.escript");

declareNamespace($Std,$Traits);

/*! InterfaceTrait ---|> Trait */
Std.Traits.InterfaceTrait := new Type(Std.Traits.Trait);
{
	var T = Std.Traits.InterfaceTrait;
	T._printableName @(override) ::= $InterfaceTrait;
	
	T.multipleUsesAllowed @(override) := true;

	//! ---|> Trait
	T.init @(const,override) ::= fn(obj,Array methodNames){
		foreach(methodNames as var name){
			if(!obj.getAttribute(name)){
				Runtime.warn("Object does not fulfill interface. Missing "+obj+"."+name);
			}
		}
	};
}

// Std._registerModuleResult("Std/Traits/InterfaceTrait",Std.Traits.InterfaceTrait); // support loading with Std.requireModule and loadOnce.
return Std.Traits.InterfaceTrait ;
// ---------------------------------------
