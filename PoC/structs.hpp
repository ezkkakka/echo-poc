#pragma once
#include <Windows.h>
#include <iostream>

#define ECHO_INTERNAL_CHECK		0x9E6A0594
#define ECHO_GET_HANDLE			0xE6224248
#define ECHO_READ_MEMORY		0x60A26124

struct init_driver_t
{
	char pad[0x30];
};

struct read_memory_t
{
	HANDLE handle;
	uintptr_t src;
	void* dst;
	size_t size;
	char pad[0x40];
};

struct get_handle_t
{
	DWORD pid;
	ACCESS_MASK flag;
	HANDLE handle;
	char pad[0x40];
};