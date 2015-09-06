#pragma once
#include <misc/iinit.hpp>
#include <misc/macros.hpp>
namespace mantis
{
	namespace hooks
	{
		class engine_hooks : public iinit
		{
			DeclareFunction(int, halo_gameTick, __stdcall);
			DeclareFunction(bool, halo_verifyMapSignature, __fastcall, void* This);

		public:
			void init() override;
		};
	}
}