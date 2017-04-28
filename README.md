# ApiHook

# usage:

	InstallApiHook("some_dll.dll", "some_method", func)
	UninstallApiHook()
  	/<br>
	BeginHook();
	StopHook();
  
  
# example:
  
void InstallHook()
{
	recvHook = new ApiHook();
	recvHook->InstallApiHook("ws2_32.dll", "recv", MyRecv);
}

void UninstallHook()
{
	if (recvHook != NULL)
	{
		recvHook->UninstallApiHook(0);
	}
}

int WINAPI MyRecv(SOCKET socket, char* buf, int len, int flags)
{
	int result = -1;

	recvHook->StopHook();

	//to do something

	recvHook->BeginHook();

	return result;
}

