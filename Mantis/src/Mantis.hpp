#pragma once
#include <misc/iinit.hpp>
#include <windows.h>

namespace mantis
{
	class client : iinit
	{
		static client* m_instance;
		client();
	public:
		static client* getInstance();

		void init();
		void preInit();
		void postInit();
	};
}