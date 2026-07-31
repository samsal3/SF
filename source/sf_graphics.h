#ifndef SF_GRAPHICS_RENDERER_H
#define SF_GRAPHICS_RENDERER_H

#include "sf_core.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

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

struct sf_graphics_glfw_platform {
	GLFWwindow *window;
	i32	    window_width;
	i32	    window_height;
};

struct sf_graphics_clear_value_rgba {
	float r;
	float g;
	float b;
	float a;
};

struct sf_graphics_clear_value_depth_stencil {
	float depth;
	u32   stencil;
};

enum sf_graphics_clear_value_type {
	SF_GRAPHICS_CLEAR_VALUE_TYPE_NONE,
	SF_GRAPHICS_CLEAR_VALUE_TYPE_COLOR,
	SF_GRAPHICS_CLEAR_VALUE_TYPE_DEPTH_STENCIL
};

union sf_graphics_clear_value_data {
	struct sf_graphics_clear_value_rgba	     rgba;
	struct sf_graphics_clear_value_depth_stencil detph_stencil;
};

struct sf_graphics_clear_value {
	enum sf_graphics_clear_value_type  type;
	union sf_graphics_clear_value_data data;
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

struct sf_graphics_descriptor {
	enum sf_graphics_descriptor_type type;
	u32				 binding;
	u32				 slot;
	u32				 entry_count;
	sf_handle			 entries[SF_GRAPHICS_MAX_DESCRIPTOR_ENTRY_COUNT];
};

struct sf_graphics_descriptor_set_layout {
	sf_bool			      is_occupied;
	VkDescriptorSetLayout	      vk_layout;
	u32			      descriptor_count;
	struct sf_graphics_descriptor descriptors[SF_GRAPHICS_MAX_DESCRIPTOR_SET_DESCRIPTOR_COUNT];
};

struct sf_graphics_descriptor_set {
	sf_bool		 is_occupied;
	sf_handle	 descriptor_set_layout;
	VkDescriptorPool vk_descriptor_pool;
	VkDescriptorSet	 vk_descriptor_set;
};

struct sf_graphics_vertex_attribute {
	enum sf_graphics_format format;
	u32			binding; // FIXME(samuel): currently ignored. Always 0
	u32			location;
	u32			offset;
};

struct sf_graphics_vertex_layout {
	u64				    stride;
	u32				    attribute_count;
	struct sf_graphics_vertex_attribute attributes[SF_GRAPHICS_MAX_VERTEX_LAYOUT_ATTRIBUTE_COUNT];
};

struct sf_graphics_pipeline {
	sf_bool		 is_occupied;
	VkShaderModule	 vk_vertex_shader;
	VkShaderModule	 vk_fragment_shader;
	VkPipelineLayout vk_pipeline_layout;
	VkPipeline	 vk_pipeline;
};

enum sf_graphics_sample_count {
	SF_GRAPHICS_SAMPLE_COUNT_1,
	SF_GRAPHICS_SAMPLE_COUNT_2,
	SF_GRAPHICS_SAMPLE_COUNT_4,
	SF_GRAPHICS_SAMPLE_COUNT_8,
	SF_GRAPHICS_SAMPLE_COUNT_16
};

enum sf_graphics_texture_type {
	SF_GRAPHICS_TEXTURE_TYPE_1D,
	SF_GRAPHICS_TEXTURE_TYPE_2D,
	SF_GRAPHICS_TEXTURE_TYPE_3D,
	SF_GRAPHICS_TEXTURE_TYPE_CUBE
};

enum sf_graphics_texture_usage {
	SF_GRAPHICS_TEXTURE_USAGE_UNDEFINED		   = 0X00000000,
	SF_GRAPHICS_TEXTURE_USAGE_TRANSFER_SRC		   = 0X00000001,
	SF_GRAPHICS_TEXTURE_USAGE_TRANSFER_DST		   = 0X00000002,
	SF_GRAPHICS_TEXTURE_USAGE_SAMPLED		   = 0X00000004,
	SF_GRAPHICS_TEXTURE_USAGE_STORAGE		   = 0X00000008,
	SF_GRAPHICS_TEXTURE_USAGE_COLOR_ATTACHMENT	   = 0X00000010,
	SF_GRAPHICS_TEXTURE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0X00000020,
	SF_GRAPHICS_TEXTURE_USAGE_RESOLVE_SRC		   = 0X00000040,
	SF_GRAPHICS_TEXTURE_USAGE_RESOLVE_DST		   = 0X00000080,
	SF_GRAPHICS_TEXTURE_USAGE_PRESENT		   = 0X00000100
};
typedef u32 sf_graphics_texture_usage_flags;

struct sf_graphics_texture {
	sf_bool			       is_occupied;
	enum sf_graphics_texture_type  type;
	enum sf_graphics_format	       format;
	enum sf_graphics_sample_count  samples;
	enum sf_graphics_texture_usage usage;
	u32			       width;
	u32			       height;
	u32			       depth;
	u32			       mips;
	sf_bool			       mapped;
	void			      *mapped_data;
	struct sf_graphics_clear_value clear_value;
	sf_bool			       vk_owns_image_and_memory;
	VkImage			       vk_image;
	VkDeviceMemory		       vk_memory;
	VkImageView		       vk_image_view;
};

struct sf_graphics_render_target {
	sf_bool			      is_occupied;
	enum sf_graphics_sample_count samples;
	u32			      width;
	u32			      height;
	u32			      total_attachment_count;
	sf_handle		      depth_stencil_attachment;
	u32			      resolve_attachment_count;
	sf_handle		      resolve_attachments[SF_GRAPHICS_MAX_RENDER_TARGET_ATTACHMENT_COUNT];
	u32			      color_attachment_count;
	sf_handle		      color_attachments[SF_GRAPHICS_MAX_RENDER_TARGET_ATTACHMENT_COUNT];
	VkRenderPass		      vk_render_pass;
	VkFramebuffer		      vk_framebuffer;
};

struct sf_graphics_command_buffer {
	sf_bool		is_occupied;
	VkCommandPool	vk_command_pool;
	VkCommandBuffer vk_command_buffer;
};

typedef void (*sf_graphics_renderer_callback)(void *data, struct sf_graphics_renderer *renderer);

struct sf_graphics_renderer_description {
	void			     *data;
	sf_bool			      enable_vsync;
	u32			      width;
	u32			      height;
	sf_graphics_renderer_callback create_vulkan_surface;
	sf_graphics_renderer_callback request_swapchain_dimensions;
	struct sf_string	      application_name;
	u32			      vk_instance_extension_count;
	char const		    **vk_instance_extensions;
	u32			      vk_instance_layer_count;
	char const		    **vk_instance_layers;
	u32			      vk_device_extension_count;
	char const		    **vk_device_extensions;
};

struct sf_graphics_renderer {
	struct sf_arena				 arena;
	VkInstance				 vk_instance;
	void					*platform_data;
	sf_graphics_renderer_callback		 create_vulkan_surface;
	sf_graphics_renderer_callback		 request_swapchain_dimensions;
	VkAllocationCallbacks			*vk_allocation_callbacks;
	PFN_vkCreateDebugUtilsMessengerEXT	 vk_create_debug_utils_messenger_ext;
	PFN_vkDestroyDebugUtilsMessengerEXT	 vk_destroy_debug_utils_messenger_ext;
	VkDebugUtilsMessengerEXT		 vk_validation_messenger;
	VkFormat				 vk_depth_stencil_format;
	sf_bool					 enable_vsync;
	VkPresentModeKHR			 vk_present_mode;
	VkSurfaceFormatKHR			 vk_surface_format;
	VkSurfaceKHR				 vk_surface;
	VkSampleCountFlagBits			 vk_samples;
	VkPhysicalDevice			 vk_physical_device;
	VkDevice				 vk_device;
	VkQueue					 vk_graphics_queue;
	VkQueue					 vk_present_queue;
	u32					 vk_present_queue_family_index;
	u32					 vk_graphics_queue_family_index;
	VkSwapchainKHR				 vk_swapchain;
	u32					 vk_swapchain_image_count;
	VkImage					 vk_swapchain_images[SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT];
	u32					 vk_draw_complete_semaphore_count;
	VkSemaphore				 vk_draw_complete_semaphores[SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT];
	enum sf_graphics_sample_count		 swapchain_sample_count;
	enum sf_graphics_format			 swapchain_color_format;
	enum sf_graphics_format			 swapchain_depth_stencil_format;
	struct sf_graphics_clear_value		 swapchain_color_clear_value;
	struct sf_graphics_clear_value		 swapchain_depth_stencil_clear_value;
	u32					 swapchain_requested_image_count;
	sf_bool					 swapchain_skip_end_frame;
	u32					 swapchain_width;
	u32					 swapchain_height;
	u32					 swapchain_render_target_count;
	sf_handle				 swapchain_render_targets[SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT];
	u32					 vk_in_flight_fence_count;
	VkFence					 vk_in_flight_fences[SF_GRAPHICS_MAX_FRAMES_IN_FLIGHT_COUNT];
	u32					 vk_image_acquired_semaphore_count;
	VkSemaphore				 vk_image_acquired_semaphores[SF_GRAPHICS_MAX_FRAMES_IN_FLIGHT_COUNT];
	u32					 current_swapchain_image_index;
	u32					 current_frame_index;
	u32					 main_command_buffer_count;
	sf_handle				 main_command_buffers[SF_GRAPHICS_MAX_FRAMES_IN_FLIGHT_COUNT];
	struct sf_arena				 render_target_arena;
	struct sf_graphics_texture		 texture_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_render_target	 render_target_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_command_buffer	 command_buffer_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_descriptor_set_layout descriptor_set_layout_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_descriptor_set	 descriptor_set_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
	struct sf_graphics_pipeline		 pipeline_pool[SF_GRAPHICS_MAX_RESOURCE_POOL_COUNT];
};

sf_public struct sf_graphics_renderer *
sf_graphics_create_renderer(struct sf_arena *arena, struct sf_graphics_renderer_description *description);

sf_public void
sf_graphics_destroy_renderer(struct sf_graphics_renderer *renderer);

sf_public sf_handle
sf_graphics_create_texture(struct sf_graphics_renderer *renderer, enum sf_graphics_texture_type type, enum sf_graphics_format format, enum sf_graphics_sample_count samples, enum sf_graphics_texture_usage usage, u32 width, u32 height, u32 depth, u32 mips, sf_bool mapped, struct sf_graphics_clear_value *clear_value);

sf_public void
sf_graphics_destroy_texture(struct sf_graphics_renderer *renderer, sf_handle handle);

sf_public sf_handle
sf_graphics_create_command_buffer(struct sf_graphics_renderer *renderer, sf_bool transient);

sf_public void
sf_graphics_destroy_command_buffer(struct sf_graphics_renderer *renderer, sf_handle handle);

sf_public sf_handle
sf_graphics_create_render_target(struct sf_graphics_renderer *renderer, u32 width, u32 height, enum sf_graphics_sample_count samples, enum sf_graphics_format color_format, enum sf_graphics_format depth_stencil_format, u32 color_attachment_count, struct sf_graphics_clear_value *color_clear_values, struct sf_graphics_clear_value *depth_stencil_clear_value);

sf_public void
sf_graphics_destroy_render_target(struct sf_graphics_renderer *renderer, sf_handle handle);

sf_public sf_handle
sf_graphics_create_pipeline(struct sf_graphics_renderer *renderer, sf_bool enable_depth, struct sf_graphics_vertex_layout *vertex_layout, struct sf_graphics_descriptor_set_layout *descriptor_set_layout, struct sf_graphics_render_target *target, u32 vertex_code_size, void const *vertex_code, u32 fragment_code_size, void const *fragment_code);

sf_public void
sf_graphics_destroy_pipeline(struct sf_graphics_renderer *renderer, sf_handle handle);

sf_public sf_handle
sf_graphics_create_descriptor_set(struct sf_graphics_renderer *renderer, u32 descriptor_count, struct sf_graphics_descriptor *descriptors);

sf_public struct sf_graphics_glfw_platform *
sf_graphics_create_glfw_platform(struct sf_arena *arena, i32 width, i32 height, struct sf_string const *title);

sf_public void
sf_graphics_destroy_glfw_platform(struct sf_graphics_glfw_platform *platform);

sf_public void
sf_graphics_glfw_platform_process_events(struct sf_graphics_glfw_platform *platform);

sf_public sf_bool
sf_graphics_glfw_platform_should_close(struct sf_graphics_glfw_platform *platform);

sf_public void
sf_graphics_glfw_platform_fill_renderer_description(struct sf_arena *arena, struct sf_graphics_glfw_platform *platform, struct sf_graphics_renderer_description *description);

#endif
