#include "mongoose.h"
#include <iostream>
#include <cstring>
#include <string>

const char* root = "../html";
struct mg_http_serve_opts opts = {
        //.mime_types = extension_to_type(extension)
};

static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        if (mg_http_match_uri(hm, "/api/hello")) {              // On /api/hello requests,
            mg_http_reply(c, 200, "", "{%m:%d}\n",
                          MG_ESC("status"), 1);                   // Send dynamic JSON response
        } else {                                                // For all other URIs,
            char * path;
            if (hm->uri.ptr[hm->uri.len - 1] == '/') {
                path = new char [strlen(root) + hm->uri.len + strlen("index.html") + 1]();
                std::strcpy (path,root);
                std::memcpy(path + strlen(root), hm->uri.ptr, hm->uri.len);
                std::strcat(path, "index.html");
            }else{
                path = new char [strlen(root) + hm->uri.len + 1]();
                std::strcpy (path, root);
                std::memcpy(path + strlen(root), hm->uri.ptr, hm->uri.len);
            }

            mg_http_serve_file(c, hm, path, &opts);

            delete[] path;
        }
    }
}


int main() {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);                                      // Init manager
    mg_http_listen(&mgr, "http://0.0.0.0:8000", fn, NULL);  // Setup listener
    for (;;) mg_mgr_poll(&mgr, 1000);                       // Infinite event loop

    return 0;
}