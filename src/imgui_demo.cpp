// Dear ImGui + ImPlot demo — GLFW + Vulkan backend

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "implot.h"
#include <stdio.h>
#include <stdlib.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// ── Vulkan global state ───────────────────────────────────────────────────────
static VkAllocationCallbacks*   g_Allocator      = nullptr;
static VkInstance               g_Instance       = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device         = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily    = (uint32_t)-1;
static VkQueue                  g_Queue          = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache  = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static uint32_t                 g_MinImageCount  = 2;
static bool                     g_SwapChainRebuild = false;
// ─────────────────────────────────────────────────────────────────────────────

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void check_vk_result(VkResult err)
{
    if (err == VK_SUCCESS) return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0) abort();
}

static bool IsExtensionAvailable(const ImVector<VkExtensionProperties>& props, const char* ext)
{
    for (const VkExtensionProperties& p : props)
        if (strcmp(p.extensionName, ext) == 0)
            return true;
    return false;
}

static void SetupVulkan(ImVector<const char*> instance_extensions)
{
    VkResult err;

    // Create Vulkan instance
    {
        uint32_t prop_count = 0;
        ImVector<VkExtensionProperties> props;
        vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, nullptr);
        props.resize(prop_count);
        err = vkEnumerateInstanceExtensionProperties(nullptr, &prop_count, props.Data);
        check_vk_result(err);

        if (IsExtensionAvailable(props, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
            instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
        if (IsExtensionAvailable(props, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
            instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

        VkInstanceCreateInfo create_info = {};
        create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.enabledExtensionCount   = (uint32_t)instance_extensions.Size;
        create_info.ppEnabledExtensionNames = instance_extensions.Data;
        err = vkCreateInstance(&create_info, g_Allocator, &g_Instance);
        check_vk_result(err);
    }

    // Select physical device and queue family
    g_PhysicalDevice = ImGui_ImplVulkanH_SelectPhysicalDevice(g_Instance);
    IM_ASSERT(g_PhysicalDevice != VK_NULL_HANDLE);
    g_QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(g_PhysicalDevice);
    IM_ASSERT(g_QueueFamily != (uint32_t)-1);

    // Create logical device
    {
        ImVector<const char*> device_extensions;
        device_extensions.push_back("VK_KHR_swapchain");

        uint32_t prop_count = 0;
        ImVector<VkExtensionProperties> props;
        vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &prop_count, nullptr);
        props.resize(prop_count);
        vkEnumerateDeviceExtensionProperties(g_PhysicalDevice, nullptr, &prop_count, props.Data);
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
        if (IsExtensionAvailable(props, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
            device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

        const float queue_priority[] = { 1.0f };
        VkDeviceQueueCreateInfo queue_info = {};
        queue_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = g_QueueFamily;
        queue_info.queueCount       = 1;
        queue_info.pQueuePriorities = queue_priority;

        VkDeviceCreateInfo create_info = {};
        create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount    = 1;
        create_info.pQueueCreateInfos       = &queue_info;
        create_info.enabledExtensionCount   = (uint32_t)device_extensions.Size;
        create_info.ppEnabledExtensionNames = device_extensions.Data;
        err = vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_Device);
        check_vk_result(err);
        vkGetDeviceQueue(g_Device, g_QueueFamily, 0, &g_Queue);
    }

    // Create descriptor pool
    {
        VkDescriptorPoolSize pool_sizes[] = {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE },
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets       = 0;
        for (VkDescriptorPoolSize& ps : pool_sizes)
            pool_info.maxSets += ps.descriptorCount;
        pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
        pool_info.pPoolSizes    = pool_sizes;
        err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
        check_vk_result(err);
    }
}

static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int w, int h)
{
    wd->Surface = surface;

    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, surface, &res);
    if (res != VK_TRUE) { fprintf(stderr, "Error: no WSI support on physical device\n"); exit(-1); }

    const VkFormat request_formats[] = {
        VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_B8G8R8_UNORM,   VK_FORMAT_R8G8B8_UNORM,
    };
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
        g_PhysicalDevice, wd->Surface,
        request_formats, (size_t)IM_COUNTOF(request_formats),
        VK_COLORSPACE_SRGB_NONLINEAR_KHR);

    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
        g_PhysicalDevice, wd->Surface, present_modes, IM_COUNTOF(present_modes));

    IM_ASSERT(g_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(
        g_Instance, g_PhysicalDevice, g_Device, wd,
        g_QueueFamily, g_Allocator, w, h, g_MinImageCount, 0);
}

static void CleanupVulkan()
{
    vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
    vkDestroyDevice(g_Device, g_Allocator);
    vkDestroyInstance(g_Instance, g_Allocator);
}

static void CleanupVulkanWindow(ImGui_ImplVulkanH_Window* wd)
{
    ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, wd, g_Allocator);
    vkDestroySurfaceKHR(g_Instance, wd->Surface, g_Allocator);
}

static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
    VkSemaphore image_sem  = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_sem = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

    VkResult err = vkAcquireNextImageKHR(
        g_Device, wd->Swapchain, UINT64_MAX, image_sem, VK_NULL_HANDLE, &wd->FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) g_SwapChainRebuild = true;
    if (err == VK_ERROR_OUT_OF_DATE_KHR) return;
    if (err != VK_SUBOPTIMAL_KHR) check_vk_result(err);

    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];

    err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);
    check_vk_result(err);
    err = vkResetFences(g_Device, 1, &fd->Fence);
    check_vk_result(err);
    err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
    check_vk_result(err);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &begin_info);
    check_vk_result(err);

    VkRenderPassBeginInfo rp_info = {};
    rp_info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp_info.renderPass               = wd->RenderPass;
    rp_info.framebuffer              = fd->Framebuffer;
    rp_info.renderArea.extent.width  = wd->Width;
    rp_info.renderArea.extent.height = wd->Height;
    rp_info.clearValueCount          = 1;
    rp_info.pClearValues             = &wd->ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &rp_info, VK_SUBPASS_CONTENTS_INLINE);

    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    vkCmdEndRenderPass(fd->CommandBuffer);

    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo sub_info = {};
    sub_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    sub_info.waitSemaphoreCount   = 1;
    sub_info.pWaitSemaphores      = &image_sem;
    sub_info.pWaitDstStageMask    = &wait_stage;
    sub_info.commandBufferCount   = 1;
    sub_info.pCommandBuffers      = &fd->CommandBuffer;
    sub_info.signalSemaphoreCount = 1;
    sub_info.pSignalSemaphores    = &render_sem;
    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result(err);
    err = vkQueueSubmit(g_Queue, 1, &sub_info, fd->Fence);
    check_vk_result(err);
}

static void FramePresent(ImGui_ImplVulkanH_Window* wd)
{
    if (g_SwapChainRebuild) return;

    VkSemaphore render_sem = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores    = &render_sem;
    info.swapchainCount     = 1;
    info.pSwapchains        = &wd->Swapchain;
    info.pImageIndices      = &wd->FrameIndex;

    VkResult err = vkQueuePresentKHR(g_Queue, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR) g_SwapChainRebuild = true;
    if (err == VK_ERROR_OUT_OF_DATE_KHR) return;
    if (err != VK_SUBOPTIMAL_KHR) check_vk_result(err);

    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount;
}

// ── main ─────────────────────────────────────────────────────────────────────
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+Vulkan example", nullptr, nullptr);

    if (!glfwVulkanSupported())
    {
        fprintf(stderr, "GLFW: Vulkan not supported\n");
        return 1;
    }

    // Collect instance extensions required by GLFW
    ImVector<const char*> extensions;
    uint32_t ext_count = 0;
    const char** glfw_ext = glfwGetRequiredInstanceExtensions(&ext_count);
    for (uint32_t i = 0; i < ext_count; i++)
        extensions.push_back(glfw_ext[i]);

    SetupVulkan(extensions);

    // Create window surface
    VkSurfaceKHR surface;
    VkResult err = glfwCreateWindowSurface(g_Instance, window, g_Allocator, &surface);
    check_vk_result(err);

    // Create swapchain / renderpass / framebuffers
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
    SetupVulkanWindow(wd, surface, w, h);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // ── CJK font loading ──────────────────────────────────────────────────────
    static const ImWchar tc_ranges[] = {
        0x0020, 0x00FF,    // Basic Latin
        0x2000, 0x206F,    // General Punctuation
        0x3000, 0x303F,    // CJK Symbols and Punctuation
        0x4E00, 0x9FFF,    // CJK Unified Ideographs
        0xFF00, 0xFFEF,    // Halfwidth / Fullwidth Forms
        0,
    };
    ImFont* font_tc = io.Fonts->AddFontFromFileTTF("data/msjh.ttc",   18.0f, nullptr, tc_ranges);
    ImFont* font_sc = io.Fonts->AddFontFromFileTTF("data/msyh.ttc",   18.0f, nullptr,
                                                    io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    ImFont* font_jp = io.Fonts->AddFontFromFileTTF("data/meiryo.ttc", 18.0f, nullptr,
                                                    io.Fonts->GetGlyphRangesJapanese());
    // ─────────────────────────────────────────────────────────────────────────

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance                     = g_Instance;
    init_info.PhysicalDevice               = g_PhysicalDevice;
    init_info.Device                       = g_Device;
    init_info.QueueFamily                  = g_QueueFamily;
    init_info.Queue                        = g_Queue;
    init_info.PipelineCache                = g_PipelineCache;
    init_info.DescriptorPool               = g_DescriptorPool;
    init_info.MinImageCount                = g_MinImageCount;
    init_info.ImageCount                   = wd->ImageCount;
    init_info.Allocator                    = g_Allocator;
    init_info.PipelineInfoMain.RenderPass  = wd->RenderPass;
    init_info.PipelineInfoMain.Subpass     = 0;
    init_info.CheckVkResultFn              = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info);

    bool show_demo_window    = true;
    bool show_another_window = false;
    bool show_cjk_window     = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Swapchain rebuild on resize
        int fb_w, fb_h;
        glfwGetFramebufferSize(window, &fb_w, &fb_h);
        if (fb_w > 0 && fb_h > 0 &&
            (g_SwapChainRebuild || wd->Width != fb_w || wd->Height != fb_h))
        {
            ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(
                g_Instance, g_PhysicalDevice, g_Device, wd,
                g_QueueFamily, g_Allocator, fb_w, fb_h, g_MinImageCount, 0);
            wd->FrameIndex     = 0;
            g_SwapChainRebuild = false;
        }
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
            ImPlot::ShowDemoWindow(&show_demo_window);

            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");
            ImGui::Text("This is some useful text.");
            ImGui::Checkbox("Demo Window",    &show_demo_window);
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::Checkbox("CJK Font Test",  &show_cjk_window);
            ImGui::SliderFloat("float",       &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color",  (float*)&clear_color);
            if (ImGui::Button("Button")) counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) show_another_window = false;
            ImGui::End();
        }

        // ── CJK Font Test window ──────────────────────────────────────────────
        if (show_cjk_window)
        {
            ImGui::Begin("CJK Font Test", &show_cjk_window);

            ImGui::SeparatorText("Traditional Chinese  (Microsoft JhengHei / msjh.ttc)");
            if (font_tc) { ImGui::PushFont(font_tc); ImGui::Text(u8"繁體中文：你好，世界！"); ImGui::PopFont(); }
            else           ImGui::TextDisabled("font_tc failed to load (data/msjh.ttc missing?)");

            ImGui::Spacing();
            ImGui::SeparatorText("Simplified Chinese  (Microsoft YaHei / msyh.ttc)");
            if (font_sc) { ImGui::PushFont(font_sc); ImGui::Text(u8"简体中文：你好，世界！"); ImGui::PopFont(); }
            else           ImGui::TextDisabled("font_sc failed to load (data/msyh.ttc missing?)");

            ImGui::Spacing();
            ImGui::SeparatorText("Japanese  (Meiryo / meiryo.ttc)");
            if (font_jp) { ImGui::PushFont(font_jp); ImGui::Text(u8"日本語：こんにちは、世界！"); ImGui::PopFont(); }
            else           ImGui::TextDisabled("font_jp failed to load (data/meiryo.ttc missing?)");

            ImGui::End();
        }
        // ─────────────────────────────────────────────────────────────────────

        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        const bool minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        if (!minimized)
        {
            wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            wd->ClearValue.color.float32[3] = clear_color.w;
            FrameRender(wd, draw_data);
            FramePresent(wd);
        }
    }

    // Cleanup
    err = vkDeviceWaitIdle(g_Device);
    check_vk_result(err);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupVulkanWindow(wd);
    CleanupVulkan();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
