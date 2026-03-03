// Sample app built with Dear ImGui and ImPlot
// This app uses implot and imgui, but does not output to any backend! It only
// serves as a proof that an app can be built, linked, and run.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>    // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && \
  !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char *description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **)
{
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char *glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

  // Create window with graphics context
  GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
  if (window == NULL)
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);    // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void) io;

  // ── CJK font loading ──────────────────────────────────────────────────────
  // Traditional Chinese: Microsoft JhengHei (微軟正黑體)
  // Covers full CJK Unified Ideographs block so TC-specific chars render correctly.
  static const ImWchar tc_ranges[] = {
    0x0020, 0x00FF,    // Basic Latin
    0x2000, 0x206F,    // General Punctuation
    0x3000, 0x303F,    // CJK Symbols and Punctuation
    0x4E00, 0x9FFF,    // CJK Unified Ideographs
    0xFF00, 0xFFEF,    // Halfwidth / Fullwidth Forms
    0,
  };
  ImFont *font_tc = io.Fonts->AddFontFromFileTTF("data/msjh.ttc", 18.0f, nullptr, tc_ranges);

  // Simplified Chinese: Microsoft YaHei (微软雅黑), ~2500 common chars
  ImFont *font_sc = io.Fonts->AddFontFromFileTTF("data/msyh.ttc", 18.0f, nullptr,
                                                  io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

  // Japanese: Meiryo (メイリオ) — hiragana, katakana, and common kanji
  ImFont *font_jp = io.Fonts->AddFontFromFileTTF("data/meiryo.ttc", 18.0f, nullptr,
                                                  io.Fonts->GetGlyphRangesJapanese());
  // ─────────────────────────────────────────────────────────────────────────

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  bool show_demo_window    = true;
  bool show_another_window = false;
  bool show_cjk_window     = true;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window) {
      ImGui::ShowDemoWindow(&show_demo_window);
      ImPlot::ShowDemoWindow(&show_demo_window);

      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!");

      ImGui::Text("This is some useful text.");
      ImGui::Checkbox("Demo Window", &show_demo_window);
      ImGui::Checkbox("Another Window", &show_another_window);
      ImGui::Checkbox("CJK Font Test", &show_cjk_window);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
      ImGui::ColorEdit3("clear color", (float *) &clear_color);

      if (ImGui::Button("Button"))
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::End();
    }

    if (show_another_window) {
      ImGui::Begin("Another Window", &show_another_window);
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

    // ── CJK Font Test window ─────────────────────────────────────────────────
    if (show_cjk_window) {
      ImGui::Begin("CJK Font Test", &show_cjk_window);

      ImGui::SeparatorText("Traditional Chinese  (Microsoft JhengHei / msjh.ttc)");
      if (font_tc) {
        ImGui::PushFont(font_tc);
        ImGui::Text(u8"繁體中文：你好，世界！");
        ImGui::PopFont();
      } else {
        ImGui::TextDisabled("font_tc failed to load (data/msjh.ttc missing?)");
      }

      ImGui::Spacing();
      ImGui::SeparatorText("Simplified Chinese  (Microsoft YaHei / msyh.ttc)");
      if (font_sc) {
        ImGui::PushFont(font_sc);
        ImGui::Text(u8"简体中文：你好，世界！");
        ImGui::PopFont();
      } else {
        ImGui::TextDisabled("font_sc failed to load (data/msyh.ttc missing?)");
      }

      ImGui::Spacing();
      ImGui::SeparatorText("Japanese  (Meiryo / meiryo.ttc)");
      if (font_jp) {
        ImGui::PushFont(font_jp);
        ImGui::Text(u8"日本語：こんにちは、世界！");
        ImGui::PopFont();
      } else {
        ImGui::TextDisabled("font_jp failed to load (data/meiryo.ttc missing?)");
      }

      ImGui::End();
    }
    // ─────────────────────────────────────────────────────────────────────────

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w,
                 clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w,
                 clear_color.w);

    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImPlot::DestroyContext();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
