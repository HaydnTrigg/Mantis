#pragma once
#include <sdk/engine/e_gametype.hpp>
#include <cstring>

namespace mantis
{
	namespace sdk
	{
		namespace engine
		{
			namespace variants
			{
				class c_game_engine_base_variant
				{
				public:
					virtual void function0();
					virtual void function1(); // Get Variant Size?
					virtual void function2();
					virtual void function3();
					virtual void function4();
					virtual void function5();
					virtual void function6();
					virtual void function7();
					virtual void function8();
					virtual void function9();

					int m_gameType;
					void* m_functionTable;
					char m_unknown_12[44];
					wchar_t m_name[16];
					char m_description[128];
					char m_author[16];

					c_game_engine_base_variant() : m_gameType(gametype_none),
						m_functionTable(nullptr)
					{
						memset(m_unknown_12, 0, sizeof(m_unknown_12));
						memset(m_name, 0, sizeof(m_name));
						memset(m_description, 0, sizeof(m_description));
						memset(m_author, 0, sizeof(m_author));
					}
				};
			}
		}
	}
}