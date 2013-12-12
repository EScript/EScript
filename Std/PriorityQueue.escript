// PriorityQueue.escript
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2013 Claudius Jähn <claudius@uni-paderborn.de>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

loadOnce(__DIR__ + "/basics.escript");

var T = new Type;

T._printableName @(override) ::= $PriorityQueue;

T.compare @(private) := void;
T.arr @(private,init) := Array;

T._constructor ::= fn( compareFunction = fn(a,b){return a<b;} ){
	this.compare = compareFunction;
};

T.add ::= fn(e){
	arr.pushBack(e);
	var i = count()-1;
	while(i>0){
		var p=( (i+1)/2).floor()-1;
		if( compare(e,arr[p])){
			var t = arr[p];
			arr[p]=e;
			arr[i]=t;
		}else{
			break;
		}
		i = p;
	}
};
T."+=" ::= T.add; // alias
T.count ::=			fn(){	return arr.count();	};
T.empty ::=			fn(){	return arr.empty();	};
T.clear ::=			fn(){	return arr.clear();	};
T.clone ::= fn(){
	var q = new (this.getType())(this.compare);
	(q->fn(clonedArr){	this.arr = clonedArr;}) (this.arr.clone());
	return q;
};

T.get ::=			fn(){	return arr.empty() ? void : arr[0];	};
T.extract ::= fn(){
	var size = arr.count();
	if(size>1){
		var min = arr[0];
		arr[0]=arr.popBack();
		heapify(0);
		return min;
	} else if(size==1) {
		var min = arr[0];
		arr.popBack();
		return min;
	} else return void;
};
T.heapify @(private) ::= fn(i){
	var left = ((i+1)*2)-1;
	var size = arr.count();
	if(left<size){
		var minI = i;
		var right = left+1;

		if( compare(arr[left],arr[minI]) )
			minI = left;
		if(right<size && compare(arr[right],arr[minI]))
			minI = right;
		if(minI!=i){
			var tmp = arr[i];
			arr[i] = arr[minI];
			arr[minI] = tmp;
			heapify(minI);
		}
	}
};

Std.PriorityQueue := T;
return T;
