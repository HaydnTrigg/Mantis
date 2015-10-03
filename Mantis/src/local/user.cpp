#include "user.hpp"
#include <Windows.h>
#include <utils/logger.hpp>
#include <bcrypt.h>
#include <SubAuth.h>

#include <boost/algorithm/hex.hpp>

#pragma comment(lib, "bcrypt")
#include <vector>

using namespace mantis::local;

bool user::calculateHash(std::wstring p_password)
{
	BCRYPT_ALG_HANDLE s_algorithm = nullptr;
	BCRYPT_HASH_HANDLE s_hashHandle = nullptr;

	auto s_result = BCryptOpenAlgorithmProvider(&s_algorithm, BCRYPT_SHA512_ALGORITHM, nullptr, 0);
	if (!NT_SUCCESS(s_result))
	{
		WriteLog("Could not open algorithm provider (%x).", s_result);
		return false;
	}

	// Calculate the size of the buffer to hold the hash object
	unsigned long s_hashObjectLength = 0, s_dataLength = 0;
	s_result = BCryptGetProperty(s_algorithm, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PBYTE>(&s_hashObjectLength), sizeof(s_hashObjectLength), &s_dataLength, 0);
	if (!NT_SUCCESS(s_result))
	{
		WriteLog("Could not get object length property (%x).", s_result);

		// Clean up our mess
		if (s_algorithm)
			BCryptCloseAlgorithmProvider(s_algorithm, 0);

		return false;
	}

	// Allocate the hash object on the heap
	auto s_hashObject = static_cast<PBYTE>(HeapAlloc(GetProcessHeap(), 0, s_hashObjectLength));
	if (!s_hashObject)
	{
		WriteLog("Memory allocation failed.");

		if (s_algorithm)
			BCryptCloseAlgorithmProvider(s_algorithm, 0);

		return false;
	}

	unsigned long s_hashLength = 0;
	s_result = BCryptGetProperty(s_algorithm, BCRYPT_HASH_LENGTH, reinterpret_cast<PBYTE>(&s_hashLength), sizeof(s_hashLength), &s_dataLength, 0);
	if (!NT_SUCCESS(s_result))
	{
		WriteLog("Could not get the length of the hash (%x).", s_result);

		if (s_algorithm)
			BCryptCloseAlgorithmProvider(s_algorithm, 0);

		if (s_hashObject)
			HeapFree(GetProcessHeap(), 0, s_hashObject);

		return false;
	}

	// allocate hash buffer
	auto s_hash = static_cast<PBYTE>(HeapAlloc(GetProcessHeap(), 0, s_hashLength));
	if (!s_hash)
	{
		WriteLog("Could not allocate memory to hold the buffer (%x).", s_hash);

		if (s_algorithm)
			BCryptCloseAlgorithmProvider(s_algorithm, 0);

		if (s_hashObject)
			HeapFree(GetProcessHeap(), 0, s_hashObject);

		return false;
	}

	// Create hash
	s_result = BCryptCreateHash(s_algorithm, &s_hashHandle, s_hashObject, s_hashObjectLength, nullptr, 0, 0);
	if (!NT_SUCCESS(s_result))
	{
		WriteLog("Could not create hash (%x).", s_result);

		if (s_algorithm)
			BCryptCloseAlgorithmProvider(s_algorithm, 0);

		if (s_hashObject)
			HeapFree(GetProcessHeap(), 0, s_hashObject);

		if (s_hash)
			HeapFree(GetProcessHeap(), 0, s_hash);

		return false;
	}

	// Hash some data
	s_result = BCryptHashData(s_hashHandle, PBYTE(p_password.c_str()), p_password.length() * sizeof(wchar_t), 0);
	if (!NT_SUCCESS(s_result))
	{
		WriteLog("Could not hash some data (%x).", s_result);

		if (s_algorithm)
			BCryptCloseAlgorithmProvider(s_algorithm, 0);

		if (s_hashHandle)
			BCryptDestroyHash(s_hashHandle);

		if (s_hashObject)
			HeapFree(GetProcessHeap(), 0, s_hashObject);

		if (s_hash)
			HeapFree(GetProcessHeap(), 0, s_hash);

		return false;
	}

	// close hash
	s_result = BCryptFinishHash(s_hashHandle, s_hash, s_hashLength, 0);
	if (!NT_SUCCESS(s_result))
	{
		WriteLog("Could not finish the hash (%x).", s_result);

		if (s_algorithm)
			BCryptCloseAlgorithmProvider(s_algorithm, 0);

		if (s_hashHandle)
			BCryptDestroyHash(s_hashHandle);

		if (s_hashObject)
			HeapFree(GetProcessHeap(), 0, s_hashObject);

		if (s_hash)
			HeapFree(GetProcessHeap(), 0, s_hash);

		return false;
	}
	
	std::vector<unsigned char> s_arrayVector;
	for (unsigned int i = 0; i < s_hashLength; ++i)
		s_arrayVector.push_back(s_hash[i]);

	std::string s_hexOutput;
	boost::algorithm::hex(s_arrayVector.begin(), s_arrayVector.end(), back_inserter(s_hexOutput));
	WriteLog("Password Hash: %s", s_hexOutput.c_str());

	if (s_algorithm)
		BCryptCloseAlgorithmProvider(s_algorithm, 0);

	if (s_hashHandle)
		BCryptDestroyHash(s_hashHandle);

	if (s_hashObject)
		HeapFree(GetProcessHeap(), 0, s_hashObject);

	if (s_hash)
		HeapFree(GetProcessHeap(), 0, s_hash);

	return true;
}
