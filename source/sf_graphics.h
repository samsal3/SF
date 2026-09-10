#ifndef SF_GRAPHICS_RENDERER_H
#define SF_GRAPHICS_RENDERER_H

#include "sf_core.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define SF_GRAPHICS_MAX_GARBAGE_ITEM_COUNT 64
#define SF_GRAPHICS_MAX_FRAMES_IN_FLIGHT_COUNT 3
#define SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT 3
#define SF_GRAPHICS_MAX_TEXTURE_COUNT 256
#define SF_GRAPHICS_MAX_RENDER_TARGET_ATTACHMENT_COUNT 4
#define SF_GRAPHICS_MAX_DESCRIPTOR_ENTRY_COUNT 4
#define SF_GRAPHICS_MAX_DESCRIPTOR_SET_DESCRIPTOR_COUNT 4
#define SF_GRAPHICS_MAX_VERTEX_LAYOUT_ATTRIBUTE_COUNT 4
#define SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT 16

struct sf_graphics_renderer;

typedef uintptr_t sf_handle;
#define SF_NULL_HANDLE 0

struct sf_graphics_resource_base {
	sf_bool is_occupied;
};

enum sf_graphics_format {
	SF_GRAPHICS_FORMAT_UNDEFINED,
	// 1 channel
	SF_GRAPHICS_FORMAT_R8_UNORM,
	SF_GRAPHICS_FORMAT_R16_UNORM,
	SF_GRAPHICS_FORMAT_R16_UINT,
	SF_GRAPHICS_FORMAT_R16_SFLOAT,
	SF_GRAPHICS_FORMAT_R32_UINT,
	SF_GRAPHICS_FORMAT_R32_SFLOAT,
	// 2 channel
	SF_GRAPHICS_FORMAT_R8G8_UNORM,
	SF_GRAPHICS_FORMAT_R16G16_UNORM,
	SF_GRAPHICS_FORMAT_R16G16_SFLOAT,
	SF_GRAPHICS_FORMAT_R32G32_UINT,
	SF_GRAPHICS_FORMAT_R32G32_SFLOAT,
	// 3 channel
	SF_GRAPHICS_FORMAT_R8G8B8_UNORM,
	SF_GRAPHICS_FORMAT_R16G16B16_UNORM,
	SF_GRAPHICS_FORMAT_R16G16B16_SFLOAT,
	SF_GRAPHICS_FORMAT_R32G32B32_UINT,
	SF_GRAPHICS_FORMAT_R32G32B32_SFLOAT,
	// 4 channel
	SF_GRAPHICS_FORMAT_B8G8R8A8_UNORM,
	SF_GRAPHICS_FORMAT_B8G8R8A8_SRGB,
	SF_GRAPHICS_FORMAT_R8G8B8A8_UNORM,
	SF_GRAPHICS_FORMAT_R16G16B16A16_UNORM,
	SF_GRAPHICS_FORMAT_R16G16B16A16_SFLOAT,
	SF_GRAPHICS_FORMAT_R32G32B32A32_UINT,
	SF_GRAPHICS_FORMAT_R32G32B32A32_SFLOAT,
	// Depth/stencil
	SF_GRAPHICS_FORMAT_D16_UNORM,
	SF_GRAPHICS_FORMAT_X8_D24_UNORM_PACK32,
	SF_GRAPHICS_FORMAT_D32_SFLOAT,
	SF_GRAPHICS_FORMAT_S8_UINT,
	SF_GRAPHICS_FORMAT_D16_UNORM_S8_UINT,
	SF_GRAPHICS_FORMAT_D24_UNORM_S8_UINT,
	SF_GRAPHICS_FORMAT_D32_SFLOAT_S8_UINT
};

enum sf_graphics_clear_value_type {
	SF_GRAPHICS_CLEAR_VALUE_TYPE_NONE,
	SF_GRAPHICS_CLEAR_VALUE_TYPE_COLOR,
	SF_GRAPHICS_CLEAR_VALUE_TYPE_DEPTH_STENCIL
};

enum sf_graphics_descriptor_type {
	SF_GRAPHICS_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	SF_GRAPHICS_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	SF_GRAPHICS_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
	SF_GRAPHICS_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
	SF_GRAPHICS_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
	SF_GRAPHICS_DESCRIPTOR_TYPE_TEXTURE,
	SF_GRAPHICS_DESCRIPTOR_TYPE_SAMPLER
};

enum sf_graphics_sample_count {
	SF_GRAPHICS_SAMPLE_COUNT_1,
	SF_GRAPHICS_SAMPLE_COUNT_2,
	SF_GRAPHICS_SAMPLE_COUNT_4,
	SF_GRAPHICS_SAMPLE_COUNT_8,
	SF_GRAPHICS_SAMPLE_COUNT_16
};

enum sf_graphics_image_type {
	SF_GRAPHICS_IMAGE_TYPE_1D,
	SF_GRAPHICS_IMAGE_TYPE_2D,
	SF_GRAPHICS_IMAGE_TYPE_3D,
	SF_GRAPHICS_IMAGE_TYPE_CUBE
};

enum sf_graphics_image_usage {
	SF_GRAPHICS_IMAGE_USAGE_UNDEFINED = 0X00000000,
	SF_GRAPHICS_IMAGE_USAGE_TRANSFER_SOURCE = 0X00000001,
	SF_GRAPHICS_IMAGE_USAGE_TRANSFER_DESTINATION = 0X00000002,
	SF_GRAPHICS_IMAGE_USAGE_SAMPLED = 0X00000004,
	SF_GRAPHICS_IMAGE_USAGE_STORAGE = 0X00000008,
	SF_GRAPHICS_IMAGE_USAGE_COLOR_ATTACHMENT = 0X00000010,
	SF_GRAPHICS_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0X00000020,
	SF_GRAPHICS_IMAGE_USAGE_RESOLVE_SOURCE = 0X00000040,
	SF_GRAPHICS_IMAGE_USAGE_RESOLVE_DESTINATION = 0X00000080,
	SF_GRAPHICS_IMAGE_USAGE_PRESENT = 0X00000100
};
typedef u32 sf_graphics_image_usage_flags;

enum sf_graphics_buffer_usage {
	SF_GRAPHICS_BUFFER_USAGE_TRANSFER_SOURCE = 0x00000001,
	SF_GRAPHICS_BUFFER_USAGE_TRANSFER_DESTINATION = 0x00000002,
	SF_GRAPHICS_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER = 0x00000004,
	SF_GRAPHICS_BUFFER_USAGE_STORAGE_TEXEL_BUFFER = 0x00000008,
	SF_GRAPHICS_BUFFER_USAGE_UNIFORM_BUFFER = 0x00000010,
	SF_GRAPHICS_BUFFER_USAGE_STORAGE_BUFFER = 0x00000020,
	SF_GRAPHICS_BUFFER_USAGE_INDEX_BUFFER = 0x00000040,
	SF_GRAPHICS_BUFFER_USAGE_VERTEX_BUFFER = 0x00000080,
	SF_GRAPHICS_BUFFER_USAGE_INDIRECT_BUFFER = 0x00000100,
	SF_GRAPHICS_BUFFER_USAGE_SHADER_DEVICE_ADDRESS = 0x00000200
};
typedef u32 sf_graphics_buffer_usage_flags;

enum sf_graphics_memory_property {
	SF_GRAPHICS_MEMORY_PROPERTY_DEVICE_LOCAL = 0x00000001,
	SF_GRAPHICS_MEMORY_PROPERTY_CPU_VISIBLE = 0x00000002,
	SF_GRAPHICS_MEMORY_PROPERTY_LAZILY_ALLOCATED = 0x00000004,
	SF_GRAPHICS_MEMORY_PROPERTY_PROTECTED = 0x0000008,
};
typedef u32 sf_graphics_memory_property_flags;

enum sf_graphics_command_buffer_usage {
	SF_GRAPHICS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT = 0x0000001,
};
typedef u32 sf_graphics_command_buffer_usage_flags;

struct sf_graphics_glfw_platform {
	GLFWwindow *window;
	i32 window_width;
	i32 window_height;
};

struct sf_graphics_clear_value_rgba {
	float r;
	float g;
	float b;
	float a;
};

struct sf_graphics_clear_value_depth_stencil {
	float depth;
	u32 stencil;
};

union sf_graphics_clear_value_data {
	struct sf_graphics_clear_value_rgba rgba;
	struct sf_graphics_clear_value_depth_stencil depth_stencil;
};

struct sf_graphics_clear_value {
	enum sf_graphics_clear_value_type type;
	union sf_graphics_clear_value_data data;
};

struct sf_graphics_descriptor {
	enum sf_graphics_descriptor_type type;
	u32 binding;
	u32 slot;
	u32 entry_count;
	sf_handle entries[SF_GRAPHICS_MAX_DESCRIPTOR_ENTRY_COUNT];
};

struct sf_graphics_descriptor_set_layout {
	struct sf_graphics_resource_base base;
	VkDescriptorSetLayout vk_layout;
	u32 descriptor_count;
	struct sf_graphics_descriptor descriptors[SF_GRAPHICS_MAX_DESCRIPTOR_SET_DESCRIPTOR_COUNT];
};

struct sf_graphics_descriptor_set {
	struct sf_graphics_resource_base base;
	sf_handle descriptor_set_layout;
	VkDescriptorPool vk_descriptor_pool;
	VkDescriptorSet vk_descriptor_set;
};

struct sf_graphics_vertex_attribute {
	enum sf_graphics_format format;
	u32 binding; // FIXME(samuel): currently ignored. Always 0
	u32 location;
	u32 offset;
};

struct sf_graphics_vertex_layout {
	u32 stride;
	u32 attribute_count;
	struct sf_graphics_vertex_attribute attributes[SF_GRAPHICS_MAX_VERTEX_LAYOUT_ATTRIBUTE_COUNT];
};

struct sf_graphics_pipeline {
	struct sf_graphics_resource_base base;
	VkShaderModule vk_vertex_shader;
	VkShaderModule vk_fragment_shader;
	VkPipelineLayout vk_pipeline_layout;
	VkPipeline vk_pipeline;
};

struct sf_graphics_image {
	struct sf_graphics_resource_base base;
	VkImage vk_image;
	VkDeviceMemory vk_memory;
	VkImageView vk_image_view;
	sf_bool vk_owns_image;
};

struct sf_graphics_buffer {
	struct sf_graphics_resource_base base;
	VkBuffer vk_buffer;
	VkDeviceMemory vk_memory;
	u32 size;
	void *cpu_mapped_data;
	VkDeviceAddress vk_address;
};

struct sf_graphics_command_buffer {
	struct sf_graphics_resource_base base;
	enum sf_graphics_command_buffer_usage_flags usage;
	sf_bool is_recording;
	sf_bool is_executable;
	VkCommandPool vk_command_pool;
	VkCommandBuffer vk_command_buffer;
};

struct sf_graphics_render_target {
	struct sf_graphics_resource_base base;

	u32 width;
	u32 height;

	enum sf_graphics_sample_count samples;
	enum sf_graphics_format color_format;
	enum sf_graphics_format depth_format;

	u32 color_attachment_count;
	sf_handle color_attachments[SF_GRAPHICS_MAX_RENDER_TARGET_ATTACHMENT_COUNT];

	sf_handle depth_stencil_attachment;
	
	u32 imgui_attachment_count;
	sf_handle imgui_attachments[SF_GRAPHICS_MAX_RENDER_TARGET_ATTACHMENT_COUNT];
};

struct sf_graphics_sampler {
	struct sf_graphics_resource_base base;
	VkSampler vk_sampler;
};

typedef void (*sf_graphics_renderer_callback)(void *data, struct sf_graphics_renderer *renderer);

struct sf_graphics_renderer_info {
	u32 width;
	u32 height;

	sf_bool request_enable_vsync;

	struct sf_string application_name;

	sf_bool vk_request_enable_validation_layers;

	u32 vk_instance_extension_count;
	char const **vk_instance_extensions;

	u32 vk_instance_layer_count;
	char const **vk_instance_layers;

	u32 vk_device_extension_count;
	char const **vk_device_extensions;

	void *plataform_data;
	sf_graphics_renderer_callback plataform_vulkan_surface_init;
	sf_graphics_renderer_callback plataform_request_swapchain_dimensions;
};

struct sf_graphics_renderer {
	struct sf_arena arena;

	void *plataform_data;
	sf_graphics_renderer_callback plataform_create_vulkan_surface;
	sf_graphics_renderer_callback plataform_request_swapchain_dimensions;

	sf_bool requested_enable_vsync;
	sf_bool requested_validation_layers;

	PFN_vkCreateDebugUtilsMessengerEXT vk_create_debug_utils_messenger_ext;
	PFN_vkDestroyDebugUtilsMessengerEXT vk_destroy_debug_utils_messenger_ext;

	VkInstance vk_instance;
	VkDebugUtilsMessengerEXT vk_validation_messenger;
	VkAllocationCallbacks *vk_allocation_callbacks;

	VkPhysicalDevice vk_physical_device;
	VkSurfaceKHR vk_surface;
	VkDevice vk_device;
	VkQueue vk_graphics_queue;
	VkQueue vk_present_queue;
	u32 vk_graphics_queue_family_index;
	u32 vk_present_queue_family_index;

	VkSurfaceCapabilities2KHR vk_surface_capabilities;

	struct sf_arena swapchain_arena;

	u32 vk_swapchain_min_image_count;
	u32 vk_swapchain_max_image_count;
	u32 vk_swapchain_requested_image_count;
	u32 vk_swapchain_requested_frames;
	VkFormat vk_swapchain_depth_stencil_format;
	VkFormat vk_swapchain_color_format;
	VkColorSpaceKHR vk_swapchain_color_space;
	sf_bool vk_swapchain_enable_vsync;
	VkPresentModeKHR vk_swapchain_present_mode;
	VkSampleCountFlagBits vk_swapchain_samples;
	u32 vk_swapchain_width;
	u32 vk_swapchain_height;
	VkSwapchainKHR vk_swapchain;

	u32 vk_swapchain_image_count;
	VkImage vk_swapchain_images[SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT];

	u32 vk_swapchain_image_view_count;
	VkImageView vk_swapchain_image_views[SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT];

	u32 vk_swapchain_draw_complete_semaphore_count;
	VkSemaphore vk_swapchain_draw_complete_semaphores[SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT];

	u32 vk_swapchain_acquire_semaphore_count;
	VkSemaphore vk_swapchain_acquire_semaphores[SF_GRAPHICS_MAX_FRAMES_IN_FLIGHT_COUNT];

	u32 vk_swapchain_current_image_index;
	u32 vk_swapchain_current_frame_index;

	sf_bool vk_swapchain_requires_rebuild;

	struct sf_graphics_image image_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_buffer buffer_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_command_buffer command_buffer_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_sampler sampler_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_render_target render_target_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];

	sf_handle vertex_buffer;
	sf_handle index_buffer;
	sf_handle scene_buffer;

	sf_handle default_sampler;

	sf_handle images[2];

	sf_handle imgui_attachment;

	VkShaderEXT vk_vertex_shader;
	VkShaderEXT vk_fragment_shader_textured;
	VkShaderEXT vk_fragment_shader_not_textured;

	VkDescriptorPool imgui_descriptor_pool;

	sf_handle sampler_buffer;
	sf_handle resource_heap_buffer;

	u64 sampler_heap_size;
	u64 sampler_reserved_offset;
	u64 sampler_reserved_size;

	u64 resource_heap_size;
	u64 resource_reserved_offset;
	u64 resource_reserved_size;
	
};

sf_public struct sf_graphics_renderer *sf_graphics_renderer_init(struct sf_arena *arena, struct sf_graphics_renderer_info *info);
sf_public void sf_graphics_renderer_deinit(struct sf_graphics_renderer *r);


sf_public struct sf_graphics_glfw_platform *sf_graphics_glfw_platform_init(struct sf_arena *arena, i32 width, i32 height, struct sf_string const *title);

sf_public void sf_graphics_glfw_platform_deinit(struct sf_graphics_glfw_platform *platform);

sf_public void sf_graphics_glfw_platform_process_events(struct sf_graphics_glfw_platform *platform);

sf_public sf_bool sf_graphics_glfw_platform_should_close(struct sf_graphics_glfw_platform *platform);

sf_public void sf_graphics_glfw_platform_fill_renderer_info(struct sf_arena *arena, struct sf_graphics_glfw_platform *platform, struct sf_graphics_renderer_info *info);

#endif
