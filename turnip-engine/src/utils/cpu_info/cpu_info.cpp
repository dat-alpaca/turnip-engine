#include "cpu_info.hpp"
#include "common.hpp"

namespace tur
{
	struct CPU_ID
	{
	public:
		explicit CPU_ID(u32 eax, u32 ecx)
		{
			// Todo: write a cpuid function for linux/macos
#ifdef TUR_PLATFORM_WIN32
			__cpuidex(reinterpret_cast<int*>(registers), eax, ecx);
#endif
		}

		explicit CPU_ID(u32 eax)
		{
#ifdef TUR_PLATFORM_WIN32
			__cpuid(reinterpret_cast<int*>(registers), eax);
#endif
		}

	public:
		const u32& eax() const
		{
			return mRegisters[0];
		}

		const u32& ebx() const
		{
			return mRegisters[1];
		}

		const u32& ecx() const
		{
			return mRegisters[2];
		}

		const u32& edx() const
		{
			return mRegisters[3];
		}

	public:
		u32 mRegisters[4] = {0, 0, 0, 0};
	};
}

namespace tur
{
	CPU_Information::CPU_Information()
	{
		prepare_vendor();

		prepare_functions();
	}

	void CPU_Information::prepare_vendor()
	{
		CPU_ID cpuID(0x0);

		mVendor += std::string(reinterpret_cast<const char*>(&cpuID.ebx()), 4);
		mVendor += std::string(reinterpret_cast<const char*>(&cpuID.edx()), 4);
		mVendor += std::string(reinterpret_cast<const char*>(&cpuID.ecx()), 4);

		const std::unordered_map<std::string, std::string> vendorMap = {
			{"AMDisbetter!", "AMD"}, {"AuthenticAMD!", "AMD"}, {"GenuineIntel", "Intel"}};

		if (vendorMap.find(mVendor) != vendorMap.end())
		{
			mVendor = vendorMap.at(mVendor);
			return;
		}

		mVendor = "Unknown Vendor";
		mIsAMD = mVendor == "AMD";
		mIsIntel = mVendor == "Intel";
	}

	std::string CPU_Information::brand() const
	{
		char str[sizeof(u32) * 13];

		CPU_ID cpuID_Part0 = CPU_ID(0x80000000);
		if (cpuID_Part0.eax() < 0x80000004)
			return "Invalid CPU brand name";

		cpuID_Part0 = CPU_ID(0x80000002);
		CPU_ID cpuID_Part1 = CPU_ID(0x80000003);
		CPU_ID cpuID_Part2 = CPU_ID(0x80000004);

		memcpy(str, reinterpret_cast<u32*>(cpuID_Part0.mRegisters), sizeof(cpuID_Part0.mRegisters));

		memcpy(str + sizeof(cpuID_Part0.mRegisters), reinterpret_cast<u32*>(cpuID_Part1.mRegisters),
			   sizeof(cpuID_Part1.mRegisters));

		memcpy(str + sizeof(cpuID_Part0.mRegisters) + sizeof(cpuID_Part1.mRegisters),
			   reinterpret_cast<u32*>(cpuID_Part2.mRegisters), sizeof(cpuID_Part2.mRegisters));

		str[sizeof(str) - 1] = '\0';
		return std::string(str);
	}

	void CPU_Information::prepare_functions()
	{
		CPU_ID cpuID(0x1);
		mFunction_eax1_ECX = cpuID.ecx();
		mFunction_eax1_EDX = cpuID.edx();

		cpuID = CPU_ID(0x7, 0x0);
		mFunction_eax7_ecx0_EBX = cpuID.ebx();
		mFunction_eax7_ecx0_ECX = cpuID.ecx();
		mFunction_eax7_ecx0_EDX = cpuID.edx();

		cpuID = CPU_ID(0x7, 0x1);
		mFunction_eax7_ecx1_EAX = cpuID.eax();
		mFunction_eax7_ecx1_EBX = cpuID.ebx();
		mFunction_eax7_ecx1_EDX = cpuID.edx();

		cpuID = CPU_ID(0x7, 0x2);
		mFunction_eax7_ecx2_EDX = cpuID.edx();

		cpuID = CPU_ID(0x80000001);
		mFunction_81h_ECX = cpuID.ecx();
		mFunction_81h_EDX = cpuID.edx();
	}
}

namespace tur
{
	void display_cpu_information()
	{
		CPU_Information information;

		auto supported = [](bool supported) -> const char* { return supported ? "Yes" : "No"; };

		TUR_LOG_DEBUG("{} {}", information.vendor(), information.brand());
		TUR_LOG_DEBUG("Instructions:");
		TUR_LOG_DEBUG(" * 3DNOW:     {}", supported(information.has3DNOW()));
		TUR_LOG_DEBUG(" * 3DNOWEXT:  {}", supported(information.has3DNOWEXT()));
		TUR_LOG_DEBUG(" * SSE:       {}", supported(information.hasSSE()));
		TUR_LOG_DEBUG(" * SSE2:      {}", supported(information.hasSSE2()));
		TUR_LOG_DEBUG(" * SSE3:      {}", supported(information.hasSSE3()));
		TUR_LOG_DEBUG(" * SSSE3:     {}", supported(information.hasS_SSE3()));
		TUR_LOG_DEBUG(" * SSE4A:     {}", supported(information.hasSSE4A()));
		TUR_LOG_DEBUG(" * SSE4.1:    {}", supported(information.hasSSE4_1()));
		TUR_LOG_DEBUG(" * SSE4.2:    {}", supported(information.hasSSE4_2()));
		TUR_LOG_DEBUG(" * MMX:       {}", supported(information.hasMMX()));
		TUR_LOG_DEBUG(" * MMXEXT:    {}", supported(information.hasMMEXT()));
		TUR_LOG_DEBUG(" * CMOV:      {}", supported(information.hasCMOV()));
		TUR_LOG_DEBUG(" * RDTSC:     {}", supported(information.hasRDTSC()));
		TUR_LOG_DEBUG(" * FPU:       {}", supported(information.hasFPU()));
		TUR_LOG_DEBUG(" * FPUx87:    {}", supported(information.hasFPUx87()));
		TUR_LOG_DEBUG(" * HTT:       {}", supported(information.hasHTT()));
	}
}