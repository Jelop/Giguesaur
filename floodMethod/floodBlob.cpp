#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#define UNVISITED 0
#define VISITED 1
#define MARKED 2
#define BLACK 3
#define TEMP 4

struct fill_queue{
    cv::Point2i queue[10000];
    int head;
    int tail;
};

cv::Mat input;
cv::Mat table;

/**
 * Could use an auxiliary function for the conditionals. Pass a < or > and a thresh. Makes it more
 * general. Then could calculate the running sum and average for every flood fill.
 *
 * Perhaps use another function between filling the white and filling the blobs. This other function
 * would invert the table(which would need to be made global), so that all visted cells would be
 * made black, and all previously black cells would be considered marked? Then scan through the
 * persistent table and find the first marked value to use as a seed. Then once that blob has been
 * filled average its values to get its centre and all it's pixels will be considered visited.
 * DON'T FORGET! That a run limit on black pixels is necessary so that the table won't be considered
 * a blob. 
 */

cv::Point2i floodFill(cv::Mat image, unsigned char lower, unsigned char upper, cv::Point2i seed, int black_flag){

    cv::Point2i sum = cv::Point2i(0,0);
    int count = 0;
    
    fill_queue circ_queue;
    circ_queue.head = 0;
    circ_queue.tail = 0;
    
    circ_queue.queue[circ_queue.tail] = seed;
    circ_queue.tail = (circ_queue.tail + 1) % 10000;

    while(circ_queue.head != circ_queue.tail){
        
        seed = circ_queue.queue[circ_queue.head];
        circ_queue.head = (circ_queue.head + 1) % 10000;

        while(image.at<uchar>(seed.x, seed.y-1) >= lower &&
              image.at<uchar>(seed.x, seed.y-1) < upper){
            seed.y--;
        }

        unsigned char value = image.at<uchar>(seed.x-1, seed.y);
        unsigned char state = table.at<uchar>(seed.x-1, seed.y);
        if(value >= lower && value < upper
           && (state == UNVISITED || state == TEMP)){
            circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x-1, seed.y);
            circ_queue.tail = (circ_queue.tail + 1) % 10000;
            table.at<uchar>(seed.x-1, seed.y) = MARKED;
        }

        value = image.at<uchar>(seed.x+1, seed.y);
        state = table.at<uchar>(seed.x+1, seed.y);
        if(value >= lower && value < upper &&
           (state == UNVISITED ||  state == TEMP)){
            circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x+1, seed.y);
            circ_queue.tail = (circ_queue.tail + 1) %10000;
            table.at<uchar>(seed.x+1, seed.y) = MARKED;
        }


        int run_length = 0;
        do{
            //std::cout << count << std::endl;
            if(black_flag == 0){
                // input.at<cv::Vec3b>(seed.x, seed.y) = cv::Vec3b(255,0,0);
            } else {
                //input.at<cv::Vec3b>(seed.x, seed.y) = cv::Vec3b(0,255,0);
            }
            table.at<uchar>(seed.x, seed.y) = VISITED;
            sum.x += seed.x;
            sum.y += seed.y;
            count++;
            run_length++;

            if(run_length > 150 && black_flag > 0)
                return cv::Point2i(-1,-1);

            value = image.at<uchar>(seed.x-1, seed.y);
            if(!(value >= lower && value < upper)){
                
                table.at<uchar>(seed.x-1, seed.y) = BLACK;

                value = image.at<uchar>(seed.x-1, seed.y+1);
                state = table.at<uchar>(seed.x-1, seed.y+1);
                if(value >= lower && value < upper &&
                   (state == UNVISITED || state == TEMP)){

                    circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x-1, seed.y+1);
                    circ_queue.tail = (circ_queue.tail + 1) % 10000;
                    table.at<uchar>(seed.x-1, seed.y+1) = MARKED;
                }
            }

            value = image.at<uchar>(seed.x+1, seed.y);
            if(!(value >= lower && value < upper)){
                
                table.at<uchar>(seed.x+1, seed.y) = BLACK;

                value = image.at<uchar>(seed.x+1, seed.y+1);
                state = table.at<uchar>(seed.x+1, seed.y+1);
                if(value >= lower && value < upper &&
                   (state == UNVISITED || state  == TEMP)){

                    circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x+1, seed.y+1);
                    circ_queue.tail = (circ_queue.tail + 1) % 10000;
                    table.at<uchar>(seed.x+1, seed.y+1) = MARKED;
                }
            }
         
            seed.y++;
            // std::cout << seed.x << " " << seed.y << " " <<  (int)image.at<uchar>(seed.x,seed.y) <<std::endl;
           
        }while(image.at<uchar>(seed.x,seed.y) >= lower && image.at<uchar>(seed.x,seed.y) < upper);
            
        
    }
    // std::cout << sum << std::endl;

    if(count < 7000){
        return cv::Point2i(-1,-1);
    }
    sum.x /= count;
    sum.y /= count;
    // std::cout << sum << std::endl;
    return sum;
    
    
}

int main(int argc, char **argv){

    if(argc != 2){
        std::cout << "Usage: ./execName ImagetoLoad" << std::endl;
        return -1;
    }

 
    cv::Mat image;
    input = cv::imread(argv[1]);
    cv::cvtColor(input,image, CV_BGR2GRAY);
    table = cv::Mat::zeros(image.rows,image.cols, CV_8UC1);

    int row = 1109;
    int col = 1630;
    //casting suppressed warning about wrap around but probably didn't change behaviour
    std::cout << floodFill(image, 140, (unsigned char)255, cv::Point2i(row,col), 0) << std::endl;

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


    std::vector<cv::Point2i> blobs;
    //Flood fill the blacks now!
    for(int i = 0; i < table.rows; i++){
        for(int j = 0; j < table.cols; j++){
            unsigned char state = table.at<uchar>(i,j);
            if(state == TEMP){
                cv::Point2i temp = floodFill(image,0, 140, cv::Point2i(i,j), 1);
                if(temp.x > -1)
                    blobs.push_back(temp);
            }
        }
    }
     std::cout << blobs << std::endl;
    // threshold(image, image, 140, 255, cv::THRESH_BINARY);
    // cv::namedWindow("Output", cv::WINDOW_NORMAL);

     for(int i = 0; i < blobs.size(); i++){
         cv::Point centre(blobs[i].y, blobs[i].x);
         circle(input, centre, 3, cv::Scalar(255,255,255), -1, 8, 0);
     }
    cv::resize(input, input, cv::Size(input.cols/4, input.rows/4));
    cv::imshow("Output", input);
    // cv::imwrite("output.png", image);
    cv::waitKey(0);
    return 0;
}
