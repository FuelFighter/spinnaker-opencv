#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <csignal>

#include "utils.h"
#include "spinnaker_driver.h"

#define IMVIEW false
#define IMSAVE true

bool done = false;

void sighandler(int signum)
{
  printf("signal: %d\n", signum);
  done = true;
}

int main()
{

  cv::Mat frame;

  if (IMVIEW)
  cv::namedWindow("imview", cv::WINDOW_NORMAL);

  long time = utils::ms();

  if (spinnaker_driver::init())
    return 1;

  signal(SIGINT, sighandler);

  while (!done)
  {
    //utils::tic(time);

    if (spinnaker_driver::getFrame(frame))
      return 1;

    if (IMVIEW)
      cv::imshow("imview", frame);

    if (IMSAVE)
    {
      static char name[100];
      sprintf(name, "img/ff_%ld.jpg", utils::ms());
      cv::imwrite(name, frame);
    }

    spinnaker_driver::releaseFrame();

    char c = (char)cv::waitKey(5);
    if (c == 27)
      done = true;
  }

  spinnaker_driver::shutdown();

  return 0;
}
