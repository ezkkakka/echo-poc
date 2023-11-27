#include "driver.hpp"

HANDLE DriverHelper::GetHandleByPid(const DWORD pid)
{
	get_handle_t req{ pid, GENERIC_ALL };

	if (!CallDriver(ECHO_GET_HANDLE, req))
	{
		printf("[!] Failed DeviceIoControl ECHO_GET_HANDLE (0xE6224248)...\n");
		return INVALID_HANDLE_VALUE;
	}

	printf("[>] Called DeviceIoControl ECHO_GET_HANDLE (0xE6224248)\n");
	printf("[*] Target process id: %d\n", req.pid);

	return req.handle;
}

void DriverHelper::ReadVirtualMemory(const uintptr_t src, void* dst, const size_t size)
{
	read_memory_t req{ this->proc_handle, src, dst, size };

	if (!CallDriver(ECHO_READ_MEMORY, req))
	{
		printf("[!] Failed DeviceIoControl ECHO_READ_MEMORY (0x60A26124)...\n");
		return;
	}

	printf("[>] Called DeviceIoControl ECHO_READ_MEMORY (0x60A26124)\n");
}