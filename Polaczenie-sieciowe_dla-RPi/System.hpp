//
// Created by wr on 12/27/22.
//

#ifndef INZYNIERKA_SYSTEM_HPP
#define INZYNIERKA_SYSTEM_HPP

#include "Image_Sender.hpp"
#include "Custom_Data_IO.hpp"
#include "Broadcast_connector.hpp"
#include "Image_Sender.hpp"

#include "GPIO_Control.hpp"

class System {
public:
    explicit System(sf::Int64 update_period_microseconds_ = 1000); // domyślne maksymalne tempo aktualizacji 1ms

    bool update();

    enum class Connection_State{
        broadcast_listen,
        bind_error,
        both_wait_to_pernamant_connect,
        image_reciver_work,
        custom_data_io_work,
        both_work
    };
private:
    sf::Int64 update_period_microseconds;
    Connection_State connection_state = Connection_State::broadcast_listen;
    unsigned short port = 50238;
    sf::IpAddress remote_ip_address = sf::IpAddress::None;

    std::unique_ptr<Broadcast_Connector> broadcast_connector = nullptr;
    std::unique_ptr<Image_Sender> image_sender = nullptr;
    std::unique_ptr<Custom_Data_IO> custom_data_io = nullptr;

    std::unique_ptr<GPIO_Control> gpio_control = nullptr;


    void configure_custom_data();
};


#endif //INZYNIERKA_SYSTEM_HPP
