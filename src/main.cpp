#include <iostream>
#include <string>
#include "core/core.hpp"
#include "ia/ia.hpp"

// Colores ANSI
#define RESET   "\033[0m"
#define VERDE   "\033[32m"
#define AZUL    "\033[34m"
#define ROJO    "\033[31m"
#define GRIS    "\033[90m"
#define BOLD    "\033[1m"

void mostrarResultado(const json& resultado) {
    std::string status = resultado.value("status", "error");
    if (status == "ok") {
        std::cout << VERDE "  ✓ " RESET;
    } else {
        std::cout << ROJO  "  ✗ " RESET;
    }
    // Imprime cada campo del resultado
    for (auto& [clave, valor] : resultado.items()) {
        if (clave == "status") continue;
        std::cout << GRIS << clave << ": " << RESET;
        if (valor.is_string())
            std::cout << valor.get<std::string>();
        else
            std::cout << valor.dump();
        std::cout << "\n    ";
    }
    std::cout << "\n";
}

int main() {
    std::cout << BOLD "\nasistente " RESET GRIS "v0.1 · ollama:mistral · arch linux\n" RESET;
    std::cout << GRIS "────────────────────────────────────\n" RESET;

    Core::inicializar();
    std::cout << "\n";

    std::string input;
    while (true) {
        std::cout << AZUL "› " RESET;
        if (!std::getline(std::cin, input)) break;
        if (input.empty()) continue;
        if (input == "salir" || input == "exit") break;

        std::cout << GRIS "  pensando..." RESET "\n";

        // M1: IA interpreta
        std::string jsonCmd = IA::interpretar(input);

        // Muestra el JSON para debug (quitar después)
        std::cout << GRIS "  → " << jsonCmd << "\n" RESET;

        // M2: Core ejecuta
        json resultado = Core::ejecutar(jsonCmd);
        mostrarResultado(resultado);
    }

    std::cout << GRIS "\nhasta luego.\n" RESET;
    return 0;
}
