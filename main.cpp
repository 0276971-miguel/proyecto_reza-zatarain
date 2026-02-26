#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <iomanip>

// Usamos el estándar C++17 para manejar archivos de forma moderna
namespace fs = std::filesystem;

struct ProcessInfo {
    std::string pid;
    std::string name;
    std::string state;
};

// Función técnica: Parsea el archivo /proc/[pid]/status para extraer info del kernel
ProcessInfo getProcessDetails(const std::string& pid) {
    ProcessInfo proc;
    proc.pid = pid;
    std::ifstream statusFile("/proc/" + pid + "/status");
    std::string line;

    // Leemos línea por línea buscando etiquetas específicas
    while (std::getline(statusFile, line)) {
        if (line.substr(0, 5) == "Name:") {
            proc.name = line.substr(6);
            // Limpiar espacios en blanco al inicio
            proc.name.erase(0, proc.name.find_first_not_of(" \t"));
        } else if (line.substr(0, 6) == "State:") {
            proc.state = line.substr(7, 1); // Tomamos solo el primer caracter (R, S, Z, etc.)
        }
    }
    return proc;
}

int main() {
    std::cout << "====================================================" << std::endl;
    std::cout << "   SENTINEL OS - MONITOR DE PROCESOS (ENTREGA 1)    " << std::endl;
    std::cout << "====================================================" << std::endl;
    
    // Encabezados con formato alineado
    std::cout << std::left << std::setw(10) << "PID" 
              << std::setw(10) << "ESTADO" 
              << "NOMBRE DEL PROCESO" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;

    try {
        // Iteramos sobre el sistema de archivos virtual /proc
        for (const auto& entry : fs::directory_iterator("/proc")) {
            if (entry.is_directory()) {
                std::string dirName = entry.path().filename().string();
                
                // Si el nombre de la carpeta es un número, es un proceso activo
                if (!dirName.empty() && std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                    try {
                        ProcessInfo p = getProcessDetails(dirName);
                        if(!p.name.empty()) {
                            std::cout << std::left << std::setw(10) << p.pid 
                                      << std::setw(10) << p.state 
                                      << p.name << std::endl;
                        }
                    } catch (...) {
                        // Ignorar procesos que se cierran durante la lectura
                        continue;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error accediendo a /proc: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "----------------------------------------------------" << std::endl;
    std::cout << "Fin del reporte de procesos activos." << std::endl;
    
    return 0;
}
