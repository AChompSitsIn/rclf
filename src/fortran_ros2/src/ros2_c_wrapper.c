#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/float64_multi_array.h>
#include <std_msgs/msg/string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

// Node handle
typedef struct {
    rcl_node_t node;
    rcl_context_t context;
    rcl_init_options_t init_options;
    rcl_allocator_t allocator;
} fortran_node_t;

// Publisher handle
typedef struct {
    rcl_publisher_t publisher;
    std_msgs__msg__Float64MultiArray float_msg;
    std_msgs__msg__String string_msg;
    int msg_type; // 1=float64array, 2=string
} fortran_publisher_t;

// Subscriber handle
typedef struct {
    rcl_subscription_t subscription;
    std_msgs__msg__Float64MultiArray float_msg;
    std_msgs__msg__String string_msg;
    int msg_type;
} fortran_subscriber_t;

// Initialize ROS2
fortran_node_t* fortran_ros2_init(const char* node_name) {
    fortran_node_t* f_node = (fortran_node_t*)malloc(sizeof(fortran_node_t));
    
    // Get default allocator
    f_node->allocator = rcl_get_default_allocator();
    
    // Initialize init_options
    f_node->init_options = rcl_get_zero_initialized_init_options();
    rcl_ret_t ret = rcl_init_options_init(&f_node->init_options, f_node->allocator);
    if (ret != RCL_RET_OK) {
        printf("Failed to initialize init options\n");
        free(f_node);
        return NULL;
    }
    
    // Initialize context
    f_node->context = rcl_get_zero_initialized_context();
    ret = rcl_init(0, NULL, &f_node->init_options, &f_node->context);
    if (ret != RCL_RET_OK) {
        printf("Failed to initialize RCL\n");
        free(f_node);
        return NULL;
    }
    
    // Create node
    f_node->node = rcl_get_zero_initialized_node();
    rcl_node_options_t node_options = rcl_node_get_default_options();
    
    ret = rcl_node_init(&f_node->node, node_name, "", &f_node->context, &node_options);
    if (ret != RCL_RET_OK) {
        printf("Failed to create node\n");
        rcl_shutdown(&f_node->context);
        free(f_node);
        return NULL;
    }
    
    printf("ROS2 node '%s' initialized\n", node_name);
    return f_node;
}

// Create string publisher
fortran_publisher_t* fortran_create_string_publisher(fortran_node_t* node, const char* topic_name) {
    fortran_publisher_t* f_pub = (fortran_publisher_t*)malloc(sizeof(fortran_publisher_t));
    f_pub->msg_type = 2; // String
    
    const rosidl_message_type_support_t* type_support = 
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);
    
    // Regular publisher
    f_pub->publisher = rcl_get_zero_initialized_publisher();
    rcl_publisher_options_t pub_options = rcl_publisher_get_default_options();
    rcl_ret_t ret = rcl_publisher_init(&f_pub->publisher, &node->node, 
                                      type_support, topic_name, &pub_options);
    
    if (ret != RCL_RET_OK) {
        printf("Failed to create string publisher\n");
        free(f_pub);
        return NULL;
    }
    
    // Initialize message
    std_msgs__msg__String__init(&f_pub->string_msg);
    
    printf("String publisher created on topic '%s'\n", topic_name);
    return f_pub;
}

// Publish string
void fortran_publish_string(fortran_publisher_t* pub, const char* str) {
    if (pub->msg_type != 2) return;
    
    // Set string data
    pub->string_msg.data.data = (char*)str;
    pub->string_msg.data.size = strlen(str);
    pub->string_msg.data.capacity = strlen(str) + 1;
    
    // Publish
    rcl_ret_t ret = rcl_publish(&pub->publisher, &pub->string_msg, NULL);
    
    if (ret != RCL_RET_OK) {
        printf("Failed to publish string message\n");
    }
}

// Create string subscriber
fortran_subscriber_t* fortran_create_string_subscriber(fortran_node_t* node, const char* topic_name) {
    fortran_subscriber_t* f_sub = (fortran_subscriber_t*)malloc(sizeof(fortran_subscriber_t));
    f_sub->msg_type = 2; // String
    
    f_sub->subscription = rcl_get_zero_initialized_subscription();
    const rosidl_message_type_support_t* type_support = 
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String);
    
    rcl_subscription_options_t sub_options = rcl_subscription_get_default_options();
    
    rcl_ret_t ret = rcl_subscription_init(&f_sub->subscription, &node->node, 
                                          type_support, topic_name, &sub_options);
    
    if (ret != RCL_RET_OK) {
        printf("Failed to create string subscriber\n");
        free(f_sub);
        return NULL;
    }
    
    std_msgs__msg__String__init(&f_sub->string_msg);
    
    printf("String subscriber created on topic '%s'\n", topic_name);
    return f_sub;
}

// Take string message
int fortran_take_string(fortran_subscriber_t* sub, char* buffer, int buffer_size) {
    if (sub->msg_type != 2) return 0;
    
    rmw_message_info_t message_info;
    rcl_ret_t ret = rcl_take(&sub->subscription, &sub->string_msg, &message_info, NULL);
    
    if (ret == RCL_RET_OK) {
        int copy_size = (sub->string_msg.data.size < buffer_size - 1) ? 
                        sub->string_msg.data.size : buffer_size - 1;
        strncpy(buffer, sub->string_msg.data.data, copy_size);
        buffer[copy_size] = '\0';
        return copy_size;
    }
    return 0;
}

// Create publisher for float64 array
fortran_publisher_t* fortran_create_publisher(fortran_node_t* node, const char* topic_name) {
    fortran_publisher_t* f_pub = (fortran_publisher_t*)malloc(sizeof(fortran_publisher_t));
    f_pub->msg_type = 1; // Float64MultiArray
    
    // Initialize publisher
    f_pub->publisher = rcl_get_zero_initialized_publisher();
    const rosidl_message_type_support_t* type_support = 
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64MultiArray);
    
    rcl_publisher_options_t pub_options = rcl_publisher_get_default_options();
    rcl_ret_t ret = rcl_publisher_init(&f_pub->publisher, &node->node, 
                                       type_support, topic_name, &pub_options);
    
    if (ret != RCL_RET_OK) {
        printf("Failed to create publisher\n");
        free(f_pub);
        return NULL;
    }
    
    // Initialize message
    std_msgs__msg__Float64MultiArray__init(&f_pub->float_msg);
    
    printf("Publisher created on topic '%s'\n", topic_name);
    return f_pub;
}

// Publish float64 array
void fortran_publish(fortran_publisher_t* pub, double* data, int size) {
    if (pub->msg_type == 1) {
        // Resize array if needed
        if (pub->float_msg.data.capacity < (size_t)size) {
            pub->float_msg.data.data = (double*)realloc(pub->float_msg.data.data, 
                                                        size * sizeof(double));
            pub->float_msg.data.capacity = size;
        }
        
        // Copy data
        memcpy(pub->float_msg.data.data, data, size * sizeof(double));
        pub->float_msg.data.size = size;
        
        // Publish
        rcl_ret_t ret = rcl_publish(&pub->publisher, &pub->float_msg, NULL);
        if (ret != RCL_RET_OK) {
            printf("Failed to publish message\n");
        }
    }
}

// Create subscriber
fortran_subscriber_t* fortran_create_subscriber(fortran_node_t* node, const char* topic_name) {
    fortran_subscriber_t* f_sub = (fortran_subscriber_t*)malloc(sizeof(fortran_subscriber_t));
    f_sub->msg_type = 1; // Float64MultiArray
    
    f_sub->subscription = rcl_get_zero_initialized_subscription();
    const rosidl_message_type_support_t* type_support = 
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64MultiArray);
    
    rcl_subscription_options_t sub_options = rcl_subscription_get_default_options();
    rcl_ret_t ret = rcl_subscription_init(&f_sub->subscription, &node->node, 
                                          type_support, topic_name, &sub_options);
    
    if (ret != RCL_RET_OK) {
        printf("Failed to create subscriber\n");
        free(f_sub);
        return NULL;
    }
    
    std_msgs__msg__Float64MultiArray__init(&f_sub->float_msg);
    
    printf("Subscriber created on topic '%s'\n", topic_name);
    return f_sub;
}

// Check for new message (non-blocking)
int fortran_take_message(fortran_subscriber_t* sub, double* buffer, int buffer_size) {
    rmw_message_info_t message_info;
    rcl_ret_t ret = rcl_take(&sub->subscription, &sub->float_msg, &message_info, NULL);
    
    if (ret == RCL_RET_OK && sub->msg_type == 1) {
        int copy_size = (sub->float_msg.data.size < buffer_size) ? 
                        sub->float_msg.data.size : buffer_size;
        memcpy(buffer, sub->float_msg.data.data, copy_size * sizeof(double));
        return copy_size;
    }
    return 0;
}

// Simple spin implementation
void fortran_spin_once(fortran_node_t* node, int timeout_ms) {
    usleep(timeout_ms * 1000);
}

// Cleanup
void fortran_shutdown(fortran_node_t* node) {
    rcl_node_fini(&node->node);
    rcl_shutdown(&node->context);
    rcl_context_fini(&node->context);
    free(node);
    printf("ROS2 node shutdown\n");
}
