#include "ia.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>

using json = nlohmann::json;

// ── system prompt ────────────────────────────────────────────────
static const std::string SYSTEM_PROMPT = R"(
Eres el intérprete de comandos de un asistente personal en Linux.
Tu única función es entender lo que el usuario quiere hacer en lenguaje natural
y convertirlo al JSON correspondiente. El usuario habla en español casual.

ACCIONES DISPONIBLES:
- abrirAplicacion     → params: { "nombre": string }
- cerrarAplicacion    → params: { "nombre": string }
- ejecutarTerminal    → params: { "comando": string }
- abrirCarpeta        → params: { "ruta": string }
- crearArchivo        → params: { "ruta": string, "contenido": string }
- leerArchivo         → params: { "ruta": string }
- buscarArchivo       → params: { "nombre": string, "directorio": string }
- compilarCpp         → params: { "ruta": string }
- abrirVSCode         → params: { "ruta": string }
- buscarInternet      → params: { "query": string }

REGLAS:
1. Responde SOLO con JSON válido, sin explicaciones ni texto extra.
2. Si no entiendes responde: {"accion":"desconocido","parametros":{}}
3. Formato exacto: {"accion":"nombreAccion","parametros":{...}}
4. Interpreta la INTENCIÓN, no las palabras exactas.
5. El usuario nunca te dirá el nombre exacto del comando, tú lo deduces.

EJEMPLOS DE LENGUAJE CASUAL → JSON:

"oye abre el visual code"
{"accion":"abrirVSCode","parametros":{"ruta":"."}}

"abre una pestaña del vs code"
{"accion":"abrirVSCode","parametros":{"ruta":"."}}

"quiero programar, abre el editor"
{"accion":"abrirVSCode","parametros":{"ruta":"."}}

"abre el code en mi carpeta de proyectos"
{"accion":"abrirVSCode","parametros":{"ruta":"~/proyectos"}}

"oye necesito una terminal"
{"accion":"ejecutarTerminal","parametros":{"comando":"foot &"}}

"abre el navegador"
{"accion":"abrirAplicacion","parametros":{"nombre":"brave"}}

"cierra el spotify"
{"accion":"cerrarAplicacion","parametros":{"nombre":"spotify"}}

"crea un archivo para mis notas de hoy"
{"accion":"crearArchivo","parametros":{"ruta":"~/notas_hoy.txt","contenido":""}}

"oye busca cómo usar threads en c++"
{"accion":"buscarInternet","parametros":{"query":"como usar threads en c++"}}

"compila lo que tengo en proyectos"
{"accion":"compilarCpp","parametros":{"ruta":"~/proyectos"}}

"dónde está mi archivo de configuración de cmake"
{"accion":"buscarArchivo","parametros":{"nombre":"CMakeLists.txt","directorio":"~"}}

"léeme el main"
{"accion":"leerArchivo","parametros":{"ruta":"./src/main.cpp"}}
)";

// ── callback para recibir respuesta de curl ──────────────────────
static size_t escribirRespuesta(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

// ── función principal ────────────────────────────────────────────
std::string IA::interpretar(const std::string& input) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return R"({"accion":"error","parametros":{"mensaje":"no se pudo iniciar curl"}})";
    }

    // Construye el body para la API de Ollama
    json body = {
        {"model", "mistral"},
        {"stream", false},
        {"messages", json::array({
            {{"role", "system"}, {"content", SYSTEM_PROMPT}},
            {{"role", "user"},   {"content", input}}
        })}
    };

    std::string bodyStr   = body.dump();
    std::string respuesta = "";

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:11434/api/chat");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyStr.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirRespuesta);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &respuesta);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 140L);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return R"({"accion":"error","parametros":{"mensaje":"ollama no responde"}})";
    }

    // Ollama devuelve JSON con el mensaje dentro de .message.content
    try {
        json respJson   = json::parse(respuesta);
        std::string txt = respJson["message"]["content"].get<std::string>();

        // Limpia posibles ```json ... ``` que el modelo agregue
        auto inicio = txt.find('{');
        auto fin    = txt.rfind('}');
        if (inicio != std::string::npos && fin != std::string::npos) {
            txt = txt.substr(inicio, fin - inicio + 1);
        }

        // Valida que sea JSON real
        auto validacion = json::parse(txt); // valida que sea JSON real
        (void)validacion; // suprime el warning
        return txt;

    } catch (...) {
        return R"({"accion":"desconocido","parametros":{}})";
    }
}
