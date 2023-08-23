//
// Created by nz on 22.08.2023.
//
#include "Kapellmeister.h"
#include "iostream"

void Kapellmeister::eventProcessorUDP(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_READ) {
        //MG_INFO(("Got a response"));

        for(int i = 0; i < c->recv.len; i++){
            //std::cout << (int)c->recv.buf[i] << std::endl;
        }
        if(c->recv.len == sizeof(Reply)){
            memcpy(mirror.status.buffer, c->recv.buf, c->recv.len);
            //store timestamp
        }else{
            MG_INFO(("Bad response length"));
            std::cout << c->recv.len << ' ' << sizeof(Reply) << std::endl;
        }
        /*
        for(int i = 0; i < sizeof(c->rem.ip); i++){
            std::cout << (int)c->rem.ip[i] << std::endl;
            //192
            //168
            //10
            //70
            //0
            //0
            //0
            //0
            //0
            //...
        }
*/

        c->recv.len = 0UL;
    }
}

Json Kapellmeister::requestHandler(Json req){
    Json resp;

    if(req.contains("request")){
        int mirrorInd = 0;
        if(req.contains("mirror")){
            mirrorInd = (int)req.at("mirror") - 1;
            /*
            if(mirrorInd < 0 || mirrorInd > addresses.size()){
                return {
                        {"ok", false},
                        {"err", "mirror index out of bound"}
                };
            }
             */
        }else{
            return {
                    {"ok", false},
                    {"err", "mirror index is missing"}
            };
        }

        if(req.at("request") == "connect"){
            if(mirror.init){
                mirror.timer.stop();
                mg_mgr_free(&mirror.mgr);
                mirror.init = false;
            }
            mg_mgr_init(&mirror.mgr);
            mirror.c = mg_connect(&mirror.mgr, mirror.address.c_str(), eventProcessorUDP, NULL);
            mirror.init = true;

            mirror.timer.start(std::chrono::milliseconds(1000), [this, mirrorInd] {
                mirror.update();
            });
            mirror.update();

            resp = mirror.getStatus();
        }else
        if(req.at("request") == "disconnect"){
            mirror.timer.stop();
            mg_mgr_free(&mirror.mgr);
            mirror.init = false;
            //close connections?
            resp = {
                    {"ok", true}
            };
        }else
        if(req.at("request") == "status"){
            resp = mirror.getStatus();
        }else
        if(req.at("request") == "move"){
            if(req.contains("axis")){
                std::string val = req.at("axis");
                uint8_t axis = 0;
                if(std::equal(val.begin(), val.end(), "vertical")){
                    axis = 0;
                }else if(std::equal(val.begin(), val.end(), "horizontal")){
                    axis = 1;
                }else{
                    return {
                            {"ok", false},
                            {"err", "bad axis"}
                    };
                }
                if(req.contains("speed")){
                    mirror.move(axis, req.at("speed"));
                    resp = {
                            {"ok", true}
                    };
                }else{
                    return {
                            {"ok", false},
                            {"err", "axis direction is missing"}
                    };
                }
            }else{
                return {
                        {"ok", false},
                        {"err", "mirror axis is missing"}
                };
            }
        }else
        if(req.at("request") == "stop"){
            mirror.stop();
            resp = {
                    {"ok", true}
            };
        }else{
            resp = {
                    {"ok", false},
                    {"err", "request is not listed"}
            };
        }
    }else{
        resp = {
                {"ok", false},
                {"err", "request is missing"}
        };
    }
    return resp;
}

void Mirror::update() {
    //check last connection time

    //if (c == NULL) return;
    this->outBuffer[0] = 0;
    this->outBuffer[1] = 0;
    this->outBuffer[2] = 0;
    this->outBuffer[3] = 0;
    this->outBuffer[4] = 0;
    this->outBuffer[5] = 0;
    mg_send(this->c, &outBuffer, 6);

    mg_mgr_poll(&this->mgr, 500);
    //std::puts("received or timeout\n");
}

void Mirror::move(int axis, int speed) {
    bool dir = (speed < 0);

    this->outBuffer[0] = 0;
    this->outBuffer[1] = 0;
    this->outBuffer[2] = 0;
    this->outBuffer[3] = 0;
    this->outBuffer[4] = 0;
    this->outBuffer[5] = 0;
    std::cout << speed << " " << (uint8_t)abs(speed) << std::endl;
    this->outBuffer[axis * 3] = (uint8_t)abs(speed);
    this->outBuffer[axis * 3] = (this->outBuffer[axis * 3] & ~(1UL << 5)) | (dir << 5);
    this->outBuffer[axis * 3] = (this->outBuffer[axis * 3] & ~(1UL << 6)) | (1 << 6);
    this->outBuffer[axis * 3] = (this->outBuffer[axis * 3] & ~(1UL << 7)) | (0 << 7);

    mg_send(this->c, &outBuffer, 6);
    mg_mgr_poll(&this->mgr, 500);
}

void Mirror::stop() {
    this->outBuffer[0] = 0;
    this->outBuffer[1] = 0;
    this->outBuffer[2] = 0;
    this->outBuffer[3] = 0;
    this->outBuffer[4] = 0;
    this->outBuffer[5] = 0;

    this->outBuffer[0]  |= (1 << 5);
    this->outBuffer[0]  |= (0 << 6);
    this->outBuffer[0]  |= (1 << 7);
    this->outBuffer[3]  |= (1 << 5);
    this->outBuffer[3]  |= (0 << 6);
    this->outBuffer[3]  |= (1 << 7);

    mg_send(this->c, &outBuffer, 6);
    mg_mgr_poll(&this->mgr, 500);
}