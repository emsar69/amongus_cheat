#include "hooks.h"

#include <stdexcept>
#include <cstdint>
#include <intrin.h>
#include "game.h"
#include "logs.h"

#include <MinHook.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

void hooks::Setup() {
	if (MH_Initialize() != MH_OK)
		throw std::runtime_error("MinHook initialization error");

	std::uintptr_t* swapChainPointer =
		static_cast<std::uintptr_t*>(static_cast<void*>(gui::g_pSwapChain));

	std::uintptr_t* virtualTablePointer =
		reinterpret_cast<std::uintptr_t*>(swapChainPointer[0]);

	oPresent = (PresentFn)(virtualTablePointer[8]);
    oResize = (ResizeFn)(virtualTablePointer[13]);

	if (MH_CreateHook(
		reinterpret_cast<LPVOID>(virtualTablePointer[8]),
		reinterpret_cast<LPVOID>(&PresentHook),
		reinterpret_cast<LPVOID*>(&oPresent)
	)) throw std::runtime_error("unable to create PresentHook");

	if (MH_CreateHook(
		reinterpret_cast<LPVOID>(virtualTablePointer[13]),
		reinterpret_cast<LPVOID>(&ResizeHook),
		reinterpret_cast<LPVOID*>(&oResize)
	)) throw std::runtime_error("unable to create ResizeHook");

	DWORD old_protect;
	VirtualProtect(&oPresent, 2, PAGE_EXECUTE_READWRITE, &old_protect);

	if (MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Unable to enable hooks");
}

void hooks::Destroy() noexcept {
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

HRESULT __stdcall hooks::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) noexcept {
	if (!gui::setup) {
		logger::Write("Setting up gui");
		pSwapChain->GetDevice(__uuidof(gui::g_pd3dDevice), reinterpret_cast<void**>(&gui::g_pd3dDevice));
		gui::g_pd3dDevice->GetImmediateContext(&gui::g_pd3dContext);

		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		gui::g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &gui::g_mainTargetView);
		pBackBuffer->Release();

		gui::SetupMenu(gui::g_pd3dDevice, gui::g_pd3dContext);
	}
	
	if (gui::open){
		gui::Render();
	}
	return oPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT __stdcall hooks::ResizeHook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) noexcept
{
	logger::Write("Resize ImGui...");
	if (gui::g_mainTargetView) {
		gui::g_pd3dContext->OMSetRenderTargets(0, 0, 0);
		gui::g_mainTargetView->Release();
	}

	HRESULT hr = oResize(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);

	ID3D11Texture2D* pBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBuffer);
	// Perform error handling here!

	gui::g_pd3dDevice->CreateRenderTargetView(pBuffer, NULL, &gui::g_mainTargetView);
	// Perform error handling here!
	pBuffer->Release();

	gui::g_pd3dContext->OMSetRenderTargets(1, &gui::g_mainTargetView, NULL);
	logger::Write("Resized");
	// Set up the viewport.
	D3D11_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	gui::g_pd3dContext->RSSetViewports(1, &vp);
	return hr;
}
