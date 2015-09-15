#pragma once
#include <utils/util.hpp>
#include <utils/logger.hpp>

#define DeclareFunction(returnType, name, callingMethod, ...) \
	typedef returnType (callingMethod* name ## _t)(__VA_ARGS__); \
	static name ## _t o_ ## name; \
	static returnType callingMethod c_ ## name(__VA_ARGS__);

#define HookedFunction(nameSpace, returnType, name, callingMethod, ...) \
	returnType callingMethod nameSpace::c_ ## name(__VA_ARGS__)

#define DeclareHookAtOffset(name, offset) \
	unsigned int s_ ## name = offset; \
	o_ ## name = (name ## _t)DetourFunction((PBYTE)s_ ## name, (PBYTE)c_ ## name);

#define DeclareHookPattern(name, offset, length, pattern, mask) \
	unsigned int s_ ## name = mantis::utils::util::find_pattern(reinterpret_cast<void*>(offset), length, pattern, mask); \
	if (!s_ ## name) { \
		WriteLog("Hooking Failed for %s.", #name); }\
				else { \
		o_ ## name = (name ## _t)DetourFunction((PBYTE)s_ ## name, (PBYTE)c_ ## name); \
		WriteLog("%s Hooked.", #name); }

#define DeclareFunctionValue(nameSpace, name) \
	nameSpace ## :: ## name ## _t nameSpace ## :: ## o_ ## name = nullptr;

#define DeclareFunctionValueOffset(nameSpace, name, offset) \
	nameSpace ## :: ## name ## _t nameSpace ## :: ## o_ ## name = offset;