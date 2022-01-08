#include <thread>

#include "includes.h"
#include "cfg.hpp"
#include "cheat/cheat.h"
#include "menu/menu.h"


/// <summary>
/// 
/// *********************
///  GITHUB REPOSITORY: https://github.com/1hAck-0/UE4-Cheat-Source-Code
/// *********************
/// 
/// *********************
/// BUILD and COMPILE:
/// *********************
/// - You can only compile in VS22 or higher since the project was created with this version of Visual Studio. For ya all lazy asses who haven't updated from VS19, it's worth it anyway
/// - Build and compile in x64 architecture since Sword With Suace is 64-bit
/// - If you are still not able to compile, please put the correct path to the DirectX SDK in project settings->Configuration Properties->VC++ Directories->Include Directories and Library Directories
/// *********************
/// 
/// 
/// 
/// *********************
/// TO DO:
/// *********************
/// - Silent Aim
/// - Consider the Enemy velocity in the Aimbot calculations for more accurate aimbot/silent aim (I was able to get the velocity of the enemies but somewhy it made my aimbot very laggy)
/// - No Recoil/Spread
/// - View the first comment in FHitResult::GetActor() in UnrealEngine.cpp; why do we have to add 3 to get the actual serial number!?
/// 
/// If you know how to do anything in the TO DO list, please share you knowledge with a pull request in the GitHub Repo. Thanks!
/// *********************
/// 
/// 
/// 
/// *********************
/// FEATURES:
/// *********************
/// - ESP
///   - Boxes
///     - 2D Boxes
///     - 3D Boxes
///     - Corner Boxes
///   - Snaplines
///   - Skeletons
/// 
/// - Aimbot
///   - Visibile Check
///   - FOV + FOV Circle
///   - Smoothness
///   - Custom Hotkey
/// 
/// - Triggerbot
///   - Custom Hotkey
///   - Delay
///     - Specific Custom Delay
///     - Random Delay (you choose the range)
/// 
/// - Exploits
///   - Infinite Health
///   - Infinite Ammo
///   - No Spread (doesn't work well)
///   - No Clip (with custom toggle key option)
///	  - Flyhack (with custom toggle key option)
///   - Speedhack + Speed slider
///   - Super Jump + Jump Height slider
///   - Modify Gravity + Gravity slider
/// *********************
/// 
/// </summary>

#define IsKeyPressed(key) GetAsyncKeyState(key) & 1 && GetAsyncKeyState(key) & 0x8000

#pragma region Kiero

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
Present oPresent;
HWND window = 0;
WNDPROC oWndProc;
ID3D11Device* pDevice = 0;
ID3D11DeviceContext* pContext = 0;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuiStyle& style = ImGui::GetStyle();
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;

	io.Fonts->AddFontDefault();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

#pragma endregion


HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!cfg->menu->bInit)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)& pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, 0, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			cfg->menu->bInit = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (IsKeyPressed(cfg->menu->ToggleKey))
		cfg->menu->bShowMenu = !cfg->menu->bShowMenu;

	Menu::Render();

	Cheat::DrawESP();

	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, 0);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

// ENTRY POINT
DWORD WINAPI Main(HMODULE hMod)
{
	if (!Cheat::Init())
	{
		FreeLibraryAndExitThread(hMod, 0);
		return 0;
	}

	// HOOK KIERO
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);

	while (true)
	{
		// close menu
		if (IsKeyPressed(VK_F4) || cfg->menu->bShutdown)
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// CLEAN UP
	cfg->menu->bShutdown = true;
	kiero::shutdown();
	Cheat::Uninit();
	FreeLibraryAndExitThread(hMod, 0);
	return 0;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hMod);
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Main, hMod, 0, nullptr);
		return TRUE;
	}
	return FALSE;
}
