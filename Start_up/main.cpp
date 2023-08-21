#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>

int main()
{
    // Set up the camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        return EXIT_FAILURE;
    }

    // Set up the background subtractor
    cv::Ptr<cv::BackgroundSubtractor> bgsub = cv::createBackgroundSubtractorMOG2(100, 50, false);

    // Set up the face recognition script
    std::string face_recognition_script = "/home/pi/software/Face-Recognition/FaceRecognition";
    
    std::string script_directory = face_recognition_script.substr(0, face_recognition_script.find_last_of("/\\"));
    
    if (chdir(script_directory.c_str()) != 0) {
        std::cerr << "Error: Could not change working directory to " << script_directory << std::endl;
        return EXIT_FAILURE;
    }

    // Set up the threshold for change detection
    float change_threshold_ratio = 0.1;
    int i = 0;
    bool change_detected = false;
    
        // Capture a few frames to create an initial background model
    cv::Mat initial_frame;
    for (int j = 0; j < 10; j++) {
        cap.read(initial_frame);
        if (initial_frame.empty()) {
            break;
        }
        bgsub->apply(initial_frame, initial_frame);
    }

    // Delay for a few seconds to allow for initial changes to settle down
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    std::cout << "Program is looking for changes in camera feed" << std::endl;
    
    // Loop over frames from the camera
    while (true) {
        // Read a frame from the camera
        cv::Mat frame;
        cap.read(frame);
        if (frame.empty()) {
            break;
        }

        // Apply the background subtractor to the frame
        cv::Mat fgmask;
        bgsub->apply(frame, fgmask);

        // Calculate the ratio of changed pixels
        int total_pixels = fgmask.total();
        int num_changes = cv::countNonZero(fgmask == 255);
        float change_ratio = static_cast<float>(num_changes) / total_pixels;

        // Check if the ratio of changed pixels exceeds the threshold

        if (change_ratio > change_threshold_ratio && !change_detected) {
            // Call the face recognition script

            std::cout <<i<< " Massive change detected!" << std::endl;
            std::cout << "Face recognition script starting!"<< std::endl;

            i++;
            change_detected = true;
            std::cout << std::endl;
            
            cap.release();

            // Use wait() to wait for the face recognition script to finish
            int ret = std::system(face_recognition_script.c_str());
            if (WIFEXITED(ret) && (WEXITSTATUS(ret) == 0)) {
                std::cout << "Face recognition script finished successfully." << std::endl;
            } else {
            std::cerr << "Error: face recognition script returned " << ret << std::endl;
            }

            // Reopen the camera
            cap.open(0);
            
        }else if (change_detected && change_ratio <= change_threshold_ratio) {
            // Reset the change detected flag when the scene returns to normal
            change_detected = false;
        }

        // Show the camera output for troubleshooting
        //cv::imshow("Camera Output", frame);

        // Wait for a key press
        int key = cv::waitKey(1);
        if (key == 'q') {
            break;
        }
    }

    // Release the camera
    cap.release();

    return EXIT_SUCCESS;
}
