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
            mg_mgr_init(&mirror.mgr);
            mirror.c = mg_connect(&mirror.mgr, mirror.address.c_str(), eventProcessorUDP, NULL);

            mirror.timer.start(std::chrono::milliseconds(1000), [this, mirrorInd] {
                this->update(mirrorInd);
            });
            this->update(mirrorInd);

            resp = mirror.getStatus();
        }else
        if(req.at("request") == "disconnect"){
            mirror.timer.stop();
            mg_mgr_free(&mirror.mgr);
            //close connections?
            resp = {
                    {"ok", true}
            };
        }else
        if(req.at("request") == "status"){
            resp = mirror.getStatus();
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

void Kapellmeister::update(int index) {
    //check last connection time


    //if (c == NULL) return;
    this->outBuffer[0] = 0;
    this->outBuffer[1] = 1;
    this->outBuffer[2] = 2;
    this->outBuffer[3] = 0;
    this->outBuffer[4] = 1;
    this->outBuffer[5] = 2;
    mg_send(mirror.c, &outBuffer, 6);

    mg_mgr_poll(&mirror.mgr, 500);
    //std::puts("received or timeout\n");
}
