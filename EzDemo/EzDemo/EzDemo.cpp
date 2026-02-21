#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include "httplib.h"  
using namespace std;  
#ifdef _WIN32
#pragma comment(lib, "Ws2_32.lib")
#endif

double CelsiusToFahrenheit(double celsius) {
    return (celsius * 9 / 5) + 32;
}
double FahrenheitToCelsius(double fahrenheit) {
    return (fahrenheit - 32) * 5 / 9;
}
double CelsiusToKelvin(double celsius) {
    return celsius + 273;
}
double KelvinToCelsius(double kelvin) {
    return kelvin - 273;
}

static string json_ok(double result) {                                    //değer alınıp stringe çevrilir çünkü http üzerinden frontende sadece string gönderebilirsin 
    return string("{\"ok\":true,\"result\":") + to_string(result) + "}";  // sondaki parantez stringi kapatır ve yarım klaıyormuş gibi gözükmesini engeller
}
static double stod_nothrow(const string& s) { // nothrow= hata fırlatmadan çalışsın anlamında , stringi alıp double a çeviriyor
    try {
        size_t i = 0;
        double v = stod(s, &i);
        if (i != s.size()) return 0.0;
        return v; 
    }                           //kodun güvenli çalışmasıyla ilgili daha çok sayı yerine string falan girerse program patlamasın diye 0.0 çevirir
    catch (...) {               // böylece backend her zaman çalışır
        return 0.0;
    }
}

static string read_text_file(const filesystem::path& path) {
    ifstream in(path, ios::in | ios::binary);
    if (!in) return "";
    ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static void start_server() {       //sunucuyu başlatmak için yardımcı fonksiyonlar
    httplib::Server svr;

    namespace fs = std::filesystem;
    const fs::path cwd = fs::current_path();
    const fs::path p1 = cwd / "public" / "index.html";
    const fs::path p2 = cwd / ".." / ".." / "public" / "index.html";
    fs::path index_path;
    if (fs::exists(p1)) index_path = p1;
    else if (fs::exists(p2)) index_path = p2;

    svr.Get("/api/convert", [](const httplib::Request& req, httplib::Response& res) { // req=gelen isteğin içeriği    res =bizim döndüreceğimiz cevap
        const string type = req.has_param("type") ? req.get_param_value("type") : "CtoF"; // req.has_param paremetresi var mı? varsa req.get_param_value("type") yoksa ctoF type değişkeni dönüşüm tipini tutar.
        const string value = req.has_param("value") ? req.get_param_value("value") : "0"; // değer var mı yok mu onu kontrol ediyor varsa o sayıyı yazıyor yoksa 0 çeviriyor

        const double v = stod_nothrow(value); // bu stringi güvenli şekilde double a çevirir, v=gelen girdinin sayıya çevrilmiş hali
        double out = 0.0; //başlangıçta 0.0 olarak değer atanıyor, out = dönüşüm fonksiyonlarının hesapladığı çıktı.

        if (type == "CtoF")      out = CelsiusToFahrenheit(v);
        else if (type == "FtoC") out = FahrenheitToCelsius(v);
        else if (type == "CtoK") out = CelsiusToKelvin(v);
        else if (type == "KtoC") out = KelvinToCelsius(v);       

        res.set_content(json_ok(out), "application/json"); //sonucu json formatına çevirir Frontend çok kolay şekilde bunu nesneye çevirip kullanabiliyor:
        });

    svr.Get("/", [index_path](const httplib::Request&, httplib::Response& res) {
        if (index_path.empty()) {
            res.status = 500;
            res.set_content("index.html bulunamadi", "text/plain; charset=utf-8");
            return;
        }
        const string html = read_text_file(index_path);
        if (html.empty()) {
            res.status = 500;
            res.set_content("index.html okunamadi", "text/plain; charset=utf-8");
            return;
        }
        res.set_content(html, "text/html; charset=utf-8");
    });

    cout << "HTTP server basladi: http://127.0.0.1:8081\n"; //kullanıcıya bilgi verir
    cout << "Calisma dizini: " << cwd << endl;
    if (index_path.empty()) cout << "HATA: index.html bulunamadi\n";
    else cout << "Index yolu: " << index_path << endl;
    svr.listen("127.0.0.1", 8081); //server ı başlatır
}


int main(int argc, char* argv[]) {
    if (argc > 1 && string(argv[1]) == "--server") {
        start_server();
        return 0;
    }

    int choice;
    double temperature, result;
    cout << "Temperatures Converter" << endl;
    cout << "1. Celsius'tan Fahrenheit'a" << endl;
    cout << "2. Fahrenheit'tan Celsius'a" << endl;
    cout << "3. Celsius'tan Kelvin'e" << endl;
    cout << "4. Kelvin'den Celsius'a" << endl;

    cout << "Lutfen seciminizi giriniz(1-4)" << endl;
    cin >> choice;
    cout << "Lutfen dereceyi giriniz" << endl;
    cin >> temperature;

    switch (choice)
    {
    case 1:
        result = CelsiusToFahrenheit(temperature);
        break;
    case 2:
        result = FahrenheitToCelsius(temperature);
        break;
    case 3:
        result = CelsiusToKelvin(temperature);
        break;
    case 4:
        result = KelvinToCelsius(temperature);
        break;
    }

    cout << "derecenin donusturulmus hali :" << result << endl;
    return 0;
}
