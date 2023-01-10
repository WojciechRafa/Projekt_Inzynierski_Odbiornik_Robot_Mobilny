#include "Image_Sender.hpp"
#include "main_functions.hpp"

#include <iostream>

Image_Sender::Image_Sender(unsigned short port_, sf::IpAddress remote_dev_ip_):
Pernament_Connector(port_, remote_dev_ip_)
{
    update_period_microseconds = 500000; // na pierwszym etapie aktualziacja jest co 0,5s

    // nieaktualne -> funkcja jest wywoływana dwukrotnie ponieważ za pierwszym razem wynik jest błędny
//    camera.measure_operation_time_compress();
    compress_time = camera.measure_operation_time_compress();
}

void Image_Sender::update() {
    if(mode == Pernament_Connector::p_connector_mode::establish_connection){
        update_establish_connection_mode();
    }else{
        update_pernament_communication_mode();
    }
}

bool Image_Sender::update_image_and_number() {
    image_and_number.is_compressed = camera.get_is_compressed();
    image_and_number.image_number = last_sended_frame_number;
    return camera.get_frame(
            image_and_number.img_vec,
            image_and_number.col,
            image_and_number.row,
            image_and_number.type
            );

}

bool Image_Sender::need_update() {
    if(mode == Pernament_Connector::p_connector_mode::establish_connection)
        return Time_Object::need_update();
    else{

        return need_update_recive() or need_update_send();
    }
}

void Image_Sender::update_establish_connection_mode() {
    Pernament_Connector::update(); // funkcja ta aktualizuje zegar
    if(mode == Pernament_Connector::p_connector_mode::pernament_communication){
        update_period_microseconds = 50000;
        update_image_and_number();
    }
}

void Image_Sender::update_pernament_communication_mode() {
    if(need_update_send()){
        try_send_new_image();
        last_update_time_send = clock.getElapsedTime().asMicroseconds();
    }

    if(need_update_recive()){
        sf::Packet packet;

        // test
        setBlocking(false);
        auto status = receive(packet);
        setBlocking(true);

        if(status == sf::Socket::Status::Done) {
            packet >> last_recived_frame_number;

            sf::Int8 recived_set_mode;
            packet >> recived_set_mode;
            auto new_mode = static_cast<Sender_Mode>(recived_set_mode);

            if(new_mode != sender_mode){
                sender_mode = static_cast<Sender_Mode>(recived_set_mode);
                change_sender_mode(new_mode);

                compress_time = camera.measure_operation_time_compress();
            }
        }


        last_update_time_recive = clock.getElapsedTime().asMicroseconds();
    }
}

bool Image_Sender::need_update_recive() {
    return (clock.getElapsedTime().asMicroseconds() - last_update_time_recive) > update_period_microseconds_recive;
}

bool Image_Sender::need_update_send() {
    return clock.getElapsedTime().asMicroseconds() - last_update_time_send   > update_period_microseconds_send;
}

bool Image_Sender::try_send_new_image() {
    if((last_sended_frame_number - last_recived_frame_number) < max_frame_delay) {
        sf::Packet packet;
        packet << image_and_number;

        auto send_status = send(packet);


        if (send_status == sf::Socket::Done) {
            // Prawidłowo wysłano zdjęcie
            last_sended_frame_number++;
            manage_sender_mode();
        }
        auto camera_read_status = update_image_and_number();

        if (not camera_read_status)
            std::cout << "Nieprawidlowe odczytanie obrazu z kamery!\n";

        return send_status == sf::Socket::Done and camera_read_status;
    }
    return false;
}

void Image_Sender::change_sender_mode(Image_Sender::Sender_Mode mode) {
    if(mode == Sender_Mode::scale_x1){
        auto itr_pos = std::find(camera.resize_factors.begin(), camera.resize_factors.end(), 1.0);
        if(itr_pos == camera.resize_factors.end()){
            throw std::exception();
        }

        camera.set_resize_itr(static_cast<int>( std::distance(camera.resize_factors.begin(), itr_pos)));
    } else if(mode == Sender_Mode::scale_x0_5){
        auto itr_pos = std::find(camera.resize_factors.begin(), camera.resize_factors.end(), 0.5);
        if(itr_pos == camera.resize_factors.end()){
            throw std::exception();
        }

        camera.set_resize_itr(static_cast<int>( std::distance(camera.resize_factors.begin(), itr_pos)));
    }

    last_average_time_compressed = -1;
    last_average_time_not_compressed = -1;



    sender_mode = mode;
}

bool Image_Sender::is_complex_image_management_method_used() {
    return sender_mode == Sender_Mode::const_20_fps;
}

void Image_Sender::manage_sender_mode() {
    // aktualizacja czasu
    last_img_send_time.push_back(clock.getElapsedTime().asMicroseconds());

    if(last_img_send_time.size() > number_of_remembered_samples){

        sf::Int64 last_sample;
        sf::Int64 time_sum = 0;
        for(auto& time_sample : last_img_send_time){
            if(time_sample == *last_img_send_time.begin()){
                last_sample = time_sample;
                continue;
            }
            time_sum += time_sample - last_sample;
            last_sample = time_sample;
        }
        sf::Int64 average_cycle_time = time_sum / number_of_remembered_samples;

        last_img_send_time.pop_front();

        if(is_complex_image_management_method_used()){
            manage_time_complex(average_cycle_time);
        }else{
            manage_time_simple(average_cycle_time);
        }
    }
}

bool Image_Sender::manage_time_simple(sf::Int64 average_cycle_time) {
    bool is_working_method_changed = false;

    if(camera.get_is_compressed()){
        auto compress_relative_value = static_cast<float>(compress_time)  / static_cast<float>(average_cycle_time);

        if( compress_relative_value > max_compress_relative_to_whole_time and
                (last_average_time_not_compressed == -1 or static_cast<float>(last_average_time_not_compressed) * change_threshold_compress < static_cast<float>(average_cycle_time))
                ){
            camera.set_is_compresed(false);
            last_img_send_time = {};

            std::cout<<"Zmieniono tryb pracy na nie skompresowany"<<std::endl;
            is_working_method_changed = true;
        }
        last_average_time_compressed = average_cycle_time;
    }else{
        auto not_compress_relative_value = static_cast<float>(compress_time)  / static_cast<float>(average_cycle_time);

        if( not_compress_relative_value < min_compress_relative_to_whole_time and
            (last_average_time_compressed == -1 or static_cast<float>(last_average_time_compressed) * change_threshold_compress < static_cast<float>(average_cycle_time))
                ){
            camera.set_is_compresed(true);
            last_img_send_time = {};

            std::cout<<"Zmieniono tryb pracy na skompresowany"<<std::endl;
            is_working_method_changed = true;
        }
        last_average_time_not_compressed = average_cycle_time;
    }

    return is_working_method_changed;
}

bool Image_Sender::manage_time_complex(sf::Int64 average_cycle_time) {
//    metoda ta zmienia również tryb pracy pomiędzy skompresowanym i nie skompesowanym,
//    z tego powodu wykorzystana zostanie funkcja manage_time_simple
//    ponieważ nie można jednocześnie wprowadzać dwóch zmian, jeżeli funkcja zwróci true, nie są wprowadzane
//    dalesz zmiany
    bool is_simple_manage_used = manage_time_simple(average_cycle_time);

    if(is_simple_manage_used){
        return true;
    }

    if( average_cycle_time < quality_improvment_threshold){
        camera.increase_qulity();
        std::cout<<"Polepszono rozdzielczość"<<std::endl;

        compress_time = camera.measure_operation_time_compress();

        return true;
    }else if(average_cycle_time > quality_deterioration_threshold){
        camera.decrease_qulity();
        std::cout<<"Pogorszono rozdzielczość"<<std::endl;

        compress_time = camera.measure_operation_time_compress();

        return true;
    }

    return false;
}
