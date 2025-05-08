#include <format>
#include <string>
#include <nlohmann/json.hpp>
#include <print>
#include <mutex>
#include <optional>
#include <unordered_set>
#include "nadi.h"


class main_t{
    std::mutex lock_;
    std::optional<std::jthread> thread_;
    nadi_receive_callback receive_;
    nadi_status handle_management(nadi_message* message){
        auto json = nlohmann::json::parse(message->data);
        if(json.contains("command")){
            auto command  = json["command"];
        }
        return 0;
    }
    public:
    main_t(nadi_receive_callback cb):receive_{cb}{}
    nadi_status send(nadi_message* message){
        try{
            switch (message->channel){
                case 0x8000:
                return handle_management(message);
                default:
                return 0;
            }
        }
        catch(...){

        }
        return 0;
    }
    void free(nadi_message* message){
        delete[] message->meta;
        delete[] message->data;
        delete message;
    }
};



extern "C" {
    DLL_EXPORT nadi_status nadi_init(nadi_instance_handle* instance, nadi_receive_callback cb){
        *instance = new main_t(cb);
        return 0;
    }

    DLL_EXPORT nadi_status nadi_deinit(nadi_instance_handle instance){
        delete instance;
        return 0;
    }

    DLL_EXPORT nadi_status nadi_send(nadi_message* message, nadi_instance_handle instance){
        static_cast<main_t*>(instance)->send(message);
        return 0;
    }

    DLL_EXPORT void nadi_free(nadi_message* message){
        static_cast<main_t*>(message->instance)->free(message);
    }

    DLL_EXPORT const char* nadi_descriptor(){
        return R"({"name":"nadi-cache-node","version":"1.0"})";
    }
}
