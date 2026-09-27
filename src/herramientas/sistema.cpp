#include <nlohmann/json.hpp>
#include <unordered_map>
#include <functional>
#include <cstdlib>
#include <string>

using json = nlohmann::json;
using Herramienta = std::function<json(const json&)>;

namespace Herramientas {

void registrarSistema(std::unordered_map<std::string, Herramienta>& reg) {

    reg["abrirAplicacion"] = [](const json& p) -> json {
        std::string nombre = p.value("nombre", "");
        if (nombre.empty()) return {{"status","error"},{"mensaje","falta el nombre"}};
        std::string cmd = nombre + " &";
        int r = std::system(cmd.c_str());
        return {{"status", r == 0 ? "ok" : "error"},
                {"mensaje", "abriendo " + nombre}};
    };

    reg["cerrarAplicacion"] = [](const json& p) -> json {
        std::string nombre = p.value("nombre", "");
        if (nombre.empty()) return {{"status","error"},{"mensaje","falta el nombre"}};
        std::string cmd = "pkill -x " + nombre;
        int r = std::system(cmd.c_str());
        return {{"status", r == 0 ? "ok" : "error"},
                {"mensaje", "cerrando " + nombre}};
    };

    reg["ejecutarTerminal"] = [](const json& p) -> json {
        std::string comando = p.value("comando", "");
        if (comando.empty()) return {{"status","error"},{"mensaje","falta el comando"}};
        int r = std::system(comando.c_str());
        return {{"status", r == 0 ? "ok" : "error"},
                {"comando", comando}};
    };

    reg["abrirCarpeta"] = [](const json& p) -> json {
        std::string ruta = p.value("ruta", "~");
        std::string cmd  = "xdg-open \"" + ruta + "\" &";
        std::system(cmd.c_str());
        return {{"status","ok"},{"ruta", ruta}};
    };

    reg["abrirVSCode"] = [](const json& p) -> json {
        std::string ruta = p.value("ruta", ".");
        std::string cmd  = "code \"" + ruta + "\" &";
        int r = std::system(cmd.c_str());
        return {{"status", r == 0 ? "ok" : "error"},
                {"ruta", ruta}};
    };

    reg["compilarCpp"] = [](const json& p) -> json {
        std::string ruta = p.value("ruta", ".");
        std::string cmd  = "cd \"" + ruta + "\" && cmake -B build && cmake --build build";
        int r = std::system(cmd.c_str());
        return {{"status", r == 0 ? "ok" : "error"},
                {"ruta", ruta}};
    };
}

} // namespace Herramientas
