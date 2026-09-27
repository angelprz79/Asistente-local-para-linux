#pragma once
#include <string>

namespace IA {
    // Envía el texto del usuario a Ollama
    // Devuelve un string JSON con { "accion": "...", "parametros": {...} }
    std::string interpretar(const std::string& input);
}
