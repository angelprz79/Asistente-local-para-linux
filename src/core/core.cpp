#include "core.hpp"
#include <iostream>

// Declaraciones de los registradores de cada módulo
namespace Herramientas {
    void registrarSistema(std::unordered_map<std::string, Herramienta>&);
    void registrarArchivos(std::unordered_map<std::string, Herramienta>&);
}

// Registro global
std::unordered_map<std::string, Herramienta> Core::herramientas;

void Core::inicializar() {
    Herramientas::registrarSistema(herramientas);
    Herramientas::registrarArchivos(herramientas);
    std::cout << "\033[32m✓\033[0m core inicializado · "
              << herramientas.size() << " herramientas cargadas\n";
}

json Core::ejecutar(const std::string& jsonStr) {
    try {
        json cmd    = json::parse(jsonStr);
        std::string accion = cmd.value("accion", "");
        json params = cmd.value("parametros", json::object());

        // Acción desconocida o error de IA
        if (accion.empty() || accion == "desconocido" || accion == "error") {
            return {{"status", "error"}, {"mensaje", "no entendí esa instrucción"}};
        }

        // Busca la herramienta en el registro
        auto it = herramientas.find(accion);
        if (it == herramientas.end()) {
            return {{"status", "error"}, {"mensaje", "herramienta no encontrada: " + accion}};
        }

        // Ejecuta y devuelve resultado
        return it->second(params);

    } catch (const json::exception& e) {
        return {{"status", "error"}, {"mensaje", std::string("JSON inválido: ") + e.what()}};
    } catch (const std::exception& e) {
        return {{"status", "error"}, {"mensaje", std::string("excepción: ") + e.what()}};
    }
}
