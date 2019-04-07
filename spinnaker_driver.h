#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream> 
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "utils.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int PrintDeviceInfo(INodeMap & nodeMap)
{
  int result = 0;
  
  std::cout << std::endl << "*** DEVICE INFORMATION ***" << std::endl << std::endl;
  try
  {
    FeatureList_t features;
    CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
    if (IsAvailable(category) && IsReadable(category))
    {
      category->GetFeatures(features);
      FeatureList_t::const_iterator it;
      for (it = features.begin(); it != features.end(); ++it)
      {
        CNodePtr pfeatureNode = *it;
        std::cout << pfeatureNode->GetName() << " : ";
        CValuePtr pValue = (CValuePtr)pfeatureNode;
        std::cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
        std::cout << std::endl;
      }
    }
    else
    {
      std::cout << "Device control information not available." << std::endl;
    }
  }
  catch (Spinnaker::Exception &e)
  {
    std::cout << "Error: " << e.what() << std::endl;
    result = -1;
  }
  
  return result;
}

namespace spinnaker_driver
{

  bool firstGetFrame = true;
  SystemPtr system;
  CameraList camList;
  CameraPtr pCam = 0;
  ImagePtr pResultImage;

  // Example entry point; please see Enumeration example for more in-depth 
  // comments on preparing and cleaning up the system.
  int init()
  {
  
    //if (IMVIEW)
    //{
    //  cv::namedWindow("imview", cv::WINDOW_NORMAL);
    //}
  
    int result = 0;
    
    // Print application build information
    //std::cout << "Application build date: " << __DATE__ << " " << __TIME__ << std::endl << std::endl;
    
    // Retrieve singleton reference to system object
    system = System::GetInstance();
    // Retrieve list of cameras from the system
    camList = system->GetCameras();
    unsigned int numCameras = camList.GetSize();
    
    std::cout << "Number of cameras detected: " << numCameras << std::endl << std::endl;
    
    if (numCameras != 1)
    {
      camList.Clear();
      system->ReleaseInstance();
      std::cout << "Not one single camera!" << std::endl;
  
      return -1;
    }
    //
    // Create shared pointer to camera
    //
    // *** NOTES ***
    // The CameraPtr object is a shared pointer, and will generally clean itself
    // up upon exiting its scope. However, if a shared pointer is created in the
    // same scope that a system object is explicitly released (i.e. this scope),
    // the reference to the shared point must be broken manually.
    //
    // *** LATER ***
    // Shared pointers can be terminated manually by assigning them to NULL.
    // This keeps releasing the system from throwing an exception.
    //
    // Select camera
    pCam = camList.GetByIndex(0);
    
    try
    {
      // Retrieve TL device nodemap and print device information
      INodeMap & nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
      
      result = PrintDeviceInfo(nodeMapTLDevice);
      
      // Initialize camera
      pCam->Init();
    }
    catch (Spinnaker::Exception &e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      result = -1;
    }
      
    // Retrieve GenICam nodemap
    INodeMap & nodeMap = pCam->GetNodeMap();
  
    //
    // Set acquisition mode to continuous
    //
    // *** NOTES ***
    // Because the example acquires and saves 10 images, setting acquisition 
    // mode to continuous lets the example finish. If set to single frame
    // or multiframe (at a lower number of images), the example would just
    // hang. This would happen because the example has been written to
    // acquire 10 images while the camera would have been programmed to 
    // retrieve less than that.
    // 
    // Setting the value of an enumeration node is slightly more complicated
    // than other node types. Two nodes must be retrieved: first, the 
    // enumeration node is retrieved from the nodemap; and second, the entry
    // node is retrieved from the enumeration node. The integer value of the
    // entry node is then set as the new value of the enumeration node.
    //
    // Notice that both the enumeration and the entry nodes are checked for
    // availability and readability/writability. Enumeration nodes are
    // generally readable and writable whereas their entry nodes are only
    // ever readable.
    // 
  
    CEnumerationPtr ptrPixelFormat = nodeMap.GetNode("PixelFormat");
      std::cout << "Pixel format is " << ptrPixelFormat->GetCurrentEntry()->GetSymbolic() << "..." << std::endl;
      printf("isavailable: %d, iswritable: %d\n", IsAvailable(ptrPixelFormat), IsWritable(ptrPixelFormat));
      if (IsAvailable(ptrPixelFormat) && IsWritable(ptrPixelFormat))
      {
        // Retrieve the desired entry node from the enumeration node
        CEnumEntryPtr ptrPixelFormatRGB8 = ptrPixelFormat->GetEntryByName("BGR8");
        printf("isavailable: %d, isreadable: %d\n", IsAvailable(ptrPixelFormatRGB8), IsReadable(ptrPixelFormatRGB8));
        if (IsAvailable(ptrPixelFormatRGB8) && IsReadable(ptrPixelFormatRGB8))
        {
          // Retrieve the integer value from the entry node
          int64_t pixelFormatRGB8 = ptrPixelFormatRGB8->GetValue();
          // Set integer as new value for enumeration node
          ptrPixelFormat->SetIntValue(pixelFormatRGB8);
          std::cout << "Pixel format set to " << ptrPixelFormat->GetCurrentEntry()->GetSymbolic() << "..." << std::endl;
        }
        else
        {
          std::cout << "Pixel format not available..." << std::endl;
          std::cout << "Pixel format is " << ptrPixelFormat->GetCurrentEntry()->GetSymbolic() << "..." << std::endl;
        }
      }
      else
      {
        std::cout << "Pixel format not available..." << std::endl;
      }
  
  
  
    // Retrieve enumeration node from nodemap
    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
    {
      std::cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << std::endl << std::endl;
      return -1;
    }
  
    // Retrieve entry node from enumeration node
    CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
    if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
    {
      std::cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << std::endl << std::endl;
      return -1;
    }
  
    // Retrieve integer value from entry node
    int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
    
    // Set integer value from entry node as new value of enumeration node
    ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
    
    std::cout << "Acquisition mode set to continuous..." << std::endl;
    
    //
    // Begin acquiring images
    //
    // *** NOTES ***
    // What happens when the camera begins acquiring images depends on the
    // acquisition mode. Single frame captures only a single image, multi 
    // frame catures a set number of images, and continuous captures a 
    // continuous stream of images. Because the example calls for the 
    // retrieval of 10 images, continuous mode has been set.
    // 
    // *** LATER ***
    // Image acquisition must be ended when no more images are needed.
    //
    pCam->BeginAcquisition();
    std::cout << "Acquiring images..." << std::endl;
    
    //
    // Retrieve device serial number for filename
    //
    // *** NOTES ***
    // The device serial number is retrieved in order to keep cameras from 
    // overwriting one another. Grabbing image IDs could also accomplish
    // this.
    //
    gcstring deviceSerialNumber("");
    CStringPtr ptrStringSerial = pCam->GetTLDeviceNodeMap().GetNode("DeviceSerialNumber");
    if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
    {
      deviceSerialNumber = ptrStringSerial->GetValue();
      std::cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << std::endl;
    }
    std::cout << std::endl;
    
    return result;
  }
  
  int getFrame(cv::Mat& frame)
  {
    //int AcquireImage(CameraPtr pCam, INodeMap & nodeMap, INodeMap & nodeMapTLDevice)
    int result = 0;
    
    try
    {
      pResultImage = pCam->GetNextImage();
      if (pResultImage->IsIncomplete())
        std::cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << std::endl << std::endl;
      else
      {
        //size_t width = pResultImage->GetWidth();
        //size_t height = pResultImage->GetHeight();
  
        //std::cout << "Grabbed image, width = " << width << ", height = " << height << ", format = " << (pCam->PixelFormat.GetValue() == PixelFormat_BGR8) << std::endl;
  
        if (firstGetFrame)
        {
          printf("firstgetframe\n");
          frame = cv::Mat(pResultImage->GetHeight() + pResultImage->GetYPadding(), pResultImage->GetWidth() + pResultImage->GetXPadding(), CV_8UC3);
          firstGetFrame = false;
        }
  
        frame.data = (uchar*)pResultImage->GetData();
  
        //if (false)
        //{
        //  ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);
        //  // Create a unique filename
        //  std::ostringstream filename;
  
        //  filename << "Acquisition-";
        //  if (deviceSerialNumber != "")
        //  {
        //                  filename << deviceSerialNumber.c_str() << "-";
        //  }
        //  filename << "file.jpg";
        //  //
        //  // Save image
        //  // 
        //  // *** NOTES ***
        //  // The standard practice of the examples is to use device
        //  // serial numbers to keep images of one device from 
        //  // overwriting those of another.
        //  //
        //  convertedImage->Save(filename.str().c_str());
        //  std::cout << "Image saved at " << filename.str() << std::endl;
        //}
      }
    }
    catch (Spinnaker::Exception &e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      result = -1;
    }
    
    return result;
  }

  int releaseFrame()
  {
    int result = 0;
    try
    {
      //
      // Release image
      //
      // *** NOTES ***
      // Images retrieved directly from the camera (i.e. non-converted
      // images) need to be released in order to keep from filling the
      // buffer.
      //
      pResultImage->Release();
    }
    catch (Spinnaker::Exception &e)
    {
      std::cout << "Error: " << e.what() << std::endl;
      result = -1;
    }
    return result;
  }
  
  void shutdown()
  {
    //
    // End acquisition
    //
    // *** NOTES ***
    // Ending acquisition appropriately helps ensure that devices clean up
    // properly and do not need to be power-cycled to maintain integrity.
    //
    pCam->EndAcquisition();
  
    // Deinitialize camera
    pCam->DeInit();
  
    //
    // Release reference to the camera
    //
    // *** NOTES ***
    // Had the CameraPtr object been created within the for-loop, it would not
    // be necessary to manually break the reference because the shared pointer
    // would have automatically cleaned itself up upon exiting the loop.
    //
    pCam = 0;
    // Clear camera list before releasing system
    camList.Clear();
    // Release system
    system->ReleaseInstance();
    //std::cout << std::endl << "Done! Press Enter to exit..." << std::endl;
    //getchar();
  }
}
