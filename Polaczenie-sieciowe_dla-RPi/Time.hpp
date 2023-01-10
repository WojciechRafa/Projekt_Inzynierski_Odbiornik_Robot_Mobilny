#pragma once

#include <SFML/System.hpp>


inline void Delay_ms(int delay){
	static sf::Clock clock_delay;
	while(true){
		sf::Time time = clock_delay.getElapsedTime();
        if(time.asMilliseconds() > delay){
			clock_delay.restart();
			return;
		}
	}
	
}

