#include <hooks/engine_hooks.hpp>
#include <utils/settings.hpp>

using namespace mantis::hooks;

HookedFunction(engine_hooks, bool, halo_verifyMapSignature, __fastcall, void* This)
{
	auto s_verifyPassed = o_halo_verifyMapSignature(This);

	auto s_value = true;
	auto s_ret = utils::settings::getInstance()->getSetting("mapVerificationEnabled", s_value);
	if (!s_ret)
		return s_verifyPassed;

	return s_value;
}