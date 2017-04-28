// ApiHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ApiHook.h"

ApiHook::ApiHook()
{
	isHook = false;
	isInjected = false;

	memset(oldCode, 0, sizeof(oldCode));
}

void handleMemoryProtect(PMEMORY_BASIC_INFORMATION memoryProp)
{
	if (memoryProp->Protect == PAGE_READONLY) //可读
	{
		VirtualProtectEx(HANDLE(-1), memoryProp->AllocationBase, memoryProp->RegionSize, 4, 0);
	}
	else if (memoryProp->Protect == PAGE_EXECUTE) //可运行
	{
		VirtualProtectEx(HANDLE(-1), memoryProp->AllocationBase, memoryProp->RegionSize, 64, 0);
	}
	else if (memoryProp->Protect == PAGE_EXECUTE_READ) //可运行可读
	{
		VirtualProtectEx(HANDLE(-1), memoryProp->AllocationBase, memoryProp->RegionSize, 64, 0);
	}
}

void ApiHook::BeginHook()
{
	if (isHook == true)
	{
		return;
	}

	if (funcEntry != NULL)
	{
		DWORD dwTemp = 0;
		DWORD dwOldProtect;

		VirtualProtectEx(HANDLE(-1), funcEntry, 5, PAGE_READWRITE, &dwOldProtect);
		WriteProcessMemory(HANDLE(-1), funcEntry, newCode, 5, 0);
		VirtualProtectEx(HANDLE(-1), funcEntry, 5, dwOldProtect, &dwTemp);

		isHook = true;
	}
}

void ApiHook::StopHook()
{
	if (isHook == false)
	{
		return;
	}

	if (funcEntry != NULL)
	{
		DWORD dwTemp = 0;
		DWORD dwOldProtect;

		VirtualProtectEx(HANDLE(-1), funcEntry, 5, PAGE_READWRITE, &dwOldProtect);
		WriteProcessMemory(HANDLE(-1), funcEntry, oldCode, 5, 0);
		VirtualProtectEx(HANDLE(-1), funcEntry, 5, dwOldProtect, &dwTemp);

		isHook = false;
	}
}

bool ApiHook::InstallApiHook(const char* dllName, const char* apiName, void* func)
{
	HMODULE dllHandle;
	PMEMORY_BASIC_INFORMATION memoryProp = {0};
	int funcAddr = (int)func;
	
	char* aaaaa = "aaaaaaa";

	if (isInjected == false)
	{
		dllHandle = GetModuleHandleA(dllName);
		if (dllHandle == NULL)
		{
			dllHandle = LoadLibrary(dllName);
		}
		if (dllHandle != NULL)
		{
			funcEntry = GetProcAddress(dllHandle, apiName);
			if (funcEntry != NULL)
			{
				if (VirtualQueryEx(HANDLE(-1), funcEntry, memoryProp, 28))
				{
					handleMemoryProtect(memoryProp);
				}

				BYTE* oldCodeAsm = oldCode;
				BYTE* newCodeAsm = newCode;
				FARPROC funcEntryAsm = funcEntry;

				_asm
				{
					mov edi, oldCodeAsm
					mov esi, funcEntryAsm
					cld
					movsd
					movsb
				}

				newCode[0] = 0xE9;

				_asm
				{
					mov ecx, newCodeAsm
					mov eax, funcAddr
					mov ebx, funcEntryAsm
					sub eax, ebx
					sub eax, 5
					mov dword ptr [ecx + 1], eax
				}

				BeginHook();

				isInjected = true;
			}
		}
	}
	
	return isInjected;
}

void ApiHook::UninstallApiHook(int temp)
{
	if (funcEntry != NULL)
	{
		StopHook();
	}
	memset(oldCode, 0, sizeof(oldCode));
	memset(newCode, 0, sizeof(newCode));

	isInjected = false;
}