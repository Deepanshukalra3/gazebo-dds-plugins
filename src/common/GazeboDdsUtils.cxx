#include <gazebo/common/common.hh>
#include <gazebo/physics/physics.hh>

namespace gazebo { namespace dds { namespace utils {

template <class T>
void get_world_parameter(
        sdf::ElementPtr sdf,
        T &tag_variable,
        const char *tag_name,
        const T &default_value)
{
    if (sdf->HasElement(tag_name)) {
        tag_variable = sdf->Get<T>(tag_name);
    } else {
        tag_variable = default_value;
        gzwarn << "Missing <" << tag_name
               << ">, set to default: " << tag_variable << std::endl;
    }
}

gazebo::physics::JointPtr get_joint(
        sdf::ElementPtr sdf,
        gazebo::physics::ModelPtr parent,
        const char *tag_name,
        const std::string &joint_default_name)
{
    std::string joint_name;
    get_world_parameter<std::string>(
            sdf, joint_name, tag_name, joint_default_name);
    gazebo::physics::JointPtr joint = parent->GetJoint(joint_name);

    return joint;
}

common::Time get_sim_time(physics::ModelPtr parent)
{
#if GAZEBO_MAJOR_VERSION >= 8
    return parent->GetWorld()->SimTime();
#else
    return parent->GetWorld()->GetSimTime();
#endif
}


}  // namespace utils
}  // namespace dds
}  // namespace gazebo