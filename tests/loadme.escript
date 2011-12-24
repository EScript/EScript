
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
