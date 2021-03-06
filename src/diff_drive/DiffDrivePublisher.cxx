/*
 * Copyright 2018 Real-Time Innovations, Inc.
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

#include <cstdlib>
#include <unordered_map>

#include <dds/core/ddscore.hpp>
#include <dds/domain/find.hpp>
#include <dds/pub/ddspub.hpp>

#include "common/ParametersManager.hpp"
#include "common/DdsUtils.hpp"
#include "geometry_msgs/msg/Twist.hpp"

void publisher_main(
        int domain_id,
        std::string topic_name,
        double linear_x,
        double angular_z)
{
    // Find a DomainParticipant
    dds::domain::DomainParticipant participant(dds::domain::find(domain_id));
    if (participant == dds::core::null) {
        participant = dds::domain::DomainParticipant(domain_id);
    }

    // Create a Topic -- and automatically register the type
    dds::topic::Topic<geometry_msgs::msg::Twist> topic(participant, topic_name);

    // Create a DataWriter with default Qos (Publisher created in-line)
    dds::pub::DataWriter<geometry_msgs::msg::Twist> writer(
            dds::pub::Publisher(participant), topic);

    geometry_msgs::msg::Twist sample;
    sample.linear().x(linear_x);
    sample.angular().z(angular_z);

    // Wait until it has a publication matched
    gazebo::dds::utils::wait_for_publication_matched(
            writer, dds::core::Duration(4));

    // Write sample
    std::cout << "Sending data..." << std::endl;
    writer.write(sample);

    writer.wait_for_acknowledgments(dds::core::Duration(4));
}

int main(int argc, char *argv[])
{
    int ret_code = 0;

    gazebo::dds::utils::ParametersManager parameters_manager(argc, argv);

    if (parameters_manager.has_flag("-h")) {
        std::cout << "Usage: diffdrivepublisher [options]" << std::endl
                  << "Generic options:" << std::endl
                  << "\t-h                      - Prints this page and exits"
                  << std::endl
                  << "\t-d <domain id>          - Sets the domainId (default 0)"
                  << std::endl
                  << "\t-t <topic name>         - Sets the topic name"
                  << std::endl
                  << "\t-s <sample information> - Sets information of the "
                     "sample (default 0, 0)"
                  << std::endl;
        return 0;
    }

    // Handle signals (e.g., CTRL+C)
    gazebo::dds::utils::setup_signal_handler();

    try {
        // Check arguments
        int domain_id = 0;
        if (parameters_manager.has_flag("-d")) {
            domain_id = atoi(parameters_manager.get_flag_value("-d").c_str());
        }

        float linear_x = 0.0;
        float angular_z = 0.0;
        if (parameters_manager.has_flag("-s")) {
            parameters_manager.process_sample_information("-s");

            std::unordered_map<std::string, std::vector<std::string>>
                    sample_information
                    = parameters_manager.get_sample_information();

            // Initialize parameters configuration
            gazebo::dds::utils::ParametersConfiguration parameters;
            parameters.arguments({ "linear_velocity", "angular_velocity" });
            parameters.missing_error(
                    "\nERROR: Missing  arguments to call publisher: \nMissing "
                    "arguments:");
            parameters.expected(
                    "\n\nExcepted: diffdrivepublisher -d <domain id> -t <topic "
                    "name> -s \"linear_velocity: <axis x> angular_velocity: "
                    "<axis z>\"");

            // Check request information
            parameters_manager.validate_sample(parameters);

            linear_x = atof(sample_information["linear_velocity"][0].c_str());
            angular_z = atof(sample_information["angular_velocity"][0].c_str());
        }

        publisher_main(
                domain_id,
                std::string(parameters_manager.get_flag_value("-t")),
                linear_x,
                angular_z);

    } catch (const std::exception &ex) {
        // This will catch DDS and CommandLineParser exceptions
        std::cerr << ex.what() << std::endl;
        ret_code = -1;
    }

    dds::domain::DomainParticipant::finalize_participant_factory();

    return ret_code;
}
