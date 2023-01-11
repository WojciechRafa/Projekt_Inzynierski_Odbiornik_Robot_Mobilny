//
// Created by rodzi on 08.01.2023.
//

#ifndef INZYNIERKA_GPIO_CONTROL_HPP
#define INZYNIERKA_GPIO_CONTROL_HPP

#include "Time_Object.hpp"
#include "Custom_Data_IO.hpp"

class GPIO_Control: public Time_Object{
public:
    explicit GPIO_Control(Custom_Data_IO& custom_data_IO_);
    void update() override;
private:
    Custom_Data_IO& custom_data_IO;

    int PWM_L =  23;
    int L1 = 24;
    int L2 = 25;

    int PWM_R = 27;
    int R1 =  28;
    int R2 = 29;

    int power_left = 0;
    int power_right = 0;

    int get_variable_int(const std::string& name);

    static void update_power(int& actual_power, sf::Int32 commend);
};


#endif //INZYNIERKA_GPIO_CONTROL_HPP
