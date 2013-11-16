// ObjectExtensions.escript
// This file is part of the EScript StdLib library.
// See copyright notice in basics.escript
// ------------------------------------------------------
/*!
 **  Extensions for several EScript Types
 **/
 
// -------------------------------------
// Array extensions

//! Chunks an array into 'size' large chunks. The last chunk may contain less than 'size' elements. 
GLOBALS.Array.chunk ::= fn(size){
	var chunks = [];
	var currentChunk;
	foreach(this as var index,var obj){
		if( (index % size)==0){
			currentChunk = [];
			chunks += currentChunk;
		}
		currentChunk += obj;
	}
	return chunks;
};

Array.fill ::= fn(value){
	var end = count();
	for(var i = 0;i<end;++i)
		this[i] = value;
};

Array.fill_n ::= fn(Number start,Number size, value){
	var end = start+size;
	if( end > count()) 
		end = count();
	if( start<0 ) 
		start = 0;
	for(var i = start;i<end;++i)
		this[i] = value;
};


// -------------------------------------
// Collection extensions

//!	Calculate the average value of Maps and Arrays.
GLOBALS.Collection.avg ::= fn(){
    if(this.count()==0)
        return 0;
    var accum;
    var first = true;
    foreach(this as var v){
        if(first){
            accum = v;
            first = false;
        }else{
            accum+=v;
        }
    }
    return accum/this.count();
};

//!	 Calculate the sum of values of Maps and Arrays.
GLOBALS.Collection.sum ::= fn(){
    if(this.count()==0)
        return 0;
    var accum;
    var first = true;
    foreach(this as var v){
        if(first){
            accum = v;
            first = false;
        }else{
            accum+=v;
        }
    }
    return accum;
};


// -------------------------------------
// Math extensions

/**
 * Calculate k-combinations for the set {0, 1, 2, ..., n - 1}.
 * The binomial(n, k) subsets of size k are the possible combinations of the set of size n.
 * 
 * @param n Size of the set
 * @param k Size of the subsets
 * @return Array of binomial(n, k) arrays of size k
 * @note Code taken from http://compprog.wordpress.com/2007/10/17/generating-combinations-1/ including fix suggested in comments.
 * @see http://en.wikipedia.org/wiki/Combinations
 */
GLOBALS.Math.createCombinations := fn(Number n, Number k) {
	var nextCombination = fn(Array comb, Number n, Number k) {
		var i = k - 1;
		comb[i] = comb[i] + 1;
		while ((i > 0) && (comb[i] >= n - k + 1 + i)) {
			--i;
			comb[i] = comb[i] + 1;
		}

		if (comb[0] > n - k) { // Combination (n-k, n-k+1, ..., n) reached.
			return false; // No more combinations can be generated.
		}

		// comb now looks like (..., x, n, n, n, ..., n).
		// Turn it into (..., x, x + 1, x + 2, ...).
		for (var j = i + 1; j < k; ++j) {
			comb[j] = comb[j - 1] + 1;
		}
		return true;
	};

	var comb = [];

	// Setup the initial combination.
	for (var i = 0; i < k; ++i) {
		comb.pushBack(i);
	}

	// Generate all other combinations.
	var combinations = [comb.clone()];
	while (nextCombination(comb, n, k)) {
		combinations.pushBack(comb.clone());
	}
	return combinations;
};

//!	Set all the bits set in \param mask to the value \param b.
Number.setBitMask ::= fn(Number mask,Bool b=true){
	return b ? (this|mask) : (this^(this&mask));
};

// ------------------------------------------
