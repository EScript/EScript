// loadme.escript
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

loadTestVar++;

var testvar=1;
while(testvar<5){
	testvar++;
}
if(testvar==5)
GLOBALS.testFunction:=fn (i){
	return i*i;
};


return testvar;
