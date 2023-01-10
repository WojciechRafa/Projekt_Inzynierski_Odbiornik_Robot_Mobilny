
#include "Camera.hpp"

Camera::Camera():
    cv::VideoCapture(0, 0),
    compression_params({cv::IMWRITE_JPEG_QUALITY, 95})
{
    if(not isOpened()){
        std::cout<<"Kamera nie została otwarta";
    }
}

sf::Vector2<int> Camera::get_orginal_size() {
    cv::Mat img;
    read(img);

    auto img_size = img.size();
    return {img_size.width, img_size.height};
}

bool Camera::get_resizes_frame(cv::Mat &img) {
    cv::Mat local_img;
    bool is_frame_grabbed = read(local_img);

    if(not is_frame_grabbed){
        return false;
    }

    cv::resize(local_img, img, img.size(), get_resize_factor(), get_resize_factor(), cv::INTER_LINEAR);


    return true;
}

bool Camera::get_frame_compress_choice(bool compress,  std::vector<uchar>& img_raw_data, sf::Int32& col, sf::Int32& row, sf::Int32& type) {
    cv::Mat img;
    bool is_frame_grabbed = get_resizes_frame(img);

    if(not is_frame_grabbed){
        return false;
    }

    if(compress) {
        cv::imencode(".jpeg", img, img_raw_data, compression_params);
    }else{
        size_t totalElements = img.total()*img.channels(); // Note: image.total() == rows*cols.
        cv::Mat flat_img = img.reshape(1, static_cast<int>(totalElements)); // 1xN mat of 1 channel, O(1) operation
        if(!img.isContinuous()) {
            std::cout<<"Nie Continous !";
            flat_img = flat_img.clone(); // O(N),
        }
        img_raw_data = std::vector<uchar>(flat_img.data, flat_img.data + flat_img.total());
    }

    col = img.cols;
    row = img.rows;
    type = img.type();

    return is_frame_grabbed;
}

float Camera::get_resize_factor() {
    return resize_factors[resize_itr];
}

bool Camera::increase_qulity() {
    if (resize_itr <= 0)
        return false;
    resize_itr --;
    return true;
}

bool Camera::decrease_qulity() {
    if (resize_itr >= resize_factors.size() - 1)
        return false;
    resize_itr ++;
    return true;
}

bool Camera::set_resize_itr(int itr) {
    if(itr < 0 or itr >= resize_factors.size()){
        return false;
    }
    resize_itr = itr;

    return true;
}

sf::Int64 Camera::measure_operation_time_compress() {
    // nowe

    cv::Mat img;
    bool is_frame_grabbed = get_resizes_frame(img);

    if(not is_frame_grabbed){
        return -1;
    }

    std::vector<uchar> img_raw_data;

    clock.restart();

    cv::imencode(".jpeg", img, img_raw_data, compression_params);
    return clock.getElapsedTime().asMicroseconds();
}

bool Camera::get_frame(std::vector<uchar> &img_raw_data, sf::Int32 &col, sf::Int32 &row, sf::Int32 &type) {
    return get_frame_compress_choice(is_compressed, img_raw_data, col, row, type);
}

bool Camera::get_is_compressed() const {
    return is_compressed;
}

void Camera::set_is_compresed(bool is_compressed_) {
    is_compressed = is_compressed_;
}



