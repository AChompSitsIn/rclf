# Fortran-ROS2 Integration

A minimal Fortran binding for ROS2 that enables Fortran programs to participate in ROS2 communication networks.

## Features

- **Native Fortran API** for ROS2 nodes, publishers, and subscribers
- **Support for multiple message types**:
  - Float64MultiArray for numeric data
  - String messages for text communication
- **Non-blocking communication** patterns
- **Docker-based development** environment
- **Cross-platform support** (ARM64/AMD64)
  
## Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/AChompSitsIn/rclf.git
   cd fortran-ros2
   ```

2. **Build and start the Docker container**
   ```bash
   docker-compose build
   docker-compose up -d
   ```

3. **Enter the container and build the workspace**
   ```bash
   docker exec -it fortran_ros2_dev bash
   cd /ros2_ws
   source /opt/ros/humble/setup.bash
   colcon build --packages-select fortran_ros2
   source install/setup.bash
   ```
   
### Basic Operations

```fortran
use ros2_fortran

! Initialize node
type(ros2_node) :: node
call ros2_init(node, "my_node_name")

! Create publisher
type(ros2_publisher) :: pub
call create_publisher(node, pub, "/topic_name")

! Publish data
real(8) :: data(3) = [1.0, 2.0, 3.0]
call publish(pub, data)

! Create subscriber
type(ros2_subscriber) :: sub
call create_subscriber(node, sub, "/topic_name")

! Subscribe (non-blocking)
real(8), allocatable :: received_data(:)
if (subscribe(sub, received_data)) then
    ! Process received_data
end if

! Spin and shutdown
call spin_once(node, 10)  ! 10ms timeout
call shutdown(node)
```

### String Messages

```fortran
! String publisher
type(ros2_publisher) :: str_pub
call create_string_publisher(node, str_pub, "/chat")
call publish_string(str_pub, "Hello ROS2!")

! String subscriber
type(ros2_subscriber) :: str_sub
character(len=:), allocatable :: message
call create_string_subscriber(node, str_sub, "/chat")
if (subscribe_string(str_sub, message)) then
    print *, "Received: ", message
end if
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.
