#pragma once

#include <opencv2/opencv.hpp>
#include <SFML/System.hpp>



class Camera: private cv::VideoCapture{
public:
    Camera();

    sf::Vector2<int> get_orginal_size(); // Uwaga - w scelu sprawdzenia wielkości wykonywane jest zdjęcie, z tego powodu może to zajmować wzgldnie dużo czasu

    bool get_resizes_frame(cv::Mat& img);
    bool get_frame(std::vector<uchar>& img_raw_data, sf::Int32& col, sf::Int32& row, sf::Int32& type);

    bool increase_qulity();
    bool decrease_qulity();
    bool set_resize_itr(int itr);
    float get_resize_factor();

    [[nodiscard]] bool get_is_compressed() const;
    void set_is_compresed(bool is_compressed_);

    sf::Int64 measure_operation_time_compress(); // Jeżeli funkcja zwróci "-1" BŁĄÐ

    std::vector<float> resize_factors = {1, 0.7, 0.5, 0.3, 0.25, 0.2, 0.15, 0.1};
private:
    bool get_frame_compress_choice(bool compress, std::vector<uchar>& img_raw_data, sf::Int32& col, sf::Int32& row, sf::Int32& type);

    int resize_itr = 0;

    sf::Clock clock;

    std::vector<int> compression_params;

    bool is_compressed = true;
};