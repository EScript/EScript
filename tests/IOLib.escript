
print_r(IO.dir("."));
print_r(IO.fileSize("test.cpp"));

a=IO.getFile("test.cpp");
//print_r(a);
var b=a.map(function(k,v){return v.trim();});
print_r(b);
IO.filePutContents("test.txt","Test dum di dum");
