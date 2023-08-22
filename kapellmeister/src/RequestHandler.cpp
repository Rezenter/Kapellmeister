//
// Created by nz on 22.08.2023.
//
#include "RequestHandler.h"
#include <chrono>

using Json = nlohmann::json;

std::basic_string<char> handleRequest(char* request_str){
    Json payload;
    try{
        payload = Json::parse(request_str);
    }catch(Json::parse_error& err){
        return to_string(Json({
                                      {"ok", false},
                                      {"err", "request is not a valid JSON"}
                              }));
    }

    Json resp;
    if(payload.contains("subsystem")){
        if(payload.at("subsystem") == "mirror"){
            resp = mirror.requestHandler(payload);
        }else{
            resp = {
                    {"ok", false},
                    {"err", "requested subsystem not found"}
            };
        }


    }else{
        resp = {
                {"ok", false},
                {"err", "request has no 'subsystem'"}
        };
    }
    resp["unix"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    return to_string(resp);
};