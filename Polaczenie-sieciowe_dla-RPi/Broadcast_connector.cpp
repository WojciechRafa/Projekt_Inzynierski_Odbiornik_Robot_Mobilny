#include "Broadcast_connector.hpp"

#include "Sended_struct.hpp"
#include "main_functions.hpp"
#include "Time.hpp"

Broadcast_Connector::Broadcast_Connector(unsigned short port_):
port(port_){
    udp_socket.setBlocking(false);
    auto bind_status = udp_socket.bind(port);
    if(bind_status != sf::Socket::Status::Done) {
        std::cout << "Bind error \n";
        Delay_ms(1000);
    }

    update_period_microseconds = 1000000;
}

void Broadcast_Connector::update(){

    if(mode == b_connector_mode::waiting_to_first_conntact){
        sf::Packet packet_received;
        sf::Socket::Status status = udp_socket.receive(packet_received, remote_dev_ip, port);
        write_comunicate_sockte_status(status);
        static int itr = 0;
        std::cout<<itr<<std::endl;
        itr++;

        std::cout << "Status polaczenia  "<< udp_socket.getLocalPort() <<" "<<port<< " "<< udp_socket.isBlocking() << " "<< remote_dev_ip.toString()<<std::endl;

        if (status == sf::Socket::Done) {
            if (remote_dev_ip.toInteger() == get_ip(packet_received).toInteger() and remote_dev_ip.toInteger() != 0) {
                std::cout << "IP kontroler : " << remote_dev_ip.toString() << std::endl;
                mode = b_connector_mode::send_response_and_waiting_to_second_conntact;
            }
        }
    }else if(mode == b_connector_mode::send_response_and_waiting_to_second_conntact){

        Double_ip_message ip_message_sended = {
                sf::IpAddress::getLocalAddress(),
                remote_dev_ip
        };

        std::cout << "Wyslano:"<< std::endl;
        std::cout << remote_dev_ip.toString()<< std::endl;
        std::cout << sf::IpAddress::getLocalAddress().toString()<< std::endl;
        // Wysłanie wiadmości
        sf::Packet sended_packet;
        sended_packet << ip_message_sended;

        udp_socket.send(sended_packet, remote_dev_ip, port);
        std::cout << "Wyslano potwierdzenie"<< std::endl;

        // Odbieranie wiadomości
        sf::Packet received_packet;
        sf::IpAddress sender_ip;
        if (udp_socket.receive(received_packet, sender_ip, port) == sf::Socket::Done) {

            Double_ip_message ip_message_received;
            received_packet >> ip_message_received;

            std::cout << "Odebrano potwiedzenie"<< std::endl;

            std::cout << "Odbiornik : "<< ip_message_received.receiver.toString() << std::endl;
            std::cout << "Nadajnik :  "<< ip_message_received.sender.toString() << std::endl;


            if(sf::IpAddress::getLocalAddress() == ip_message_received.receiver and
               remote_dev_ip == ip_message_received.sender
                    ){
                std::cout << "Komunikacja potwierdzona "<< std::endl;
                mode = b_connector_mode::connection_confirmed;
            }
        }
    }else{
        throw std::runtime_error("Nieprawidłowy tryb pracy wyszukiwania połączenia");
    }
    last_update_time = clock.getElapsedTime().asMicroseconds();
}


sf::IpAddress Broadcast_Connector::get_remote_ip(){
    return remote_dev_ip;
}

Broadcast_Connector::b_connector_mode Broadcast_Connector::get_mode() {
    return mode;
};
