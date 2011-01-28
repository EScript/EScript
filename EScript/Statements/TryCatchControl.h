// TryCatchControl.h
// This file is part of the EScript programming language.
// See copyright notice in EScript.h
// ------------------------------------------------------
#ifndef TRYCATCHCONTROL_H
#define TRYCATCHCONTROL_H

#include "../Object.h"

namespace EScript {
class Block;

/*! [TryCatchControl]  ---|> [Object]   */
class TryCatchControl : public Object {
		ES_PROVIDES_TYPE_NAME(TryCatchControl)
	public:
		TryCatchControl(Object * tryBlock,Block * catchBlock,identifierId varNameId);
		virtual ~TryCatchControl();

		/// ---|> [Object]
		virtual std::string toString()const;
		virtual Object * execute(Runtime & rt);

	private:
		ObjRef tryBlockRef;
		ERef<Block> catchBlockRef;
		identifierId varNameId;
};
}

#endif // TRYCATCHCONTROL_H
