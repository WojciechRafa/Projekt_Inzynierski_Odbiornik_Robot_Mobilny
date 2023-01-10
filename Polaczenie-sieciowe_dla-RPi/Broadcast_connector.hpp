#pragma once

#include <SFML/Network.hpp>
#include <iostream>
#include <cstdlib>
#include <string>
#include "Time_Object.hpp"

class Broadcast_Connector: public Time_Object{
public:
    enum class b_connector_mode{
        waiting_to_first_conntact,
        send_response_and_waiting_to_second_conntact,
        connection_confirmed
    };

	explicit Broadcast_Connector(unsigned short port_);
	void update() override;
    b_connector_mode get_mode();
	sf::IpAddress get_remote_ip();
private:
	unsigned short port;
	sf::UdpSocket udp_socket;
	
	b_connector_mode mode = b_connector_mode::waiting_to_first_conntact;
	
	sf::IpAddress remote_dev_ip;
};
