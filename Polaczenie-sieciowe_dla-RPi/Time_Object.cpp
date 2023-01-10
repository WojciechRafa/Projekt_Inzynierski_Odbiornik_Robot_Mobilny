#include "Time_Object.hpp"

Time_Object::Time_Object() {
    clock.restart();
    last_update_time = clock.getElapsedTime().asMicroseconds();
}

Time_Object::Time_Object(sf::Int64 update_period_microseconds_):
        update_period_microseconds(update_period_microseconds_)
{
    clock.restart();
    last_update_time = clock.getElapsedTime().asMicroseconds();

}

std::list<Time_Object &>* Time_Object::get_all_contaneted_time_object() {
    return nullptr;
}

bool Time_Object::need_update() {
    return clock.getElapsedTime().asMicroseconds() - last_update_time > update_period_microseconds;
}