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

		public:
			void init() override;
		};
	}
}