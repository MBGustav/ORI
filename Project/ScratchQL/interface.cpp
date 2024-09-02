#include <d3d11.h>
#include <tchar.h>
#include <vector>
#include <set>
#include "DataInterface.h"
#include "SimpleTableHandler.h"
#include "FileHandler.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Link necessary libraries for DirectX
#pragma comment(lib, "d3d11.lib")

// Function declarations
void readTable();
void mostrartabelas();
void createTable(char name[128], vector <EntityProperties> vector1, int rows);
void showCreateTableWindow();
void showMainMenu();

// Global variables for ImGui
char table_name[128] = "";
int num_columns = 0;
std::vector<EntityProperties> columns;
std::vector<std::string> column_names;
std::vector<int> data_types; // 1: FLOAT, 2: STRING, 3: INT, 4: DATE
int num_rows = 0;

// DirectX variables
HWND hwnd;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* device_context = nullptr;
IDXGISwapChain* swap_chain = nullptr;
ID3D11RenderTargetView* render_target_view = nullptr;

// Forward declaration of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main() {
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    RegisterClassEx(&wc);
    hwnd = CreateWindow(wc.lpszClassName, _T("ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, device_context);

    // Main loop
    bool done = false;
    while (!done) {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        // Start the ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Show the main menu
        showMainMenu();

        // Rendering
        ImGui::Render();
        const float clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
        device_context->OMSetRenderTargets(1, &render_target_view, NULL);
        device_context->ClearRenderTargetView(render_target_view, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swap_chain->Present(1, 0); // Present with vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions for DirectX and ImGui setup

bool CreateDeviceD3D(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2,
                                      D3D11_SDK_VERSION, &sd, &swap_chain, &device, &featureLevel, &device_context) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
    CleanupRenderTarget();
    if (swap_chain) { swap_chain->Release(); swap_chain = nullptr; }
    if (device_context) { device_context->Release(); device_context = nullptr; }
    if (device) { device->Release(); device = nullptr; }
}

void CreateRenderTarget() {
    ID3D11Texture2D* pBackBuffer;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    device->CreateRenderTargetView(pBackBuffer, NULL, &render_target_view);
    pBackBuffer->Release();
}

void CleanupRenderTarget() {
    if (render_target_view) { render_target_view->Release(); render_target_view = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg) {
        case WM_SIZE:
            if (device != NULL && wParam != SIZE_MINIMIZED) {
                CleanupRenderTarget();
                swap_chain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            void createTable(char name[128], vector <EntityProperties> vector1, int rows);

}
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}



void showCreateTableWindow() {
    ImGui::Begin("Create Table");

    ImGui::InputText("Table Name", table_name, sizeof(table_name), ImGuiInputTextFlags_CharsNoBlank);
    ImGui::InputInt("Number of Columns", &num_columns);

    if (ImGui::Button("Add Column")) {
        if (num_columns > 0) {
            columns.push_back({"", DataType::DATA_STRING}); // Placeholder
            column_names.push_back("");
            data_types.push_back(0);
            num_columns--;
        }
    }

    for (size_t i = 0; i < column_names.size(); ++i) {
        char column_name[128] = "";
        string collumn_nameFinal;
        strncpy(column_name, column_names[i].c_str(), sizeof(column_name));
        ImGui::InputText(("Column Name " + to_string(i + 1)).c_str(),
                         column_name, sizeof(column_name), ImGuiInputTextFlags_CharsNoBlank);
        collumn_nameFinal = column_name;
        column_names[i] = collumn_nameFinal;
        ImGui::Combo(("Data Type " + to_string(i + 1)).c_str(), &data_types[i], "FLOAT\0STRING\0INT\0DATE\0");
        DataType type = static_cast<DataType>(data_types[i]);
        columns[i].type = type;
    }



    ImGui::InputInt("Number of Rows", &num_rows);


    if (ImGui::Button("Criar Tabela")) {
        for (size_t i = 0; i < column_names.size(); ++i) {
            columns[i].name = column_names[i];
            columns[i].type = static_cast<DataType>(data_types[i]);
        }

        createTable(table_name, columns, num_rows);
    }



    ImGui::End();
}

void createTable(char name[128], vector <EntityProperties> vector1, int rows) {
    string name_final = name;
    SimpleTableHandler test(name_final, vector1, false);
    for (size_t i = 0; i < rows; ++i) {
        vector<DataInterface *> row;
        for (const auto &column : vector1) {
            std::string input_final;
            char input[64] = "";
            ImGui::InputText(("Valor para " + column.name).c_str(), input, ImGuiInputTextFlags_CharsNoBlank);

            input_final = input;

            // Allocate the appropriate DataInterface based on the data type
            row.push_back(dt_alloc(column.type, input_final));
        }
        test.write_row(row);

        // Clean up dynamically allocated memory
        for (auto ptr : row) {
            delete ptr;
        }
    }

    test.display();
    std::remove(name);
}

void showMainMenu() {
    ImGui::Begin("Main Menu");

    static int option = 0;
    ImGui::Text("Escolha uma opção:");
    ImGui::RadioButton("Criar nova tabela", &option, 1);
    ImGui::RadioButton("Ler tabela", &option, 2);
    ImGui::RadioButton("Atualizar tabela (não implementado)", &option, 3);
    ImGui::RadioButton("Deletar tabela", &option, 4);
    ImGui::RadioButton("Sair", &option, 99);

    if (option == 1) {
        showCreateTableWindow();
    } else if (option == 2) {
        mostrartabelas();
        readTable();
    } else if (option == 4) {
        ImGui::InputText("Qual tabela deseja deletar?", table_name, sizeof(table_name), ImGuiInputTextFlags_CharsNoBlank);
        if (ImGui::Button("Deletar")) {
            SimpleTableHandler::delete_table(table_name);
        }
    }

    ImGui::End();
}


void mostrartabelas() {
    // Exibir tabelas disponíveis
    auto list = list_tables(HEADERS_ONLY);
    for (const auto &table : list) {
        ImGui::Text(table.c_str());
    }
}

void readTable() {
    // Ler tabela através da interface
    ImGui::InputText("Nome da Tabela", table_name, sizeof(table_name), ImGuiInputTextFlags_CharsNoBlank);

    if (ImGui::Button("Ler Tabela")) {
        SimpleTableHandler test(table_name, false);
        test.display();
    }

    // Pesquisa por índice
    static int tipo = 1;
    ImGui::Combo("Pesquisar por", &tipo, "Primário\0Secundário\0Sair\0");
    if (tipo == 1) {
        char valor[128] = "";
        ImGui::InputText("Valor do Índice Primário", valor, sizeof(valor), ImGuiInputTextFlags_CharsNoBlank);
        if (ImGui::Button("Pesquisar")) {
            if (strlen(valor) > 0) {
                SimpleTableHandler test(table_name, false);
                auto row = test.read_pkey(valor);
                if (row.empty()) {
                    ImGui::Text("Nenhum registro encontrado");
                } else {
                    int numero_coluna = 1;
                    ImGui::Text("Registro encontrado:");
                    for (const auto &data: row) {
                        ImGui::Text("Valor da Coluna %d == %s", numero_coluna, data->toString().c_str());
                        numero_coluna++;
                    }
                }
            }
        }
    } else if (tipo == 2) {
        char coluna[128] = "", valor[128] = "";
        ImGui::InputText("Nome da Coluna", coluna, sizeof(coluna), ImGuiInputTextFlags_CharsNoBlank);
        ImGui::InputText("Valor a Pesquisar", valor, sizeof(valor), ImGuiInputTextFlags_CharsNoBlank);
        if (ImGui::Button("Pesquisar")) {
            if (strlen(coluna) > 0 && strlen(valor) > 0) {
                SimpleTableHandler test(table_name, false);
                vector<vector<DataInterface *>> rows;
                rows = test.read_skey(valor, coluna);
                if (rows.empty()) {
                    ImGui::Text("Nenhum registro encontrado");
                } else {
                    ImGui::Text("Registros encontrados:");
                    int numero_linha = 1;
                    for (const auto &row : rows) {
                        ImGui::Text("Linha %d:", numero_linha);
                        int numero_coluna = 1;
                        for (const auto &data : row) {
                            ImGui::Text("Valor da Coluna %d == %s", numero_coluna, data->toString().c_str());
                            numero_coluna++;
                        }
                        numero_linha++;
                    }
                }
            }
        }
    }
}