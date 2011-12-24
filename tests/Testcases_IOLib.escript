var OK="\t ok\n";
var FAILED="\t failed\n";

{
	var s="Test dum di dum"+Rand.equilikely(0,1000);
	var filename="test.txt";
	IO.filePutContents("test.txt",s);
	// get files in dir "." and remove "./"
	var files=IO.dir(".").map( fn(index,file){return IO.condensePath(file); });

//	out("\n",s,"\n");
//	out(IO.fileGetContents(filename) == s,"\n");
//	out(files.contains(filename),"\n");
//	out(IO.isFile(filename),"\n");
//	out(!IO.isFile("this is no file"),"\n");

	test( "IOLib:",
			files.contains(filename)
			&& IO.fileGetContents(filename) == s
			&& IO.fileSize(filename) == s.length()
			&& IO.isFile(filename) && !IO.isFile("this is no file") );
}
