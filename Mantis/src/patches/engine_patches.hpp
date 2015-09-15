#pragma once
#include <misc/iinit.hpp>

namespace mantis
{
	namespace patches
	{
		class engine_patches : public iinit
		{
		public:
			void init() override;
		};
	}
}