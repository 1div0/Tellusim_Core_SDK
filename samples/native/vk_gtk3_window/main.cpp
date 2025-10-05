// Copyright (C) 2018-2025, Tellusim Technologies Inc. All rights reserved
// https://tellusim.com/

#define GDK_DISABLE_DEPRECATION_WARNINGS

#include <gtk/gtk.h>

#include <gdk/gdkx.h>
#include <gdk/gdkwayland.h>
#define VK_USE_PLATFORM_XLIB_KHR	1
#define VK_USE_PLATFORM_WAYLAND_KHR	1

#include <vulkan/vulkan.h>

#include <core/TellusimLog.h>
#include <core/TellusimTime.h>
#include <math/TellusimMath.h>
#include <platform/TellusimContext.h>
#include <platform/TellusimSurface.h>
#include <platform/TellusimPipeline.h>
#include <platform/TellusimCommand.h>
#include <platform/TellusimDevice.h>

/*
 */
namespace Tellusim {
	
	/*
	 */
	class VKGTK3Window {
			
		public:
			
			VKGTK3Window();
			~VKGTK3Window();
			
			// create window
			bool create();
			
		private:
			
			// window signals
			static gboolean draw_signal(GtkWidget *widget, cairo_t *cairo, gpointer data);
			static gboolean key_press_signal(GtkWidget *widget, GdkEventKey *event, gpointer data);
			
			// create context
			bool create_context();
			void release_context();
			
			// create swap chain
			bool create_swap_chain();
			void release_swap_chain();
			
			// create buffers
			bool create_buffers();
			void release_buffers();
			
			// image barrier
			VkPipelineStageFlags get_stage_mask(VkAccessFlags access_mask) const;
			void barrier(VkImage image, VkAccessFlags src_mask, VkAccessFlags dest_mask, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask);
			
			// draw event
			bool draw_vk();
			
			// rendering loop
			bool create_vk();
			bool render_vk();
			
			enum {
				NumFrames = 3,
			};
			
			struct Frame {
				VkImage color_image = VK_NULL_HANDLE;
				VkImageView color_image_view = VK_NULL_HANDLE;
				VkSemaphore acquire_semaphore = VK_NULL_HANDLE;
				VkSemaphore present_semaphore = VK_NULL_HANDLE;
				VkSemaphore wait_semaphore = VK_NULL_HANDLE;
				VkFramebuffer framebuffer = VK_NULL_HANDLE;
			};
			
			GtkWidget *window = nullptr;
			
			uint32_t window_scale = 1;
			uint32_t window_width = 0;
			uint32_t window_height = 0;
			
			VkSurfaceKHR window_surface = VK_NULL_HANDLE;
			VkRenderPass render_pass = VK_NULL_HANDLE;
			VkSwapchainKHR swap_chain = VK_NULL_HANDLE;
			
			Array<Frame> frames;
			uint32_t frame_index = NumFrames - 1;
			VkSurfaceFormatKHR color_format = {};
			
			VkImage depth_image = VK_NULL_HANDLE;
			VkImageView depth_image_view = VK_NULL_HANDLE;
			VkFormat depth_image_format = VK_FORMAT_UNDEFINED;
			VkDeviceMemory depth_image_memory = VK_NULL_HANDLE;
			
			VKContext context;
			VKSurface surface;
			
			Device device;
			
			Pipeline pipeline;
			Buffer vertex_buffer;
			Buffer index_buffer;
	};
	
	/*
	 */
	VKGTK3Window::VKGTK3Window() {
		
	}
	
	VKGTK3Window::~VKGTK3Window() {
		
	}
	
	/*
	 */
	bool VKGTK3Window::create() {
		
		TS_ASSERT(window == nullptr);
		
		// create window
		window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window), "Vulkan Tellusim::VKGTK3Window");
		gtk_widget_set_double_buffered(window, false);
		gtk_widget_set_app_paintable(window, true);
		
		// window size
		gtk_widget_realize(window);
		window_scale = max(gtk_widget_get_scale_factor(window), 1);
		gtk_window_set_default_size(GTK_WINDOW(window), 1600 / window_scale, 900 / window_scale);
		
		// window signals
		g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
		g_signal_connect(window, "key-press-event", G_CALLBACK(key_press_signal), this);
		g_signal_connect(window, "draw", G_CALLBACK(draw_signal), this);
		
		// show window
		gtk_widget_show_all(window);
		
		// create context
		if(!create_context()) {
			TS_LOG(Error, "VKGTK3Window::create(): can't create context\n");
			return false;
		}
		
		// initialize Vulkan
		if(!create_vk()) {
			TS_LOG(Error, "VKGTK3Window::create(): can't create Vulkan\n");
			return false;
		}
		
		return true;
	}
	
	/*
	 */
	gboolean VKGTK3Window::draw_signal(GtkWidget *widget, cairo_t *cairo, gpointer data) {
		return ((VKGTK3Window*)data)->draw_vk();
	}
	
	gboolean VKGTK3Window::key_press_signal(GtkWidget *widget, GdkEventKey *event, gpointer data) {
		if(event->keyval == GDK_KEY_Escape) gtk_main_quit();
		return false;
	}
	
	/*
	 */
	bool VKGTK3Window::create_context() {
		
		// create context
		if(!context.create()) {
			TS_LOG(Error, "VKGTK3Window::create_context(): can't create context\n");
			return false;
		}
		
		// create surface
		surface = VKSurface(context);
		if(!surface) {
			TS_LOG(Error, "VKGTK3Window::create_context(): can't create context\n");
			return false;
		}
		
		// create device
		device = Device(surface);
		if(!device) {
			TS_LOG(Error, "VKGTK3Window::create_context(): can't create device\n");
			return false;
		}
		
		// gdk display
		GdkDisplay *gdk_display = gtk_widget_get_display(window);
		if(!gdk_display) {
			TS_LOG(Error, "VKGTK3Window::create_context(): can't get gdk display\n");
			return false;
		}
		
		// gdk window
		GdkWindow *gdk_window = gtk_widget_get_window(window);
		if(!gdk_window) {
			TS_LOG(Error, "VKGTK3Window::create_context(): can't get gdk window\n");
			return false;
		}
		
		// create surface
		if(GDK_IS_X11_DISPLAY(gdk_display)) {
			VkXlibSurfaceCreateInfoKHR surface_info = {};
			surface_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
			surface_info.dpy = gdk_x11_display_get_xdisplay(gdk_display);
			surface_info.window = gdk_x11_window_get_xid(gdk_window);
			if(VKContext::error(vkCreateXlibSurfaceKHR(surface.getInstance(), &surface_info, nullptr, &window_surface))) {
				TS_LOG(Error, "VKGTK3Window::create_context(): can't create xlib surface\n");
				release_context();
				return false;
			}
		} else if(GDK_IS_WAYLAND_DISPLAY(gdk_display)) {
			VkWaylandSurfaceCreateInfoKHR surface_info = {};
			surface_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
			surface_info.display = gdk_wayland_display_get_wl_display(gdk_display);
			surface_info.surface = gdk_wayland_window_get_wl_surface(gdk_window);
			if(VKContext::error(vkCreateWaylandSurfaceKHR(surface.getInstance(), &surface_info, nullptr, &window_surface))) {
				TS_LOG(Error, "VKGTK3Window::create_context(): can't create wayland surface\n");
				release_context();
				return false;
			}
		} else {
			TS_LOG(Error, "VKGTK3Window::create_context(): unknown display\n");
			release_context();
			return false;
		}
		
		// check surface queue
		VkBool32 surface_supported = VK_FALSE;
		if(VKContext::error(vkGetPhysicalDeviceSurfaceSupportKHR(surface.getAdapter(), surface.getFamily(), window_surface, &surface_supported)) || surface_supported == false) {
			TS_LOG(Error, "VKGTK3Window::create_context(): surface is not supported by adapter\n");
			release_context();
			return false;
		}
		
		// supported formats
		const VkFormat vk_formats[] = { VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT_S8_UINT };
		const Format formats[] = { FormatRGBAu8n, FormatBGRAu8n, FormatDu24Su8, FormatDf32Su8 };
		
		// surface color format
		uint32_t num_color_formats = 0;
		if(VKContext::error(vkGetPhysicalDeviceSurfaceFormatsKHR(surface.getAdapter(), window_surface, &num_color_formats, nullptr)) || num_color_formats == 0) {
			TS_LOG(Error, "VKGTK3Window::create_context(): can't get surface formats count\n");
			release_context();
			return false;
		}
		Array<VkSurfaceFormatKHR> color_formats(num_color_formats);
		if(vkGetPhysicalDeviceSurfaceFormatsKHR(surface.getAdapter(), window_surface, &num_color_formats, color_formats.get())) {
			TS_LOG(Error, "VKGTK3Window::create_context(): can't get surface formats\n");
			release_context();
			return false;
		}
		for(uint32_t i = 0; i < color_formats.size(); i++) {
			for(uint32_t j = 0; j < TS_COUNTOF(formats); j++) {
				if(color_formats[i].format == vk_formats[j]) {
					surface.setColorFormat(formats[j]);
					color_format = color_formats[i];
					break;
				}
			}
		}
		if(surface.getColorFormat() == FormatUnknown) {
			TS_LOG(Error, "VKGTK3Window::create_context(): unknown color format\n");
			release_context();
			return false;
		}
		
		// surface depth format
		VkImageFormatProperties image_properties = {};
		for(uint32_t i = 0; i < TS_COUNTOF(formats); i++) {
			if(vkGetPhysicalDeviceImageFormatProperties(surface.getAdapter(), vk_formats[i], VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, &image_properties) == VK_SUCCESS) {
				surface.setDepthFormat(formats[i]);
				depth_image_format = vk_formats[i];
				break;
			}
		}
		if(surface.getDepthFormat() == FormatUnknown) {
			TS_LOG(Error, "VKGTK3Window::create_context(): unknown depth format\n");
			release_context();
			return false;
		}
		
		// create render pass
		VkAttachmentDescription attachments_desc[2] = {};
		
		// color attachment
		attachments_desc[0].flags = 0;
		attachments_desc[0].format = color_format.format;
		attachments_desc[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments_desc[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments_desc[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments_desc[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments_desc[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments_desc[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments_desc[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		
		// depth attachment
		attachments_desc[1].flags = 0;
		attachments_desc[1].format = depth_image_format;
		attachments_desc[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments_desc[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments_desc[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments_desc[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments_desc[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments_desc[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments_desc[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		VkAttachmentReference depth_attachment = {};
		depth_attachment.attachment = 1;
		depth_attachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		
		// subpass description
		VkSubpassDescription subpass_desc = {};
		subpass_desc.flags = 0;
		subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_desc.inputAttachmentCount = 0;
		subpass_desc.pInputAttachments = nullptr;
		subpass_desc.colorAttachmentCount = 1;
		subpass_desc.pColorAttachments = &color_attachment;
		subpass_desc.pResolveAttachments = nullptr;
		subpass_desc.pDepthStencilAttachment = &depth_attachment;
		subpass_desc.preserveAttachmentCount = 0;
		subpass_desc.pPreserveAttachments = nullptr;
		
		// render pass info
		VkRenderPassCreateInfo render_pass_info = {};
		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		render_pass_info.attachmentCount = 2;
		render_pass_info.pAttachments = attachments_desc;
		render_pass_info.subpassCount = 1;
		render_pass_info.pSubpasses = &subpass_desc;
		render_pass_info.dependencyCount = 0;
		render_pass_info.pDependencies = nullptr;
		
		// create render pass
		if(VKContext::error(vkCreateRenderPass(surface.getDevice(), &render_pass_info, nullptr, &render_pass))) {
			TS_LOG(Error, "VKGTK3Window::create_context(): vkCreateRenderPass(): can't create render pass\n");
			release_context();
			return false;
		}
		
		// surface render pass
		surface.setRenderPass(render_pass);
		
		return true;
	}
	
	/*
	 */
	void VKGTK3Window::release_context() {
		
		release_buffers();
		release_swap_chain();
		
		// release window surface
		if(window_surface) vkDestroySurfaceKHR(surface.getInstance(), window_surface, nullptr);
		if(render_pass) vkDestroyRenderPass(surface.getDevice(), render_pass, nullptr);
		window_surface = VK_NULL_HANDLE;
		render_pass = VK_NULL_HANDLE;
	}
	
	/*
	 */
	bool VKGTK3Window::create_swap_chain() {
		
		// save swap chain
		VkSwapchainKHR old_swap_chain = swap_chain;
		
		// surface present mode
		uint32_t num_present_modes = 0;
		if(VKContext::error(vkGetPhysicalDeviceSurfacePresentModesKHR(surface.getAdapter(), window_surface, &num_present_modes, nullptr)) || num_present_modes == 0) {
			TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't get surface present modes count\n");
			release_context();
			return false;
		}
		Array<VkPresentModeKHR> present_modes(num_present_modes);
		if(VKContext::error(vkGetPhysicalDeviceSurfacePresentModesKHR(surface.getAdapter(), window_surface, &num_present_modes, present_modes.get()))) {
			TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't get surface present modes\n");
			release_context();
			return false;
		}
		
		// surface capabilities
		VkSurfaceCapabilitiesKHR capabilities = {};
		if(VKContext::error(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(surface.getAdapter(), window_surface, &capabilities))) {
			TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't get surface capabilities\n");
			return false;
		}
		
		// swap chain composite alpha
		VkCompositeAlphaFlagBitsKHR composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		if(capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		else if(capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) composite_alpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
		else if(capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) composite_alpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
		else if(capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) composite_alpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
		else {
			TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't select composite alpha\n");
			return false;
		}
		
		// number of images
		uint32_t num_images = max(capabilities.minImageCount + 1, (uint32_t)NumFrames);
		if(capabilities.maxImageCount > 0) num_images = min(num_images, capabilities.maxImageCount);
		
		// swap chain size
		if(capabilities.currentExtent.width == Maxu32) {
			capabilities.currentExtent.width = window_width;
			capabilities.currentExtent.height = window_height;
			surface.setSize(window_width, window_height);
		} else {
			surface.setSize(capabilities.currentExtent.width, capabilities.currentExtent.height);
		}
		
		// swap chain transformation
		if(capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
			capabilities.currentTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		
		// create swap chain
		VkSwapchainCreateInfoKHR swap_chain_info = {};
		swap_chain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swap_chain_info.surface = window_surface;
		swap_chain_info.minImageCount = num_images;
		swap_chain_info.imageFormat = color_format.format;
		swap_chain_info.imageColorSpace = color_format.colorSpace;
		swap_chain_info.imageExtent = capabilities.currentExtent;
		swap_chain_info.imageArrayLayers = 1;
		swap_chain_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swap_chain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swap_chain_info.preTransform = capabilities.currentTransform;
		swap_chain_info.compositeAlpha = composite_alpha;
		swap_chain_info.presentMode = present_modes[0];
		swap_chain_info.clipped = VK_TRUE;
		swap_chain_info.oldSwapchain = old_swap_chain;
		
		if(VKContext::error(vkCreateSwapchainKHR(surface.getDevice(), &swap_chain_info, nullptr, &swap_chain))) {
			TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't create swap chain\n");
			return false;
		}
		
		// release old swap chain
		if(old_swap_chain) vkDestroySwapchainKHR(surface.getDevice(), old_swap_chain, nullptr);
		
		// swap chain images
		uint32_t num_swap_chain_images = 0;
		if(VKContext::error(vkGetSwapchainImagesKHR(surface.getDevice(), swap_chain, &num_swap_chain_images, nullptr)) || num_swap_chain_images == 0) {
			TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't get swap chain images count\n");
			release_swap_chain();
			return false;
		}
		Array<VkImage> swap_chain_images(num_swap_chain_images);
		if(VKContext::error(vkGetSwapchainImagesKHR(surface.getDevice(), swap_chain, &num_swap_chain_images, swap_chain_images.get()))) {
			TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't get swap chain images\n");
			release_swap_chain();
			return false;
		}
		
		// create semaphores
		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		// create swap chain image views
		VkImageViewCreateInfo color_image_view_info = {};
		color_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		color_image_view_info.flags = 0;
		color_image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		color_image_view_info.format = color_format.format;
		color_image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		color_image_view_info.subresourceRange.baseMipLevel = 0;
		color_image_view_info.subresourceRange.levelCount = 1;
		color_image_view_info.subresourceRange.baseArrayLayer = 0;
		color_image_view_info.subresourceRange.layerCount = 1;
		
		// create frames
		frames.resize(num_swap_chain_images);
		for(uint32_t i = 0; i < num_swap_chain_images; i++) {
			Frame &frame = frames[i];
			TS_ASSERT(frame.framebuffer == VK_NULL_HANDLE);
			
			// create acquire semaphore
			if(frame.acquire_semaphore == VK_NULL_HANDLE && VKContext::error(vkCreateSemaphore(surface.getDevice(), &semaphore_info, nullptr, &frame.acquire_semaphore))) {
				TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't create acquire semaphore\n");
				release_swap_chain();
				return false;
			}
			
			// create present semaphore
			if(frame.present_semaphore == VK_NULL_HANDLE && VKContext::error(vkCreateSemaphore(surface.getDevice(), &semaphore_info, nullptr, &frame.present_semaphore))) {
				TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't create present semaphore\n");
				release_swap_chain();
				return false;
			}
			
			// create image view
			frame.color_image = swap_chain_images[i];
			color_image_view_info.image = swap_chain_images[i];
			if(VKContext::error(vkCreateImageView(surface.getDevice(), &color_image_view_info, nullptr, &frame.color_image_view))) {
				TS_LOG(Error, "VKGTK3Window::create_swap_chain(): can't create swap chain image view\n");
				release_swap_chain();
				return false;
			}
			
			// color image layout
			barrier(swap_chain_images[i], 0, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
		}
		
		return true;
	}
	
	void VKGTK3Window::release_swap_chain() {
		
		// release frame resources
		for(uint32_t i = 0; i < frames.size(); i++) {
			Frame &frame = frames[i];
			TS_ASSERT(frame.framebuffer == VK_NULL_HANDLE);
			TS_ASSERT(frame.color_image_view == VK_NULL_HANDLE);
			if(frame.acquire_semaphore) vkDestroySemaphore(surface.getDevice(), frame.acquire_semaphore, nullptr);
			if(frame.present_semaphore) vkDestroySemaphore(surface.getDevice(), frame.present_semaphore, nullptr);
			frame.acquire_semaphore = VK_NULL_HANDLE;
			frame.present_semaphore = VK_NULL_HANDLE;
			frame.color_image = VK_NULL_HANDLE;
		}
		frames.clear();
		
		// release swap chain
		if(swap_chain) vkDestroySwapchainKHR(surface.getDevice(), swap_chain, nullptr);
		swap_chain = VK_NULL_HANDLE;
	}
	
	/*
	 */
	bool VKGTK3Window::create_buffers() {
		
		// create depth image
		VkImageCreateInfo depth_image_info = {};
		depth_image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depth_image_info.flags = 0;
		depth_image_info.imageType = VK_IMAGE_TYPE_2D;
		depth_image_info.format = depth_image_format;
		depth_image_info.extent.width = surface.getWidth();
		depth_image_info.extent.height = surface.getHeight();
		depth_image_info.extent.depth = 1;
		depth_image_info.mipLevels = 1;
		depth_image_info.arrayLayers = 1;
		depth_image_info.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		depth_image_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depth_image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		depth_image_info.queueFamilyIndexCount = 0;
		depth_image_info.pQueueFamilyIndices = nullptr;
		depth_image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		if(VKContext::error(vkCreateImage(surface.getDevice(), &depth_image_info, nullptr, &depth_image))) {
			TS_LOG(Error, "VKGTK3Window::create_buffers(): can't create depth image\n");
			release_buffers();
			return false;
		}
		
		// depth image memory requirements
		VkMemoryRequirements memory_requirements = {};
		VkPhysicalDeviceMemoryProperties memory_properties = {};
		vkGetPhysicalDeviceMemoryProperties(surface.getAdapter(), &memory_properties);
		vkGetImageMemoryRequirements(surface.getDevice(), depth_image, &memory_requirements);
		
		// allocate depth image memory
		VkMemoryAllocateInfo memory_allocate = {};
		memory_allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_allocate.allocationSize = memory_requirements.size;
		memory_allocate.memoryTypeIndex = 0;
		
		for(uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
			if((memory_requirements.memoryTypeBits & (1 << i)) == 0) continue;
			if((memory_properties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == 0) continue;
			memory_allocate.memoryTypeIndex = i;
			break;
		}
		
		if(VKContext::error(vkAllocateMemory(surface.getDevice(), &memory_allocate, nullptr, &depth_image_memory))) {
			TS_LOG(Error, "VKGTK3Window::create_buffers(): can't allocate depth image memory\n");
			release_buffers();
			return false;
		}
		
		// bind depth image memory
		if(VKContext::error(vkBindImageMemory(surface.getDevice(), depth_image, depth_image_memory, 0))) {
			TS_LOG(Error, "VKGTK3Window::create_buffers(): can't bind depth image memory\n");
			release_buffers();
			return false;
		}
		
		// create depth image view
		VkImageViewCreateInfo depth_image_view_info = {};
		depth_image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		depth_image_view_info.flags = 0;
		depth_image_view_info.image = depth_image;
		depth_image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		depth_image_view_info.format = depth_image_format;
		depth_image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		depth_image_view_info.subresourceRange.baseMipLevel = 0;
		depth_image_view_info.subresourceRange.levelCount = 1;
		depth_image_view_info.subresourceRange.baseArrayLayer = 0;
		depth_image_view_info.subresourceRange.layerCount = 1;
		
		if(VKContext::error(vkCreateImageView(surface.getDevice(), &depth_image_view_info, nullptr, &depth_image_view))) {
			TS_LOG(Error, "VKGTK3Window::create_buffers(): can't create depth image view\n");
			release_buffers();
			return false;
		}
		
		// depth image layout
		barrier(depth_image, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
		
		// create framebuffers
		VkImageView attachments[2] = {};
		attachments[1] = depth_image_view;
		
		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.flags = 0;
		framebuffer_info.renderPass = render_pass;
		framebuffer_info.attachmentCount = 2;
		framebuffer_info.pAttachments = attachments;
		framebuffer_info.width = surface.getWidth();
		framebuffer_info.height = surface.getHeight();
		framebuffer_info.layers = 1;
		
		for(uint32_t i = 0; i < frames.size(); i++) {
			Frame &frame = frames[i];
			TS_ASSERT(frame.framebuffer == VK_NULL_HANDLE);
			attachments[0] = frame.color_image_view;
			if(VKContext::error(vkCreateFramebuffer(surface.getDevice(), &framebuffer_info, nullptr, &frame.framebuffer))) {
				TS_LOG(Error, "VKGTK3Window::create_buffers(): can't create framebuffer\n");
				release_buffers();
				return false;
			}
		}
		
		return true;
	}
	
	void VKGTK3Window::release_buffers() {
		
		// finish device
		if(device) device.finish();
		
		// release depth image
		if(depth_image_memory) vkFreeMemory(surface.getDevice(), depth_image_memory, nullptr);
		if(depth_image_view) vkDestroyImageView(surface.getDevice(), depth_image_view, nullptr);
		if(depth_image) vkDestroyImage(surface.getDevice(), depth_image, nullptr);
		depth_image_memory = VK_NULL_HANDLE;
		depth_image_view = VK_NULL_HANDLE;
		depth_image = VK_NULL_HANDLE;
		
		// release framebuffers
		for(uint32_t i = 0; i < frames.size(); i++) {
			Frame &frame = frames[i];
			if(frame.color_image_view) vkDestroyImageView(surface.getDevice(), frame.color_image_view, nullptr);
			if(frame.framebuffer) vkDestroyFramebuffer(surface.getDevice(), frame.framebuffer, nullptr);
			frame.color_image_view = VK_NULL_HANDLE;
			frame.framebuffer = VK_NULL_HANDLE;
		}
	}
	
	/*
	 */
	VkPipelineStageFlags VKGTK3Window::get_stage_mask(VkAccessFlags access_mask) const {
		VkPipelineStageFlags stage_mask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		if(access_mask & VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT) stage_mask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if(access_mask & VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT) stage_mask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		return stage_mask;
	}
	
	void VKGTK3Window::barrier(VkImage image, VkAccessFlags src_mask, VkAccessFlags dest_mask, VkImageLayout old_layout, VkImageLayout new_layout, VkImageAspectFlags aspect_mask) {
		
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask = src_mask;
		barrier.dstAccessMask = dest_mask;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspect_mask;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		
		VkPipelineStageFlags src_stage_mask = get_stage_mask(barrier.srcAccessMask);
		VkPipelineStageFlags dest_stage_mask = get_stage_mask(barrier.dstAccessMask);
		
		VkCommandBuffer command = surface.getCommand();
		if(command) vkCmdPipelineBarrier(command, src_stage_mask, dest_stage_mask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}
	
	/*
	 */
	bool VKGTK3Window::draw_vk() {
		
		// window size
		uint32_t old_width = window_width;
		uint32_t old_height = window_height;
		window_width = gtk_widget_get_allocated_width(window) * window_scale;
		window_height = gtk_widget_get_allocated_height(window) * window_scale;
		
		// create resources
		if(swap_chain == VK_NULL_HANDLE || old_width != window_width || old_height != window_height) {
			release_buffers();
			if(!create_swap_chain()) return false;
			if(!create_buffers()) return false;
			old_width = window_width;
			old_height = window_height;
		}
		
		// acquire next image
		uint32_t old_frame_index = frame_index;
		VkResult result = vkAcquireNextImageKHR(surface.getDevice(), swap_chain, Maxu64, frames[frame_index].acquire_semaphore, VK_NULL_HANDLE, &frame_index);
		for(uint32_t i = 0; result == VK_ERROR_OUT_OF_DATE_KHR && i < Maxu8; i++) {
			release_buffers();
			if(!create_swap_chain()) return false;
			if(!create_buffers()) return false;
			result = vkAcquireNextImageKHR(surface.getDevice(), swap_chain, Maxu64, frames[frame_index].acquire_semaphore, VK_NULL_HANDLE, &frame_index);
		}
		if(result != VK_SUBOPTIMAL_KHR && VKContext::error(result)) {
			TS_LOG(Error, "VKGTK3Window::draw_vk(): can't acquire image\n");
			return false;
		}
		
		// swap frames
		Frame *frame = &frames[frame_index];
		frame->wait_semaphore = frames[old_frame_index].acquire_semaphore;
		
		// color image layout
		barrier(frame->color_image, VK_ACCESS_MEMORY_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
		
		// surface framebuffer
		surface.setFramebuffer(frame->framebuffer);
		
		// render Vulkan
		if(!render_vk()) return false;
		
		// color image layout
		barrier(frame->color_image, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_IMAGE_ASPECT_COLOR_BIT);
		
		// flush device
		device.flush();
		
		// submit semaphore
		VkSubmitInfo submit_info = {};
		VkPipelineStageFlags acquire_mask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		if(frame->wait_semaphore) {
			submit_info.waitSemaphoreCount = 1;
			submit_info.pWaitSemaphores = &frame->wait_semaphore;
			submit_info.pWaitDstStageMask = &acquire_mask;
		}
		submit_info.commandBufferCount = 0;
		submit_info.pCommandBuffers = nullptr;
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &frame->present_semaphore;
		
		VkQueue queue = surface.getQueue();
		if(VKContext::error(vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE))) {
			TS_LOG(Error, "VKSDLWindow::draw_vk(): can't submit command buffer\n");
			return false;
		}
		
		// present swap chain
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &frame->present_semaphore;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &swap_chain;
		present_info.pImageIndices = &frame_index;
		result = vkQueuePresentKHR(queue, &present_info);
		
		if(result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUCCESS) {
			TS_LOGF(Error, "VKSDLWindow::draw_vk(): can't present image %d\n", result);
			return false;
		}
		
		// flip device
		device.flip();
		
		gtk_widget_queue_draw(window);
		
		return true;
	}
	
	/*
	 */
	bool VKGTK3Window::create_vk() {
		
		// create pipeline
		pipeline = device.createPipeline();
		pipeline.setUniformMask(0, Shader::MaskVertex);
		pipeline.addAttribute(Pipeline::AttributePosition, FormatRGBf32, 0, sizeof(float32_t) * 0, sizeof(float32_t) * 6);
		pipeline.addAttribute(Pipeline::AttributeNormal, FormatRGBf32, 0, sizeof(float32_t) * 3, sizeof(float32_t) * 6);
		pipeline.setDepthFunc(Pipeline::DepthFuncLessEqual);
		pipeline.setColorFormat(surface.getColorFormat());
		pipeline.setDepthFormat(surface.getDepthFormat());
		pipeline.setMultisample(surface.getMultisample());
		if(!pipeline.loadShaderGLSL(Shader::TypeVertex, "main.shader", "VERTEX_SHADER=1")) return false;
		if(!pipeline.loadShaderGLSL(Shader::TypeFragment, "main.shader", "FRAGMENT_SHADER=1")) return false;
		if(!pipeline.create()) return false;
		
		// create mesh geometry
		#include "main_mesh.h"
		vertex_buffer = device.createBuffer(Buffer::FlagVertex, mesh_vertices, sizeof(float32_t) * num_mesh_vertices);
		index_buffer = device.createBuffer(Buffer::FlagIndex, mesh_indices, sizeof(uint32_t) * num_mesh_indices);
		if(!vertex_buffer || !index_buffer) return false;
		
		return true;
	}
	
	/*
	 */
	bool VKGTK3Window::render_vk() {
		
		// structures
		struct CommonParameters {
			Matrix4x4f projection;
			Matrix4x4f modelview;
			Matrix4x4f transform;
			Vector4f camera;
		};
		
		// widget target
		Target target = device.createTarget(surface);
		target.setClearColor(Color("#ac162c"));
		target.begin();
		{
			// current time
			float32_t time = (float32_t)Time::seconds();
			
			// common parameters
			CommonParameters common_parameters;
			common_parameters.camera = Vector4f(2.0f, 2.0f, 1.0f, 0.0f);
			common_parameters.projection = Matrix4x4f::perspective(60.0f, (float32_t)surface.getWidth() / surface.getHeight(), 0.1f, 1000.0f);
			if(target.isFlipped()) common_parameters.projection = Matrix4x4f::scale(1.0f, -1.0f, 1.0f) * common_parameters.projection;
			common_parameters.modelview = Matrix4x4f::lookAt(Vector3f(common_parameters.camera), Vector3f::zero, Vector3f::oneZ);
			common_parameters.transform = Matrix4x4f::rotateZ(time * 32.0f) * Matrix4x4f::rotateY(60.0f + time * 8.0f);
			
			// create command list
			Command command = device.createCommand(target);
			
			// draw mesh
			command.setPipeline(pipeline);
			command.setUniform(0, common_parameters);
			command.setVertexBuffer(0, vertex_buffer);
			command.setIndexBuffer(FormatRu32, index_buffer);
			command.drawElements((uint32_t)index_buffer.getSize() / 4);
		}
		target.end();
		
		return true;
	}
}

/*
 */
int32_t main(int32_t argc, char **argv) {
	
	// initialize gtk
	gtk_init(&argc, &argv);
	
	// create window
	Tellusim::VKGTK3Window window;
	if(!window.create()) return 1;
	
	// run application
	gtk_main();
	
	return 0;
}
