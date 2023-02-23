#define RPC_USE_NATIVE_WCHAR

#include <Windows.h>
#include <combaseapi.h>
#include <winrt/Windows.Foundation.h>

#include "../ComProxyStub/ICalculator_h.h"

#include <iostream>

int main()
{
	winrt::init_apartment();

	winrt::com_ptr<ICalculator> calc;

	system("pause");

	HRESULT hr;
	while (FAILED(hr = ::CoCreateInstance(CLSID_Calculator, nullptr, CLSCTX_SERVER, winrt::guid_of<ICalculator>(), calc.put_void())))
	{
		std::wcout << L"Couldn't create instance: " << winrt::hresult_error(hr).message().data() << std::endl;
		::Sleep(1000);
	} 

	std::cout << "Enter two numbers: ";

	int left, right;
	std::cin >> left >> right;

	int output;
	winrt::check_hresult(calc->Add(left, right, &output));

	std::cout << std::endl << left << " + " << right << " = " << output << std::endl;
}