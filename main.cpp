#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "utils.h"
#include "spinnaker_driver.h"

#define IMVIEW true

int main()
{

  cv::Mat frame;
  cv::namedWindow("imview", cv::WINDOW_NORMAL);

  if (spinnaker_driver::init())
    return 1;

  bool done = false;

  while (!done)
  {
    if (spinnaker_driver::getFrame(frame))
      return 1;

    cv::imshow("imview", frame);
  
    char c = (char)cv::waitKey(5);
    if (c == 27)
      done = true;

    spinnaker_driver::releaseFrame();
  }

  spinnaker_driver::shutdown();

  return 0;
}
