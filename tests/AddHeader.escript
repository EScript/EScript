// AddHeader.escript (2011-01-23)
// Small example script used to add and update the licence note at the beginning of every file.

var rootDir = args[2];

if(!rootDir){
	out("Usage: EScript "+__FILE__+" {rootDirectory} \n");
	return;
}

var files = IO.dir( rootDir , IO.DIR_FILES|IO.DIR_RECURSIVE );
files.filter( fn(filename){return filename.endsWith(".h") || filename.endsWith(".cpp"); } );

foreach(files as var file){
	out(file,"\t");

	var input = IO.fileGetContents(file); // read file

	// use '\n' as line ending
	var lineEnding="\n";

	// extract lines
	var lines = input.split("\n");
	if(lines.empty()){
		out("empty\n");
		continue;
	}

	var output="";
	var headerRead = false;
	if(lines[0].beginsWith("/*")){
		headerRead = true;
	}else{
		output+="// " + file.substr(file.rFind("/")+1) + lineEnding;
		output+="// This file is part of the EScript programming language." + lineEnding;
		output+="// See copyright notice in EScript.h" + lineEnding;
		output+="// ------------------------------------------------------" + lineEnding;
	}

	// remove additional end lines
	while(lines.back().trim().empty()){
		lines.popBack();
	}
	// process lines
	foreach(lines as var line){
		// ignore old header
		if(!headerRead && line.beginsWith("// "))
			continue;
		headerRead = true;

		// remove whitespaces at the end and replace whitespaces at the beginnen by tabs.
		line = line.rTrim();
		if(line.beginsWith(" ")||line.beginsWith("\t")){
			var spaceCount = 0;
			var tabCount = 0;
			for(var pos = 0; true ; ++pos){
				var c = line[pos];
				if(c==" "){
					++spaceCount;
					if(spaceCount==4){
						++tabCount;
						spaceCount = 0;
					}
				}else if(c=="\t"){
					++tabCount;
				}else{
					break;
				}
			}
			line = "\t"*tabCount + " " * spaceCount + line.lTrim();
		}
		output+=line+lineEnding;
	}
	if(output!=input){
		try{
			IO.filePutContents(file,output);
		out("***changed***\n");
		}catch(e){
			out("\n",e,"\n");
		}
	}else{
		out("unchanged\n");
	}

//	break;
}
out("Bye!");
