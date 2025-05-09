#include <algorithm>
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

namespace library {

using page_number = uint16_t;
using user_counter = uint32_t;
using user_id = uint32_t;

static const page_number MAX_PAGE_NUMBERS = 1000;
static const user_counter MAX_USER_ID = 100000;

enum class Command{
    READ, CHEER
};

struct Request {
    Command cmd;
    user_id id = 0;
    page_number page = 0;
};

//парсинг строки запроса. возвращает структуру Request
Request ParseRequest(string_view row_request){

    Request req;

    auto first_space_pos = row_request.find(' ');

    auto second_space_pos = row_request.find(' ', first_space_pos + 1);

    string cmd = string(row_request.substr(0,first_space_pos));

    if( cmd == "CHEER"){

        req.cmd = Command::CHEER;
        req.id = static_cast<user_id>(stoi(string(row_request.substr(first_space_pos))));

    } else if (cmd == "READ") {

        req.cmd = Command::READ;
        req.id = static_cast<user_id>(stoi(string(row_request.substr(first_space_pos,second_space_pos))));

        req.page = static_cast<page_number>(stoi(string(row_request.substr(second_space_pos))));

    }
    
    return req;
}

//обработка запроса
vector<Request> GetRequest(istream& in_stream){
    
    vector<Request> result;

    uint32_t req_count;
    in_stream >> req_count;
    
    string tmp;
    getline(in_stream, tmp);

    for (uint32_t i = 0; i < req_count; ++i) {
        string line;
        getline(in_stream, line);
        result.push_back(ParseRequest(line));
    }

    return result;
}

//рассчитать процент отстающих от пользователя, который прочитал page_number страницы
double CalculateLooserPersent(vector<user_counter>& users_progress, page_number user_page){

    //если пользователь дочитал до 0 страницы (вообще не читал) - возвращаем 0.0
    if (user_page == 0){
        return 0.0;
    }
    
    //если читатель один - всегда возвращаем 1.0
    if ( users_progress[1] == 1 ){
        return 1.0;
    } 

    //если только пользователь дочитал до user_page - возвращаем 1.0
    if (users_progress[user_page] == 1){
        return 1.0;
    }

    //если общее количество читателей равно количеству читателей, до которой дочитал пользователь - возвращаем 0.0    
    if (users_progress[1] == users_progress[user_page]){
        return 0.0;
    }

    double result = static_cast<double>( users_progress[1] - users_progress[user_page]) / static_cast<double>(users_progress[1] - 1);

    return result;
}

//увеличивает счётчик прочтённой страницы на 1 для всех страниц от 1 до page
void AddUserReadInfo(vector<user_counter>& users_progress,vector<page_number>& saved_pages, user_id id, page_number page){

    for (page_number i = saved_pages[id] + 1; i <= page; ++i){
        ++users_progress[i];
    }

    saved_pages[id] = page;

}

void Read(istream& in_stream = cin) {

    vector<Request> requests = GetRequest(in_stream);

    vector<page_number> saved_pages;
    saved_pages.resize(MAX_USER_ID + 1);

    vector<user_counter> users_progress;
    users_progress.resize(MAX_PAGE_NUMBERS + 1);

    cout << setprecision(6);

    for (const auto req : requests) {

        switch (req.cmd) {

            case Command::CHEER : 
                cout << CalculateLooserPersent(users_progress, saved_pages[req.id]) << endl;
                break;

            case Command::READ : 
                AddUserReadInfo(users_progress,saved_pages, req.id, req.page);
                break;
        }
    }
}





}
//конце namespace ebook

int main() {
    using namespace library;
    Read();
    return 0;
}
