#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#define UNVISITED 0
#define VISITED 1
#define MARKED 2
#define BLACK 3

struct fill_queue{
    cv::Point2i queue[10000];
    int head;
    int tail;
};

cv::Mat input;


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

void floodFill(cv::Mat image, unsigned char thresh){
    fill_queue circ_queue;
    circ_queue.head = 0;
    circ_queue.tail = 0;

    cv::Mat table = cv::Mat::zeros(image.rows,image.cols, CV_8UC1);
    int row = 1109;
    int col = 1630;
    circ_queue.queue[circ_queue.tail] = cv::Point2i(row,col);
    circ_queue.tail = (circ_queue.tail + 1) % 10000;

    while(circ_queue.head != circ_queue.tail){

        cv::Point2i seed = circ_queue.queue[circ_queue.head];
        circ_queue.head = (circ_queue.head + 1) % 10000;
        while(image.at<uchar>(seed.x, seed.y - 1) >= thresh){
            seed.y--;
        }
        if(image.at<uchar>(seed.x-1, seed.y) >= thresh && table.at<uchar>(seed.x-1, seed.y) == UNVISITED){
            circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x-1, seed.y);
            circ_queue.tail = (circ_queue.tail + 1) % 10000;
            table.at<uchar>(seed.x-1, seed.y) = MARKED;
        }
        if(image.at<uchar>(seed.x+1, seed.y) >= thresh && table.at<uchar>(seed.x+1, seed.y) == UNVISITED){
            circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x+1, seed.y);
            circ_queue.tail = (circ_queue.tail + 1) %10000;
            table.at<uchar>(seed.x+1, seed.y) = MARKED;
        }

        do{
            input.at<cv::Vec3b>(seed.x, seed.y) = cv::Vec3b(255,0,0);
            table.at<uchar>(seed.x, seed.y) = VISITED;

            if(image.at<uchar>(seed.x-1, seed.y) < thresh){
                
                table.at<uchar>(seed.x-1, seed.y) = BLACK;
                
                if(image.at<uchar>(seed.x-1, seed.y+1) >= thresh &&
                   table.at<uchar>(seed.x-1, seed.y+1) == UNVISITED){

                    circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x-1, seed.y+1);
                    circ_queue.tail = (circ_queue.tail + 1) % 10000;
                    table.at<uchar>(seed.x-1, seed.y+1) = MARKED;
                }
            }

            if(image.at<uchar>(seed.x+1, seed.y) < thresh){
                
                table.at<uchar>(seed.x+1, seed.y) = BLACK;
                
                if(image.at<uchar>(seed.x+1, seed.y+1) >= thresh &&
                   table.at<uchar>(seed.x+1, seed.y+1) == UNVISITED){

                    circ_queue.queue[circ_queue.tail] = cv::Point2i(seed.x+1, seed.y+1);
                    circ_queue.tail = (circ_queue.tail + 1) % 10000;
                    table.at<uchar>(seed.x+1, seed.y+1) = MARKED;
                }
            }
         
            seed.y++;
        }while(image.at<uchar>(seed.x,seed.y) >= thresh);
            
        
    }
    
    
    
    
}

int main(int argc, char **argv){

    if(argc != 2){
        std::cout << "Usage: ./execName ImagetoLoad" << std::endl;
        return -1;
    }

 
    cv::Mat image;
    input = cv::imread(argv[1]);
    cv::cvtColor(input,image, CV_BGR2GRAY);

    floodFill(image, 140);
    
    // threshold(image, image, 140, 255, cv::THRESH_BINARY);
    // cv::namedWindow("Output", cv::WINDOW_NORMAL);
    cv::resize(input, input, cv::Size(input.cols/4, input.rows/4));
    cv::imshow("Output", input);
    cv::waitKey(0);
    return 0;
}
