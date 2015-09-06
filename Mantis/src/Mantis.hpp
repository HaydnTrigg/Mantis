#pragma once
#include <misc/iinit.hpp>
#include <windows.h>

namespace mantis
{
	class client : iinit
	{
		static client* m_instance;

		iinit* m_winHooks;
		iinit* m_engineHooks;

		client();
	public:
		static client* getInstance();

		void init() override;
		void preInit() override;
		void postInit() override;
	};
}