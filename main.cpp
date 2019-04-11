#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <csignal>
#include <cstdio>

#include "utils.h"
#include "spinnaker_driver.h"

bool done = false;

void sighandler(int signum)
{
  printf("signal: %d\n", signum);
  done = true;
}

int main(int argc, char* argv[])
{

  bool imview = false, imsave = false;

  if (argc == 1 || (argc > 1 && !(strcmp(argv[1], "-h") && strcmp(argv[1], "--help") && strcmp(argv[1], "-help") && strcmp(argv[1], "help"))))
  {
    printf("Usage: %s <imshow (1 or 0)> <imsave (1 or 0), default is 0>\n", argv[0]);
    exit(0);
  }

  if (argc > 1)
  {
    imview = atoi(argv[1]);
  }
  if (argc > 2)
    imsave = atoi(argv[2]);

  cv::Mat frame;

  if (imview)
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

    if (imview)
      cv::imshow("imview", frame);

    if (imsave)
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
