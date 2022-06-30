// Dear ImGui: standalone example application for DirectX 9
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include <d3d9.h>
#include <tchar.h>

#include <tchar.h>

#include <array>
#include <vector>
#include <dwmapi.h>

constexpr int   menuTabsCount = 4;

#define ICON_SKIN     u8"\ue900"
#define ICON_AIM      u8"\ue901"
#define ICON_VISUALS  u8"\ue902"
#define ICON_SETTINGS u8"\ue903"


static constexpr std::array<const char8_t*, 5> menuIcons
{
    ICON_AIM, ICON_VISUALS, ICON_SETTINGS, ICON_SKIN
};

constexpr ImWchar iconsRanges[] = { 0xe900, 0xe903, 0 };

namespace gFonts
{
    namespace segoeui
    {
#include "Fonts/segoeui.hpp"
    }

    namespace framdit
    {
#include "Fonts/framdit.hpp"
    }

    namespace ntailu
    {
#include "Fonts/ntailu.hpp"
    }

    namespace icons
    {
#include "Fonts/icons.hpp"
    }

    static ImFont* iconsFont;
}

// Data
static HWND                     gWindow = nullptr;
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
bool                            te1 = true;
int                             testInt = 0;
float                           testFloat = 0.f;
float                           animspeed;
bool                            customize;



// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool init(HINSTANCE instance);
void DrawTabs(int& currentTab, bool& shouldBreak);

void runLoop()
{
    MSG                msg;
    static int         tab;
    bool               shouldBreak;
    ImVec2             zeroImVec2{ 0.f, 0.f };
    HRESULT            result;
    constexpr D3DCOLOR directXClearColor = 4278190080UL;
    constexpr int      windowFlags = ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse;

    tab = 0;
    shouldBreak = false;
    while (shouldBreak == false) {
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                shouldBreak = true;
        }

        if (shouldBreak)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(zeroImVec2, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
        if (ImGui::Begin("haha", nullptr, windowFlags)) {
            DrawTabs(tab, shouldBreak);
            //DrawPanel(tab, shouldBreak);
            ImGui::End();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();


        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, directXClearColor, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }
}

// Main code
int main(int, char**)
{
    HINSTANCE instance = GetModuleHandle(nullptr);
    if (init(instance)) {
        runLoop();
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        CleanupDeviceD3D();
        DestroyWindow(gWindow);
        UnregisterClass("something", instance);
    }

    return 0;
}

void styleColorsGold()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.114f, 0.114f, 0.125f, 1.f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.114f, 0.114f, 0.125f, 1.f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.098f, 0.098f, 0.106f, 1.0f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.149f, 0.149f, 0.161f, 1.f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.149f, 0.149f, 0.161f, 1.f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.149f, 0.149f, 0.161f, 1.f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.933f, 0.631f, 0.055f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.933f, 0.631f, 0.055f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.933f, 0.631f, 0.055f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.098f, 0.098f, 0.106f, 1.f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.149f, 0.149f, 0.161f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.149f, 0.149f, 0.161f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.149f, 0.149f, 0.161f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.149f, 0.149f, 0.161f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.149f, 0.149f, 0.161f, 1.00f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void InitStyle()
{
    auto& Style = ImGui::GetStyle();

    Style.WindowRounding = 12.000f;
    //Style.ChildRounding = 12.000f;
    Style.PopupRounding = 12.000f;
    Style.FrameRounding = 12.000f;
    //Style.FrameBorderSize = 1.000f;
    Style.GrabMinSize = 40.000f;
    Style.GrabRounding = 12.000f;
    Style.SelectableTextAlign = ImVec2(0.500f, 0.500f);
}

bool init(HINSTANCE instance)
{
    int          preference;
    HICON        icon2;
    WNDCLASSEX   windowClassEx;
    ImFontConfig fontConfig;

    constexpr const char*  windowName = "Easy Victory";

    icon2 = nullptr;//LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
    windowClassEx.cbSize = sizeof(WNDCLASSEX);
    windowClassEx.style = CS_CLASSDC;
    windowClassEx.lpfnWndProc = WndProc;
    windowClassEx.cbClsExtra = 0;
    windowClassEx.cbWndExtra = 0;
    windowClassEx.hInstance = instance;
    windowClassEx.hIcon = icon2;
    windowClassEx.hCursor = nullptr;
    windowClassEx.hbrBackground = nullptr;
    windowClassEx.lpszMenuName = nullptr;
    windowClassEx.lpszClassName = "something";
    windowClassEx.hIconSm = nullptr;
    RegisterClassEx(&windowClassEx);

    gWindow = CreateWindow(
        "something",
        windowName,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        100,
        100,
        570,
        500,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!CreateDeviceD3D(gWindow)) {
        CleanupDeviceD3D();
        UnregisterClass(windowClassEx.lpszClassName, windowClassEx.hInstance);
        return false;
    }

    ShowWindow(gWindow, SW_SHOWDEFAULT);
    UpdateWindow(gWindow);

    /* remove rounded coreners of my window in Win 11 */
    preference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(gWindow, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    styleColorsGold();
    InitStyle();

    fontConfig.FontDataOwnedByAtlas = false;
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(gFonts::ntailu::rawData, sizeof(gFonts::ntailu::rawData), 20.f, &fontConfig);

    fontConfig = {};
    fontConfig.FontDataOwnedByAtlas = false;
    gFonts::iconsFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(gFonts::icons::rawData, sizeof(gFonts::icons::rawData), 50.f, 0, iconsRanges);

    ImGui_ImplWin32_Init(gWindow);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    return true;
}

void drawSeparator()
{
    float       itemSpacingY = ImGui::GetStyle().ItemSpacing.y;
    ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
    ImVec2      cursorScreenPos = ImGui::GetCursorScreenPos();

    cursorScreenPos.y += itemSpacingY;
    ImGui::SetCursorScreenPos(cursorScreenPos);
    windowDrawList->AddLine(ImVec2(cursorScreenPos.x - 9999, cursorScreenPos.y), ImVec2(cursorScreenPos.x + 9999, cursorScreenPos.y), ImGui::GetColorU32(ImGuiCol_Border));
    cursorScreenPos.y += itemSpacingY * 2.f;
    ImGui::SetCursorScreenPos(cursorScreenPos);
}

static void aimbotTab()
{
    /**static double dummyVar = 0.0;

    ImGui::Combo("aim action", &gGlobalVars->features.automatic, "none\0aim\0play\0");
    if (gGlobalVars->features.automatic == 2) {
        ImGui::PushID("power");
        ImGui::SliderFloat(skCrypt("play power"), &gGlobalVars->features.autoPlayMaxPower, 10.f, 100.f, "%.f%%", ImGuiSliderFlags_AlwaysClamp);
        ImGui::PopID();
    }

    ImGui::PushID("delay");
    ImGui::Combo("delay mode", &gGlobalVars->features.autoPlayDelayMode, "custom\0random\0");
    switch (gGlobalVars->features.autoPlayDelayMode)
    {
    case 0:
        ImGui::SliderFloat(skCrypt("delay"), &gGlobalVars->features.autoPlayDelay, 0.f, 15.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
        break;
    default:
        ImGui::SliderFloat2(skCrypt("delay bounds"), &gGlobalVars->features.autoPlayDelayBounds[0], 0.f, 5.f, "%f", ImGuiSliderFlags_AlwaysClamp);
        break;
    }

    ImGui::PopID();

    /*ImGui::Checkbox("find best shot", &gGlobalVars->features.findBestShot);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("find shot with max balls could go in pockets\n");
    }*/

    ImGui::Text("Aim");
}

static void visualsTab()
{
    /*ImGui::Checkbox(skCrypt("draw prediction"), &gGlobalVars->features.esp.predicionPath);
    if (gGlobalVars->features.esp.predicionPath) {
        ImGui::PushID(skCrypt("predicionPath"));
        ImGui::SliderInt(skCrypt("prediction draw power"), &gGlobalVars->features.esp.displayPercantage, 10, 100, "%d%%");
        ImGui::SliderFloat(skCrypt("line tickness"), &gGlobalVars->features.esp.ballLineTickness, 1.f, 10.f);
        ImGui::SliderFloat(skCrypt("ball circle radius"), &gGlobalVars->features.esp.ballCircleRadius, 1.f, 40.f);
        ImGui::Checkbox(skCrypt("ball circle filled"), &gGlobalVars->features.esp.ballCircleFilled);
        ImGui::PopID();
    }

    ImGui::Checkbox(skCrypt("draw shot state"), &gGlobalVars->features.esp.shotState);
    if (gGlobalVars->features.esp.shotState) {
        ImGui::PushID(skCrypt("shot state"));
        ImGui::SliderFloat(skCrypt("circle tickness"), &gGlobalVars->features.esp.shotStateCircleTickness, 1.f, 10.f);
        ImGui::SliderFloat(skCrypt("circle radius"), &gGlobalVars->features.esp.shotStateCircleRadius, 1.f, 50.f);
        ImGui::Checkbox(skCrypt("circle filled"), &gGlobalVars->features.esp.shotStateCircleFilled);
        ImGui::PopID();
    }

    ImGui::Combo(skCrypt("prediction state"), &gGlobalVars->features.esp.state, skCrypt("player\0opponent\0both\0"));
    ImGui::SliderFloat(skCrypt("transparency"), &gGlobalVars->features.esp.transparency, 0.f, 100.f, "%.f%%");

    drawSeparator();

    if (ImGui::Checkbox(skCrypt("wide guideline"), &gGlobalVars->features.esp.wideGuideLine))
        UserSettings::setWideGuideLine(gGlobalVars->features.esp.wideGuideLine);

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip(skCrypt("requires rejoin.\n"));*/

    ImGui::Text("Visual");
}

static void miscTab()
{
    //ImGui::SliderInt("max win streak", &gGlobalVars->features.maxWinStreak, 3, 50, "%d");
    //if (ImGui::IsItemHovered()) {
    //    ImGui::SetTooltip("it forces you lose after you reach the max win streak.\nlower it to avoid shadow ban.\n");
    //}

    /*ImGui::Checkbox(skCrypt("adBlock"), &gGlobalVars->features.adBlock);
    drawSeparator();
    if (ImGui::Combo(skCrypt("colors style"), &gGlobalVars->features.menu.colorsStyle, skCrypt("gold\0classic\0dark\0light\0"))) {
        switch (gGlobalVars->features.menu.colorsStyle)
        {
        case 0:
            styleColorsGold();
            break;
        case 1:
            ImGui::StyleColorsClassic();
            break;
        case 2:
            ImGui::StyleColorsDark();
            break;
        default:
            ImGui::StyleColorsLight();
            break;
        }
    }

    if (ImGui::Button(skCrypt("load"), ImVec2(200.f, 0.f)))
        Config::load();

    if (ImGui::Button(skCrypt("save"), ImVec2(200.f, 0.f)))
        Config::save();

    if (ImGui::Button(skCrypt("unload"), ImVec2(200.f, 0.f)))
        shouldBreak = true;*/

    ImGui::Text("Misc");
}

static void skinTab(bool& shouldBreak)
{
    ImGui::Text("Skin");
}

void DrawPanel(int& tab, bool& shouldBreak)
{
    ImGui::SetCursorPos(ImVec2(95, 10));
    ImGui::BeginChild("MainPanel");
    switch (tab)
    {
    case 0:
        aimbotTab();
        break;
    case 1:
        visualsTab();
        break;
    case 2:
        miscTab();
        break;
    case 3:
        skinTab(shouldBreak);
        break;
    }

    //Example shit
    ImGui::Checkbox("Checkbox", &te1);
    static ImGuiComboFlags flags = 0;
    const char* items[] = { "None", "Aim", "Play"};
    static int item_current_idx = 0; // Here we store our selection data as an index.
    const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
    if (ImGui::BeginCombo("Combo", combo_preview_value, flags))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SliderInt("Slider Int", &testInt, 0, 10);
    ImGui::SliderFloat("Slider Float", &testFloat, 0, 100.f);
    ImGui::SliderFloat("Slider Float2", &testFloat, 0, 100, "%.f%%");
    ImGui::Button("Button");


    ImGui::EndChild();

}

void DrawTabs(int& tab, bool& shouldBreak)
{
    struct tabs_data {
        const char* name;
    };

    std::vector<tabs_data> tabs =
    {
        {"##aimbotTab"},
        {"##visualsTab"},
        {"##settingsTab"},
        {"##skinTab"}
    };

    ImDrawList* draw;
    ImVec2 pos;
    draw = ImGui::GetWindowDrawList();
    pos = ImGui::GetWindowPos();
    float tabHeight = 450/tabs.size();
    float tabWidth = 80.f;
    ImU32 iconColor = ImColor(70, 70, 70);

    draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 85, pos.y + 700), ImColor(25, 25, 27), 0.f, 10);
    draw->AddLine(ImVec2(pos.x + 85, pos.y + 0), ImVec2(pos.x + 85, pos.y + 700), ImColor(38, 38, 41), 3);

    ImGui::SetCursorPos(ImVec2(0, 0));

    ImGui::PushFont(gFonts::iconsFont);
    for (unsigned int i = 0; i < tabs.size(); i++)
    {
        bool selected = (tab == i);

        if (ImGui::Selectable(tabs[i].name, &selected, NULL, ImVec2(tabWidth, tabHeight)))
            tab = i;


        if (selected)
        {
            iconColor = ImColor(255, 180, 0);
            ImGui::SetItemDefaultFocus();
        }

        ImGui::PushID(reinterpret_cast<void*>(i));
        draw->AddText(ImVec2(tabWidth/2 - 25.f + 2.f, tabHeight/2 - 25.f + 115.f*i), iconColor, reinterpret_cast<const char*>(menuIcons[i]));
        ImGui::PopID();

        iconColor = ImColor(70, 70, 70);
    }
    ImGui::PopFont();

    DrawPanel(tab, shouldBreak);

}


// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
