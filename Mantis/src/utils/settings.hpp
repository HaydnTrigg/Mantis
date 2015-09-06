#pragma once
#include <misc/iinit.hpp>

namespace mantis
{
	namespace utils
	{
		class settings : iinit
		{
			
		public:
			static bool m_mapVerificationEnabled;
			void init() override;
		};
	}
}