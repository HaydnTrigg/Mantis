#pragma once
#include "c_message.h"

namespace messages
{
	class c_load_screen_message : public c_message
	{
	public:
		virtual int unknown_function_00(int a2) = 0;
		__int32 N011CDD3D; //0x0004 arg0
		__int32 N011CDD3E; //0x0008 defaults to 3
		__int32 N011CDD3F; //0x000C arg1
		__int32 N011CDD40; //0x0010 arg2
		__int32 N011CDD41; //0x0014 arg3
		__int32 N011CDD42; //0x0018 defaults to 1
		__int8 N011CDD43; //0x001C defaults to 1
		char _0x001D[3];
		__int32 N011CDD44; //0x0020 defaults to -1
		__int32 N011CDD45; //0x0024 defaults to -1
		__int32 N011CDD46; //0x0028 defaults to -1
		__int32 N011CDD47; //0x002C defaults to -1
		__int32 N011CDD48; //0x0030 defaults to -1
		__int32 N011CDD49; //0x0034 
		__int8 N011CDD4A; //0x0038 defaults to 0
	};
}