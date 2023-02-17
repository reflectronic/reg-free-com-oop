#define RPC_USE_NATIVE_WCHAR

#include <Windows.h>
#include <combaseapi.h>
#include <winrt/Windows.Foundation.h>

#include "../ComProxyStub/ICalculator_h.h"

#include <iostream>

int main()
{
	winrt::init_apartment();

	CLSID clsid;
	winrt::check_win32(::UuidCreate(&clsid));

	RPC_WSTR clsidStr;
	winrt::check_win32(::UuidToStringW(&clsid, &clsidStr));

	PROCESS_INFORMATION pi{ 0 };
	STARTUPINFO si{ 0 };

	std::wcout << L"Creating COM server process, requesting CLSID: " << clsidStr << std::endl;

	winrt::check_bool(::CreateProcessW(L"ComServer\\ComServer.exe",
		clsidStr,
		nullptr,
		nullptr,
		TRUE,
		0,
		nullptr,
		nullptr,
		&si,
		&pi));

	winrt::check_win32(::RpcStringFreeW(&clsidStr));

	winrt::com_ptr<ICalculator> calc;

	HRESULT hr;
	while ((hr = ::CoCreateInstance(clsid, nullptr, CLSCTX_SERVER, winrt::guid_of<ICalculator>(), calc.put_void())) != S_OK)
	{
		std::wcout << L"Couldn't create instance: " << winrt::hresult_error(hr).message().data() << std::endl;
		Sleep(1000);
	}

	std::cout << "Enter two numbers: ";

	int left, right;
	std::cin >> left >> right;

	int output;
	winrt::check_hresult(calc->Add(left, right, &output));

	std::cout << std::endl << left << " + " << right << " = " << output << std::endl;

	system("pause");

	TerminateProcess(pi.hProcess, 0);
}