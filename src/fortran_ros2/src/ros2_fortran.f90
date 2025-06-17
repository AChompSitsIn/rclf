module ros2_fortran
  use iso_c_binding
  implicit none
  
  ! Opaque handle types
  type :: ros2_node
    type(c_ptr) :: handle = c_null_ptr
  end type
  
  type :: ros2_publisher
    type(c_ptr) :: handle = c_null_ptr
  end type
  
  type :: ros2_subscriber
    type(c_ptr) :: handle = c_null_ptr
  end type
  
  ! C function interfaces
  interface
    function fortran_ros2_init(node_name) bind(C, name="fortran_ros2_init")
      use iso_c_binding
      type(c_ptr) :: fortran_ros2_init
      character(c_char), dimension(*) :: node_name
    end function
    
    function fortran_create_publisher(node, topic) bind(C, name="fortran_create_publisher")
      use iso_c_binding
      type(c_ptr) :: fortran_create_publisher
      type(c_ptr), value :: node
      character(c_char), dimension(*) :: topic
    end function
    
    function fortran_create_subscriber(node, topic) bind(C, name="fortran_create_subscriber")
      use iso_c_binding
      type(c_ptr) :: fortran_create_subscriber
      type(c_ptr), value :: node
      character(c_char), dimension(*) :: topic
    end function
    
    subroutine fortran_publish(pub, data, size) bind(C, name="fortran_publish")
      use iso_c_binding
      type(c_ptr), value :: pub
      real(c_double), dimension(*) :: data
      integer(c_int), value :: size
    end subroutine
    
    function fortran_take_message(sub, buffer, buffer_size) bind(C, name="fortran_take_message")
      use iso_c_binding
      integer(c_int) :: fortran_take_message
      type(c_ptr), value :: sub
      real(c_double), dimension(*) :: buffer
      integer(c_int), value :: buffer_size
    end function
    
    function fortran_create_string_publisher(node, topic) bind(C, name="fortran_create_string_publisher")
      use iso_c_binding
      type(c_ptr) :: fortran_create_string_publisher
      type(c_ptr), value :: node
      character(c_char), dimension(*) :: topic
    end function
    
    subroutine fortran_publish_string(pub, str) bind(C, name="fortran_publish_string")
      use iso_c_binding
      type(c_ptr), value :: pub
      character(c_char), dimension(*) :: str
    end subroutine
    
    function fortran_create_string_subscriber(node, topic) bind(C, name="fortran_create_string_subscriber")
      use iso_c_binding
      type(c_ptr) :: fortran_create_string_subscriber
      type(c_ptr), value :: node
      character(c_char), dimension(*) :: topic
    end function
    
    function fortran_take_string(sub, buffer, buffer_size) bind(C, name="fortran_take_string")
      use iso_c_binding
      integer(c_int) :: fortran_take_string
      type(c_ptr), value :: sub
      character(c_char), dimension(*) :: buffer
      integer(c_int), value :: buffer_size
    end function
    
    subroutine fortran_spin_once(node, timeout_ms) bind(C, name="fortran_spin_once")
      use iso_c_binding
      type(c_ptr), value :: node
      integer(c_int), value :: timeout_ms
    end subroutine
    
    subroutine fortran_shutdown(node) bind(C, name="fortran_shutdown")
      use iso_c_binding
      type(c_ptr), value :: node
    end subroutine
  end interface
  
contains
  
  ! Initialize ROS2 node
  subroutine ros2_init(node, node_name)
    type(ros2_node), intent(out) :: node
    character(len=*), intent(in) :: node_name
    character(len=:), allocatable, target :: c_name
    
    c_name = trim(node_name) // c_null_char
    node%handle = fortran_ros2_init(c_name)
  end subroutine
  
  ! Create publisher
  subroutine create_publisher(node, pub, topic_name)
    type(ros2_node), intent(in) :: node
    type(ros2_publisher), intent(out) :: pub
    character(len=*), intent(in) :: topic_name
    character(len=:), allocatable, target :: c_topic
    
    c_topic = trim(topic_name) // c_null_char
    pub%handle = fortran_create_publisher(node%handle, c_topic)
  end subroutine
  
  ! Create subscriber
  subroutine create_subscriber(node, sub, topic_name)
    type(ros2_node), intent(in) :: node
    type(ros2_subscriber), intent(out) :: sub
    character(len=*), intent(in) :: topic_name
    character(len=:), allocatable, target :: c_topic
    
    c_topic = trim(topic_name) // c_null_char
    sub%handle = fortran_create_subscriber(node%handle, c_topic)
  end subroutine
  
  ! Create string publisher
  subroutine create_string_publisher(node, pub, topic_name)
    type(ros2_node), intent(in) :: node
    type(ros2_publisher), intent(out) :: pub
    character(len=*), intent(in) :: topic_name
    character(len=:), allocatable, target :: c_topic
    
    c_topic = trim(topic_name) // c_null_char
    pub%handle = fortran_create_string_publisher(node%handle, c_topic)
  end subroutine
  
  ! Create string subscriber
  subroutine create_string_subscriber(node, sub, topic_name)
    type(ros2_node), intent(in) :: node
    type(ros2_subscriber), intent(out) :: sub
    character(len=*), intent(in) :: topic_name
    character(len=:), allocatable, target :: c_topic
    
    c_topic = trim(topic_name) // c_null_char
    sub%handle = fortran_create_string_subscriber(node%handle, c_topic)
  end subroutine
  
  ! Publish data
  subroutine publish(pub, data)
    type(ros2_publisher), intent(in) :: pub
    real(8), intent(in) :: data(:)
    
    call fortran_publish(pub%handle, data, size(data))
  end subroutine
  
  ! Publish string
  subroutine publish_string(pub, message)
    type(ros2_publisher), intent(in) :: pub
    character(len=*), intent(in) :: message
    character(len=:), allocatable, target :: c_str
    
    c_str = trim(message) // c_null_char
    call fortran_publish_string(pub%handle, c_str)
  end subroutine
  
  ! Subscribe (non-blocking)
  function subscribe(sub, data) result(success)
    type(ros2_subscriber), intent(in) :: sub
    real(8), allocatable, intent(out) :: data(:)
    logical :: success
    real(8) :: buffer(1024)  ! Max size buffer
    integer :: received_size
    
    received_size = fortran_take_message(sub%handle, buffer, 1024)
    
    if (received_size > 0) then
      allocate(data(received_size))
      data = buffer(1:received_size)
      success = .true.
    else
      success = .false.
    end if
  end function
  
  ! Subscribe to string
  function subscribe_string(sub, message) result(success)
    type(ros2_subscriber), intent(in) :: sub
    character(len=:), allocatable, intent(out) :: message
    logical :: success
    character(c_char) :: buffer(1024)
    integer :: received_size
    integer :: i
    
    received_size = fortran_take_string(sub%handle, buffer, 1024)
    
    if (received_size > 0) then
      allocate(character(len=received_size) :: message)
      do i = 1, received_size
        message(i:i) = buffer(i)
      end do
      success = .true.
    else
      success = .false.
    end if
  end function
  
  ! Spin once
  subroutine spin_once(node, timeout_ms)
    type(ros2_node), intent(in) :: node
    integer, optional, intent(in) :: timeout_ms
    integer :: timeout
    
    timeout = 10  ! Default 10ms
    if (present(timeout_ms)) timeout = timeout_ms
    
    call fortran_spin_once(node%handle, timeout)
  end subroutine
  
  ! Shutdown
  subroutine shutdown(node)
    type(ros2_node), intent(inout) :: node
    
    if (c_associated(node%handle)) then
      call fortran_shutdown(node%handle)
      node%handle = c_null_ptr
    end if
  end subroutine
  
end module
