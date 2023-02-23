#define RPC_USE_NATIVE_WCHAR

#include <Windows.h>
#include <combaseapi.h>
#include <winrt/Windows.Foundation.h>

#include "../ComProxyStub/ICalculator_h.h"

#include <iostream>
#include <filesystem>

struct Calculator : winrt::implements<Calculator, ICalculator>
{
    HRESULT __stdcall Add(int left, int right, int* out) noexcept override
    {
        *out = left + right;
        return S_OK;
    }
};

struct CalculatorFactory : winrt::implements<CalculatorFactory, IClassFactory>
{
    HRESULT __stdcall CreateInstance(
        IUnknown* outer,
        GUID const& iid,
        void** result) noexcept final
    {
        *result = nullptr;

        if (outer)
        {
            return CLASS_E_NOAGGREGATION;
        }


        HRESULT hr = winrt::make<Calculator>()->QueryInterface(iid, result);
        return hr;
    }

    HRESULT __stdcall LockServer(BOOL) noexcept final
    {
        return S_OK;
    }
};

std::wstring GetExePath()
{
    wchar_t buffer[MAX_PATH];

    GetModuleFileNameW(nullptr, buffer, MAX_PATH);

    return std::wstring(buffer);
}

int main(int argc, char* argv[])
{
    winrt::init_apartment();

    std::wstring manifestPath = std::filesystem::path(GetExePath()).parent_path().parent_path().append(L"ActivationManifest.manifest");
    ACTCTXW actctx = {
        .cbSize = sizeof(ACTCTXW),
        .lpSource = manifestPath.c_str(),
    };

    HANDLE actctxHandle = ::CreateActCtxW(&actctx);
    winrt::check_bool(actctxHandle != INVALID_HANDLE_VALUE);

    ULONG_PTR activationCookie{ 0 };
    winrt::check_bool(::ActivateActCtx(actctxHandle, &activationCookie));

    DWORD regCookie;
    winrt::check_hresult(::CoRegisterClassObject(
        CLSID_Calculator,
        winrt::make<CalculatorFactory>().get(),
        CLSCTX_LOCAL_SERVER,
        REGCLS_MULTIPLEUSE,
        &regCookie));

    winrt::check_bool(::DeactivateActCtx(0, activationCookie));
    ::ReleaseActCtx(actctxHandle);

    std::wcout << L"Successfully registered factory" << std::endl;

    MSG msg{ 0 };
    while (::GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
}