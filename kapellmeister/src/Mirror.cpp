//
// Created by nz on 22.08.2023.
//
#include "Mirror.h"
#include "iostream"

void Mirror::eventProcessorUDP(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_READ) {
        MG_INFO(("Got a response"));
        char* in = new char[c->recv.len + 1];
        std::memcpy(in, c->recv.buf, c->recv.len);
        in[c->recv.len] = NULL;
        for(int i = 0; i < c->recv.len; i++){
            std::cout << (int)c->recv.buf[i] << std::endl;
        }

        c->recv.len = 0UL;
        delete[] in;
    }
}

Json Mirror::requestHandler(Json req){
    Json resp;

    if(req.contains("request")){
        int mirrorInd = 0;
        if(req.contains("mirror")){
            mirrorInd = (int)req.at("mirror") - 1;
            if(mirrorInd < 0 || mirrorInd > addresses.size()){
                return {
                        {"ok", false},
                        {"err", "mirror index out of bound"}
                };
            }
        }else{
            return {
                    {"ok", false},
                    {"err", "mirror index is missing"}
            };
        }

        if(req.at("request") == "connect"){
            mg_mgr_init(&this->mgr);
            c = mg_connect(&mgr, addresses.at(mirrorInd).c_str(), eventProcessorUDP, NULL);

            this->timer.start(std::chrono::milliseconds(1000), [this, mirrorInd] {
                this->update(mirrorInd);
            });
            this->update(mirrorInd);
            //return state
            resp = {
                    {"ok", true}
            };
        }else
        if(req.at("request") == "disconnect"){
            this->timer.stop();
            mg_mgr_free(&this->mgr);
            //close connections?
            resp = {
                    {"ok", true}
            };
        }else
        if(req.at("request") == "state"){
            resp = {
                    {"ok", false},
                    {"err", "not implemented"}
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

void Mirror::update(int index) {
    //check last connection time


    //if (c == NULL) return;
    this->outBuff[0] = 0;
    this->outBuff[1] = 1;
    this->outBuff[2] = 2;
    this->outBuff[3] = 0;
    this->outBuff[4] = 1;
    this->outBuff[5] = 2;
    mg_send(this->c, &outBuff, 6);

    mg_mgr_poll(&this->mgr, 500);
    std::puts("received or timeout\n");
}
