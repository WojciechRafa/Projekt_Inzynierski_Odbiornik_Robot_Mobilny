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
        //std::cout << "Utrata kontaktu \n";
        power_left = 0;
        power_right = 0;
    }

    // aktualizacja mocy
    int mode_left = get_variable_int("Tryb_mocy_lewy_silnik");

    switch (mode_left) {
        case -1:
            //std::cout<<"A"<<std::endl;
            power_left -= 10;
            if(power_left < -100)
                power_left = -100;
            break;

        case 0:
            //std::cout<<"B"<<std::endl;
            power_left = 0;
            break;

        case 1:
            //std::cout<<"C"<<std::endl;
            power_left += 10;
            if(power_left > 100)
                power_left = 100;
            break;
    }

    int mode_right = get_variable_int("Tryb_mocy_prawy_silnik");

    switch (mode_right) {
        case -1:
            power_right -= 10;
            if(power_right < -100)
                power_right = -100;
            break;

        case 0:
            power_right = 0;
            break;

        case 1:
            power_right += 10;
            if(power_right > 100)
                power_right = 100;
            break;
    }

    softPwmWrite(PWM_L, power_left);
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

}

int GPIO_Control::get_variable_int(std::string name)
{
    sf::Int32 variable;
    if(not custom_data_IO.get_variable_by_name_int(name, variable)) {
        std::cout << "Nie znaleziono "<<name <<" \n";
        throw std::exception();
    }
    return variable;
}
