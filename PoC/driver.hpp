#pragma once
#include "structs.hpp"

class DriverHelper
{
private:
	HANDLE drv_handle = nullptr;
	HANDLE proc_handle = nullptr;

public:
	DriverHelper()
	{
		drv_handle = CreateFileA(
			"\\\\.\\EchoDrv",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (drv_handle == INVALID_HANDLE_VALUE)
		{
			printf("[!] Failed CreateFileA...\n");
		}

		// BSOD 방지용 임시 버퍼 생성
		init_driver_t init_drv{};

		// 내부 검사 우회 (0x9E6A0594)
		// 해당 호출이 없으면 드라이버와의 통신이 불가능함
		/*
		if ( ioControlCode == 0x9E6A0594 )            // internal check
		{
			v1 = __irp->AssociatedIrp.MasterIrp;
			if ( (int)InternalCheck(*(PUCHAR *)&v1->Type, (ULONG)v1->MdlAddress) >= 0 )
				LOBYTE(v1->Flags) = 1;
			*(&v1->Flags + 1) = 0x1000;
			goto LABEL_27;
		}
		*/
		if (!CallDriver(ECHO_INTERNAL_CHECK, init_drv))
		{
			printf("[!] Failed DeviceIoControl ECHO_INTERNAL_CHECK (0x9E6A0594)...\n");
			CloseHandle(drv_handle);
		}

		printf("[>] Called DeviceIoControl ECHO_INTERNAL_CHECK (0x9E6A0594)\n");

		// 타겟 프로세스의 핸들 획득
		proc_handle = GetHandleByPid(GetCurrentProcessId());
	}

	~DriverHelper()
	{
		CloseHandle(proc_handle);
		CloseHandle(drv_handle);
	}

	/*
	case 0xE6224248:
		irp = __irp->AssociatedIrp.MasterIrp;   // Get handle of target process
		Process = 0i64;
		if ( PsLookupProcessByProcessId((HANDLE)*(unsigned int *)&irp->Type, &Process) >= 0
		  && ObOpenObjectByPointer(
			   Process,
			   0,
			   0i64,
			   *(_DWORD *)(&irp->Size + 1),
			   (POBJECT_TYPE)PsProcessType,
			   0,
			   (PHANDLE)&Handle) >= 0 )
		{
		  if ( Process )
			ObfDereferenceObject(Process);
		  irp->MdlAddress = (PMDL)Handle;       // 핸들 전달
		  LOBYTE(irp->Flags) = 1;
	}
	*/
	HANDLE GetHandleByPid(const DWORD pid);

	/*
	case 0x60A26124u:                         // MmCopyVirtualMemory (0x60A26124)
		v8 = __irp->AssociatedIrp.MasterIrp;
		Process = 0i64;
		if ( ObReferenceObjectByHandle(*(HANDLE *)&v8->Type, 0, (POBJECT_TYPE)PsProcessType, 0, (PVOID *)&Process, 0i64) >= 0
		  && (int)MmCopyVirtualMemoryWrapper() >= 0 )
		{
		  LOBYTE(v8->ThreadListEntry.Blink) = 1;
		}
		if ( Process )
		  ObfDereferenceObject(Process);
		HIDWORD(v8->ThreadListEntry.Blink) = 0x1002;
		v6 = 0;
		v5 = 48i64;
		break;
	*/
	void ReadVirtualMemory(const uintptr_t src, void* dst, const size_t size);

	template <typename T>
	T ReadMemory(const uintptr_t src)
	{
		auto dst = T();
		ReadVirtualMemory(src, &dst, sizeof(dst));
		return dst;
	};

	template <typename T>
	bool CallDriver(const DWORD& ioctl, T& req) noexcept
	{
		return DeviceIoControl(this->drv_handle, ioctl, &req, sizeof(req), &req, sizeof(req), nullptr, nullptr);
	}

	bool CallDriver(const DWORD& ioctl, read_memory_t& req) noexcept
	{
		DWORD bytes_read;
		return DeviceIoControl(this->drv_handle, ioctl, &req, sizeof(read_memory_t), &req, sizeof(read_memory_t), &bytes_read, nullptr);
	}

};
