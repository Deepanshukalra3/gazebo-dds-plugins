/*
 * Copyright 2018 Real-Time Innovations, Inc.
 * Copyright 2012 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "common/GazeboCameraUtils.h"

namespace gazebo { namespace dds {

struct CameraInformation {
    rendering::CameraPtr camera;
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    std::string format;
    event::ConnectionPtr new_frame_connection;
}; 

class MultiCamera : public SensorPlugin
{
public:
    /**
     * @brief Constructor
     */
    MultiCamera();

    /**
     * @brief Destructor
     */
    ~MultiCamera();

    /**
     * @brief Load the plugin inside Gazebo's system
     *
     * @param parent object of Gazebo's sensor
     * @param sdf object of Gazebo's world
     */
    void Load(sensors::SensorPtr parent, sdf::ElementPtr sdf) override;

protected:

    /**
     * @brief Update the controller
     *
     * @param image raw information of the current image
     * @param width width of the current image
     * @param height height of the current image
     * @param depth  depth of the current image
     * @param format format of the current image
     */
    virtual void on_new_frame(
            const unsigned char * image,
            unsigned int width,
            unsigned int height,
            unsigned int depth,
            const std::string & format);

    /**
     * @brief Update the controller of the camera left
     *
     * @param image raw information of the current image
     * @param width width of the current image
     * @param height height of the current image
     * @param depth  depth of the current image
     * @param format format of the current image
     */
    virtual void on_new_frame_left(
            const unsigned char * image,
            unsigned int width,
            unsigned int height,
            unsigned int depth,
            const std::string & format);            
    
    /**
     * @brief Update the controller of the camera right
     *
     * @param image raw information of the current image
     * @param width width of the current image
     * @param height height of the current image
     * @param depth  depth of the current image
     * @param format format of the current image
     */
    virtual void on_new_frame_right(
            const unsigned char * image,
            unsigned int width,
            unsigned int height,
            unsigned int depth,
            const std::string & format);   

private:
    common::Time last_update_time_;
    sensors::MultiCameraSensorPtr parent_sensor;
    std::vector<CameraInformation> cameras_;
};

}  // namespace dds
}  // namespace gazebo
