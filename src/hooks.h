#pragma once
#include "gui.h"
#include "game.h"

namespace hooks {
	void Setup();
	void Destroy() noexcept;

	typedef HRESULT(__stdcall* PresentFn)(IDXGISwapChain*, UINT, UINT);
	inline PresentFn oPresent = nullptr;

	HRESULT __stdcall PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) noexcept;

	typedef HRESULT(__stdcall* ResizeFn)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
	inline ResizeFn oResize = nullptr;

	HRESULT __stdcall ResizeHook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) noexcept;

}