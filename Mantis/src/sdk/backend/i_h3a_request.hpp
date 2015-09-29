#pragma once

namespace backend
{
	class i_h3a_request
	{
	public:
		virtual ~i_h3a_request() {}

		virtual char* getService() = 0;
		virtual int unkownFunction00() = 0;
		virtual unsigned short unkownFunction01() = 0;
		virtual unsigned short unkownFunction02(__int16 a2) = 0; // Set's this+4 to a2
		virtual char unknownFunction03() = 0;
		virtual unsigned short unknownFunction04(__int16 a2) = 0; // Set's this+6 to a2
	};
}