cmake_minimum_required(VERSION 3.16)
project(Inzynierka)

set(OpenCV_DIR   "/home/wr/Biblioteki/OpenCV/opencv-4.x/cmake")

set(CMAKE_CXX_STANDARD 20)


find_package(OpenCV REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS})

add_executable(Inzynierka main.cpp
	main_functions.hpp
	Sended_struct.hpp Sended_struct.cpp
	Broadcast_connector.hpp Broadcast_connector.cpp
	Pernament_Connector.hpp Pernament_Connector.cpp
	Image_Sender.hpp Image_Sender.cpp
	Time.hpp
	Camera.hpp Camera.cpp
	System.cpp System.hpp
	Time_Object.cpp Time_Object.hpp
	Custom_Data_IO.cpp Custom_Data_IO.hpp 
	GPIO_Control.cpp GPIO_Control.hpp)
        
target_link_libraries(Inzynierka sfml-network sfml-system sfml-window ${OpenCV_LIBS}
		wiringPi
		)
#target_compile_options(Inzynierka PRIVATE -Wall)
