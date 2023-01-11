//
// Created by rodzi on 08.01.2023.
//

#include "GPIO_Control.hpp"
#include <wiringPi.h>
#include <softPwm.h>

GPIO_Control::GPIO_Control(Custom_Data_IO& custom_data_IO_):
custom_data_IO(custom_data_IO_)
{
    update_period_microseconds = 100000;

    wiringPiSetup();
    pinMode(PWM_L,OUTPUT); /// PWM-L
    pinMode(L1,OUTPUT); /// L1
    pinMode(L2,OUTPUT); /// L2

    pinMode(PWM_R,OUTPUT); /// PWM-R
    pinMode(R1,OUTPUT); /// R1
    pinMode(R2,OUTPUT); /// R2

    softPwmCreate(PWM_L,0,100);
    softPwmCreate(PWM_R,0,100);
//
    digitalWrite(L1,HIGH);
    digitalWrite(L2,LOW );
    digitalWrite(R1,HIGH);
    digitalWrite(R2,LOW );
}

void GPIO_Control::update() {
    // zabezpieczenie przed barkiem połączenia

    static int cycle_without_iterator_update;
    static int iterator;

    int new_iterator = get_variable_int("Iterator");


    if(new_iterator != iterator){
        iterator = new_iterator;
        cycle_without_iterator_update = 0;
    } else{

        cycle_without_iterator_update ++;
    }

    if(cycle_without_iterator_update > 10){
        std::cout << "Utrata kontaktu, cykle bez update - "<< cycle_without_iterator_update <<"\n";
        power_left = 0;
        power_right = 0;
    }

    // aktualizacja mocy
    int mode_left = get_variable_int("Tryb_mocy_lewy_silnik");
    update_power(power_left, mode_left);


    int mode_right = get_variable_int("Tryb_mocy_prawy_silnik");
    update_power(power_right, mode_right);


    std::cout<<" moc   lewy - "<<power_left<<" prawy "<<power_right<<std::endl;

    if(power_left < 0){
        std::cout<<"Tyl L \n";
        digitalWrite(L1, LOW);
        digitalWrite(L1, HIGH);
    }else if(power_left == 0){
        digitalWrite(L1, LOW);
        digitalWrite(L1, LOW);
    }else{
        std::cout<<"Przod L \n";
        digitalWrite(L1, HIGH);
        digitalWrite(L1, LOW);
    }
    softPwmWrite(PWM_R, power_right);
    
    if(power_right < 0){
        std::cout<<"Tyl R \n";
        digitalWrite(R1, LOW);
        digitalWrite(R1, HIGH);
    }else if(power_right == 0){
        digitalWrite(R1, LOW);
        digitalWrite(R1, LOW);
    }else{
        std::cout<<"Przod R \n";
        digitalWrite(R1, HIGH);
        digitalWrite(R1, LOW);
    }

    // wysyłanie
    custom_data_IO.update_variable_by_name_int("Moc_lewy_silnik", power_left);
    custom_data_IO.update_variable_by_name_int("Moc_prawy_silnik", power_right);

    last_update_time = clock.getElapsedTime().asMicroseconds();
}

int GPIO_Control::get_variable_int(const std::string& name)
{
    sf::Int32 variable;
    if(not custom_data_IO.get_variable_by_name_int(name, variable)) {
        std::cout << "Nie znaleziono "<<name <<" \n";
        throw std::exception();
    }
    return variable;
}

void GPIO_Control::update_power(int &power, sf::Int32 commend) {
    switch (commend) {
        case -1:
            if(power > 0) {
                power = 0;
                break;
            }
                power -= 10;
            if(power < -100)
                power = -100;
            break;

        case 0:
            if(power > 0) {
                if(power < 10){ // w teorii nie powinno dochodzić do takich sytuacji
                    power = 0;
                } else {
                    power -= 10;
                }
            } else if(power < 0) {
                if(power < -10){ // w teorii nie powinno dochodzić do takich sytuacji
                    power = 0;
                } else {
                    power += 10;
                }
            } else{
                power = 0;
            }
            break;

        case 1:
            if(power < 0) {
                power = 0;
                break;
            }


            power += 10;
            if(power > 100)
                power = 100;
            break;

        default:
            std::cout<<"Błąd przy konfiguracji mocy"<<std::endl;
            throw std::exception();
    }
}
