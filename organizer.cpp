#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <utility>

const char* window_name = " - PPPoker(39.0)";

//ESSA FUNÇÃO RETORNA TODOS OS ID'S (HWND'S) DAS JANELAS ESCOLHIDAS
std::vector<HWND> getWindowsByName() {
    std::vector<HWND> windows;
    HWND hwnd = GetWindow(GetDesktopWindow(), GW_CHILD);
    while (hwnd != NULL) {
        char title[256];
        GetWindowTextA(hwnd, title, sizeof(title));

        if (strcmp(window_name, title) == 0) {
            windows.push_back(hwnd);
        }

        hwnd = GetWindow(hwnd, GW_HWNDNEXT);
    }
    return windows;
}

//ESSA FUNÇÃO SALVA EM UM ARQUIVO .TXT AS POSIÇÕES E OS TAMANHOS DAS JANELAS
void writeWindowProperties(const std::vector<HWND>& windows, const std::string& file_name) {
    std::ofstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Failed to open file " << file_name << " for writing" << std::endl;
        return;
    }

    for (auto hwnd : windows) {
        RECT rect;
        if (GetWindowRect(hwnd, &rect)) {
            file << "Position: (" << rect.left << "," << rect.top << ")" << std::endl;
            file << "Size: [" << rect.right - rect.left << "x" << rect.bottom - rect.top << "]" << std::endl;
        } else {
            std::cerr << "Failed to get window rect for handle " << hwnd << std::endl;
        }
    }
    file.close();
}

//ESSA FUNÇÃO RETORNA O NÚMERO DE JANELAS COM O NOME ESCOLHIDO
int countWindows() {
    int count = 0;
    HWND hwnd = FindWindow(NULL, window_name);
    while (hwnd != NULL) {
        count++;
        hwnd = FindWindowEx(NULL, hwnd, NULL, window_name);
    }
    return count;
}

void save_infos(){
    std::vector<HWND> windows = getWindowsByName();
    std::string windows_count = std::to_string(windows.size());
    writeWindowProperties(windows, "window_properties_" + windows_count + ".txt");
}

//ESSA FUNÇÃO RECEBE UM VETOR DE POSIÇÕES E UM VETOR DE TAMANHOS
//E ALTERA AS POSIÇÕES E TAMANHOS DAS JANELAS ABERTAS NO MOMENTO
void adjustWindows(const std::vector<std::pair<int, int>> &positions, const std::vector<std::pair<int, int>> &sizes) {
    auto hwnds = getWindowsByName();
    for (int i = 0; i < hwnds.size(); ++i) {
        MoveWindow(hwnds[i], positions[i].first, positions[i].second, sizes[i].first, sizes[i].second, TRUE);
    }
}

//ESSA FUNÇÃO LÊ O ARQUIVO .TXT E CRIA OS VETORES DE POSIÇÕES E TAMANHOS
//E AS ENVIA PARA O MÉTODO adjustWindows 
void readFileAndOrganize() {
    std::ifstream file("window_properties_" + std::to_string(countWindows()) + ".txt"); // open the file for reading
    std::string line;
    std::vector<std::pair<int, int>> positions;
    std::vector<std::pair<int, int>> size;
    while (std::getline(file, line)) {
        if (line.find("Position") != std::string::npos) {
            int x, y;
            size_t sep1 = line.find('(');
            size_t sep2 = line.find(',');
            size_t sep3 = line.find(')');

            x = std::atoi(line.substr(sep1 + 1, sep2 - sep1 - 1).c_str());
            y = std::atoi(line.substr(sep2 + 1, sep3 - sep2 - 1).c_str());
            
            positions.push_back(std::make_pair(x, y));
        }
        else if (line.find("Size") != std::string::npos) {
            int x, y;
            size_t sep1 = line.find('[');
            size_t sep2 = line.find('x');
            size_t sep3 = line.find(']');

            x = std::atoi(line.substr(sep1 + 1, sep2 - sep1 - 1).c_str());
            y = std::atoi(line.substr(sep2 + 1, sep3 - sep2 - 1).c_str());

            size.push_back(std::make_pair(x, y));
        }
    }

    file.close(); // close the file
    adjustWindows(positions, size);
}


int main() {
    while (true) {
        if (GetAsyncKeyState(VK_F11)) {
            Sleep(100);
            save_infos();
            std::cout<< "Perfil com " + std::to_string(countWindows()) + " telas salvo!" << std::endl;
        }

        if (GetAsyncKeyState(VK_F12)) {
            Sleep(100);
            readFileAndOrganize();
            std::cout<< "Organizado com " + std::to_string(countWindows()) + " telas!" << std::endl;
        }
    }

    return 0;
}