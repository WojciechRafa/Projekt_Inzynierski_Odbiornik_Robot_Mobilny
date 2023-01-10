#include "Pernament_Connector.hpp"

#include "Sended_struct.hpp"
#include "main_functions.hpp"

Pernament_Connector::Pernament_Connector(unsigned short port_, sf::IpAddress remote_dev_ip_):
	port(port_),
	remote_dev_ip(remote_dev_ip_){
		
	auto listen_status = tcp_listener.listen(port);
    
	std::cout<<"TCP - listener status: \n";
	write_comunicate_sockte_status(listen_status);
	
	setBlocking(false);
    tcp_listener.setBlocking(false);
}


Pernament_Connector::p_connector_mode Pernament_Connector::get_mode() {
    return mode;
}

void Pernament_Connector::update() {

    if(mode == p_connector_mode::establish_connection){
        bool is_block =isBlocking();
        auto status = tcp_listener.accept(*this);

        if(status == sf::Socket::Status::Done) {
            std::cout<<"TCP - Połączenie poprawne \n";
            mode = p_connector_mode::pernament_communication;
            setBlocking(true);
        }
    }else
        throw std::exception();

    last_update_time = clock.getElapsedTime().asMicroseconds();
}
