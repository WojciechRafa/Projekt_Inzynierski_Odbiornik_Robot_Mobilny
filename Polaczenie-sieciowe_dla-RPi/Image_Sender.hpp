#pragma once

#include <SFML/Network.hpp>
#include "Pernament_Connector.hpp"
#include "Camera.hpp"
#include "Sended_struct.hpp"




class Image_Sender: public Pernament_Connector{
public:
    enum class Sender_Mode{
        const_20_fps,
        scale_x1,
        scale_x0_5
    };

	Image_Sender(unsigned short port_, sf::IpAddress remote_dev_ip_);

    void update() override;

    bool need_update() override; // w trybie "pernament_communication" sposób dziłania funkcji jest niestandardowy ponieważ update który sprawdza odebranie jest częstszy niż ten który sprawdza nadawanie
private:
    bool try_send_new_image();// wysyła kolejną klatę i robi zdjęcie kolejnej, jeżeli nie był wstanie tego zrobić zwraca false
    bool update_image_and_number();

    void update_establish_connection_mode();
    void update_pernament_communication_mode();


    const int max_frame_delay = 20; // frame delay - różnica między najwyższym numerem wysłanego zdjęcia i odebranym numerem zwrotnym
    sf::Int32 last_recived_frame_number = 0;
    sf::Int32 last_sended_frame_number = 0;

    Camera camera;
    Image_and_number image_and_number;

    bool need_update_recive();
    bool need_update_send();

    sf::Int64 update_period_microseconds_send   = 40000;
    sf::Int64 update_period_microseconds_recive = 20000;
    sf::Int64 last_update_time_send = 0;
    sf::Int64 last_update_time_recive = 0;

    Sender_Mode sender_mode = Sender_Mode::scale_x1;

    // obsługa czasu
    const int number_of_remembered_samples = 20;// arbitralnie przyjęta wartość

    const float max_compress_relative_to_whole_time = 0.6; // przy włączonej kompresji-jeżeli udział czasu kompresji w całości cyklu przesyłania przekroczy ten próg, kompresja jest wywyłączana
    const float min_compress_relative_to_whole_time = 0.4; // przy wyłączonej kompresji-jeżeli czas potrzebny na kompresje w porównaniu do całości cyklu przesyłania przekroczy ten próg, kompresja jest włączana
    const float change_threshold_compress = 1.5; // celem progu jest ochrona przed zbyt częstym zmienianiem trybu pracy, aby doszło do przełączenia, inny sposób pracy powinnien mieć szanse być tylukrotnie lepszy

    // Ponieważ zmiana skali plików wiąże się ze zmianą jakości, przyjęto dla niej inne typy
    // progów, czas, który jest na tyle nieski, że wiąże się z podwyższeniem jakości, oraz
    // na tyle długi, że wywołuje jej pogorszenie
    const sf::Int64 quality_improvment_threshold = 50000;
    const sf::Int64 quality_deterioration_threshold = quality_improvment_threshold * 2;

    // dla trybu prostego
    sf::Int64 last_average_time_compressed = -1; // -1 -> wartość domyślna
    sf::Int64 last_average_time_not_compressed = -1;

//    //  dla tryby zmieniającego skale obraz
//    sf::Int64 last_average_time_higher_quality = -1; // -1 -> wartość domyślna
//    sf::Int64 last_average_time_lower_quality = -1;

    void change_sender_mode(Sender_Mode mode);
    bool is_complex_image_management_method_used(); // metoda zaawansowana to taka przy której wyznaczona jest docelowa prędkość i możliwym jest płynne zmienianie skali
    void manage_sender_mode();

    // funkcje zwracają true jeżeli dokonano jakiś zmian
    bool manage_time_simple(sf::Int64 average_cycle_time);
    bool manage_time_complex(sf::Int64 average_cycle_time);

    std::list<sf::Int64> last_img_send_time = {};
    sf::Int64 compress_time;

};
