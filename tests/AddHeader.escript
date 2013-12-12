// AddHeader.escript
// This file is part of the EScript programming language (http://escript.berlios.de)
//
// Copyright (C) 2011-2013 Claudius Jähn <claudius@uni-paderborn.de>
// Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------

//! example:
	// Runtime.cpp
	// This file is part of the EScript programming language (http://escript.berlios.de)
	//
	// Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	// Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	//
	// Licensed under the MIT License. See LICENSE file for details.
	// ---------------------------------------------------------------------------------


// Small example script used to add and update the license note at the beginning of every file.

var rootDir = args[2];

if(!rootDir){
	out("Usage: EScript "+__FILE__+" {rootDirectory} [svnLog]\n");
	return;
}

var useSVNLog = false;
for(var i=3;i<args.count();++i){
	if(args[i]=="svnLog"){
		useSVNLog = true;
	}else{
		Runtime.exception("Error: unknown option:"+args[i]);
	}
}

static lineEnding = "\n";
static tempFileName = "log.tmp";
static authorAliases = {
	"claudiusj" : "Claudius Jähn <claudius@uni-paderborn.de>",
	"eikel" : "Benjamin Eikel <benjamin@eikel.org>"
};

var getCopyrightFromSVN = fn(file){
	IO.saveTextFile(tempFileName,"");
	system("svn log "+file+" >"+tempFileName);
	var commits = IO.loadTextFile(tempFileName).split("-"*72);
	if(commits.count()<=1){
		outln(commits[0]);
		return false;
	}
	var authors = new Map; // name -> [years]
	foreach(commits as var commitText){
		var header = commitText.trim().split("\n")[0];
		if(!header)
			continue;
//		outln(header);
//		outln(commitText);
		// r472 | claudiusj | 2013-03-07 18:47:18 +0100 (Do, 07 Mrz 2013) | 1 line
		var dateParts = header.split("|");
		var author = dateParts[1].trim();
		var year = 0 + dateParts[2].split("-",2)[0].trim();
		if(!authors[author]){
			authors[author] = [year];
		}else{
			if(!authors[author].contains(year))
				authors[author] += year;
		}
//		outln(author,":",year);
	}
	// Copyright (C) 2008-2013 Benjamin Eikel <benjamin@eikel.org>
//	print_r(authors);
	var str = "";
	foreach(authors as var author,var years){
		str+="// Copyright (C) ";

		years.sort();
		var lastYear = 0;
		foreach(years as var i, var year){
			if(year==lastYear+1 && years[i+1] == lastYear+2 ){
				// skip
			}else if(year==lastYear+1){
				str +="-"+year;
			}else if(i>0){
				str +=","+year;
			}else {
				str +=year;
			}
			lastYear=year;
		}
		if(authorAliases[author])
			author = authorAliases[author];
		str+=" "+author+lineEnding;
	}

	return str;

};

static endings = [".h",".cpp",".escript"];

var files = IO.dir( rootDir , IO.DIR_FILES|IO.DIR_RECURSIVE );
files.filter( fn(filename){
	foreach(endings as var ending)
		if( filename.endsWith(ending) )
			return true;
	return false;
} );

foreach(files as var nr, var file){
	out(file,"\t");

	var copyrightInfo = useSVNLog ? getCopyrightFromSVN(file) : "";
	if(!copyrightInfo)
		continue;

	var input = IO.loadTextFile(file); // read file

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
		output+="// This file is part of the EScript programming language (http://escript.berlios.de)" + lineEnding;
		output+="//" + lineEnding;
		output+=copyrightInfo;
		output+="//" + lineEnding;
		output+="// Licensed under the MIT License. See LICENSE file for details." + lineEnding;
		output+="// ---------------------------------------------------------------------------------" + lineEnding;
	}

	// remove additional end lines
	while(lines.back().trim().empty()){
		lines.popBack();
	}
	// process lines
	foreach(lines as var line){
		// ignore old header
		if(!headerRead && line.beginsWith("//"))
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
			IO.saveTextFile(file,output);
		outln("***changed***");
		}catch(e){
			outln("\n",e);
		}
	}else{
		outln("unchanged\n");
	}

//	break;
}
outln("Bye!");
