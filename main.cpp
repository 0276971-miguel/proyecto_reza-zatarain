#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <iomanip> // Eeto es para que las columnas no salgan chuecas
#include <algorithm>

namespace fs = std::filesystem;

// aqui guardo los 4 datos que me importan de cada proceso
struct ProcessInfo {
    std::string pid;
    std::string name;
    std::string state;
    std::string memory; 
};

//  se mete a los archivos de linux para sacar la info
ProcessInfo getProcessDetails(const std::string& pid) {
    ProcessInfo proc;
    proc.pid = pid;
    
    // status, donde linux guarda el proceso
    std::ifstream statusFile("/proc/" + pid + "/status");
    std::string line;

    while (std::getline(statusFile, line)) {
        // busco la línea que empieza con name para saber cómo se llama el programa
        if (line.substr(0, 5) == "Name:") {
            proc.name = line.substr(6);
            // esto quita espacios feos al principio del nombre
            proc.name.erase(0, proc.name.find_first_not_of(" \t"));
        } 
        // busco si esta corriendo
        else if (line.substr(0, 6) == "State:") {
            proc.state = line.substr(7, 1);
        } 
        // segundaentrega cuanta ram esta usando 
        else if (line.substr(0, 6) == "VmRSS:") {
            proc.memory = line.substr(7);
            proc.memory.erase(0, proc.memory.find_first_not_of(" \t"));
        }
    }
    
    // si por algo no dice cuánta ram usa, le pongo 0 para que no salga vacío
    if (proc.memory.empty()) proc.memory = "0 kB";
    
    return proc;
}

int main() {
    std::cout << "============================================================" << std::endl;
    std::cout << "   MI PROYECTO: MONITOR DE MEMORIA - MIGUEL REZA            " << std::endl;
    std::cout << "============================================================" << std::endl;
    
    // el número es el espacio que dejamos entre palabras
    std::cout << std::left << std::setw(8) << "PID" 
              << std::setw(10) << "ESTADO" 
              << std::setw(15) << "RAM USADA"
              << "NOMBRE DEL PROGRAMA" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    // entro a la carpeta /proc de mi laptop
    for (const auto& entry : fs::directory_iterator("/proc")) {
        if (entry.is_directory()) {
            std::string dirName = entry.path().filename().string();
            
            // reviso que la carpeta sea un número (si es número, es un proceso)
            if (!dirName.empty() && std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                try {
                    // llamo a la funcion de arriba para sacar la info de este proceso
                    ProcessInfo p = getProcessDetails(dirName);
                    
                    // si el nombre no esta vacío, lo imprimo en la tabla
                    if(!p.name.empty()) {
                        std::cout << std::left << std::setw(8) << p.pid 
                                  << std::setw(10) << p.state 
                                  << std::setw(15) << p.memory
                                  << p.name << std::endl;
                    }
                } catch (...) { 
                    // si un programa se cierra justo ahorita, me lo salto para que no truene
                    continue; 
                }
            }
        }
    }
    
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "Entrega del 22 de abril lista." << std::endl;
    
    return 0;
}
