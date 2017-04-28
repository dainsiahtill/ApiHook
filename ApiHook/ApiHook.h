#pragma once

class ApiHook
{
private:
	BYTE oldCode[5];
	BYTE newCode[5];

public:
	ApiHook();
	bool InstallApiHook(const char* dllName, const char* apiName, void* func);
	void UninstallApiHook();
	void BeginHook();
	void StopHook();
    bool isInjected;
protected:
	bool isHook;
	FARPROC funcEntry;
	
};
