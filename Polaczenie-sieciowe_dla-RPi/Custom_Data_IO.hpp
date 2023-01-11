//
// Created by wr on 12/27/22.
//

#ifndef INZYNIERKA_CUSTOM_DATA_IO_HPP
#define INZYNIERKA_CUSTOM_DATA_IO_HPP

#include "Pernament_Connector.hpp"

class Custom_Data_IO: public Pernament_Connector{
public:
    struct message{
        sf::Int32 id;
        std::string name = "Nie nadano nazwy";
        bool is_int; // jeżeli false to float
        bool is_read_from_remote_device = true;
        sf::Int32 data_int = 0;
        float data_float = 0.0;
    };
    Custom_Data_IO(unsigned short port_, sf::IpAddress remote_dev_ip_);

    void update() override;
    void add_sended_message(Custom_Data_IO::message&& message);
    void add_recived_message(Custom_Data_IO::message&& message);

    bool update_variable_by_name_int(const std::string& name, sf::Int32 value);
    bool update_variable_by_name_float(const std::string& name, float value);

    bool get_variable_by_name_int(const std::string& name, sf::Int32& value);
    bool get_variable_by_name_float(const std::string& name, float& value);

    bool get_variable_by_id_int(const std::string& name, sf::Int32& value);
    bool get_variable_by_id_float(const std::string& name, float& value);

    std::vector<Custom_Data_IO::message>& get_recived_message_list();


private:
    std::vector<Custom_Data_IO::message> sended_message_list;
    std::vector<Custom_Data_IO::message> recived_message_list;

    Custom_Data_IO::message* find_message_by_id_recived(sf::Int32 id);

    bool update_received(sf::Packet& recived_packet);

    sf::Packet prepare_packet_to_send();

    bool receive_n_time(sf::Packet& received_packet);
    void display_recived_data();

    const int max_number_of_recived_check = 10;
};
// :)

#endif //INZYNIERKA_CUSTOM_DATA_IO_HPP
