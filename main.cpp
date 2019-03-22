//=============================================================================
// Copyright � 2015 Point Grey Research, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of 
// Point Grey Research, Inc. ("Confidential Information"). You shall not
// disclose such Confidential Information and shall use it only in 
// accordance with the terms of the "License Agreement" that you 
// entered into with PGR in connection with this software.
//
// UNLESS OTHERWISE SET OUT IN THE LICENSE AGREEMENT, THIS SOFTWARE IS 
// PROVIDED ON AN �AS-IS� BASIS AND POINT GREY RESEARCH INC. MAKES NO 
// REPRESENTATIONS OR WARRANTIES ABOUT THE SOFTWARE, EITHER EXPRESS 
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES OR 
// CONDITIONS OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR 
// NON-INFRINGEMENT. POINT GREY RESEARCH INC. SHALL NOT BE LIABLE FOR ANY 
// DAMAGES, INCLUDING BUT NOT LIMITED TO ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, PUNITIVE, OR CONSEQUENTIAL DAMAGES, OR ANY LOSS OF PROFITS, 
// REVENUE, DATA OR DATA USE, ARISING OUT OF OR IN CONNECTION WITH THIS 
// SOFTWARE OR OTHERWISE SUFFERED BY YOU AS A RESULT OF USING, MODIFYING 
// OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"
#include <iostream>
#include <sstream> 
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;
// This function acquires and saves 10 images from a device.  
int AcquireImages(CameraPtr pCam, INodeMap & nodeMap, INodeMap & nodeMapTLDevice)
{
        int result = 0;
        
        cout << endl << endl << "*** IMAGE ACQUISITION ***" << endl << endl;
        
        try
        {
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
                // Retrieve enumeration node from nodemap
                CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
                if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode))
                {
                        cout << "Unable to set acquisition mode to continuous (enum retrieval). Aborting..." << endl << endl;
                        return -1;
                }
                
                // Retrieve entry node from enumeration node
                CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
                if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous))
                {
                        cout << "Unable to set acquisition mode to continuous (entry retrieval). Aborting..." << endl << endl;
                        return -1;
                }
                
                // Retrieve integer value from entry node
                int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();
                
                // Set integer value from entry node as new value of enumeration node
                ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);
                
                cout << "Acquisition mode set to continuous..." << endl;
                
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
                cout << "Acquiring images..." << endl;
                
                //
                // Retrieve device serial number for filename
                //
                // *** NOTES ***
                // The device serial number is retrieved in order to keep cameras from 
                // overwriting one another. Grabbing image IDs could also accomplish
                // this.
                //
                gcstring deviceSerialNumber("");
                CStringPtr ptrStringSerial = nodeMapTLDevice.GetNode("DeviceSerialNumber");
                if (IsAvailable(ptrStringSerial) && IsReadable(ptrStringSerial))
                {
                        deviceSerialNumber = ptrStringSerial->GetValue();
                        cout << "Device serial number retrieved as " << deviceSerialNumber << "..." << endl;
                }
                cout << endl;
                
                // Retrieve, convert, and save images
                const unsigned int k_numImages = 10;
                
                for (unsigned int imageCnt = 0; imageCnt < k_numImages; imageCnt++)
                {
                        try
                        {
                                //
                                // Retrieve next received image
                                //
                                // *** NOTES ***
                                // Capturing an image houses images on the camera buffer. Trying
                                // to capture an image that does not exist will hang the camera.
                                //
                                // *** LATER ***
                                // Once an image from the buffer is saved and/or no longer 
                                // needed, the image must be released in order to keep the 
                                // buffer from filling up.
                                //
                                ImagePtr pResultImage = pCam->GetNextImage();
                                //
                                // Ensure image completion
                                //
                                // *** NOTES ***
                                // Images can easily be checked for completion. This should be
                                // done whenever a complete image is expected or required.
                                // Further, check image status for a little more insight into
                                // why an image is incomplete.
                                //
                                if (pResultImage->IsIncomplete())
                                {
                                        
                                        cout << "Image incomplete with image status " << pResultImage->GetImageStatus() << "..." << endl << endl;
                                }
                                else
                                {
                                        //
                                        // Print image information; height and width recorded in pixels
                                        //
                                        // *** NOTES ***
                                        // Images have quite a bit of available metadata including
                                        // things such as CRC, image status, and offset values, to
                                        // name a few.
                                        //
                                        size_t width = pResultImage->GetWidth();
                                        
                                        size_t height = pResultImage->GetHeight();
                                        
                                        cout << "Grabbed image " << imageCnt << ", width = " << width << ", height = " << height << endl;
                                        //
                                        // Convert image to mono 8
                                        //
                                        // *** NOTES ***
                                        // Images can be converted between pixel formats by using 
                                        // the appropriate enumeration value. Unlike the original 
                                        // image, the converted one does not need to be released as 
                                        // it does not affect the camera buffer.
                                        //
                                        // When converting images, color processing algorithm is an
                                        // optional parameter.
                                        // 
                                        ImagePtr convertedImage = pResultImage->Convert(PixelFormat_Mono8, HQ_LINEAR);
                                        // Create a unique filename
                                        ostringstream filename;
                                        
                                        filename << "Acquisition-";
                                        if (deviceSerialNumber != "")
                                        {
                                                        filename << deviceSerialNumber.c_str() << "-";
                                        }
                                        filename << imageCnt << ".jpg";
                                        //
                                        // Save image
                                        // 
                                        // *** NOTES ***
                                        // The standard practice of the examples is to use device
                                        // serial numbers to keep images of one device from 
                                        // overwriting those of another.
                                        //
                                        convertedImage->Save(filename.str().c_str());
                                        cout << "Image saved at " << filename.str() << endl;
                                }
                                //
                                // Release image
                                //
                                // *** NOTES ***
                                // Images retrieved directly from the camera (i.e. non-converted
                                // images) need to be released in order to keep from filling the
                                // buffer.
                                //
                                pResultImage->Release();
                                cout << endl;
                        }
                        catch (Spinnaker::Exception &e)
                        {
                                cout << "Error: " << e.what() << endl;
                                result = -1;
                        }
                }
                
                //
                // End acquisition
                //
                // *** NOTES ***
                // Ending acquisition appropriately helps ensure that devices clean up
                // properly and do not need to be power-cycled to maintain integrity.
                //
                pCam->EndAcquisition();
        }
        catch (Spinnaker::Exception &e)
        {
                cout << "Error: " << e.what() << endl;
                result = -1;
        }
        
        return result;
}
// This function prints the device information of the camera from the transport
// layer; please see NodeMapInfo example for more in-depth comments on printing
// device information from the nodemap.
int PrintDeviceInfo(INodeMap & nodeMap)
{
        int result = 0;
        
        cout << endl << "*** DEVICE INFORMATION ***" << endl << endl;
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
                                cout << pfeatureNode->GetName() << " : ";
                                CValuePtr pValue = (CValuePtr)pfeatureNode;
                                cout << (IsReadable(pValue) ? pValue->ToString() : "Node not readable");
                                cout << endl;
                        }
                }
                else
                {
                        cout << "Device control information not available." << endl;
                }
        }
        catch (Spinnaker::Exception &e)
        {
                cout << "Error: " << e.what() << endl;
                result = -1;
        }
        
        return result;
}
// This function acts as the body of the example; please see NodeMapInfo example 
// for more in-depth comments on setting up cameras.
int RunSingleCamera(CameraPtr pCam)
{
        int result = 0;
        try
        {
                // Retrieve TL device nodemap and print device information
                INodeMap & nodeMapTLDevice = pCam->GetTLDeviceNodeMap();
                
                result = PrintDeviceInfo(nodeMapTLDevice);
                
                // Initialize camera
                pCam->Init();
                
                // Retrieve GenICam nodemap
                INodeMap & nodeMap = pCam->GetNodeMap();
                // Acquire images
                result = result | AcquireImages(pCam, nodeMap, nodeMapTLDevice);
                
                // Deinitialize camera
                pCam->DeInit();
        }
        catch (Spinnaker::Exception &e)
        {
                cout << "Error: " << e.what() << endl;
                result = -1;
        }
        return result;
}
// Example entry point; please see Enumeration example for more in-depth 
// comments on preparing and cleaning up the system.
int main(int /*argc*/, char** /*argv*/)
{
        int result = 0;
        
        // Print application build information
        cout << "Application build date: " << __DATE__ << " " << __TIME__ << endl << endl;
        
        // Retrieve singleton reference to system object
        SystemPtr system = System::GetInstance();
        // Retrieve list of cameras from the system
        CameraList camList = system->GetCameras();
        unsigned int numCameras = camList.GetSize();
        
        cout << "Number of cameras detected: " << numCameras << endl << endl;
        
        // Finish if there are no cameras
        if (numCameras == 0)
        {
                // Clear camera list before releasing system
                camList.Clear();
                // Release system
                system->ReleaseInstance();
                cout << "Not enough cameras!" << endl;
                cout << "Done! Press Enter to exit..." << endl;
                getchar();
                
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
        CameraPtr pCam = NULL;
        // Run example on each camera
        for (unsigned int i = 0; i < numCameras; i++)
        {
                // Select camera
                pCam = camList.GetByIndex(i);
                cout << endl << "Running example for camera " << i << "..." << endl;
                
                // Run example
                result = result | RunSingleCamera(pCam);
                
                cout << "Camera " << i << " example complete..." << endl << endl;
        }
        //
        // Release reference to the camera
        //
        // *** NOTES ***
        // Had the CameraPtr object been created within the for-loop, it would not
        // be necessary to manually break the reference because the shared pointer
        // would have automatically cleaned itself up upon exiting the loop.
        //
        pCam = NULL;
        // Clear camera list before releasing system
        camList.Clear();
        // Release system
        system->ReleaseInstance();
        cout << endl << "Done! Press Enter to exit..." << endl;
        getchar();
        return result;
}
