#include "gui.h"
#include "game.h"
#include <d3d11.h>
#include <string>
#include "logs.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include <stdexcept>

//VARIABLES FOR HACK
bool noCooldown = false;
bool esp = false;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool gui::SetupDirectX() noexcept {
	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	
	ZeroMemory(&g_d3dSwapChain, sizeof(g_d3dSwapChain));

	g_d3dSwapChain.BufferCount = 1;
	g_d3dSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	g_d3dSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	g_d3dSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	g_d3dSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	g_d3dSwapChain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	g_d3dSwapChain.OutputWindow = gui::window;
	g_d3dSwapChain.SampleDesc.Count = 1;
	g_d3dSwapChain.Windowed = ((GetWindowLongPtr(gui::window, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
	g_d3dSwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	g_d3dSwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	g_d3dSwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	g_d3dSwapChain.BufferDesc.Width = 1;
	g_d3dSwapChain.BufferDesc.Height = 1;
	g_d3dSwapChain.BufferDesc.RefreshRate.Numerator = 0;
	g_d3dSwapChain.BufferDesc.RefreshRate.Denominator = 1;

	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		levels,
		sizeof(levels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&g_d3dSwapChain,
		&g_pSwapChain,
		&g_pd3dDevice,
		nullptr,
		&g_pd3dContext
	);

	if (FAILED(result)) 
		return false;

	return true;
}

void gui::DestroyDirectX() noexcept {
	if (g_pd3dDevice) {
		g_pd3dDevice->Release();
		g_pd3dDevice = NULL;
	}
}

void gui::Setup() {
	gui::window = FindWindowA(NULL, "Among Us");
	if (gui::window == NULL) throw std::runtime_error("Window Not Found.");
	if (!SetupDirectX())
		throw std::runtime_error("Failed to setup DirectX");
}

void gui::SetupMenu(ID3D11Device* device, ID3D11DeviceContext* context) noexcept {

	if (ImGui::CreateContext() == nullptr) MessageBoxA(NULL, "NO CONTEXT HAS CREATED!", "E", 0);
	//ImGui::StyleColorsDark();

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.00f, 0.00f, 1.00f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.02f, 0.00f, 1.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.51f, 0.17f, 0.55f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.64f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(1.00f, 0.00f, 0.31f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.82f, 0.36f, 0.84f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.40f, 0.43f, 0.96f, 0.40f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.25f, 0.26f, 0.53f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.52f, 0.19f, 0.77f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.77f, 0.30f, 0.00f, 0.86f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.95f, 0.51f, 0.14f, 0.80f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.67f, 0.07f, 0.07f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowBorderSize = 1.0f;
	style.WindowTitleAlign = ImVec2(.5, .5);
	style.FrameBorderSize = 1.0f;
	style.WindowRounding = 12;
	style.FrameRounding = 7;
	style.WindowPadding = ImVec2(20, 7);

	ImGui::GetIO().FontGlobalScale = 1.30f;

	gui::originalWindowProcess = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(gui::window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wndProc))
	);

	ImGui_ImplWin32_Init(gui::window);

	game::init();
	
	if (device == nullptr) MessageBoxA(NULL, "device not found", "ked", 0);
	if (!ImGui_ImplDX11_Init(device, context)) MessageBoxA(NULL, "DX11 INIT ERROR", "!", 0);

	setup = true;
}

void gui::Destroy() noexcept {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(
		gui::window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	DestroyDirectX();
}

void gui::Render() noexcept {

	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowStyleEditor();
	//ImGui::ShowDemoWindow();

	ImVec2 fixSize = ImVec2(400, 500);

	ImGui::SetNextWindowSize(fixSize, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(fixSize, fixSize);

	ImGui::Begin("Made by Emsar");
	ImGui::BeginTabBar("firsttabbar");

	if (ImGui::BeginTabItem("Common")) {
		ImGui::Checkbox("malfax", &open);
		ImGui::Separator();
		ImGui::SeparatorText("oyle");
		ImGui::Spacing();

		AmongUsClient* client = game::getAmongUsClient();
		if (client->GameState == GameStates::Joined || client->GameState == GameStates::Started) {
			Player* localPlayer = game::getLocalPlayer();
			if (localPlayer != nullptr) {
				ImGui::SliderFloat("Player Speed", &localPlayer->Physics->PlayerSpeed, .1, 20, "%.1f");
			}
		}
		else {
			ImGui::Text("Game NOT Started");
		}

		ImGui::Checkbox("ESP Enabled", &esp);

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
		ImGui::Separator();

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Players")) {

		ImGui::TextColored(ImVec4(0, 1, 1, 1), "Players");

		ImGui::Separator();
		ImGui::SeparatorText("Player Roles");

		std::vector<Player*> players = game::getPlayerList();
		if (players.size() > 0) {
			for (Player* plr : players) {
				if (plr == nullptr) continue;
				if (plr->cacheddata == nullptr) continue;
				if (plr->cacheddata->Role == nullptr) continue;

				std::string playerName = game::getPlayerName(plr);

				int role = plr->cacheddata->Role->TeamType;
				ImGui::TextColored(role == 0 ? ImVec4(0, 1, 0, 1) : ImVec4(1,.3,.3,1), (playerName + ": " + game::getRoleName(plr->cacheddata->Role->roleType)).c_str());
			}
		}

		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Host Only")) {

		if (ImGui::Button("Be Impostor")) {
			AmongUsClient* client = game::getAmongUsClient();
			if (client->GameState == GameStates::Started || client->GameState == GameStates::Joined) {
				Player* localPlayer = game::getLocalPlayer();
				if (localPlayer != nullptr) {
					game::setRole(localPlayer, RoleTypes::Impostor);
				}
			}
		}

		ImGui::Checkbox("No Kill Cooldown", &noCooldown);

		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();

	ImGui::Spacing();

	if (noCooldown) {
		Player* localPlayer = game::getLocalPlayer();
		if (localPlayer != nullptr) {
			localPlayer->KillTimer = 0;
		}
	}

	if (esp) {
		//TODO: ESP
	}

	ImGui::End();
	ImGui::Render();

	gui::g_pd3dContext->OMSetRenderTargets(1, &gui::g_mainTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (GetAsyncKeyState(VK_DELETE) & 1)
		gui::open = !gui::open;

	if (gui::open && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return 1L;

	return CallWindowProc(gui::originalWindowProcess, hWnd, msg, wParam, lParam);
}