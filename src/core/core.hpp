#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using Herramienta = std::function<json(const json&)>;

namespace Core {
    // Registra todas las herramientas disponibles
    void inicializar();

    // Recibe el JSON de la IA y ejecuta la herramienta correspondiente
    json ejecutar(const std::string& jsonStr);

    // Registro global de herramientas
    extern std::unordered_map<std::string, Herramienta> herramientas;
}
