#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

int String_to_int(std::string s) {
    int res = 0;
    for (int i = 0; i < s.size(); i++) {
        res *= 10;
        res += int(s[i]) - int('0');
    }
    return res;
}

std::string Red_opening(std::string opening, int n) {
    std::string res = "";
    for (int i = 0; i < opening.size(); i++) {
        if (res.size() == 0 || res[res.size() - 1] == '\n') {
            for (int j = 0; j < n; j++) {
                res += '\t';
            }
        }
        res += opening[i];
    }
    return res;
}

class App {
    std::map<std::string, nlohmann::json> chars;
    std::vector<nlohmann::json> films;
    std::vector<nlohmann::json> planets;
    httplib::Client* cli;
    static std::string Char_Info_req;
    static std::string Film_Info_req;
    static std::string Planet_Info_req;
    nlohmann::json Make_request(std::string req) {
        auto jsn = cli->Get(req);
        if (jsn->status != 200) {
            std::cout << "error!\n";
        }
        nlohmann::json res = nlohmann::json::parse(jsn->body);
        return res;
    }
public:
    App() {
        this->cli = new httplib::Client("https://swapi.dev");
        nlohmann::json res1 = nlohmann::json::parse(cli->Get(Film_Info_req)->body);
        for (int i = 0; i < 6; i++) {
            films.push_back(res1["results"][i]);
        }
        nlohmann::json res2 = nlohmann::json::parse(cli->Get(Planet_Info_req)->body);
        for (int i = 0; i < 60; i++) {
            planets.push_back(res2["results"][i]);
        }
    }
    void Print_Char_Info(std::string character) {
        nlohmann::json res = NULL;
        if (chars.find(character) == chars.end()) {
            res = Make_request(Char_Info_req + "?search=" + character);
            if (res["count"] != 1) {
                res = NULL;
            }
            else {
                chars[character] = res["results"][0];
                res = chars[character];
            }
        }
        else {
            res = chars[character];
        }
        if (res == NULL) {
            std::cout << "uncorrect name!\n";
        }
        else {
            std::cout << res["name"].get<std::string>().substr(0, res["name"].size() - 2) << '\n';
            std::cout << "\theight - " << res["height"] << '\n';
            std::cout << "\tweight - " << res["mass"] << '\n';
            std::cout << "\thomeworld - " << 
            planets[String_to_int(res["homeworld"].get<std::string>().
            substr(30, 1 + res["homeworld"].get<std::string>().size() - 32)) - 1]["name"] << '\n';
            std::cout << "\tfilms:\n";
            for (int i = 0; i < res["films"].size(); i++) {
                std::cout << "\t\t" << films[int(res["films"][i].get<std::string>()[28])
                - int('1')]["title"] << '\n';
            }
        }
    }
    void Print_Film_Info(std::string film) {
        bool was = false;
        for (int i = 0; i < films.size(); i++) {
            if (films[i]["title"].get<std::string>() == film) {
                std::cout << films[i]["title"].get<std::string>().substr(0, 
                films[i]["name"].size() - 2) << '\n';
                std::cout << "\tdirector - " << films[i]["director"].get<std::string>().substr(0,
                films[i]["director"].size() - 2) << '\n';
                std::cout << "\tproducer - " << films[i]["producer"].get<std::string>().substr(0,
                films[i]["producer"].size() - 2) << '\n';
                std::cout << "\trelease date - " << films[i]["release_date"].get<std::string>().substr(0,
                films[i]["release_date"].size() - 2) << '\n';
                std::cout << "\topening:\n";
                std::cout << Red_opening(films[i]["opening_crawl"].get<std::string>(), 2) << '\n';
                was = true;
                break;
            }
        }
        if (!was) {
            std::cout << "uncorrect film!\n";
        }
    }
    void Print_Planet_Info(std::string planet) {
        bool was = false;
        for (int i = 0; i < planets.size(); i++) {
            if (planets[i]["name"] == planet) {
                std::cout << planets[i]["name"].get<std::string>().substr(0, 
                planets[i]["name"].size() - 2) << '\n';
                std::cout << "\tclimate - " << planets[i]["climate"].get<std::string>().substr(0,
                planets[i]["climate"].size() - 2) << '\n';
                std::cout << "\tterrain - " << planets[i]["terrain"].get<std::string>().substr(0,
                planets[i]["terrain"].size() - 2) << '\n';
                std::cout << "\tpopulation - " << planets[i]["population"].get<std::string>().substr(0,
                planets[i]["population"].size() - 2) << '\n';
                was = true;
                break;
            }
        }
        if (!was) {
            std::cout << "uncorrect planet!\n";
        }
    }
};

std::string App::Film_Info_req = "/api/films/";
std::string App::Planet_Info_req = "/api/planets/";
std::string App::Char_Info_req = "/api/people";

void Clear() {
    std::cout << "Press Enter...\n";
    getchar();
    system("cls");
}

void Show_Menu() {
    std::cout << "Welcome to my app!\n";
    std::cout << "What do you want to do?\n";
    std::cout << "1)See Character information(character)\n";
    std::cout << "2)See Film information(film)\n";
    std::cout << "3)See Planet information(planet)\n";
    std::cout << "4)Quit(quit)\n";
    std::cout << "comand> ";
}

std::pair<int, std::string> Comand(std::string comand) {
    if (comand.substr(0, 9) == "character") {
        return std::make_pair(1, comand.substr(10, comand.size() - 10));
    }
    else if (comand.substr(0, 4) == "film") {
        return std::make_pair(2, comand.substr(5, comand.size() - 5));
    }
    else if (comand.substr(0, 6) == "planet") {
        return std::make_pair(3, comand.substr(7, comand.size() - 7));
    }
    else if (comand == "quit") {
        return std::make_pair(4, "vseNorm");
    }
    else {
        return std::make_pair(4, "vseNeNorm");
    }
}

int main()
{
    std::cout << "Loading...\n";
    std::string comand;
    App app;
    std::pair<int, std::string> Do;
    bool work = true;
    system("cls");
    while (work) {
        Show_Menu();
        std::getline(std::cin, comand);
        Do = Comand(comand);
        switch(Do.first) {
        case 1:
            app.Print_Char_Info(Do.second);
            break;
        case 2:
            app.Print_Film_Info(Do.second);
            break;
        case 3:
            app.Print_Planet_Info(Do.second);
            break;
        case 4:
            if (Do.second == "vseNorm") {
                work = false;
            }
            else if (Do.second == "vseNeNorm") {
                std::cout << "Uncorrect comand!\n";
            }
            break;
        }
        Clear();
    }
}