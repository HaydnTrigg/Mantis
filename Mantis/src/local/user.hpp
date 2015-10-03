#pragma once
#include <string>

namespace mantis
{
	namespace local
	{
		class user
		{
			std::wstring m_name;
			unsigned short m_rank;

		public:
			bool calculateHash(std::wstring p_password);
		};
	}
}