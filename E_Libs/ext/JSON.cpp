#include "JSON.h"

#include "../../EScript/EScript.h"
#include "../../EScript/Parser/Tokenizer.h"

using namespace EScript;

/*!	(static)	*/
std::string JSON::toJSON(Object * obj,bool formatted/*=true*/){
    std::ostringstream s;
    JSON::toJSON(s,obj,formatted);
    return s.str();
}

/*!	(static)	*/
void JSON::toJSON(std::ostringstream & out,Object * obj,bool formatted/*=true*/,int level/*=0*/){
    if(dynamic_cast<Void *>(obj) || (obj==NULL) ){
        out<<"null";
    }else if(Object * number=dynamic_cast<Number *>(obj)){
        out<<number->toFloat();
    }else if(Bool * b=dynamic_cast<Bool *>(obj)){
        out<<b->toString();
    }else if(String * s=dynamic_cast<String *>(obj)){
        out<<"\""<<s->toString()<<"\""; // TODO! Escape
    }else if(Array * a=dynamic_cast<Array *>(obj)){
        ERef<Iterator> itRef=dynamic_cast<Iterator *>(a->getIterator());

        out<<"[";
        if(formatted){
            out<<"\n";
        }
        bool first=true;
        while ( (!itRef.isNull()) && (! itRef->end())) {
            ObjRef valueRef=itRef->value();
            if(first){
                first=false;
            }else{
                out<<",";
                if(formatted){
                    out<<"\n";
                }
            }
            if(formatted){
                for(int j=0;j<level+1;j++)
                    out <<"\t";
            }
            toJSON(out,valueRef.get(),formatted,level+1);
            itRef->next();
        }
        if(formatted){
            out <<"\n";
            for(int j=0;j<level;j++)
                out <<"\t";
        }
        out<<"]";
    }else if(Map * m=dynamic_cast<Map *>(obj)){
        ERef<Iterator> itRef=dynamic_cast<Iterator *>(m->getIterator());
        out<<"{";
        if(formatted){
            out<<"\n";
        }
        bool first=true;
        while ( (!itRef.isNull()) && (! itRef->end())) {

            ObjRef keyRef=itRef->key();
            ObjRef valueRef=itRef->value();
            if(first){
                first=false;
            }else{
                out<<",";
                if(formatted){
                    out<<"\n";
                }
            }
            if(formatted){
                for(int j=0;j<level+1;j++)
                    out <<"\t";
            }
            out<<"\""<<keyRef.toString()<<"\":"; // TODO! Escape
            toJSON(out,valueRef.get(),formatted,level+1);

            itRef->next();
        }
        if(formatted){
            out <<"\n";
            for(int j=0;j<level;j++)
                out <<"\t";
        }
        out<<"}";
    }//  todo Object
    else {
        out<<"\""<<obj->toString()<<"\"";
    }
}

/*!	helper	*/
Object * _parseJSON(Tokenizer::tokenList::iterator & cursor,const Tokenizer::tokenList::iterator end){
    if(cursor==end)
        return NULL;
    Token * token=(*cursor);
    if(dynamic_cast<TOperator*>(token)&& token->toString()=="_-"){ /// unary minus
        ++cursor;
        token=(*cursor);
        TObject * tObj=dynamic_cast<TObject *>(token);
        if(tObj==NULL || !dynamic_cast<Number*>(tObj->obj.get())){
            std::cout << "Number expected! \n";
            return NULL;
        }

        ++cursor;

        return Number::create(-tObj->obj->toDouble());
//    }else if(dynamic_cast<String*>(token) || dynamic_cast<Number*>(token) || dynamic_cast<Bool*>(token)){
//        ++cursor;
//        return token->clone();
//    }else if(dynamic_cast<Void*>(token)){
//        ++cursor;
//        return Void::get();
	}else if(TObject * tObj=dynamic_cast<TObject *>(token)){
		++cursor;
		return tObj->obj->clone();
    }else if (dynamic_cast<TStartBlock *>(token)){
        Map *m=Map::create();
        ++cursor;
        while( true){
            if(cursor==end){
                std::cout << "M1 \n";
                break;
            }else  if(dynamic_cast<TEndBlock*>(*cursor)){
                cursor++;
                break;
            }
            token=(*cursor);
            TObject * tObj2=dynamic_cast<TObject *>(token);
            String * key= (tObj2==NULL ? NULL : dynamic_cast<String*>(tObj2->obj.get()));
            if(!key){
                std::cout << token->toString()<<"M2! \n";
                break;
            }
            ++cursor;
            if(cursor==end){
                std::cout << "M3! \n";
                break;
            }else if(!dynamic_cast<TColon*>(*cursor)){
                std::cout << "M3 : expected! \n";
                break;
            }
            ++cursor;
            if(cursor==end){
                std::cout << "M4! \n";
                break;
            }
            Object * o=_parseJSON(cursor,end);
            if(!o){

                std::cout << "M5! \n"<<(*cursor)->toString()<<"\n";
                break;
            }
            m->setValue(key->clone(),o);
//            ++cursor;
            if(cursor==end){
                std::cout << "M6! \n";
                break;
            }else  if(dynamic_cast<TEndBlock*>(*cursor)){
                cursor++;
                break;
            }else if(dynamic_cast<TDelimiter*>(*cursor)){
                cursor++;
                continue;
            }
            std::cout << "M7! \n";
            std::cout <<(*cursor)->toString()<<"\n";
            break;
        }
        return m;
    }else if (dynamic_cast<TStartIndex *>(token)){
        Array *a=Array::create();
        ++cursor;
        while( true){
            if(cursor==end){
                std::cout << "A1! \n";
                break;
            }else if(dynamic_cast<TEndIndex*>(*cursor)){
                cursor++;
                break;
            }
            Object * o=_parseJSON(cursor,end);
            if(!o){
                std::cout << "A2! \n";
                break;
            }
            a->pushBack(o);
//            ++cursor;
            if(cursor==end){
                std::cout << "A3! \n";
                break;
            }else  if(dynamic_cast<TEndIndex*>(*cursor)){
                cursor++;
                break;
            }else if(dynamic_cast<TDelimiter*>(*cursor)){
                cursor++;
                continue;
            }
            std::cout << "A4! \n";
            std::cout <<(*cursor)->toString()<<"\n";
            break;
        }
        return a;
    }
    else if(TIdentifier* ti=dynamic_cast<TIdentifier*>(token)){
        std::cout << "Unknown Identifier: "<<ti->toString()<<"\n";
        return NULL;

    }
    return NULL;
}

/*!	(static)	*/
Object* JSON::parseJSON(const std::string &s){
    Tokenizer t;

    Tokenizer::tokenList tokens;
    t.getTokens(s.c_str(),tokens);
    Tokenizer::tokenList::iterator it=tokens.begin();
    Object * result= _parseJSON(it,tokens.end());
    if(it!=tokens.end() && !dynamic_cast<TEndScript*>(*it)){
        std::cout << "JSON Syntax Error\n";
    }
    if(result==NULL){
        result=String::create(s);
    }
    for(it=tokens.begin();it!=tokens.end();++it){
        Token::removeReference(*it);
    }
    return result;
}
