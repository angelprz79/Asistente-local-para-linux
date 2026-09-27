#include <nlohmann/json.hpp>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

using json = nlohmann::json;
using Herramienta = std::function<json(const json&)>;

namespace Herramientas {

void registrarArchivos(std::unordered_map<std::string, Herramienta>& reg) {

    reg["crearArchivo"] = [](const json& p) -> json {
        std::string ruta      = p.value("ruta", "");
        std::string contenido = p.value("contenido", "");
        if (ruta.empty()) return {{"status","error"},{"mensaje","falta la ruta"}};
        std::ofstream f(ruta);
        if (!f) return {{"status","error"},{"mensaje","no se pudo crear: " + ruta}};
        f << contenido;
        return {{"status","ok"},{"ruta", ruta},{"bytes", contenido.size()}};
    };

    reg["leerArchivo"] = [](const json& p) -> json {
        std::string ruta = p.value("ruta", "");
        if (ruta.empty()) return {{"status","error"},{"mensaje","falta la ruta"}};
        std::ifstream f(ruta);
        if (!f) return {{"status","error"},{"mensaje","no se pudo abrir: " + ruta}};
        std::ostringstream ss;
        ss << f.rdbuf();
        return {{"status","ok"},{"ruta", ruta},{"contenido", ss.str()}};
    };

    reg["buscarArchivo"] = [](const json& p) -> json {
        std::string nombre = p.value("nombre", "");
        std::string dir    = p.value("directorio", "~");
        if (nombre.empty()) return {{"status","error"},{"mensaje","falta el nombre"}};
        std::string cmd = "find " + dir + " -name \"" + nombre + "\" 2>/dev/null";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return {{"status","error"},{"mensaje","error ejecutando find"}};
        std::string resultado;
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), pipe)) resultado += buffer;
        pclose(pipe);
        return {{"status","ok"},{"resultados", resultado}};
    };

    reg["copiarArchivo"] = [](const json& p) -> json {
        std::string origen  = p.value("origen", "");
        std::string destino = p.value("destino", "");
        if (origen.empty() || destino.empty())
            return {{"status","error"},{"mensaje","faltan origen o destino"}};
        std::string cmd = "cp -r \"" + origen + "\" \"" + destino + "\"";
        int r = std::system(cmd.c_str());
        return {{"status", r == 0 ? "ok" : "error"},
                {"origen", origen}, {"destino", destino}};
    };

    reg["moverArchivo"] = [](const json& p) -> json {
        std::string origen  = p.value("origen", "");
        std::string destino = p.value("destino", "");
        if (origen.empty() || destino.empty())
            return {{"status","error"},{"mensaje","faltan origen o destino"}};
        std::string cmd = "mv \"" + origen + "\" \"" + destino + "\"";
        int r = std::system(cmd.c_str());
        return {{"status", r == 0 ? "ok" : "error"},
                {"origen", origen}, {"destino", destino}};
    };
}

} // namespace Herramientas
