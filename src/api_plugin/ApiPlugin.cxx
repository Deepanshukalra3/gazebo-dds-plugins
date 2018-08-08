/*
 * Copyright 2018 Real-Time Innovations, Inc.
 * Copyright 2012-2014 Open Source Robotics Foundation
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

#include "common/Properties.h"
#include "common/GazeboUtils.hpp"
#include "common/DdsUtils.hpp"
#include "ApiPlugin.h"

namespace gazebo { namespace dds {

// Register this plugin with the simulator
GZ_REGISTER_WORLD_PLUGIN(ApiPlugin)

ApiPlugin::ApiPlugin()
        : participant_(::dds::core::null),
          replier_(::dds::core::null),
          listener_(std::bind(
                  &ApiPlugin::delete_model,
                  this,
                  std::placeholders::_1))
{
}

ApiPlugin::~ApiPlugin()
{
}

void ApiPlugin::Load(physics::WorldPtr parent, sdf::ElementPtr sdf)
{
    parent_=parent;

    // Obtain the qos profile information from loaded world
    std::string qos_profile_file;
    utils::get_world_parameter<std::string>(
            sdf, qos_profile_file, QOS_PROFILE_FILE_PROPERTY_NAME.c_str(), "");
    
    std::string qos_profile;
    utils::get_world_parameter<std::string>(
            sdf, qos_profile, QOS_PROFILE_PROPERTY_NAME.c_str(), "");

    ::dds::core::QosProvider qos_provider(::dds::core::null);
    if(qos_profile_file != "" || qos_profile !=""){
        qos_provider
            = ::dds::core::QosProvider(qos_profile_file, qos_profile);
    }
    else{
        qos_provider
            = ::dds::core::QosProvider::Default();
    }

    // Obtain the domain id from loaded world
    int domain_id;
    utils::get_world_parameter<int>(
            sdf, domain_id, DOMAIN_ID_PROPERTY_NAME.c_str(), 0);

    utils::find_domain_participant(
            domain_id, participant_, qos_provider, qos_profile);

    // Obtain the topic name from loaded world
    std::string topic_name;
    utils::get_world_parameter<std::string>(
            sdf, topic_name, TOPIC_NAME_PROPERTY_NAME.c_str(), "force_torque_wrench");

    rti::request::ReplierParams replier_params(participant_);
    replier_params.service_name("TestService");
    
    replier_ = rti::request::Replier<
            gazebo_msgs::srv::DeleteModel_Request,
            gazebo_msgs::srv::Default_Response>(replier_params);

    replier_.listener(&listener_);

    gzmsg << std::endl;
    gzmsg << "Starting Api plugin"<< std::endl;
    gzmsg << "* Publications:" << std::endl;
    gzmsg << "  - " << topic_name << " [gazebo_msgs/msg/LinkStates]"
          << std::endl;
    gzmsg << "* Subscritions:" << std::endl;
    gzmsg << "  - " << topic_name << " [geometry_msgs/msg/Wrench]" 
          << std::endl;
}

gazebo_msgs::srv::Default_Response ApiPlugin::delete_model(gazebo_msgs::srv::DeleteModel_Request request){
    std::cout<< request.mode_name() << " deleted..."<<std::endl;
    gazebo_msgs::srv::Default_Response reply;

    reply.success(true); 
    reply.status_message("Message from ApiPlugin"); 

    return reply;
}

}  // namespace dds
}  // namespace gazebo
