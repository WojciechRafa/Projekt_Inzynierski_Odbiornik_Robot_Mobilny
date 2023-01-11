//
// Created by wr on 12/27/22.
//

#include "System.hpp"

System::System(sf::Int64 update_period_microseconds_):
update_period_microseconds(update_period_microseconds_){
    broadcast_connector = std::make_unique<Broadcast_Connector>(port);
}

bool System::update() {

    static sf::Clock clock;
    static sf::Int64 last_update_time_micro = clock.getElapsedTime().asMicroseconds();

    sf::Int64 time_to_wait = update_period_microseconds - (clock.getElapsedTime().asMicroseconds() - last_update_time_micro);
    if(time_to_wait > 0)
        sf::sleep(sf::microseconds(time_to_wait));


    last_update_time_micro = clock.getElapsedTime().asMicroseconds();


        std::list<Time_Object*> time_object_list;

        if(connection_state == Connection_State::broadcast_listen and broadcast_connector != nullptr) {
            if (broadcast_connector->need_update()) {
                time_object_list.push_back(broadcast_connector.get());
            }
        }

        if(connection_state != Connection_State::broadcast_listen){
            if(image_sender == nullptr or custom_data_io == nullptr)
                throw std::exception();

            if(image_sender -> need_update())
                time_object_list.push_back(image_sender.get());

            if(custom_data_io -> need_update())
                time_object_list.push_back(custom_data_io.get());

            if(gpio_control -> need_update()) {
//                bool elo = gpio_control -> need_update();
                time_object_list.push_back(gpio_control.get());
            }
        }


        for(auto& time_object: time_object_list){
            time_object->update();
        }


        // ST - short time
        if(connection_state == Connection_State::broadcast_listen and
           broadcast_connector != nullptr and
           broadcast_connector->get_mode() == Broadcast_Connector::b_connector_mode::connection_confirmed){
            remote_ip_address = broadcast_connector->get_remote_ip();

            connection_state = Connection_State::both_wait_to_pernamant_connect;
            image_sender = std::make_unique<Image_Sender>(port, remote_ip_address);
            custom_data_io = std::make_unique<Custom_Data_IO>(port + 1, remote_ip_address);
            configure_custom_data();

            gpio_control = std::make_unique<GPIO_Control>(*custom_data_io);

            std::cout<<"Procedura zawiązywania polaczenia przez Broadcast zakonczona !"<<std::endl;
            broadcast_connector = nullptr;
        }


        if(connection_state == Connection_State::both_wait_to_pernamant_connect){
            if(image_sender->get_mode() == Pernament_Connector::p_connector_mode::pernament_communication){
                connection_state = Connection_State::image_reciver_work;
            }else if(custom_data_io -> get_mode() == Pernament_Connector::p_connector_mode::pernament_communication){
                connection_state = Connection_State::custom_data_io_work;
            }
        }else if (connection_state == Connection_State::custom_data_io_work){
            if(image_sender->get_mode() == Pernament_Connector::p_connector_mode::pernament_communication){
                connection_state = Connection_State::both_work;
            }
        }else if(connection_state == Connection_State::image_reciver_work){
            if(custom_data_io -> get_mode() == Pernament_Connector::p_connector_mode::pernament_communication){
                connection_state = Connection_State::custom_data_io_work;
            }
        }

    return false;
}

void System::configure_custom_data() {

    Custom_Data_IO::message sended_message_1;
    Custom_Data_IO::message sended_message_2;

    Custom_Data_IO::message recived_message_1;
    Custom_Data_IO::message recived_message_2;
    Custom_Data_IO::message recived_message_3;


    recived_message_1.is_int = true;
    recived_message_1.name   = "Tryb_mocy_lewy_silnik"; // 1 - przód 0 - stop -1 - tył
    recived_message_1.id     = 1;

    recived_message_2.is_int = true;
    recived_message_2.name   = "Tryb_mocy_prawy_silnik";
    recived_message_2.id     = 2;

    recived_message_3.is_int = true;
    recived_message_3.name   = "Iterator"; // zwiększa regularnie swą wartość, informacja o tym chroni przed błędami
    recived_message_3.id     = 3;

    sended_message_1.is_int = true;
    sended_message_1.name   = "Moc_lewy_silnik";
    sended_message_1.id     = 1;

    sended_message_2.is_int = true;
    sended_message_2.name   = "Moc_prawy_silnik";
    sended_message_2.id     = 2;

    custom_data_io->add_recived_message(std::move(recived_message_1));
    custom_data_io->add_recived_message(std::move(recived_message_2));
    custom_data_io->add_recived_message(std::move(recived_message_3));

    custom_data_io->add_sended_message(std::move(sended_message_1));
    custom_data_io->add_sended_message(std::move(sended_message_2));
}
