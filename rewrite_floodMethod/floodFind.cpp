#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

static const unsigned char UNVISITED = 0;
static const unsigned char VISITED = 1;
static const unsigned char MARKED = 2;
static const unsigned char BLACK = 3;
static const unsigned char TEMP = 4;

static const int array_size = 10000;
static const unsigned char thresh = 120;
static const int skip = 30;

struct fill_queue{
    cv::Point2i queue[array_size];
    int head;
    int tail;
};

struct fill_data{
    int white_pixels;
    //made it non static, beware memory leak?
    std::vector<cv::Point2i> black_list;
    //might need to make min
    cv::Point max_x, max_y, min_x, min_y, dot_tr, dot_br, dot_tl, dot_bl;
};

cv::Mat input;
cv::Mat table;

fill_data max_white;
fill_data temp_fill;

cv::Point2i flood_fill(cv::Mat image, unsigned char lower, unsigned char upper, cv::Point2i seed, int skip, bool white_flag){

    unsigned char value, state;
    cv::Point2i sum = cv::Point2i(0,0);
    int count = 0;
    fill_queue circ_queue;
    circ_queue.head = 0;
    circ_queue.tail = 0;
    
    circ_queue.queue[circ_queue.tail] = seed;
    circ_queue.tail = (circ_queue.tail + 1) % array_size;

    while(circ_queue.head != circ_queue.tail){
        //std::cout << "in queue while" << std::endl;
        seed = circ_queue.queue[circ_queue.head];
        circ_queue.head = (circ_queue.head + 1) % array_size;

        value = image.at<uchar>(seed.x, seed.y-1);
        while(value >= lower && value < upper){
            //std::cout << "in backtrack loop" << std::endl;
            seed.y--;
            value = image.at<uchar>(seed.x, seed.y-1);
        }

        //might flip out if white near edges, UPDATE: Now start scanning from skip.
        value = image.at<uchar>(seed.x-skip, seed.y);
        state = table.at<uchar>(seed.x-skip, seed.y);
        if(value >= lower && value < upper
           && (state == UNVISITED || state == TEMP)){
            circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x-skip, seed.y);
            circ_queue.tail = (circ_queue.tail + 1) % array_size;
            table.at<uchar>(seed.x-skip, seed.y) = MARKED;
        }

        value = image.at<uchar>(seed.x+skip, seed.y);
        state = table.at<uchar>(seed.x+skip, seed.y);
        if(value >= lower && value < upper &&
           (state == UNVISITED ||  state == TEMP)){
            circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x+skip, seed.y);
            circ_queue.tail = (circ_queue.tail + 1) % array_size;
            table.at<uchar>(seed.x+skip, seed.y) = MARKED;
        }

        bool mark_flag_up = false;
        bool mark_flag_down = false;
        bool run_start = true;
        do{
            //std::cout << "in do" << std::endl;
            table.at<uchar>(seed.x, seed.y) = VISITED;
            
            
            if(!white_flag){
                sum.x += seed.x;
                sum.y += seed.y;
                count++;
            }

            else{
                temp_fill.white_pixels++;
                input.at<cv::Vec3b>(seed.x,seed.y) = cv::Vec3b(0,0,255);
                if(run_start){
                    if(seed.x < temp_fill.min_x.x){
                        temp_fill.min_x.x = seed.x;
                        temp_fill.min_x.y = seed.y;
                    }
                    if(seed.y < temp_fill.min_y.y){
                        temp_fill.min_y.y = seed.y;
                        temp_fill.min_y.x = seed.x;
                    }
                    run_start = false;
                }

                value = image.at<uchar>(seed.x, seed.y+skip);
                if(!(value >= lower && value < upper)){
                    if(seed.x > temp_fill.max_x.x){
                        temp_fill.max_x.x = seed.x;
                        temp_fill.max_x.y = seed.y;
                    }
                    if(seed.y > temp_fill.max_y.y){
                        temp_fill.max_y.y = seed.y;
                        temp_fill.max_y.x = seed.x;
                    }
                    //might not be necessary
                    table.at<uchar>(seed.x, seed.y+skip) = BLACK;
                    temp_fill.black_list.push_back(cv::Point2i(seed.x, seed.y+skip));
                }
            }
                

            value = image.at<uchar>(seed.x-skip, seed.y);
            state = table.at<uchar>(seed.x-skip, seed.y);
            if(mark_flag_up && value >= lower && value < upper
               && (state == UNVISITED || state == TEMP )){
                
                circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x-skip, seed.y);
                circ_queue.tail = (circ_queue.tail + 1) % array_size;
                table.at<uchar>(seed.x-skip, seed.y) = MARKED;
                mark_flag_up = false;
            }
            
            else if(!(value >= lower && value < upper)){
                mark_flag_up = true;
            }

            value = image.at<uchar>(seed.x+skip, seed.y);
            state = table.at<uchar>(seed.x+skip, seed.y);
            if(mark_flag_down && value >= lower && value < upper
               && (state == UNVISITED || state == TEMP)){

                circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x+skip, seed.y);
                circ_queue.tail = (circ_queue.tail + 1) % array_size;
                table.at<uchar>(seed.x+skip, seed.y) = MARKED;
                mark_flag_down = false;
            }

            else if(!(value >= lower && value < upper)){
                mark_flag_down = true;
            }
            std::cout << seed.x << " " << seed.y << "\n"
                      << temp_fill.black_list.size() << "\n" <<
                circ_queue.head << " " << circ_queue.tail << std::endl;
            seed.y += skip;
        }while(image.at<uchar>(seed.x, seed.y) >= lower && image.at<uchar>(seed.x, seed.y) < upper);
    }
    if(!white_flag){
        sum.x /= count;
        sum.y /= count;
        return sum;
    }

    return cv::Point2i(-1,-1);
}

int main(int argc, char **argv){

    if(argc != 2){
        std::cout << "Usage: ./execName ImagetoLoad" << std::endl;
        return -1;
    }

    cv::Mat image;
    input = cv::imread(argv[1]);
    cv::cvtColor(input, image, CV_BGR2GRAY);
    table = cv::Mat::zeros(image.rows,image.cols, CV_8UC1);
    //cv::imwrite("output.png", image);
    max_white.white_pixels = 0;
    max_white.max_x = cv::Point2i(0,0);
    max_white.max_y = cv::Point2i(0,0);
    max_white.min_x = cv::Point2i(4000,4000);
    max_white.min_y = cv::Point2i(4000,4000);
    
    temp_fill.white_pixels = 0;
    temp_fill.max_x = cv::Point2i(0,0);
    temp_fill.max_y = cv::Point2i(0,0);
    temp_fill.min_x = cv::Point2i(4000,4000);
    temp_fill.min_y = cv::Point2i(4000,4000);
    int count = 0;
    for(int i = skip; i < image.rows-skip; i+=skip){
        for(int j = skip; j < image.cols-skip; j+=skip){
            if(image.at<uchar>(i,j) >= thresh && table.at<uchar>(i,j) == UNVISITED){
                //std::cout << "i = " << i << " j = " << j << std::endl;
                //issue with 255 value, less than in algorithm
                 std::cout << (int)image.at<uchar>(i,j) << std::endl;
                //std::cout << ++count << std::endl;
                flood_fill(image, 140, 255, cv::Point2i(i,j), skip, true); 
                if(temp_fill.white_pixels > max_white.white_pixels){
                    max_white = temp_fill;
                }
                temp_fill.white_pixels = 0;
                temp_fill.black_list.clear();
                temp_fill.max_x = cv::Point2i(0,0);
                temp_fill.max_y = cv::Point2i(0,0);
                temp_fill.min_x = cv::Point2i(4000,4000);
                temp_fill.min_y = cv::Point2i(4000,4000);
            }
        }
    }

    //calculate dot products, no point in doing it more than once. 

    //this might not be neccesary anymore
    for(int i = 0; i < table.rows; i++){
        for(int j = 0; j < table.cols; j++){
            unsigned char state = table.at<uchar>(i,j);
            if(state == VISITED){
                table.at<uchar>(i,j) = BLACK;
            } else if(state == BLACK){
                table.at<uchar>(i,j) = TEMP;
            }
        }
    }

    // std::cout << max_white.white_pixels << std::endl;

    cv::Point centre((max_white.max_y.y + max_white.min_y.y) / 2, (max_white.max_x.x + max_white.min_x.x) /2);
    circle(input,centre,3,cv::Scalar(255,0,0), -1,8,0);

    cv::Point max_x(max_white.max_x.y, max_white.max_x.x);
    cv::Point max_y(max_white.max_y.y, max_white.max_y.x);
    cv::Point min_x(max_white.min_x.y, max_white.min_x.x);
    cv::Point min_y(max_white.min_y.y, max_white.min_y.x);
    circle(input, max_x, 8, cv::Scalar(0,255,0), -1, 8, 0);
    circle(input, max_y, 8, cv::Scalar(0,0,255), -1, 8, 0);
    circle(input, min_x, 8, cv::Scalar(255,255,0), -1, 8, 0);
    circle(input, min_y, 8, cv::Scalar(255,0,0), -1, 8, 0);

    std::cout << max_white.max_x << " " << max_white.max_y << "\n" <<
        max_white.min_x << " " << max_white.min_y << std::endl;
    cv::resize(input, input, cv::Size(input.cols/4, input.rows/4));
    cv::imshow("Output", input);
    cv::waitKey(0);
    return 0;
}
