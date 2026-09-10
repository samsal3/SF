#include "sf_graphics.h"

#include <stdio.h>

#define SF_VULKAN_CHECK(e) sf_graphics_vulkan_check((e), #e, __LINE__, __FILE__)
#define SF_VULKAN_PROC(name, i) (PFN_##name) vkGetInstanceProcAddr(i, #name)

sf_private char const *sf_graphics_string_from_vulkan_result(VkResult vk_result) {
	char const *result = NULL;

	switch (vk_result) {
		case VK_SUCCESS:
			result = "VK_SUCCESS";
			break;
		case VK_NOT_READY:
			result = "VK_NOT_READY";
			break;
		case VK_TIMEOUT:
			result = "VK_TIMEOUT";
			break;
		case VK_EVENT_SET:
			result = "VK_EVENT_SET";
			break;
		case VK_EVENT_RESET:
			result = "VK_EVENT_RESET";
			break;
		case VK_INCOMPLETE:
			result = "VK_INCOMPLETE";
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			result = "VK_ERROR_OUT_OF_HOST_MEMORY";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			result = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			result = "VK_ERROR_INITIALIZATION_FAILED";
			break;
		case VK_ERROR_DEVICE_LOST:
			result = "VK_ERROR_DEVICE_LOST";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			result = "VK_ERROR_MEMORY_MAP_FAILED";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			result = "VK_ERROR_LAYER_NOT_PRESENT";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			result = "VK_ERROR_EXTENSION_NOT_PRESENT";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			result = "VK_ERROR_FEATURE_NOT_PRESENT";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			result = "VK_ERROR_INCOMPATIBLE_DRIVER";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			result = "VK_ERROR_TOO_MANY_OBJECTS";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			result = "VK_ERROR_FORMAT_NOT_SUPPORTED";
			break;
		case VK_ERROR_FRAGMENTED_POOL:
			result = "VK_ERROR_FRAGMENTED_POOL";
			break;
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			result = "VK_ERROR_OUT_OF_POOL_MEMORY";
			break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			result = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			result = "VK_ERROR_SURFACE_LOST_KHR";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			result = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			break;
		case VK_SUBOPTIMAL_KHR:
			result = "VK_SUBOPTIMAL_KHR";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			result = "VK_ERROR_OUT_OF_DATE_KHR";
			break;

		case VK_ERROR_UNKNOWN:
		default:
			result = "VK_ERROR_UNKNOWN";
			break;
	}

	return result;
}

sf_private sf_bool sf_graphics_vulkan_check(VkResult result, char const *what, int line, char const *file) {
	char const *result_string = sf_graphics_string_from_vulkan_result(result);
	fprintf(stderr, "%s - %s - %s:%i\n", result_string, what, file, line);
	return result == VK_SUCCESS;
}

sf_private sf_bool sf_graphics_vulkan_is_extension_available(char const *required_extension, u32 available_extension_count, VkExtensionProperties *available_extensions) {
	u32 i = 0;
	struct sf_string required = {0};
	sf_string_from_non_literal(required_extension, VK_MAX_EXTENSION_NAME_SIZE, &required);

	for (i = 0; i < available_extension_count; ++i) {
		struct sf_string available = {0};
		sf_string_from_non_literal(available_extensions[i].extensionName, VK_MAX_EXTENSION_NAME_SIZE, &available);

		if (sf_string_compare(&required, &available, VK_MAX_EXTENSION_NAME_SIZE))
			return SF_TRUE;
	}
	return SF_FALSE;
}

sf_private sf_bool sf_graphics_vulkan_are_extensions_available(u32 required_extension_count, char const **required_extensions, u32 available_extension_count, VkExtensionProperties*available_extensions) {
	u32 i = 0;

	for (i = 0; i < required_extension_count; ++i) {
		if (!sf_graphics_vulkan_is_extension_available(required_extensions[i], available_extension_count, available_extensions))
			return SF_FALSE;
	}
	return SF_TRUE;
}

struct sf_graphics_vulkan_extension_properties_array {
	u32 size;
	VkExtensionProperties *data;
};

sf_private void sf_graphics_vulkan_load_available_instance_extensions(struct sf_arena *arena, struct sf_graphics_vulkan_extension_properties_array *array) {
	u32 count = 0;

	if (!SF_VULKAN_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &count, NULL)))
		return;

	array->data = sf_arena_allocate(arena, sizeof(*array->data) * count);
	if (!array->data)
		return;

	if (!SF_VULKAN_CHECK(vkEnumerateInstanceExtensionProperties(NULL, &count, array->data)))
		return;

	array->size = count;
}

sf_private void sf_graphics_vulkan_load_available_device_extensions(struct sf_arena *arena, VkPhysicalDevice device, struct sf_graphics_vulkan_extension_properties_array *array) {
	u32 count = 0;

	if (!SF_VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(device, NULL, &count, NULL)))
		return;

	array->data = sf_arena_allocate(arena, sizeof(*array->data) * count);
	if (!array->data)
		return;

	if (!SF_VULKAN_CHECK(vkEnumerateDeviceExtensionProperties(device, NULL, &count, array->data)))
		return;

	array->size = count;
}

sf_private sf_bool sf_graphics_vulkan_are_instance_extensions_available(struct sf_arena *arena, u32 required_extension_count, char const **required_extensions) {
	struct sf_graphics_vulkan_extension_properties_array available_extensions = {0};

	sf_graphics_vulkan_load_available_instance_extensions(arena, &available_extensions);
	if (!available_extensions.size || !available_extensions.data)
		return SF_FALSE;

	return sf_graphics_vulkan_are_extensions_available(required_extension_count, required_extensions, available_extensions.size, available_extensions.data);
}

sf_private sf_bool sf_graphics_vulkan_are_device_extensions_available(struct sf_arena *arena, VkPhysicalDevice device, u32 required_extension_count, char const **required_extensions) {
	u32 i = 0;
	struct sf_graphics_vulkan_extension_properties_array available_extensions = {0};

	sf_graphics_vulkan_load_available_device_extensions(arena, device, &available_extensions);
	if (!available_extensions.size || !available_extensions.data)
		return SF_FALSE;

	for (i = 0; i < available_extensions.size; ++i) {
		printf("Available device extension: %s\n", available_extensions.data[i].extensionName);
	}

	return sf_graphics_vulkan_are_extensions_available(required_extension_count, required_extensions, available_extensions.size, available_extensions.data);
}

static VkBool32 VKAPI_CALL sf_graphics_vulkan_log(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData) {
	(void)messageTypes;
	(void)userData;

	switch (messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		default:
			fprintf(stderr, "%s\n", callbackData->pMessage);
			break;
	}

	return VK_TRUE;
}

sf_private void sf_graphics_vulkan_instance_init(struct sf_arena *arena, struct sf_graphics_renderer *r, struct sf_graphics_renderer_info *info) {
	u32 vk_api_version = 0;
	VkApplicationInfo app_info = {0};
	VkInstanceCreateInfo instance_info = {0};
	struct sf_string null_terminated_app_name = {0};

	if (!arena || !r || !info || r->vk_instance)
		return;

	r->vk_instance = VK_NULL_HANDLE;

	vkEnumerateInstanceVersion(&vk_api_version);
	if (vk_api_version < VK_MAKE_API_VERSION(0, 1, 4, 340))
		return;

	sf_string_null_terminate(arena, &info->application_name, &null_terminated_app_name);
	if (!null_terminated_app_name.data || !null_terminated_app_name.size)
		return;

	if (!sf_graphics_vulkan_are_instance_extensions_available(arena, info->vk_instance_extension_count, info->vk_instance_extensions))
		return;

	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = null_terminated_app_name.data;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "sf";
	app_info.engineVersion = 1;
	app_info.apiVersion = vk_api_version;

	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pNext = NULL;
	instance_info.flags = 0;
	instance_info.pApplicationInfo = &app_info;
	instance_info.enabledLayerCount = info->vk_instance_layer_count;
	instance_info.ppEnabledLayerNames = info->vk_instance_layers;
	instance_info.enabledExtensionCount = info->vk_instance_extension_count;
	instance_info.ppEnabledExtensionNames = info->vk_instance_extensions;

	if (!SF_VULKAN_CHECK(vkCreateInstance(&instance_info, r->vk_allocation_callbacks, &r->vk_instance)))
		r->vk_instance = VK_NULL_HANDLE;
}

sf_private void sf_graphics_vulkan_load_functions(struct sf_graphics_renderer *r) {
	if (!r || !r->vk_instance)
		return;

	r->vk_create_debug_utils_messenger_ext = SF_VULKAN_PROC(vkCreateDebugUtilsMessengerEXT, r->vk_instance);
	r->vk_destroy_debug_utils_messenger_ext = SF_VULKAN_PROC(vkDestroyDebugUtilsMessengerEXT, r->vk_instance);
}

sf_private void sf_graphics_vulkan_validation_messenger_init(struct sf_graphics_renderer *r) {
	VkDebugUtilsMessengerCreateInfoEXT info = {0};

	if (!r || !r->vk_instance || !r->vk_create_debug_utils_messenger_ext || !r->vk_destroy_debug_utils_messenger_ext || r->vk_validation_messenger)
		return;

	r->vk_validation_messenger = VK_NULL_HANDLE;

	sf_graphics_vulkan_load_functions(r);
	if (!r->vk_create_debug_utils_messenger_ext || !r->vk_destroy_debug_utils_messenger_ext)
		return;

	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info.pNext = NULL;
	info.flags = 0;
	info.messageSeverity = 0;
	info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
	info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
	info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
	info.messageSeverity |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	info.messageType = 0;
	info.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT;
	info.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
	info.messageType |= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	info.pfnUserCallback = sf_graphics_vulkan_log;
	info.pUserData = NULL;

	if (!SF_VULKAN_CHECK(r->vk_create_debug_utils_messenger_ext(r->vk_instance, &info, r->vk_allocation_callbacks, &r->vk_validation_messenger)))
		r->vk_validation_messenger = VK_NULL_HANDLE;
}

sf_private sf_bool sf_graphics_are_queue_family_indices_valid(struct sf_graphics_renderer *r) {
	return r->vk_graphics_queue_family_index != (u32)-1 && r->vk_present_queue_family_index != (u32)-1;
}

struct sf_graphics_queue_family_properties_array {
	u32 size;
	VkQueueFamilyProperties *data;
};

sf_private void sf_graphics_load_queue_family_properties(struct sf_arena *arena, VkPhysicalDevice device, struct sf_graphics_queue_family_properties_array *array) {
	if (!arena || !device || !array)
		return;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &array->size, NULL);
	if (!array->size)
		return;

	array->data = sf_arena_allocate(arena, array->size * sizeof(*array->data));
	if (!array->data)
		return;

	vkGetPhysicalDeviceQueueFamilyProperties(device, &array->size, array->data);
}

sf_private void sf_graphics_vulkan_find_suitable_queue_family_indices(struct sf_arena *arena, VkPhysicalDevice device, VkSurfaceKHR surface, u32 *graphics_queue_family_index, u32 *present_queue_family_index) {
	u32 i = 0;
	struct sf_graphics_queue_family_properties_array properties = {0};

	if (!arena || !device || !surface || !graphics_queue_family_index || !present_queue_family_index)
		return;

	*graphics_queue_family_index = (u32)-1;
	*present_queue_family_index = (u32)-1;

	sf_graphics_load_queue_family_properties(arena, device, &properties);

	if (!properties.size || !properties.data)
		return;

	for (i = 0; i < properties.size && (*graphics_queue_family_index == (u32)-1 || *present_queue_family_index == (u32)-1); ++i) {
		VkBool32 supports_surface = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_surface);

		if (supports_surface)
			*present_queue_family_index = i;

		if (properties.data[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			*graphics_queue_family_index = i;
	}
}

struct sf_graphics_vulkan_surface_format_array {
	u32 size;
	VkSurfaceFormat2KHR *data;
};

sf_private void sf_graphics_vulkan_load_surface_formats(struct sf_arena *arena, VkPhysicalDevice device, VkSurfaceKHR surface, struct sf_graphics_vulkan_surface_format_array *array) {
	u32 i = 0;
	u32 count = 0;

	VkPhysicalDeviceSurfaceInfo2KHR surface_info = {0};

	if (!arena || !device || !surface)
		return;

	 surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
	 surface_info.pNext = NULL;
	 surface_info.surface = surface;

	if (!SF_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormats2KHR(device, &surface_info, &count, NULL)))
		return;

	array->data = sf_arena_allocate(arena, count * sizeof(*array->data));
	if (!array->data)
		return;

	for (i = 0; i < count; ++i) {
		array->data[i].sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
		array->data[i].pNext = NULL;
	}

	if (!SF_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceFormats2KHR(device, &surface_info, &count, array->data)))
		return;

	array->size = count;
}

struct sf_graphics_vulkan_present_mode_array {
	u32 size;
	VkPresentModeKHR *data;
};

sf_private void sf_graphics_vulkan_load_present_modes(struct sf_arena *arena, VkPhysicalDevice device, VkSurfaceKHR surface, struct sf_graphics_vulkan_present_mode_array *array) {
	u32 count = 0;

	if (!arena || !device || !surface || !array)
		return;

	if (!SF_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, NULL)))
		return;

	array->data = sf_arena_allocate(arena, count * sizeof(*array->data));
	if (!array->data)
		return;

	if (!SF_VULKAN_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &count, array->data)))
		return;

	array->size = count;
}

sf_private void sf_graphics_vulkan_swapchain_find_present_mode(struct sf_arena *arena, struct sf_graphics_renderer *r) {
	u32 i = 0;
	struct sf_graphics_vulkan_present_mode_array present_modes = {0};
	VkPresentModeKHR requested_present_mode = VK_PRESENT_MODE_FIFO_KHR;

	if (!arena || !r || !r->vk_physical_device || !r->vk_surface)
		return; // NOTE(samuel): Guaranteed to be present;

	r->vk_swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;

	requested_present_mode = r->requested_enable_vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;

	sf_graphics_vulkan_load_present_modes(arena, r->vk_physical_device, r->vk_surface, &present_modes);
	if (!present_modes.data || !present_modes.size)
		return;

	for (i = 0; i < present_modes.size; ++i) {
		if (requested_present_mode == present_modes.data[i]) {
			r->vk_swapchain_present_mode = requested_present_mode;
			break;
		}
	}

	if (r->vk_swapchain_present_mode == VK_PRESENT_MODE_FIFO_KHR)
		r->vk_swapchain_enable_vsync = SF_TRUE;
	else
		r->vk_swapchain_enable_vsync = SF_FALSE;
}

sf_private VkFormat sf_graphics_vulkan_find_format(VkPhysicalDevice device, u32 available_format_count, VkFormat *available_formats, VkImageTiling tiling, VkFormatFeatureFlags2 features) {
	u32 i = 0;

	if (!device || !available_format_count || !available_formats)
		return VK_FORMAT_UNDEFINED;

	for (i = 0; i < available_format_count; ++i) {
		VkFormatProperties2 properties = {0};
		VkFormat format = available_formats[i];

		properties.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;
		properties.pNext = NULL;

		vkGetPhysicalDeviceFormatProperties2(device, format, &properties);

		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.formatProperties.linearTilingFeatures & features) == features)
			return format;

		if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.formatProperties.optimalTilingFeatures & features) == features)
			return format;
	}

	return VK_FORMAT_UNDEFINED;
}

sf_private void sf_graphics_vulkan_find_supported_color_format(struct sf_arena *arena, VkPhysicalDevice device, VkSurfaceKHR surface, VkSurfaceFormat2KHR *format) {
	u32 i = 0;
	VkSurfaceFormat2KHR requested_format = {0};
	VkSurfaceFormat2KHR default_format = {0};
	struct sf_graphics_vulkan_surface_format_array candidates = {0};

	requested_format.sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
	requested_format.pNext = NULL;
	requested_format.surfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
	requested_format.surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	default_format.sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
	default_format.pNext = NULL;
	default_format.surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
	default_format.surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	*format = default_format;

	if (!arena || !device || !surface || !format)
		return;

	sf_graphics_vulkan_load_surface_formats(arena, device, surface, &candidates);
	if (!candidates.size || !candidates.data)
		return;

	for (i = 0; i < candidates.size; ++i) {
		VkSurfaceFormat2KHR *current = &candidates.data[i];

		if (current->surfaceFormat.format == requested_format.surfaceFormat.format && current->surfaceFormat.colorSpace == requested_format.surfaceFormat.colorSpace) {
			*format = *current;
			return;
		}
	}
}

struct sf_graphics_physical_device_array {
	u32 size;
	VkPhysicalDevice *data;
};

sf_private void sf_graphics_vulkan_load_physical_devices(struct sf_arena *arena, VkInstance instance, struct sf_graphics_physical_device_array *array) {
	u32 count = 0;

	if (!arena || !instance || !array)
		return;

	if (!SF_VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &count, NULL)))
		return;

	array->data = sf_arena_allocate(arena, count * sizeof(*array->data));
	if (!array->data)
		return;

	if (!SF_VULKAN_CHECK(vkEnumeratePhysicalDevices(instance, &count, array->data)))
		return;

	array->size = count;
}

sf_private void sf_graphics_vulkan_find_suitable_physical_device(struct sf_arena *arena, struct sf_graphics_renderer *r, struct sf_graphics_renderer_info *info) {
	u32 i = 0;
	VkPhysicalDevice last_candidate = VK_NULL_HANDLE;
	struct sf_graphics_physical_device_array physical_devices = {0};

	if (!arena || !r->vk_instance || !r->vk_surface || !info)
		return;

	r->vk_physical_device = VK_NULL_HANDLE;
	r->vk_graphics_queue_family_index = (u32)-1;
	r->vk_present_queue_family_index = (u32)-1;

	sf_graphics_vulkan_load_physical_devices(arena, r->vk_instance, &physical_devices);
	if (!physical_devices.data || !physical_devices.size)
		return;

	for (i = 0; i < physical_devices.size; ++i) {
		u32 graphics_queue_family_index = (u32)-1;
		u32 present_queue_family_index = (u32)-1;

		VkPhysicalDeviceProperties2 properties2 = {0};
		VkPhysicalDeviceFeatures features = {0};
		VkPhysicalDevice device = physical_devices.data[i];

		properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

		vkGetPhysicalDeviceProperties2(device, &properties2);
		vkGetPhysicalDeviceFeatures(device, &features);

		sf_graphics_vulkan_find_suitable_queue_family_indices(arena, device, r->vk_surface, &graphics_queue_family_index, &present_queue_family_index);
		if (graphics_queue_family_index == (u32)-1 || present_queue_family_index == (u32)-1)
			continue;

		if (!sf_graphics_vulkan_are_device_extensions_available(arena, device, info->vk_device_extension_count, info->vk_device_extensions))
			continue;

		if (properties2.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			r->vk_physical_device = device;
			goto finish_physical_device_setup;
		} else {
			last_candidate = device; // NOTE(samuel): Found a candidate, but keep looking for a discrete GPU.
		}
	}

	r->vk_physical_device = last_candidate;

finish_physical_device_setup:
	if (r->vk_physical_device)
		sf_graphics_vulkan_find_suitable_queue_family_indices(arena, r->vk_physical_device, r->vk_surface, &r->vk_graphics_queue_family_index, &r->vk_present_queue_family_index);
}

sf_private void sf_graphics_vulkan_device_init(struct sf_graphics_renderer *r, struct sf_graphics_renderer_info *info) {
	VkPhysicalDeviceFeatures2 features = {0};
	VkPhysicalDeviceVulkan11Features features_1_1 = {0};
	VkPhysicalDeviceVulkan12Features features_1_2 = {0};
	VkPhysicalDeviceVulkan13Features features_1_3 = {0};
	VkPhysicalDeviceVulkan14Features features_1_4 = {0};

	float priority = 1.0F;
	VkDeviceQueueCreateInfo queue_infos[2] = {0};
	VkDeviceCreateInfo device_info = {0};

	if (!r || !r->vk_physical_device || !info || r->vk_device)
		return;

	r->vk_device = VK_NULL_HANDLE;

	features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features.pNext = &features_1_1;

	features_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	features_1_1.pNext = &features_1_2;

	features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features_1_2.pNext = &features_1_3;

	features_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	features_1_3.pNext = &features_1_4;

	features_1_4.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
	features_1_4.pNext = NULL;

	vkGetPhysicalDeviceFeatures2(r->vk_physical_device, &features);

	if (!features_1_2.timelineSemaphore)
		return;

	if (!features_1_2.bufferDeviceAddress)
		return;

	if (!features_1_3.synchronization2)
		return;

	if (!features_1_3.dynamicRendering)
		return;

	if (!features_1_4.maintenance6)
		return;

	queue_infos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_infos[0].pNext = NULL;
	queue_infos[0].flags = 0;
	queue_infos[0].queueFamilyIndex = r->vk_graphics_queue_family_index;
	queue_infos[0].queueCount = 1;
	queue_infos[0].pQueuePriorities = &priority;

	queue_infos[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queue_infos[1].pNext = NULL;
	queue_infos[1].flags = 0;
	queue_infos[1].queueFamilyIndex = r->vk_present_queue_family_index;
	queue_infos[1].queueCount = 1;
	queue_infos[1].pQueuePriorities = &priority;

	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = &features;
	device_info.flags = 0;
	if (r->vk_graphics_queue_family_index == r->vk_present_queue_family_index)
		device_info.queueCreateInfoCount = 1;
	else
		device_info.queueCreateInfoCount = SF_SIZE(queue_infos);
	device_info.pQueueCreateInfos = queue_infos;
	device_info.enabledLayerCount = 0;	// NOTE(samuel): deprecated
	device_info.ppEnabledLayerNames = NULL; // NOTE(samuel): deprecated
	device_info.enabledExtensionCount = info->vk_device_extension_count;
	device_info.ppEnabledExtensionNames = info->vk_device_extensions;
	device_info.pEnabledFeatures = NULL; // NOTE(samuel): legacy;

	if (!SF_VULKAN_CHECK(vkCreateDevice(r->vk_physical_device, &device_info, r->vk_allocation_callbacks, &r->vk_device)))
		r->vk_device = VK_NULL_HANDLE;
}

sf_private void sf_graphics_vulkan_load_device_queues(struct sf_graphics_renderer *r) {
	vkGetDeviceQueue(r->vk_device, r->vk_graphics_queue_family_index, 0, &r->vk_graphics_queue);
	vkGetDeviceQueue(r->vk_device, r->vk_present_queue_family_index, 0, &r->vk_present_queue);
}

sf_private void sf_graphics_vulkan_swapchain_init(struct sf_graphics_renderer *r) {
	VkSwapchainCreateInfoKHR info = {0};
	u32 queue_family_indices[2] = {r->vk_graphics_queue_family_index, r->vk_present_queue_family_index};

	// NOTE(samuel): No vk_swapchain reuse
	if (!r || !r->vk_device || !r->vk_surface || !r->vk_physical_device || r->vk_swapchain)
		return;

	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.pNext = NULL;
	info.flags = 0;
	info.surface = r->vk_surface;
	info.minImageCount = r->vk_swapchain_requested_image_count;
	info.imageFormat = r->vk_swapchain_color_format;
	info.imageColorSpace = r->vk_swapchain_color_space;
	info.imageExtent.width = r->vk_swapchain_width;
	info.imageExtent.height = r->vk_swapchain_height;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (r->vk_graphics_queue_family_index == r->vk_present_queue_family_index) {
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount = 1;
		info.pQueueFamilyIndices = queue_family_indices;
	} else {
		info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount = SF_SIZE(queue_family_indices);
		info.pQueueFamilyIndices = queue_family_indices;
	}
	info.preTransform = r->vk_surface_capabilities.surfaceCapabilities.currentTransform;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode = r->vk_swapchain_present_mode;
	info.clipped = VK_TRUE;
	info.oldSwapchain = VK_NULL_HANDLE;

	if (!SF_VULKAN_CHECK(vkCreateSwapchainKHR(r->vk_device, &info, r->vk_allocation_callbacks, &r->vk_swapchain)))
		r->vk_swapchain = VK_NULL_HANDLE;
}

sf_private void sf_graphics_vulkan_swapchain_load_images(struct sf_graphics_renderer *r) {
	u32 image_count = 0;

	if (!r || !r->vk_swapchain)
		return;

	if (!SF_VULKAN_CHECK(vkGetSwapchainImagesKHR(r->vk_device, r->vk_swapchain, &image_count, NULL)))
		return;

	if (image_count > SF_SIZE(r->vk_swapchain_images))
		return;

	if (!SF_VULKAN_CHECK(vkGetSwapchainImagesKHR(r->vk_device, r->vk_swapchain, &image_count, r->vk_swapchain_images)))
		return;

	r->vk_swapchain_image_count = image_count;
}

sf_private void sf_graphics_vulkan_swapchain_find_color_format(struct sf_arena *arena, struct sf_graphics_renderer *r) {
	u32 i = 0;
	VkSurfaceFormat2KHR requested_format = {0};
	struct sf_graphics_vulkan_surface_format_array candidates = {0};

	requested_format.sType = VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;
	requested_format.pNext = NULL;
	requested_format.surfaceFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
	requested_format.surfaceFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	if (!arena || !r || !r->vk_physical_device || !r->vk_surface)
		return;

	r->vk_swapchain_color_format = VK_FORMAT_UNDEFINED;
	r->vk_swapchain_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

	sf_graphics_vulkan_load_surface_formats(arena, r->vk_physical_device, r->vk_surface, &candidates);
	if (!candidates.size || !candidates.data)
		return;

	for (i = 0; i < candidates.size; ++i) {
		VkSurfaceFormat2KHR *current = &candidates.data[i];

		if (current->surfaceFormat.format == requested_format.surfaceFormat.format && current->surfaceFormat.colorSpace == requested_format.surfaceFormat.colorSpace) {
			r->vk_swapchain_color_format = current->surfaceFormat.format;
			r->vk_swapchain_color_space = current->surfaceFormat.colorSpace;
			return;
		}
	}
}

sf_private void sf_graphics_vulkan_swapchain_find_depth_stencil_format(struct sf_graphics_renderer *r) {
	VkFormat candidates[] = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM};

	if (!r)
		return;

	r->vk_swapchain_depth_stencil_format = VK_FORMAT_UNDEFINED;

	if (!r->vk_physical_device)
		return;

	r->vk_swapchain_depth_stencil_format = sf_graphics_vulkan_find_format(r->vk_physical_device, SF_SIZE(candidates), candidates, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

sf_private void sf_graphics_vulkan_swapchain_resources_deinit(struct sf_graphics_renderer *r) {
	u32 i = 0;

	if (!r)
		return;

	for (i = 0; i < SF_SIZE(r->vk_swapchain_acquire_semaphores); ++i) {
		VkSemaphore semaphore = r->vk_swapchain_acquire_semaphores[i];

		if (!semaphore)
			continue;

		vkDestroySemaphore(r->vk_device, semaphore, r->vk_allocation_callbacks);
		r->vk_swapchain_acquire_semaphores[i] = VK_NULL_HANDLE;
	}
	r->vk_swapchain_acquire_semaphore_count = 0;

	for (i = 0; i < SF_SIZE(r->vk_swapchain_draw_complete_semaphores); ++i) {
		VkSemaphore semaphore = r->vk_swapchain_draw_complete_semaphores[i];

		if (!semaphore)
			continue;

		vkDestroySemaphore(r->vk_device, semaphore, r->vk_allocation_callbacks);
		r->vk_swapchain_draw_complete_semaphores[i] = VK_NULL_HANDLE;
	}
	r->vk_swapchain_draw_complete_semaphore_count = 0;

	for (i = 0; i < SF_SIZE(r->vk_swapchain_image_views); ++i) {
		VkImageView image_view = r->vk_swapchain_image_views[i];

		if (!image_view)
			continue;

		vkDestroyImageView(r->vk_device, image_view, r->vk_allocation_callbacks);
		r->vk_swapchain_image_views[i] = VK_NULL_HANDLE;
	}
	r->vk_swapchain_image_view_count = 0;

	SF_ARRAY_INIT(r->vk_swapchain_images, VK_NULL_HANDLE);
	r->vk_swapchain_image_view_count = 0;

	r->vk_swapchain_image_count = 0;

	if (r->vk_device) {
		if (r->vk_swapchain) {
			vkDestroySwapchainKHR(r->vk_device, r->vk_swapchain, r->vk_allocation_callbacks);
			r->vk_swapchain = VK_NULL_HANDLE;
		}
	}
}

sf_private void sf_graphics_vulkan_swapchain_find_surface_capabilities(struct sf_graphics_renderer *r) {
	u32 min_image_count = 0;
	u32 max_image_count = 0;
	u32 req_image_count = 3;

	VkPhysicalDeviceSurfaceInfo2KHR surface_info = {0};

	if (!r || !r->vk_physical_device || !r->vk_surface)
		return;


	surface_info.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
	surface_info.pNext = NULL;
	surface_info.surface = r->vk_surface;

	r->vk_swapchain_requested_frames = 3; // TODO(samuel): Set from info or somewhere else

	r->vk_swapchain_width = 0;
	r->vk_swapchain_height = 0;

	r->vk_swapchain_min_image_count = 0;
	r->vk_swapchain_max_image_count = 0;
	r->vk_swapchain_requested_image_count = 0;

	r->vk_surface_capabilities.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;

	if (!SF_VULKAN_CHECK(vkGetPhysicalDeviceSurfaceCapabilities2KHR(r->vk_physical_device, &surface_info, &r->vk_surface_capabilities)))
		return;

	r->vk_swapchain_width = r->vk_surface_capabilities.surfaceCapabilities.currentExtent.width;
	r->vk_swapchain_height = r->vk_surface_capabilities.surfaceCapabilities.currentExtent.height;

	min_image_count = r->vk_surface_capabilities.surfaceCapabilities.minImageCount;
	max_image_count = r->vk_surface_capabilities.surfaceCapabilities.maxImageCount;

	r->vk_swapchain_min_image_count = min_image_count;
	r->vk_swapchain_max_image_count = max_image_count ? SF_MIN(max_image_count, SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT) : SF_GRAPHICS_MAX_SWAPCHAIN_IMAGE_COUNT;
	r->vk_swapchain_requested_image_count = SF_CLAMP(req_image_count, r->vk_swapchain_min_image_count, r->vk_swapchain_max_image_count);
}

sf_private void sf_graphics_vulkan_swapchain_image_views_init(struct sf_graphics_renderer *r) {
	u32 i = 0;
	VkImageViewCreateInfo info = {0};

	if (!r || !r->vk_device)
		return;

	r->vk_swapchain_image_view_count = 0;

	for (i = 0; i < r->vk_swapchain_image_count; ++i) {
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.pNext = NULL;
		info.flags = 0;
		info.image = r->vk_swapchain_images[i];
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = r->vk_swapchain_color_format;
		info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		if (!SF_VULKAN_CHECK(vkCreateImageView(r->vk_device, &info, r->vk_allocation_callbacks, &r->vk_swapchain_image_views[i]))) {
			r->vk_swapchain_image_views[i] = VK_NULL_HANDLE;
			return;
		}
	}

	r->vk_swapchain_image_view_count = r->vk_swapchain_image_count;
}

sf_private void sf_graphics_vulkan_swapchain_draw_complete_semaphores_init(struct sf_graphics_renderer *r) {
	u32 i = 0;
	VkSemaphoreCreateInfo info = {0};

	for (i = 0; i < r->vk_swapchain_image_count; ++i) {
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = NULL;
		info.flags = 0;

		if (!SF_VULKAN_CHECK(vkCreateSemaphore(r->vk_device, &info, r->vk_allocation_callbacks, &r->vk_swapchain_draw_complete_semaphores[i]))) {
			r->vk_swapchain_draw_complete_semaphores[i] = VK_NULL_HANDLE;
			return;
		}
	}

	r->vk_swapchain_draw_complete_semaphore_count = r->vk_swapchain_image_count;
}

sf_private void sf_graphics_vulkan_swapchain_acquire_semaphores_init(struct sf_graphics_renderer *r) {
	u32 i = 0;
	VkSemaphoreCreateInfo info = {0};

	for (i = 0; i < r->vk_swapchain_requested_frames; ++i) {
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.pNext = NULL;
		info.flags = 0;

		if (!SF_VULKAN_CHECK(vkCreateSemaphore(r->vk_device, &info, r->vk_allocation_callbacks, &r->vk_swapchain_acquire_semaphores[i]))) {
			r->vk_swapchain_acquire_semaphores[i] = VK_NULL_HANDLE;
			return;
		}
	}

	r->vk_swapchain_acquire_semaphore_count = r->vk_swapchain_image_count;
}

sf_private void sf_graphics_vulkan_swapchain_resources_init(struct sf_arena *arena, struct sf_graphics_renderer *r) {
	if (!arena || !r || !r->vk_physical_device || !r->vk_surface || !r->vk_device)
		return;

	r->vk_swapchain = VK_NULL_HANDLE;

	sf_graphics_vulkan_swapchain_find_color_format(arena, r);
	sf_graphics_vulkan_swapchain_find_depth_stencil_format(r);
	sf_graphics_vulkan_swapchain_find_present_mode(arena, r);
	sf_graphics_vulkan_swapchain_find_surface_capabilities(r);

	sf_graphics_vulkan_swapchain_init(r);
	if (!r->vk_swapchain)
		goto error;

	sf_graphics_vulkan_swapchain_load_images(r);
	if (!r->vk_swapchain_image_count)
		goto error;

	sf_graphics_vulkan_swapchain_image_views_init(r);
	if (!r->vk_swapchain_image_view_count)
		goto error;

	return;

error:
	sf_graphics_vulkan_swapchain_resources_deinit(r);
}

sf_private struct sf_graphics_image *sf_graphics_get_image_from_pool(struct sf_graphics_renderer *r) {
	u32 i = 0;

	for (i = 1; i < SF_SIZE(r->image_pool); ++i) {
		struct sf_graphics_image *current = &r->image_pool[i];

		if (!current->base.is_occupied) {
			SF_MEMORY_SET(current, 0, sizeof(*current));
			current->base.is_occupied = SF_TRUE;
			return current;
		}
	}

	return NULL;
}

sf_private sf_handle sf_graphics_handle_from_image(struct sf_graphics_renderer *r, struct sf_graphics_image *image) {
	if (!image)
		return SF_NULL_HANDLE;

	return (sf_handle)(image - &r->image_pool[0]);
}

sf_private struct sf_graphics_image *sf_graphics_image_from_handle(struct sf_graphics_renderer *r, sf_handle handle) {
	if (handle >= SF_SIZE(r->image_pool))
		return NULL;

	return &r->image_pool[handle];
}

sf_private struct sf_graphics_buffer *sf_graphics_get_buffer_from_pool(struct sf_graphics_renderer *r) {
	u32 i = 0;

	for (i = 1; i < SF_SIZE(r->buffer_pool); ++i) {
		struct sf_graphics_buffer *current = &r->buffer_pool[i];

		if (!current->base.is_occupied) {
			SF_MEMORY_SET(current, 0, sizeof(*current));
			current->base.is_occupied = SF_TRUE;
			return current;
		}
	}

	return NULL;
}

sf_private sf_handle sf_graphics_handle_from_buffer(struct sf_graphics_renderer *r, struct sf_graphics_buffer *buffer) {
	if (!buffer)
		return SF_NULL_HANDLE;

	return (sf_handle)(buffer - &r->buffer_pool[0]);
}

sf_private struct sf_graphics_buffer *sf_graphics_buffer_from_handle(struct sf_graphics_renderer *r, sf_handle handle) {
	if (handle >= SF_SIZE(r->buffer_pool))
		return NULL;

	return &r->buffer_pool[handle];
}

sf_private struct sf_graphics_command_buffer *sf_graphics_get_command_buffer_from_pool(struct sf_graphics_renderer *r) {
	u32 i = 0;

	for (i = 1; i < SF_SIZE(r->command_buffer_pool); ++i) {
		struct sf_graphics_command_buffer *current = &r->command_buffer_pool[i];

		if (!current->base.is_occupied) {
			SF_MEMORY_SET(current, 0, sizeof(*current));
			current->base.is_occupied = SF_TRUE;
			return current;
		}
	}

	return NULL;
}

sf_private sf_handle sf_graphics_handle_from_command_buffer(struct sf_graphics_renderer *r, struct sf_graphics_command_buffer *command_buffer) {
	if (!command_buffer)
		return SF_NULL_HANDLE;

	return (sf_handle)(command_buffer - &r->command_buffer_pool[0]);
}

sf_private struct sf_graphics_command_buffer *sf_graphics_command_buffer_from_handle(struct sf_graphics_renderer *r, sf_handle handle) {
	if (handle >= SF_SIZE(r->command_buffer_pool))
		return NULL;

	return &r->command_buffer_pool[handle];
}

sf_private struct sf_graphics_render_target *sf_graphics_get_render_target_from_pool(struct sf_graphics_renderer *r) {
	u32 i = 0;

	for (i = 1; i < SF_SIZE(r->render_target_pool); ++i) {
		struct sf_graphics_render_target *current = &r->render_target_pool[i];

		if (!current->base.is_occupied) {
			SF_MEMORY_SET(current, 0, sizeof(*current));
			current->base.is_occupied = SF_TRUE;
			return current;
		}
	}

	return NULL;
}

sf_private sf_handle sf_graphics_handle_from_render_target(struct sf_graphics_renderer *r, struct sf_graphics_render_target *render_target) {
	if (!render_target)
		return SF_NULL_HANDLE;

	return (sf_handle)(render_target - &r->render_target_pool[0]);
}

sf_private struct sf_graphics_render_target *sf_graphics_render_target_from_handle(struct sf_graphics_renderer *r, sf_handle handle) {
	if (handle >= SF_SIZE(r->render_target_pool))
		return NULL;

	return &r->render_target_pool[handle];
}

sf_private u32 sf_graphics_vulkan_find_memory_type_index(VkPhysicalDevice device, VkMemoryPropertyFlags memory_properties, u32 filter) {
	u32 i = 0;
	VkPhysicalDeviceMemoryProperties available = {0};

	vkGetPhysicalDeviceMemoryProperties(device, &available);

	for (i = 0; i < available.memoryTypeCount; ++i)
		if ((filter & (1 << i)) && (available.memoryTypes[i].propertyFlags & memory_properties) == memory_properties)
			return i;

	return (u32)-1;
}

sf_private VkDeviceMemory sf_graphics_vulkan_allocate_memory(struct sf_graphics_renderer *r, VkMemoryPropertyFlags memory_properties, u32 filter, u64 size) {
	u32 memory_type_index = (u32)-1;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkMemoryAllocateInfo info = {0};

	if (!r || !r->vk_physical_device || !r->vk_device || !size)
		return VK_NULL_HANDLE;

	memory_type_index = sf_graphics_vulkan_find_memory_type_index(r->vk_physical_device, memory_properties, filter);
	if (memory_type_index == (u32)-1)
		return VK_NULL_HANDLE;

	info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	info.pNext = NULL;
	info.allocationSize = size;
	info.memoryTypeIndex = memory_type_index;
	if (SF_VULKAN_CHECK(vkAllocateMemory(r->vk_device, &info, r->vk_allocation_callbacks, &memory)))
		return memory;

	return VK_NULL_HANDLE;
}

sf_private VkDeviceMemory sf_graphics_vulkan_allocate_memory_for_image(struct sf_graphics_renderer *r, VkImage image, VkMemoryPropertyFlags memory_flags) {
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkMemoryRequirements requirements = {0};

	if (!r || !r->vk_device || !image)
		return VK_NULL_HANDLE;

	vkGetImageMemoryRequirements(r->vk_device, image, &requirements);
	memory = sf_graphics_vulkan_allocate_memory(r, memory_flags, requirements.memoryTypeBits, requirements.size);
	if (!memory)
		return VK_NULL_HANDLE;

	if (!SF_VULKAN_CHECK(vkBindImageMemory(r->vk_device, image, memory, 0))) {
		vkFreeMemory(r->vk_device, memory, r->vk_allocation_callbacks);
		return VK_NULL_HANDLE;
	}

	return memory;
}

sf_private VkDeviceMemory sf_graphics_vulkan_allocate_memory_for_buffer(struct sf_graphics_renderer *r, VkBuffer buffer, VkMemoryPropertyFlags memory_flags) {
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkMemoryRequirements requirements = {0};

	if (!r || !r->vk_device || !buffer)
		return VK_NULL_HANDLE;

	vkGetBufferMemoryRequirements(r->vk_device, buffer, &requirements);
	memory = sf_graphics_vulkan_allocate_memory(r, memory_flags, requirements.memoryTypeBits, requirements.size);
	if (!memory)
		return VK_NULL_HANDLE;

	if (!SF_VULKAN_CHECK(vkBindBufferMemory(r->vk_device, buffer, memory, 0))) {
		vkFreeMemory(r->vk_device, memory, r->vk_allocation_callbacks);
		return VK_NULL_HANDLE;
	}

	return memory;
}

sf_private VkBufferUsageFlags sf_graphics_vulkan_buffer_usage_flags_from_buffer_usage_flags(sf_graphics_buffer_usage_flags flags) {
	VkBufferUsageFlags result = 0;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_TRANSFER_SOURCE)
		result |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_TRANSFER_DESTINATION)
		result |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER)
		result |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_STORAGE_TEXEL_BUFFER)
		result |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_UNIFORM_BUFFER)
		result |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_STORAGE_BUFFER)
		result |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_INDEX_BUFFER)
		result |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_VERTEX_BUFFER)
		result |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_INDIRECT_BUFFER)
		result |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

	if (flags & SF_GRAPHICS_BUFFER_USAGE_SHADER_DEVICE_ADDRESS)
		result |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

	return result;
}

sf_private VkBuffer sf_graphics_vulkan_buffer_create(struct sf_graphics_renderer *r, u64 size, sf_graphics_buffer_usage_flags buffer_flags) {
	VkBufferCreateInfo info = {0};
	VkBuffer buffer = VK_NULL_HANDLE;

	if (!r || !r->vk_device || !size)
		return VK_NULL_HANDLE;

	info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.size = size;
	info.usage = sf_graphics_vulkan_buffer_usage_flags_from_buffer_usage_flags(buffer_flags);
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = NULL;

	if (SF_VULKAN_CHECK(vkCreateBuffer(r->vk_device, &info, r->vk_allocation_callbacks, &buffer)))
		return buffer;

	return VK_NULL_HANDLE;
}

sf_private VkDeviceSize sf_graphics_vulkan_get_buffer_device_address(struct sf_graphics_renderer *r, VkBuffer buffer) {
	VkBufferDeviceAddressInfo info = {0};

	info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	info.pNext = NULL;
	info.buffer = buffer;

	// NOTE(samuel): https://docs.vulkan.org/refpages/latest/refpages/source/vkGetBufferDeviceAddress.html 0 is null
	return vkGetBufferDeviceAddress(r->vk_device, &info);
}

sf_private VkMemoryPropertyFlags sf_graphics_vulkan_memory_property_flags_from_memory_property_flags(sf_graphics_memory_property_flags flags) {
	VkMemoryPropertyFlags result = 0;

	if (flags & SF_GRAPHICS_MEMORY_PROPERTY_DEVICE_LOCAL)
		result |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	if (flags & SF_GRAPHICS_MEMORY_PROPERTY_CPU_VISIBLE) {
		result |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		result |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	}

	if (flags & SF_GRAPHICS_MEMORY_PROPERTY_LAZILY_ALLOCATED)
		result |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;

	if (flags & SF_GRAPHICS_MEMORY_PROPERTY_PROTECTED)
		result |= VK_MEMORY_PROPERTY_PROTECTED_BIT;

	return result;
}

sf_private void sf_graphics_vulkan_buffer_deinit(struct sf_graphics_renderer *r, struct sf_graphics_buffer *buffer) {
	if (!r || !buffer)
		return;

	buffer->base.is_occupied = SF_FALSE;
	buffer->cpu_mapped_data = NULL;

	if (r->vk_device) {

		if (buffer->vk_memory) {
			vkFreeMemory(r->vk_device, buffer->vk_memory, r->vk_allocation_callbacks);
			buffer->vk_memory = VK_NULL_HANDLE;
		}

		if (buffer->vk_buffer) {
			vkDestroyBuffer(r->vk_device, buffer->vk_buffer, r->vk_allocation_callbacks);
			buffer->vk_buffer = VK_NULL_HANDLE;
		}
	}
}

sf_private struct sf_graphics_buffer *sf_graphics_vulkan_buffer_init(struct sf_graphics_renderer *r, u64 size, sf_graphics_buffer_usage_flags buffer_flags, sf_graphics_memory_property_flags memory_flags) {
	struct sf_graphics_buffer *buffer = sf_graphics_get_buffer_from_pool(r);
	if (!buffer)
		return NULL;

	buffer->vk_buffer = VK_NULL_HANDLE;
	buffer->vk_memory = VK_NULL_HANDLE;
	buffer->vk_address = 0;
	buffer->cpu_mapped_data = NULL;

	buffer->vk_buffer = sf_graphics_vulkan_buffer_create(r, size, buffer_flags);
	if (!buffer->vk_buffer)
		goto error;

	buffer->vk_memory = sf_graphics_vulkan_allocate_memory_for_buffer(r, buffer->vk_buffer, memory_flags);
	if (!buffer->vk_memory)
		goto error;

	if ((buffer_flags & SF_GRAPHICS_BUFFER_USAGE_SHADER_DEVICE_ADDRESS) == SF_GRAPHICS_BUFFER_USAGE_SHADER_DEVICE_ADDRESS) {
		buffer->vk_address = sf_graphics_vulkan_get_buffer_device_address(r, buffer->vk_buffer);
		if (!buffer->vk_address)
			goto error;
	}

	if ((buffer_flags & SF_GRAPHICS_MEMORY_PROPERTY_CPU_VISIBLE) == SF_GRAPHICS_MEMORY_PROPERTY_CPU_VISIBLE) {
		if (!SF_VULKAN_CHECK(vkMapMemory(r->vk_device, buffer->vk_memory, 0, VK_WHOLE_SIZE, 0, &buffer->cpu_mapped_data)))
			goto error;
	}

	return buffer;

error:
	sf_graphics_vulkan_buffer_deinit(r, buffer);
	return NULL;
}

sf_public sf_handle sf_graphics_buffer_init(struct sf_graphics_renderer *r, u64 size, sf_graphics_buffer_usage_flags buffer_flags, sf_graphics_memory_property_flags memory_flags) {
	struct sf_graphics_buffer *buffer = SF_NULL_HANDLE;

	buffer = sf_graphics_vulkan_buffer_init(r, size, buffer_flags, memory_flags);
	if (!buffer)
		return SF_NULL_HANDLE;

	return sf_graphics_handle_from_buffer(r, buffer);
}

sf_public void sf_graphics_buffer_deinit(struct sf_graphics_renderer *r, sf_handle handle) {
	struct sf_graphics_buffer *buffer = sf_graphics_buffer_from_handle(r, handle);
	sf_graphics_vulkan_buffer_deinit(r, buffer);
}

sf_public void *sf_graphics_buffer_get_cpu_mapped_data(struct sf_graphics_renderer *r, sf_handle handle) {
	struct sf_graphics_buffer *buffer = sf_graphics_buffer_from_handle(r, handle);

	if (!buffer)
		return NULL;

	return buffer->cpu_mapped_data;
}

sf_public sf_handle sf_graphics_buffer_init_for_staging(struct sf_graphics_renderer *r, u64 data_size_in_bytes, void const *data) {
	void *mapped_data = NULL;
	sf_graphics_buffer_usage_flags usage = SF_GRAPHICS_BUFFER_USAGE_TRANSFER_SOURCE | SF_GRAPHICS_BUFFER_USAGE_TRANSFER_DESTINATION;
	sf_graphics_memory_property_flags mem_properties = SF_GRAPHICS_MEMORY_PROPERTY_CPU_VISIBLE;
	sf_handle handle = sf_graphics_buffer_init(r, data_size_in_bytes, usage, mem_properties);

	if (!handle)
		return SF_NULL_HANDLE;

	if (data) {
		mapped_data = sf_graphics_buffer_get_cpu_mapped_data(r, handle);
		if (!mapped_data)
			goto error;

		SF_MEMORY_COPY(mapped_data, data, data_size_in_bytes);
	}

	return handle;

error:
	sf_graphics_buffer_deinit(r, handle);
	return SF_NULL_HANDLE;
}

sf_private VkCommandBufferUsageFlags sf_graphics_vulkan_command_buffer_usage_flags_from_command_buffer_flags(sf_graphics_command_buffer_usage_flags flags) {
	VkCommandBufferUsageFlags result = 0;

	if (flags & SF_GRAPHICS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT)
		result = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	return result;
}

sf_private VkCommandPoolCreateFlags sf_graphics_vulkan_command_pool_create_flags_from_command_buffer_flags(sf_graphics_command_buffer_usage_flags flags) {
	VkCommandPoolCreateFlags result = 0;

	if (flags & SF_GRAPHICS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT)
		result = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

	// NOTE(samuel): all command buffers can be reset
	result |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	return result;
}

sf_private VkCommandPool sf_graphics_vulkan_command_pool_create(struct sf_graphics_renderer *r, VkCommandPoolCreateFlags flags) {
	VkCommandPoolCreateInfo info = {0};
	VkCommandPool pool = VK_NULL_HANDLE;

	if (!r || !r->vk_device)
		return VK_NULL_HANDLE;

	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = NULL;
	info.flags = flags;
	info.queueFamilyIndex = r->vk_graphics_queue_family_index;

	if (SF_VULKAN_CHECK(vkCreateCommandPool(r->vk_device, &info, r->vk_allocation_callbacks, &pool)))
		return pool;

	return VK_NULL_HANDLE;
}

sf_private VkCommandBuffer sf_graphics_vulkan_allocate_command_buffer(struct sf_graphics_renderer *r, VkCommandPool vk_command_pool, VkCommandBufferUsageFlags flags) {
	VkCommandBufferAllocateInfo info = {0};
	VkCommandBuffer command_buffer = VK_NULL_HANDLE;

	SF_UNUSED(flags);

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext = NULL;
	info.commandPool = vk_command_pool;
	info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	info.commandBufferCount = 1;

	if (SF_VULKAN_CHECK(vkAllocateCommandBuffers(r->vk_device, &info, &command_buffer)))
		return command_buffer;

	return VK_NULL_HANDLE;
}

sf_private void sf_graphics_vulkan_command_buffer_deinit(struct sf_graphics_renderer *r, struct sf_graphics_command_buffer *command_buffer) {
	if (!r || !command_buffer)
		return;

	command_buffer->base.is_occupied = SF_FALSE;

	if (r->vk_device) {
		if (command_buffer->vk_command_pool) {
			if (command_buffer->vk_command_buffer) {
				vkFreeCommandBuffers(r->vk_device, command_buffer->vk_command_pool, 1, command_buffer->vk_command_pool);
				command_buffer->vk_command_buffer = VK_NULL_HANDLE;
			}

			vkDestroyCommandPool(r->vk_device, command_buffer->vk_command_pool, r->vk_allocation_callbacks);
			command_buffer->vk_command_pool = VK_NULL_HANDLE;
		}
	}
}

sf_private struct sf_graphics_command_buffer *sf_graphics_vulkan_command_buffer_init(struct sf_graphics_renderer *r, sf_graphics_command_buffer_usage_flags flags) {
	struct sf_graphics_command_buffer *command_buffer = NULL;
	VkCommandBufferUsageFlags vk_command_buffer_usage_flags = sf_graphics_vulkan_command_buffer_usage_flags_from_command_buffer_flags(flags);
	VkCommandPoolCreateFlags vk_command_pool_create_flags = sf_graphics_vulkan_command_pool_create_flags_from_command_buffer_flags(flags);

	if (!r)
		return NULL;

	command_buffer = sf_graphics_get_command_buffer_from_pool(r);
	if (!command_buffer)
		return NULL;

	command_buffer->vk_command_pool = sf_graphics_vulkan_command_pool_create(r, vk_command_pool_create_flags);
	if (!command_buffer->vk_command_pool)
		goto error;

	command_buffer->vk_command_buffer = sf_graphics_vulkan_allocate_command_buffer(r, command_buffer->vk_command_pool, vk_command_buffer_usage_flags);
	if (!command_buffer->vk_command_buffer)
		goto error;

	command_buffer->usage = flags;

	return command_buffer;

error:
	sf_graphics_vulkan_command_buffer_deinit(r, command_buffer);
}

sf_public sf_handle sf_graphics_command_buffer_init(struct sf_graphics_renderer *r, sf_graphics_command_buffer_usage_flags flags) {
	sf_handle command_buffer = SF_NULL_HANDLE;

	command_buffer = sf_graphics_vulkan_command_buffer_init(r, flags);
	if (!command_buffer)
		return SF_NULL_HANDLE;

	return sf_graphics_handle_from_command_buffer(r, command_buffer);
}

sf_public void sf_graphics_command_buffer_deinit(struct sf_graphics_renderer *r, sf_handle handle) {
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, handle);
	sf_graphics_vulkan_command_buffer_deinit(r, command_buffer);
}

sf_private sf_bool sf_graphics_command_buffer_is_recording(struct sf_graphics_renderer *r, sf_handle handle) {
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, handle);

	if (!command_buffer)
		return SF_FALSE;

	return command_buffer->is_recording;
}

sf_private void sf_graphics_vulkan_command_buffer_reset(struct sf_graphics_renderer *r, sf_handle handle) {
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, handle);

	if (!command_buffer || !command_buffer->vk_command_pool)
		return;

	if (SF_VULKAN_CHECK(vkResetCommandPool(r->vk_device, command_buffer->vk_command_pool, 0))) {
		command_buffer->is_recording = SF_FALSE;
		command_buffer->is_executable = SF_FALSE;
	}
}

sf_public void sf_graphics_command_buffer_reset(struct sf_graphics_renderer *r, sf_handle handle) {
	sf_graphics_vulkan_command_buffer_reset(r, handle);
}

sf_private void sf_graphics_vulkan_command_buffer_begin(struct sf_graphics_renderer *r, sf_handle handle) {
	VkCommandBufferBeginInfo info = {0};
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, handle);

	if (!command_buffer || !command_buffer->vk_command_buffer || command_buffer->is_executable || command_buffer->is_recording)
		return;

	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext = NULL;
	if (command_buffer->usage & SF_GRAPHICS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT)
		info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	else
		info.flags = 0;
	info.pInheritanceInfo = NULL;

	command_buffer->is_recording = SF_VULKAN_CHECK(vkBeginCommandBuffer(command_buffer->vk_command_buffer, &info));
}

sf_private void sf_graphics_vulkan_command_buffer_end(struct sf_graphics_renderer *r, sf_handle handle) {
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, handle);

	if (!command_buffer || !command_buffer->vk_command_buffer)
		return;

	// https://docs.vulkan.org/spec/latest/chapters/cmdbuffers.html#commandbuffers-lifecycle
	if (command_buffer->is_recording) {
		if (SF_VULKAN_CHECK(vkEndCommandBuffer(command_buffer->vk_command_buffer))) {
			command_buffer->is_recording = SF_FALSE;
			command_buffer->is_executable = SF_TRUE;
		} else {
			sf_graphics_vulkan_command_buffer_reset(r, handle);
		}
	}
}

sf_private void sf_graphics_vulkan_command_buffer_submit_and_block(struct sf_graphics_renderer *r, sf_handle handle) {
	VkCommandBufferSubmitInfo command_buffer_info = {0};
	VkSubmitInfo2 info = {0};
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, handle);

	if (!command_buffer || !r || !r->vk_device || !r->vk_graphics_queue || !command_buffer->is_executable)
		return;

	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	command_buffer_info.pNext = NULL;
	command_buffer_info.commandBuffer = command_buffer->vk_command_buffer;
	command_buffer_info.deviceMask = 0;

	info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	info.pNext = NULL;
	info.flags = 0;
	info.waitSemaphoreInfoCount = 0;
	info.pWaitSemaphoreInfos = NULL;
	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos = &command_buffer_info;
	info.signalSemaphoreInfoCount = 0;
	info.pSignalSemaphoreInfos = NULL;

	SF_VULKAN_CHECK(vkQueueSubmit2(r->vk_graphics_queue, 1, &info, VK_NULL_HANDLE));
	SF_VULKAN_CHECK(vkDeviceWaitIdle(r->vk_device));
}

sf_public void sf_graphics_command_buffer_begin(struct sf_graphics_renderer *r, sf_handle handle) {
	sf_graphics_vulkan_command_buffer_begin(r, handle);
}

sf_public void sf_graphics_command_buffer_end(struct sf_graphics_renderer *r, sf_handle handle) {
	sf_graphics_vulkan_command_buffer_end(r, handle);
}

sf_public void sf_graphics_command_buffer_submit_and_block(struct sf_graphics_renderer *r, sf_handle handle) {
	sf_graphics_vulkan_command_buffer_submit_and_block(r, handle);
}

sf_public sf_handle sf_graphics_command_buffer_init_for_single_use_and_begin(struct sf_graphics_renderer *r) {
	sf_handle command_buffer = sf_graphics_command_buffer_init(r, SF_GRAPHICS_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT);

	if (!command_buffer)
		return SF_NULL_HANDLE;

	sf_graphics_command_buffer_begin(r, command_buffer);
	if (!sf_graphics_command_buffer_is_recording(r, command_buffer))
		goto error;

	return command_buffer;

error:
	sf_graphics_command_buffer_deinit(r, command_buffer);
	return SF_NULL_HANDLE;
}

sf_public void sf_graphics_command_buffer_end_submit_and_deinit(struct sf_graphics_renderer *r, sf_handle handle) {
	sf_graphics_command_buffer_submit_and_block(r, handle);
	sf_graphics_command_buffer_deinit(r, handle);
}

sf_private void sf_graphics_vulkan_command_transition_layout_for_attachment(struct sf_graphics_renderer *r, sf_handle command_buffer_handle, u32 mips, VkImageAspectFlags vk_aspect_flags, VkImage vk_image) {
	VkImageMemoryBarrier2 barrier = {0};
	VkDependencyInfo dependency = {0};
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, command_buffer_handle);

	if (!r || !command_buffer || !command_buffer->vk_command_buffer)
		return;

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = NULL;
	barrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
	barrier.srcAccessMask = VK_ACCESS_2_NONE;
	barrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = vk_image;
	barrier.subresourceRange.aspectMask = vk_aspect_flags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mips;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.pNext = NULL;
	dependency.dependencyFlags = 0;
	dependency.memoryBarrierCount = 0;
	dependency.pMemoryBarriers = NULL;
	dependency.bufferMemoryBarrierCount = 0;
	dependency.pBufferMemoryBarriers = NULL;
	dependency.imageMemoryBarrierCount = 1;
	dependency.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(command_buffer->vk_command_buffer, &dependency);
}

sf_private void sf_graphics_vulkan_command_transition_swapchain_layout_for_rendering(struct sf_graphics_renderer *r, sf_handle command_buffer_handle, u32 mips, VkImageAspectFlags vk_aspect_flags, VkImage vk_image) {
	VkImageMemoryBarrier2 barrier = {0};
	VkDependencyInfo dependency = {0};
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, command_buffer_handle);

	if (!r || !command_buffer || !command_buffer->vk_command_buffer)
		return;

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = NULL;
	barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	barrier.srcAccessMask = VK_ACCESS_2_NONE;
	barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = vk_image;
	barrier.subresourceRange.aspectMask = vk_aspect_flags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mips;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.pNext = NULL;
	dependency.dependencyFlags = 0;
	dependency.memoryBarrierCount = 0;
	dependency.pMemoryBarriers = NULL;
	dependency.bufferMemoryBarrierCount = 0;
	dependency.pBufferMemoryBarriers = NULL;
	dependency.imageMemoryBarrierCount = 1;
	dependency.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(command_buffer->vk_command_buffer, &dependency);
}

sf_private VkAccessFlags2 sf_graphics_vulkan_access_mask_from_pipeline_stage(VkPipelineStageFlags2 stage, sf_bool is_source) {
	VkAccessFlags2 result = 0;

	if ((stage & VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT) || (stage & VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT) || (stage & VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT)) {
		if (is_source)
			result |= (VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT);
		else
			result |= VK_ACCESS_2_SHADER_READ_BIT;
	}

	if (stage & VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT)
		result |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;

	if (stage & VK_PIPELINE_STAGE_2_TRANSFER_BIT) {
		if (is_source)
			result |= VK_ACCESS_2_TRANSFER_READ_BIT;
		else
			result |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
	}

	return result;
}

struct sf_graphics_buffer_memory_barrier {
	VkBuffer vk_buffer;
	VkPipelineStageFlags2 vk_source_stage_flags;
	VkPipelineStageFlags2 vk_destination_stage_flags;
	VkAccessFlags2 vk_source_access_flags;
	VkAccessFlags2 vk_destination_access_flags;
	VkDeviceSize offset;
	VkDeviceSize size;
	u32 vk_source_queue_family_index;
	u32 vk_destination_queue_family_index;
};

sf_private void sf_graphics_vulkan_command_buffer_memory_barrier(struct sf_graphics_renderer *r, sf_handle command_buffer_handle, struct sf_graphics_buffer_memory_barrier *barrier) {
	VkBufferMemoryBarrier2 vk_barrier = {0};
	VkDependencyInfo vk_dependency = {0};
	VkAccessFlags2 vk_source_access_flags = 0;
	VkAccessFlags2 vk_destination_access_flags = 0;
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, command_buffer_handle);

	if (!r || !command_buffer || !command_buffer->vk_command_buffer)
		return;

	if (!barrier->vk_source_access_flags)
		vk_source_access_flags = sf_graphics_vulkan_access_mask_from_pipeline_stage(barrier->vk_source_stage_flags, SF_TRUE);

	if (!barrier->vk_destination_access_flags)
		vk_destination_access_flags = sf_graphics_vulkan_access_mask_from_pipeline_stage(barrier->vk_destination_stage_flags, SF_FALSE);

	vk_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
	vk_barrier.pNext = NULL;
	vk_barrier.srcStageMask = barrier->vk_source_stage_flags;
	vk_barrier.srcAccessMask = vk_source_access_flags;
	vk_barrier.dstStageMask = barrier->vk_destination_stage_flags;
	vk_barrier.dstAccessMask = vk_destination_access_flags;
	vk_barrier.srcQueueFamilyIndex = barrier->vk_source_queue_family_index;
	vk_barrier.dstQueueFamilyIndex = barrier->vk_destination_queue_family_index;
	vk_barrier.buffer = barrier->vk_buffer;
	vk_barrier.offset = barrier->offset;
	vk_barrier.size = barrier->size;

	vk_dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	vk_dependency.pNext = NULL;
	vk_dependency.dependencyFlags = 0;
	vk_dependency.memoryBarrierCount = 0;
	vk_dependency.pMemoryBarriers = NULL;
	vk_dependency.bufferMemoryBarrierCount = 1;
	vk_dependency.pBufferMemoryBarriers = &vk_barrier;
	vk_dependency.imageMemoryBarrierCount = 0;
	vk_dependency.pImageMemoryBarriers = NULL;

	vkCmdPipelineBarrier2(command_buffer->vk_command_buffer, &vk_dependency);
}

sf_private VkShaderModule sf_graphics_vulkan_shader_create(struct sf_graphics_renderer *r, u32 code_size, void const *code) {
	VkShaderModuleCreateInfo info = {0};
	VkShaderModule shader = VK_NULL_HANDLE;

	if (!r || !r->vk_device)
		return VK_NULL_HANDLE;

	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.codeSize = code_size;
	info.pCode = (u32 const *)code;

	if (SF_VULKAN_CHECK(vkCreateShaderModule(r->vk_device, &info, r->vk_allocation_callbacks, &shader)))
		return shader;

	return VK_NULL_HANDLE;
}

sf_private void sf_graphics_vulkan_command_transition_swapchain_layout_for_presenting(struct sf_graphics_renderer *r, sf_handle command_buffer_handle, u32 mips, VkImageAspectFlags vk_aspect_flags, VkImage vk_image) {
	VkImageMemoryBarrier2 barrier = {0};
	VkDependencyInfo dependency = {0};
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, command_buffer_handle);

	if (!r || !command_buffer || !command_buffer->vk_command_buffer)
		return;

	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	barrier.pNext = NULL;
	barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
	barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
	barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	barrier.dstAccessMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = vk_image;
	barrier.subresourceRange.aspectMask = vk_aspect_flags;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mips;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	dependency.pNext = NULL;
	dependency.dependencyFlags = 0;
	dependency.memoryBarrierCount = 0;
	dependency.pMemoryBarriers = NULL;
	dependency.bufferMemoryBarrierCount = 1;
	dependency.pBufferMemoryBarriers = NULL;
	dependency.imageMemoryBarrierCount = 1;
	dependency.pImageMemoryBarriers = &barrier;

	vkCmdPipelineBarrier2(command_buffer->vk_command_buffer, &dependency);
}

sf_private void sf_graphics_vulkan_command_bind_shaders(struct sf_graphics_renderer *r, sf_handle command_buffer_handle, VkShaderEXT vertex, VkShaderEXT fragment) {
	VkShaderEXT shaders[5] = {0};
	VkShaderStageFlagBits stages[5] = {0};
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, command_buffer_handle);

	if (!r || !command_buffer || !command_buffer->vk_command_buffer)
		return;

	shaders[0] = vertex;
	shaders[1] = VK_NULL_HANDLE;
	shaders[2] = VK_NULL_HANDLE;
	shaders[3] = VK_NULL_HANDLE;
	shaders[4] = fragment;

	stages[0] = VK_SHADER_STAGE_VERTEX_BIT;
	stages[1] = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	stages[2] = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	stages[3] = VK_SHADER_STAGE_GEOMETRY_BIT;
	stages[4] = VK_SHADER_STAGE_FRAGMENT_BIT;

	vkCmdBindShadersEXT(command_buffer->vk_command_buffer, SF_SIZE(stages), stages, shaders);
}

sf_private void sf_graphics_vulkan_command_copy_buffer(struct sf_graphics_renderer *r, sf_handle command_buffer_handle, u64 source_offset, u64 destination_offset, u64 size, sf_handle source_buffer_handle, sf_handle destination_buffer_handle) {
	VkBufferCopy copy = {0};
	struct sf_graphics_command_buffer *command_buffer = sf_graphics_command_buffer_from_handle(r, command_buffer_handle);
	struct sf_graphics_buffer *source_buffer = sf_graphics_buffer_from_handle(r, source_buffer_handle);
	struct sf_graphics_buffer *destination_buffer = sf_graphics_buffer_from_handle(r, destination_buffer_handle);

	if (!r || !command_buffer || !source_buffer || !destination_buffer)
		return;

	copy.srcOffset = source_offset;
	copy.dstOffset = destination_offset;
	copy.size = size;

	vkCmdCopyBuffer(command_buffer->vk_command_buffer, source_buffer->vk_buffer, destination_buffer->vk_buffer, 1, &copy);
}

sf_private void sf_graphics_vulkan_device_wait_idle(struct sf_graphics_renderer *r) {
	if (!r || !r->vk_device)
		return;

	SF_VULKAN_CHECK(vkDeviceWaitIdle(r->vk_device));
}

sf_private sf_handle sf_graphics_buffer_init_and_upload_data(struct sf_graphics_renderer *r, sf_graphics_buffer_usage_flags usage, sf_graphics_memory_property_flags memory_properties, u64 data_size_in_bytes, void const *data) {
	sf_handle buffer = SF_NULL_HANDLE;
	sf_handle staging_buffer = SF_NULL_HANDLE;
	sf_handle command_buffer = SF_NULL_HANDLE;

	staging_buffer = sf_graphics_buffer_init_for_staging(r, data_size_in_bytes, data);
	if (!staging_buffer)
		goto error;

	command_buffer = sf_graphics_command_buffer_init_for_single_use_and_begin(r);
	if (!command_buffer)
		goto error;

	buffer = sf_graphics_buffer_init(r, data_size_in_bytes, usage | SF_GRAPHICS_BUFFER_USAGE_TRANSFER_DESTINATION, memory_properties);
	if (!buffer)
		goto error;

	sf_graphics_vulkan_command_copy_buffer(r, command_buffer, 0, 0, data_size_in_bytes, staging_buffer, buffer);

	goto cleanup;

error:
	sf_graphics_vulkan_device_wait_idle(r);

	sf_graphics_buffer_deinit(r, buffer);
	buffer = SF_NULL_HANDLE;

cleanup:
	sf_graphics_command_buffer_end_submit_and_deinit(r, command_buffer);
	sf_graphics_buffer_deinit(r, staging_buffer);

	return buffer;
}

sf_private VkImageViewType sf_graphics_vulkan_image_view_type_from_image_type(enum sf_graphics_image_type type) {
	VkImageViewType result = VK_IMAGE_VIEW_TYPE_1D;

	switch (type) {
		case SF_GRAPHICS_IMAGE_TYPE_1D:
			result = VK_IMAGE_VIEW_TYPE_1D;
			break;
		case SF_GRAPHICS_IMAGE_TYPE_2D:
			result = VK_IMAGE_VIEW_TYPE_2D;
			break;
		case SF_GRAPHICS_IMAGE_TYPE_3D:
			result = VK_IMAGE_VIEW_TYPE_3D;
			break;
		case SF_GRAPHICS_IMAGE_TYPE_CUBE:
			result = VK_IMAGE_VIEW_TYPE_CUBE;
			break;
		default:
			result = VK_IMAGE_VIEW_TYPE_2D;
			break;
	}

	return result;
}

sf_private VkImageType sf_graphics_vulkan_image_type_from_image_type(enum sf_graphics_image_type type) {
	VkImageType result = VK_IMAGE_TYPE_1D;

	switch (type) {
		case SF_GRAPHICS_IMAGE_TYPE_1D:
			result = VK_IMAGE_TYPE_1D;
			break;
		case SF_GRAPHICS_IMAGE_TYPE_2D:
			result = VK_IMAGE_TYPE_2D;
			break;
		case SF_GRAPHICS_IMAGE_TYPE_3D:
			result = VK_IMAGE_TYPE_3D;
			break;
		case SF_GRAPHICS_IMAGE_TYPE_CUBE:
			result = VK_IMAGE_TYPE_2D;
			break;
		default:
			result = VK_IMAGE_TYPE_2D;
			break;
	}

	return result;
}

sf_private VkFormat sf_graphics_vulkan_format_from_format(enum sf_graphics_format format) {
	VkFormat result = VK_FORMAT_UNDEFINED;

	switch (format) {
		// 1 channel
		case SF_GRAPHICS_FORMAT_R8_UNORM:
			result = VK_FORMAT_R8_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16_UNORM:
			result = VK_FORMAT_R16_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16_UINT:
			result = VK_FORMAT_R16_UINT;
			break;
		case SF_GRAPHICS_FORMAT_R16_SFLOAT:
			result = VK_FORMAT_R16_SFLOAT;
			break;
		case SF_GRAPHICS_FORMAT_R32_UINT:
			result = VK_FORMAT_R32_UINT;
			break;
		case SF_GRAPHICS_FORMAT_R32_SFLOAT:
			result = VK_FORMAT_R32_SFLOAT;
			break;
		// 2 channel
		case SF_GRAPHICS_FORMAT_R8G8_UNORM:
			result = VK_FORMAT_R8G8_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16G16_UNORM:
			result = VK_FORMAT_R16G16_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16G16_SFLOAT:
			result = VK_FORMAT_R16G16_SFLOAT;
			break;
		case SF_GRAPHICS_FORMAT_R32G32_UINT:
			result = VK_FORMAT_R32G32_UINT;
			break;
		case SF_GRAPHICS_FORMAT_R32G32_SFLOAT:
			result = VK_FORMAT_R32G32_SFLOAT;
			break;
		// 3 channel
		case SF_GRAPHICS_FORMAT_R8G8B8_UNORM:
			result = VK_FORMAT_R8G8B8_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16G16B16_UNORM:
			result = VK_FORMAT_R16G16B16_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16G16B16_SFLOAT:
			result = VK_FORMAT_R16G16B16_SFLOAT;
			break;
		case SF_GRAPHICS_FORMAT_R32G32B32_UINT:
			result = VK_FORMAT_R32G32B32_UINT;
			break;
		case SF_GRAPHICS_FORMAT_R32G32B32_SFLOAT:
			result = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		// 4 channel
		case SF_GRAPHICS_FORMAT_B8G8R8A8_UNORM:
			result = VK_FORMAT_B8G8R8A8_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_B8G8R8A8_SRGB:
			result = VK_FORMAT_B8G8R8A8_SRGB;
			break;
		case SF_GRAPHICS_FORMAT_R8G8B8A8_UNORM:
			result = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16G16B16A16_UNORM:
			result = VK_FORMAT_R16G16B16A16_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_R16G16B16A16_SFLOAT:
			result = VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case SF_GRAPHICS_FORMAT_R32G32B32A32_UINT:
			result = VK_FORMAT_R32G32B32A32_UINT;
			break;
		case SF_GRAPHICS_FORMAT_R32G32B32A32_SFLOAT:
			result = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		// Depth/stencil
		case SF_GRAPHICS_FORMAT_D16_UNORM:
			result = VK_FORMAT_D16_UNORM;
			break;
		case SF_GRAPHICS_FORMAT_X8_D24_UNORM_PACK32:
			result = VK_FORMAT_X8_D24_UNORM_PACK32;
			break;
		case SF_GRAPHICS_FORMAT_D32_SFLOAT:
			result = VK_FORMAT_D32_SFLOAT;
			break;
		case SF_GRAPHICS_FORMAT_S8_UINT:
			result = VK_FORMAT_S8_UINT;
			break;
		case SF_GRAPHICS_FORMAT_D16_UNORM_S8_UINT:
			result = VK_FORMAT_D16_UNORM_S8_UINT;
			break;
		case SF_GRAPHICS_FORMAT_D24_UNORM_S8_UINT:
			result = VK_FORMAT_D24_UNORM_S8_UINT;
			break;
		case SF_GRAPHICS_FORMAT_D32_SFLOAT_S8_UINT:
			result = VK_FORMAT_D32_SFLOAT_S8_UINT;
			break;
		default:
			result = VK_FORMAT_UNDEFINED;
			break;
	}

	return result;
}

sf_private enum sf_graphics_format sf_graphics_format_from_vulkan_format(VkFormat format) {
	enum sf_graphics_format result = SF_GRAPHICS_FORMAT_UNDEFINED;

	switch (format) {
		// 1 channel
		case VK_FORMAT_R8_UNORM:
			result = SF_GRAPHICS_FORMAT_R8_UNORM;
			break;
		case VK_FORMAT_R16_UNORM:
			result = SF_GRAPHICS_FORMAT_R16_UNORM;
			break;
		case VK_FORMAT_R16_UINT:
			result = SF_GRAPHICS_FORMAT_R16_UINT;
			break;
		case VK_FORMAT_R16_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R16_SFLOAT;
			break;
		case VK_FORMAT_R32_UINT:
			result = SF_GRAPHICS_FORMAT_R32_UINT;
			break;
		case VK_FORMAT_R32_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R32_SFLOAT;
			break;
		// 2 channel
		case VK_FORMAT_R8G8_UNORM:
			result = SF_GRAPHICS_FORMAT_R8G8_UNORM;
			break;
		case VK_FORMAT_R16G16_UNORM:
			result = SF_GRAPHICS_FORMAT_R16G16_UNORM;
			break;
		case VK_FORMAT_R16G16_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R16G16_SFLOAT;
			break;
		case VK_FORMAT_R32G32_UINT:
			result = SF_GRAPHICS_FORMAT_R32G32_UINT;
			break;
		case VK_FORMAT_R32G32_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R32G32_SFLOAT;
			break;
		// 3 channel
		case VK_FORMAT_R8G8B8_UNORM:
			result = SF_GRAPHICS_FORMAT_R8G8B8_UNORM;
			break;
		case VK_FORMAT_R16G16B16_UNORM:
			result = SF_GRAPHICS_FORMAT_R16G16B16_UNORM;
			break;
		case VK_FORMAT_R16G16B16_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R16G16B16_SFLOAT;
			break;
		case VK_FORMAT_R32G32B32_UINT:
			result = SF_GRAPHICS_FORMAT_R32G32B32_UINT;
			break;
		case VK_FORMAT_R32G32B32_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R32G32B32_SFLOAT;
			break;
		// 4 channel
		case VK_FORMAT_B8G8R8A8_UNORM:
			result = SF_GRAPHICS_FORMAT_B8G8R8A8_UNORM;
			break;
		case VK_FORMAT_B8G8R8A8_SRGB:
			result = SF_GRAPHICS_FORMAT_B8G8R8A8_SRGB;
			break;
		case VK_FORMAT_R8G8B8A8_UNORM:
			result = SF_GRAPHICS_FORMAT_R8G8B8A8_UNORM;
			break;
		case VK_FORMAT_R16G16B16A16_UNORM:
			result = SF_GRAPHICS_FORMAT_R16G16B16A16_UNORM;
			break;
		case VK_FORMAT_R16G16B16A16_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R16G16B16A16_SFLOAT;
			break;
		case VK_FORMAT_R32G32B32A32_UINT:
			result = SF_GRAPHICS_FORMAT_R32G32B32A32_UINT;
			break;
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			result = SF_GRAPHICS_FORMAT_R32G32B32A32_SFLOAT;
			break;
		// Depth/stencil
		case VK_FORMAT_D16_UNORM:
			result = SF_GRAPHICS_FORMAT_D16_UNORM;
			break;
		case VK_FORMAT_X8_D24_UNORM_PACK32:
			result = SF_GRAPHICS_FORMAT_X8_D24_UNORM_PACK32;
			break;
		case VK_FORMAT_D32_SFLOAT:
			result = SF_GRAPHICS_FORMAT_D32_SFLOAT;
			break;
		case VK_FORMAT_S8_UINT:
			result = SF_GRAPHICS_FORMAT_S8_UINT;
			break;
		case VK_FORMAT_D16_UNORM_S8_UINT:
			result = SF_GRAPHICS_FORMAT_D16_UNORM_S8_UINT;
			break;
		case VK_FORMAT_D24_UNORM_S8_UINT:
			result = SF_GRAPHICS_FORMAT_D24_UNORM_S8_UINT;
			break;
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			result = SF_GRAPHICS_FORMAT_D32_SFLOAT_S8_UINT;
			break;
		default:
			result = SF_GRAPHICS_FORMAT_UNDEFINED;
			break;
	}

	return result;
}

sf_private VkImageAspectFlags sf_graphics_vulkan_image_aspect_flags_from_format(enum sf_graphics_format format) {
	VkImageAspectFlags result = VK_IMAGE_ASPECT_NONE;
	switch (format) {
		// 1 channel
		case SF_GRAPHICS_FORMAT_R8_UNORM:
		case SF_GRAPHICS_FORMAT_R16_UNORM:
		case SF_GRAPHICS_FORMAT_R16_UINT:
		case SF_GRAPHICS_FORMAT_R16_SFLOAT:
		case SF_GRAPHICS_FORMAT_R32_UINT:
		case SF_GRAPHICS_FORMAT_R32_SFLOAT:
		// 2 channel
		case SF_GRAPHICS_FORMAT_R8G8_UNORM:
		case SF_GRAPHICS_FORMAT_R16G16_UNORM:
		case SF_GRAPHICS_FORMAT_R16G16_SFLOAT:
		case SF_GRAPHICS_FORMAT_R32G32_UINT:
		case SF_GRAPHICS_FORMAT_R32G32_SFLOAT:
		// 3 channel
		case SF_GRAPHICS_FORMAT_R8G8B8_UNORM:
		case SF_GRAPHICS_FORMAT_R16G16B16_UNORM:
		case SF_GRAPHICS_FORMAT_R16G16B16_SFLOAT:
		case SF_GRAPHICS_FORMAT_R32G32B32_UINT:
		case SF_GRAPHICS_FORMAT_R32G32B32_SFLOAT:
		// 4 channel
		case SF_GRAPHICS_FORMAT_B8G8R8A8_UNORM:
		case SF_GRAPHICS_FORMAT_B8G8R8A8_SRGB:
		case SF_GRAPHICS_FORMAT_R8G8B8A8_UNORM:
		case SF_GRAPHICS_FORMAT_R16G16B16A16_UNORM:
		case SF_GRAPHICS_FORMAT_R16G16B16A16_SFLOAT:
		case SF_GRAPHICS_FORMAT_R32G32B32A32_UINT:
		case SF_GRAPHICS_FORMAT_R32G32B32A32_SFLOAT:
			result = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		// Depth/stencil
		case SF_GRAPHICS_FORMAT_D16_UNORM:
		case SF_GRAPHICS_FORMAT_X8_D24_UNORM_PACK32:
		case SF_GRAPHICS_FORMAT_D32_SFLOAT:
			result = VK_IMAGE_ASPECT_DEPTH_BIT;
			break;
		case SF_GRAPHICS_FORMAT_S8_UINT:
			result = VK_IMAGE_ASPECT_STENCIL_BIT;
			break;
		case SF_GRAPHICS_FORMAT_D16_UNORM_S8_UINT:
		case SF_GRAPHICS_FORMAT_D24_UNORM_S8_UINT:
		case SF_GRAPHICS_FORMAT_D32_SFLOAT_S8_UINT:
			result = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			break;
		default:
			result = 0;
			break;
	}

	return result;
}

sf_private VkSampleCountFlags sf_graphics_vulkan_sample_count_from_sample_count(enum sf_graphics_sample_count samples) {
	VkSampleCountFlagBits result = VK_SAMPLE_COUNT_1_BIT;
	switch (samples) {
		case SF_GRAPHICS_SAMPLE_COUNT_1:
			result = VK_SAMPLE_COUNT_1_BIT;
			break;
		case SF_GRAPHICS_SAMPLE_COUNT_2:
			result = VK_SAMPLE_COUNT_2_BIT;
			break;
		case SF_GRAPHICS_SAMPLE_COUNT_4:
			result = VK_SAMPLE_COUNT_4_BIT;
			break;
		case SF_GRAPHICS_SAMPLE_COUNT_8:
			result = VK_SAMPLE_COUNT_8_BIT;
			break;
		case SF_GRAPHICS_SAMPLE_COUNT_16:
			result = VK_SAMPLE_COUNT_16_BIT;
			break;
		default:
			result = VK_SAMPLE_COUNT_1_BIT;
			break;
	}
	return result;
}

sf_private enum sf_graphics_sample_count sf_graphics_sample_count_from_vulkan_sample_count(VkSampleCountFlags samples) {
	enum sf_graphics_sample_count result = SF_GRAPHICS_SAMPLE_COUNT_1;
	switch (samples) {
		case VK_SAMPLE_COUNT_1_BIT:
			result = SF_GRAPHICS_SAMPLE_COUNT_1;
			break;
		case VK_SAMPLE_COUNT_2_BIT:
			result = SF_GRAPHICS_SAMPLE_COUNT_2;
			break;
		case VK_SAMPLE_COUNT_4_BIT:
			result = SF_GRAPHICS_SAMPLE_COUNT_4;
			break;
		case VK_SAMPLE_COUNT_8_BIT:
			result = SF_GRAPHICS_SAMPLE_COUNT_8;
			break;
		case VK_SAMPLE_COUNT_16_BIT:
			result = SF_GRAPHICS_SAMPLE_COUNT_16;
			break;
		default:
			result = SF_GRAPHICS_SAMPLE_COUNT_1;
			break;
	}
	return result;
}

sf_private VkImageUsageFlags sf_graphics_vulkan_image_usage_from_image_usage(sf_graphics_image_usage_flags usage) {
	VkImageUsageFlags result = 0;

	if (usage & SF_GRAPHICS_IMAGE_USAGE_TRANSFER_SOURCE) {
		result |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	}
	if (usage & SF_GRAPHICS_IMAGE_USAGE_TRANSFER_DESTINATION) {
		result |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	if (usage & SF_GRAPHICS_IMAGE_USAGE_SAMPLED) {
		result |= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	}
	if (usage & SF_GRAPHICS_IMAGE_USAGE_STORAGE) {
		result |= VK_IMAGE_USAGE_STORAGE_BIT;
	}
	if (usage & SF_GRAPHICS_IMAGE_USAGE_COLOR_ATTACHMENT) {
		result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	if (usage & SF_GRAPHICS_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT) {
		result |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	}
	if (usage & SF_GRAPHICS_IMAGE_USAGE_RESOLVE_SOURCE) {
		result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	if (usage & SF_GRAPHICS_IMAGE_USAGE_RESOLVE_DESTINATION) {
		result |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	return result;
}

sf_public VkImage sf_graphics_vulkan_image_create(struct sf_graphics_renderer *r, enum sf_graphics_image_type image_type, sf_graphics_image_usage_flags image_usage, enum sf_graphics_format format, enum sf_graphics_sample_count samples, u32 width, u32 height, u32 mips) {
	VkImageCreateInfo info = {0};
	VkImage image = VK_NULL_HANDLE;

	if (!r || !r->vk_device)
		return VK_NULL_HANDLE;

	info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.imageType = sf_graphics_vulkan_image_type_from_image_type(image_type);
	info.format = sf_graphics_vulkan_format_from_format(format);
	info.extent.width = width;
	info.extent.height = height;
	info.extent.depth = 1;
	info.mipLevels = mips;
	info.arrayLayers = 1;
	info.samples = sf_graphics_vulkan_sample_count_from_sample_count(samples);
	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = sf_graphics_vulkan_image_usage_from_image_usage(image_usage);
	info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	info.queueFamilyIndexCount = 0;
	info.pQueueFamilyIndices = NULL;
	info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (SF_VULKAN_CHECK(vkCreateImage(r->vk_device, &info, r->vk_allocation_callbacks, &image)))
		return image;

	return VK_NULL_HANDLE;
}

sf_private VkImageView sf_graphics_vulkan_image_view_create(struct sf_graphics_renderer *r, VkImage vk_image, enum sf_graphics_image_type image_type, sf_graphics_image_usage_flags usage_flags, enum sf_graphics_format format, u32 mips) {
	VkImageViewCreateInfo info = {0};
	VkImageView image_view = VK_NULL_HANDLE;

	if (!r || !r->vk_device || !vk_image)
		return VK_NULL_HANDLE;

	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.image = vk_image;
	info.viewType = sf_graphics_vulkan_image_view_type_from_image_type(image_type);
	info.format = sf_graphics_vulkan_format_from_format(format);
	info.components.r = VK_COMPONENT_SWIZZLE_R;
	info.components.g = VK_COMPONENT_SWIZZLE_G;
	info.components.b = VK_COMPONENT_SWIZZLE_B;
	info.components.a = VK_COMPONENT_SWIZZLE_A;
	info.subresourceRange.aspectMask = sf_graphics_vulkan_image_aspect_flags_from_format(format);
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = mips;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;

	if (SF_VULKAN_CHECK(vkCreateImageView(r->vk_device, &info, r->vk_allocation_callbacks, &image_view)))
		return image_view;

	return VK_NULL_HANDLE;
}

sf_private void sf_graphics_vulkan_image_deinit(struct sf_graphics_renderer *r, struct sf_graphics_image *image) {
	if (!r || !image)
		return;

	image->base.is_occupied = SF_FALSE;

	if (r->vk_device) {
		if (image->vk_image_view) {
			vkDestroyImageView(r->vk_device, image->vk_image_view, r->vk_allocation_callbacks);
			image->vk_image_view = VK_NULL_HANDLE;
		}

		if (image->vk_owns_image) {
			if (image->vk_image) {
				vkDestroyImage(r->vk_device, image->vk_image, r->vk_allocation_callbacks);
				image->vk_image = VK_NULL_HANDLE;
			}
		}
	}
}

sf_private struct sf_graphics_image *sf_graphics_vulkan_image_init_with_image(struct sf_graphics_renderer *r, enum sf_graphics_image_type image_type, sf_graphics_image_usage_flags image_usage, enum sf_graphics_format format, enum sf_graphics_sample_count samples, u32 width, u32 height, u32 mips, VkImage vk_not_owned_image) {
	struct sf_graphics_image *image = sf_graphics_get_image_from_pool(r);

	if (!image)
		return SF_NULL_HANDLE;

	if (vk_not_owned_image) {
		image->vk_owns_image = SF_FALSE;
		image->vk_image = vk_not_owned_image;
	} else {
		image->vk_owns_image = SF_TRUE;
		image->vk_image = sf_graphics_vulkan_image_create(r, image_type, image_usage, format, samples, width, height, mips);
		if (!image->vk_image)
			goto error;
	}

	image->vk_image_view = sf_graphics_vulkan_image_view_create(r, image->vk_image, image_type, image_usage, format, mips);
	if (!image->vk_image_view)
		goto error;

	return image;

error:
	sf_graphics_vulkan_image_deinit(r, image);
	return NULL;
}

sf_public sf_handle sf_graphics_image_init(struct sf_graphics_renderer *r, enum sf_graphics_image_type image_type, sf_graphics_image_usage_flags image_usage, enum sf_graphics_format format, enum sf_graphics_sample_count samples, u32 width, u32 height, u32 mips) {
	struct sf_graphics_image *image = sf_graphics_vulkan_image_init_with_image(r, image_type, image_usage, format, samples, width, height, mips, VK_NULL_HANDLE);

	if (!image)
		return SF_NULL_HANDLE;

	return sf_graphics_handle_from_image(r, image);
}

sf_public sf_handle sf_graphics_image_deinit(struct sf_graphics_renderer *r, sf_handle image_handle) {
	struct sf_graphics_image *image = sf_graphics_image_from_handle(r, image_handle);

	if (!image)
		return SF_NULL_HANDLE;

	sf_graphics_vulkan_image_deinit(r, image);
	return SF_NULL_HANDLE;
}

sf_private void sf_graphics_vulkan_render_target_deinit(struct sf_graphics_renderer *r, struct sf_graphics_render_target *render_target) {
	u32 i = 0;

	if (!r || !render_target)
		return;

	render_target->base.is_occupied = SF_FALSE;

	for (i = 0; i < SF_SIZE(render_target->color_attachments); ++i) {
		sf_handle attachment = render_target->color_attachments[i];

		if (!attachment)
			continue;

		sf_graphics_vulkan_image_deinit(r, attachment);
		render_target->color_attachments[i] = SF_NULL_HANDLE;
	}
	render_target->color_attachment_count = 0;

	if (render_target->depth_stencil_attachment) {
		sf_graphics_vulkan_image_deinit(r, render_target->depth_stencil_attachment);
		render_target->depth_stencil_attachment = SF_NULL_HANDLE;
	}

	for (i = 0; i < SF_SIZE(render_target->imgui_attachments); ++i) {
		sf_handle attachment = render_target->imgui_attachments[i];

		if (!attachment)
			continue;

		sf_graphics_vulkan_image_deinit(r, attachment);
		render_target->imgui_attachments[i] = SF_NULL_HANDLE;
	}
	render_target->imgui_attachment_count = 0;
}

sf_private struct sf_graphics_render_target *sf_graphics_vulkan_render_target_init(struct sf_graphics_renderer *r, u32 width, u32 height, enum sf_graphics_format color_attachment_format, u32 color_attachment_count, enum sf_graphics_format depth_stencil_format, enum sf_graphics_samples samples) {
	u32 i = 0;
	sf_handle command_buffer = SF_NULL_HANDLE;
	struct sf_graphics_render_target *render_target = sf_graphics_get_render_target_from_pool(r);

	if (!render_target || !width || !height)
		return NULL;

	if (color_attachment_count > SF_SIZE(render_target->color_attachments))
		goto error;

	for (i = 0; i < color_attachment_count; ++i) {
		sf_graphics_image_usage_flags usage = SF_GRAPHICS_IMAGE_USAGE_COLOR_ATTACHMENT | SF_GRAPHICS_IMAGE_USAGE_TRANSFER_SOURCE | SF_GRAPHICS_IMAGE_USAGE_TRANSFER_DESTINATION | SF_GRAPHICS_IMAGE_USAGE_SAMPLED | SF_GRAPHICS_IMAGE_USAGE_STORAGE;

		render_target->color_attachments[i] = sf_graphics_image_init(r, SF_GRAPHICS_IMAGE_TYPE_2D, usage, color_attachment_format, samples, width, height, 1);

		if (!render_target->color_attachments[i])
			goto error;

		// FIXME(SamueL):  there needs to be a second view forced to have a 1 in the alpha channel??
	}

	if (depth_stencil_format != SF_GRAPHICS_FORMAT_UNDEFINED) {
		sf_graphics_image_usage_flags usage = SF_GRAPHICS_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT | SF_GRAPHICS_IMAGE_USAGE_SAMPLED;

		render_target->depth_stencil_attachment = sf_graphics_image_init(r, SF_GRAPHICS_IMAGE_TYPE_2D, usage, depth_stencil_format, samples, width, height, 1);

		if (!render_target->depth_stencil_attachment)
			goto error;
	}

	command_buffer = sf_graphics_command_buffer_init_for_single_use_and_begin(r);
	if (!command_buffer)
		goto error;

	for (i = 0; i < color_attachment_count; ++i) {
		sf_handle attachment = render_target->color_attachments[i]; // NOTE(samuel): Should never be null here, was checked when created.
		struct sf_graphics_image *image = sf_graphics_image_from_handle(r, attachment);
		sf_graphics_vulkan_command_transition_layout_for_attachment(r, command_buffer, VK_REMAINING_MIP_LEVELS, VK_IMAGE_ASPECT_COLOR_BIT, image->vk_image);
	}

	if (render_target->depth_stencil_attachment) {
		struct sf_graphics_image *image = sf_graphics_image_from_handle(r, render_target->depth_stencil_attachment); // NOTE(samuel): Should never be null here, was checked when created.
		sf_graphics_vulkan_command_transition_layout_for_attachment(r, command_buffer, VK_REMAINING_MIP_LEVELS, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, image->vk_image);
	}

	sf_graphics_command_buffer_end_submit_and_deinit(r, command_buffer);

	return render_target;

error:
	sf_graphics_vulkan_render_target_deinit(r, render_target);
	return NULL;
}

sf_public void sf_graphics_renderer_deinit(struct sf_graphics_renderer *r) {
	if (!r)
		return;

	sf_graphics_vulkan_swapchain_resources_deinit(r);

	if (r->vk_device) {
		vkDestroyDevice(r->vk_device, r->vk_allocation_callbacks);
		r->vk_device = VK_NULL_HANDLE;
	}

	if (r->vk_instance) {
		if (r->vk_surface) {
			vkDestroySurfaceKHR(r->vk_instance, r->vk_surface, r->vk_allocation_callbacks);
			r->vk_surface = VK_NULL_HANDLE;
		}

		if (r->vk_validation_messenger && r->vk_destroy_debug_utils_messenger_ext) {
			r->vk_destroy_debug_utils_messenger_ext(r->vk_instance, r->vk_validation_messenger, r->vk_allocation_callbacks);
			r->vk_validation_messenger = VK_NULL_HANDLE;
		}

		vkDestroyInstance(r->vk_instance, r->vk_allocation_callbacks);
		r->vk_instance = VK_NULL_HANDLE;
	}
}

sf_public struct sf_graphics_renderer *sf_graphics_renderer_init(struct sf_arena *arena, struct sf_graphics_renderer_info *info) {
	struct sf_graphics_renderer *r = sf_arena_allocate(arena, sizeof(struct sf_graphics_renderer));

	if (!r)
		return NULL;

	r->plataform_data = info->plataform_data;

	sf_arena_scratch(arena, SF_KB(10), &r->swapchain_arena);
	if (!r->swapchain_arena.data)
		goto error;

	sf_graphics_vulkan_instance_init(arena, r, info);
	if (!r->vk_instance)
		goto error;

	if (info->vk_request_enable_validation_layers) {
		sf_graphics_vulkan_validation_messenger_init(r);
	}

	info->plataform_vulkan_surface_init(info->plataform_data, r);
	if (!r->vk_surface)
		goto error;

	sf_graphics_vulkan_find_suitable_physical_device(arena, r, info);
	if (!r->vk_physical_device)
		goto error;

	sf_graphics_vulkan_device_init(r, info);
	if (!r->vk_device)
		goto error;

	sf_graphics_vulkan_load_device_queues(r);
	if (!r->vk_graphics_queue || !r->vk_present_queue)
		goto error;

	sf_arena_scratch(&r->arena, SF_KB(2), &r->swapchain_arena);
	if (!r->swapchain_arena.data)
		goto error;

	sf_graphics_vulkan_swapchain_resources_init(&r->swapchain_arena, r);
	if (!r->vk_swapchain)
		goto error;

	return r;

error:
	sf_graphics_renderer_deinit(r);
	return NULL;
}

sf_private u64 sf_graphics_stride_from_format(enum sf_graphics_format format) {
	switch (format) {
		// 1 channel
		case SF_GRAPHICS_FORMAT_R8_UNORM:
			return 1;
		case SF_GRAPHICS_FORMAT_R16_UNORM:
			return 2;
		case SF_GRAPHICS_FORMAT_R16_UINT:
			return 2;
		case SF_GRAPHICS_FORMAT_R16_SFLOAT:
			return 2;
		case SF_GRAPHICS_FORMAT_R32_UINT:
			return 4;
		case SF_GRAPHICS_FORMAT_R32_SFLOAT:
			return 4;
		// 2 CHANNEL
		case SF_GRAPHICS_FORMAT_R8G8_UNORM:
			return 2;
		case SF_GRAPHICS_FORMAT_R16G16_UNORM:
			return 4;
		case SF_GRAPHICS_FORMAT_R16G16_SFLOAT:
			return 4;
		case SF_GRAPHICS_FORMAT_R32G32_UINT:
			return 8;
		case SF_GRAPHICS_FORMAT_R32G32_SFLOAT:
			return 8;
		// 3 CHANNEL
		case SF_GRAPHICS_FORMAT_R8G8B8_UNORM:
			return 3;
		case SF_GRAPHICS_FORMAT_R16G16B16_UNORM:
			return 6;
		case SF_GRAPHICS_FORMAT_R16G16B16_SFLOAT:
			return 6;
		case SF_GRAPHICS_FORMAT_R32G32B32_UINT:
			return 12;
		case SF_GRAPHICS_FORMAT_R32G32B32_SFLOAT:
			return 12;
		// 4 CHANNEL
		case SF_GRAPHICS_FORMAT_B8G8R8A8_UNORM:
			return 4;
		case SF_GRAPHICS_FORMAT_R8G8B8A8_UNORM:
			return 4;
		case SF_GRAPHICS_FORMAT_R16G16B16A16_UNORM:
			return 8;
		case SF_GRAPHICS_FORMAT_R16G16B16A16_SFLOAT:
			return 8;
		case SF_GRAPHICS_FORMAT_R32G32B32A32_UINT:
			return 16;
		case SF_GRAPHICS_FORMAT_R32G32B32A32_SFLOAT:
			return 16;
		// DEPTH/STENCIL
		case SF_GRAPHICS_FORMAT_D16_UNORM:
			return 0;
		case SF_GRAPHICS_FORMAT_X8_D24_UNORM_PACK32:
			return 0;
		case SF_GRAPHICS_FORMAT_D32_SFLOAT:
			return 0;
		case SF_GRAPHICS_FORMAT_S8_UINT:
			return 0;
		case SF_GRAPHICS_FORMAT_D16_UNORM_S8_UINT:
			return 0;
		case SF_GRAPHICS_FORMAT_D24_UNORM_S8_UINT:
			return 0;
		case SF_GRAPHICS_FORMAT_D32_SFLOAT_S8_UINT:
			return 0;
		default:
			return 0;
	}
}

// NOTE(samuel): taken from the vulkan header as reference.
#define SF_VULKAN_MAX_DESCRIPTOR_POOL_SIZE_COUNT 10

sf_private VkDescriptorType sf_graphics_vulkan_descriptor_type_from_type(enum sf_graphics_descriptor_type type) {
	switch (type) {
		case SF_GRAPHICS_DESCRIPTOR_TYPE_SAMPLER:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
			//		case SF_GRAPHICS_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			//			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case SF_GRAPHICS_DESCRIPTOR_TYPE_TEXTURE:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			//		case SF_GRAPHICS_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			//			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			//		case SF_GRAPHICS_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			//			return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
			//		case SF_GRAPHICS_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			//			return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		case SF_GRAPHICS_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		case SF_GRAPHICS_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		case SF_GRAPHICS_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		case SF_GRAPHICS_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		case SF_GRAPHICS_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		default:
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}

sf_private void sf_graphics_glfw_platform_framebuffer_resize_callback(GLFWwindow *window, i32 width, i32 height) {
	struct sf_graphics_glfw_platform *platform = (struct sf_graphics_glfw_platform *)glfwGetWindowUserPointer(window);

	if (!platform)
		return;

	platform->window_width = width;
	platform->window_height = height;
}

sf_public struct sf_graphics_glfw_platform *sf_graphics_glfw_platform_init(struct sf_arena *arena, i32 width, i32 height, struct sf_string const *title) {
	struct sf_string window_title = {0};

	struct sf_graphics_glfw_platform *platform = sf_arena_allocate(arena, sizeof(struct sf_graphics_glfw_platform));
	if (!platform)
		return NULL;

	if (!glfwInit())
		return NULL;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	sf_string_null_terminate(arena, title, &window_title);
	platform->window = glfwCreateWindow(width, height, window_title.data, NULL, NULL);

	if (!platform->window)
		goto error;

	glfwSetWindowUserPointer(platform->window, platform);
	glfwSetFramebufferSizeCallback(platform->window, sf_graphics_glfw_platform_framebuffer_resize_callback);
	glfwGetFramebufferSize(platform->window, &platform->window_width, &platform->window_height);

	return platform;

error:
	sf_graphics_glfw_platform_deinit(platform);
	return NULL;
}

sf_public void sf_graphics_glfw_platform_deinit(struct sf_graphics_glfw_platform *platform) {
	if (!platform)
		return;

	if (platform->window) {
		glfwDestroyWindow(platform->window);
		platform->window = NULL;
	}

	platform->window_width = 0;
	platform->window_height = 0;

	glfwTerminate();
}

sf_public void sf_graphics_glfw_platform_process_events(struct sf_graphics_glfw_platform *platform) {
	if (!platform)
		return;

	glfwPollEvents();
}

sf_public sf_bool sf_graphics_glfw_platform_should_close(struct sf_graphics_glfw_platform *platform) {
	if (!platform)
		return SF_FALSE;

	return glfwWindowShouldClose(platform->window);
}

sf_private void sf_graphics_glfw_platform_vulkan_surface_init(void *data, struct sf_graphics_renderer *r) {
	struct sf_graphics_glfw_platform *platform = (struct sf_graphics_glfw_platform *)data;

	if (!platform || !r || !r->vk_instance)
		return;

	if (!SF_VULKAN_CHECK(glfwCreateWindowSurface(r->vk_instance, platform->window, r->vk_allocation_callbacks, &r->vk_surface)))
		r->vk_surface = VK_NULL_HANDLE;
}

sf_private void sf_graphics_glfw_platform_request_swapchain_dimensions(void *data, struct sf_graphics_renderer *r) {
	struct sf_graphics_glfw_platform *platform = (struct sf_graphics_glfw_platform *)data;

	if (!platform || !r)
		return;

	r->vk_swapchain_width = platform->window_width;
	r->vk_swapchain_height = platform->window_height;
}

sf_public void sf_graphics_glfw_platform_fill_renderer_info(struct sf_arena *arena, struct sf_graphics_glfw_platform *platform, struct sf_graphics_renderer_info *info) {
	u32 base_instance_extension_count = 0;
	char const **base_instance_extensions = NULL;

	u32 required_instance_extension_count = 0;

	sf_local_persist char const *validation_layers[] = {"VK_LAYER_KHRONOS_validation"};
	sf_local_persist char const *device_extensions[] = {
	    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	    "VK_EXT_descriptor_heap"
#ifdef __APPLE__
	    , "VK_KHR_portability_subset"
#endif
	};

	if (!platform || !info)
		return;

	info->plataform_data = platform;
	info->request_enable_vsync = SF_TRUE;
	info->width = platform->window_width;
	info->height = platform->window_height;
	info->plataform_vulkan_surface_init = sf_graphics_glfw_platform_vulkan_surface_init;
	info->plataform_request_swapchain_dimensions = sf_graphics_glfw_platform_request_swapchain_dimensions;

	info->application_name.data = "SF";
	info->application_name.size = sizeof("SF");

	base_instance_extensions = glfwGetRequiredInstanceExtensions(&base_instance_extension_count);

#ifdef __APPLE__
	required_instance_extension_count = base_instance_extension_count + 3;
#else
	required_instance_extension_count = base_instance_extension_count + 1;
#endif
	info->vk_instance_extensions = sf_arena_allocate(arena, (required_instance_extension_count) * sizeof(char const *));
	if (info->vk_instance_extensions) {
		u32 i = 0;
		info->vk_instance_extension_count = required_instance_extension_count;

		for (i = 0; i < base_instance_extension_count; ++i)
			info->vk_instance_extensions[i] = base_instance_extensions[i];

		info->vk_instance_extensions[base_instance_extension_count + 0] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#ifdef __APPLE__
		info->vk_instance_extensions[base_instance_extension_count + 1] = "VK_KHR_portability_enumeration";
		info->vk_instance_extensions[base_instance_extension_count + 2] = "VK_KHR_get_physical_device_properties2";
#endif
	}

	info->vk_instance_layer_count = SF_SIZE(validation_layers);
	info->vk_instance_layers = validation_layers;

	info->vk_device_extension_count = SF_SIZE(device_extensions);
	info->vk_device_extensions = device_extensions;
}
