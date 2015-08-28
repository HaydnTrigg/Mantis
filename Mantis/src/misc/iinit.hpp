#pragma once

namespace mantis
{
	class iinit
	{
	public:
		virtual void init() = 0;
		virtual void preInit(){};
		virtual void postInit(){};
	};
}