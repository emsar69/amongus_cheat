#pragma once
#include <d3d11.h>

namespace gui {
	inline bool open = true;

	inline bool setup = false;

	inline HWND window = NULL;
	inline WNDPROC originalWindowProcess = nullptr;

	inline ID3D11Device* g_pd3dDevice = nullptr;
	inline DXGI_SWAP_CHAIN_DESC g_d3dSwapChain = {};
	inline ID3D11DeviceContext* g_pd3dContext = nullptr;
	inline IDXGISwapChain* g_pSwapChain = nullptr;
	inline ID3D11RenderTargetView* g_mainTargetView = nullptr;

	bool SetupDirectX() noexcept;
	void DestroyDirectX() noexcept;

	void Setup();

	void SetupMenu(ID3D11Device* device, ID3D11DeviceContext* context) noexcept;
	void Destroy() noexcept;

	void Render() noexcept;
}