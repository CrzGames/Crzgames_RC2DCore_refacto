#include <RC2D/RC2D_imgui.h>
#include <RC2D/RC2D_internal.h>
#include <RC2D/RC2D_logger.h>
#include <RC2D/RC2D_assert.h>
#include <RC2D/RC2D_config.h>

#include <cimgui.h>
#include <cimgui_impl.h>

bool rc2d_cimgui_init(void) 
{
    // Créer le contexte ImGui
    igCreateContext(NULL);
    ImGuiIO* io = igGetIO_Nil(); (void)io;
    if (!io) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to get ImGui IO");
        return false;
    }

    // Configurer les flags pour les entrées et autres fonctionnalités
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Activer les contrôles clavier
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Activer les contrôles manette
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Activer le docking

    // Activer IsSRGB si le swapchain est en sRGB
    SDL_GPUTextureFormat swapchain_format = SDL_GetGPUSwapchainTextureFormat(rc2d_gpu_getDevice(), rc2d_window_getWindow());
    if (swapchain_format == SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB || swapchain_format == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM_SRGB) 
    {
        io->ConfigFlags |= ImGuiConfigFlags_IsSRGB;
        RC2D_log(RC2D_LOG_INFO, "sRGB swapchain detected, enabling ImGuiConfigFlags_IsSRGB");
    }

    // Configurer la taille d'affichage pour le viewport
    if (rc2d_engine_state.gpu_current_viewport) 
    {
        io->DisplaySize.x = rc2d_engine_state.gpu_current_viewport->w;
        io->DisplaySize.y = rc2d_engine_state.gpu_current_viewport->h;
        io->DisplayFramebufferScale.x = rc2d_engine_state.render_scale;
        io->DisplayFramebufferScale.y = rc2d_engine_state.render_scale;
    }

    // Appliquer un style par défaut
    igStyleColorsDark(NULL);

    // Initialiser le backend SDL3
    if (!ImGui_ImplSDL3_InitForSDLGPU(rc2d_engine_state.window)) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to initialize cimgui SDL3 backend");
        return false;
    }

    // Initialiser le backend SDLGPU3
    ImGui_ImplSDLGPU3_InitInfo init_info;
    init_info.Device = rc2d_gpu_getDevice();
    init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(rc2d_gpu_getDevice(), rc2d_window_getWindow());
    init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
    if (!ImGui_ImplSDLGPU3_Init(&init_info)) 
    {
        RC2D_log(RC2D_LOG_CRITICAL, "Failed to initialize cimgui SDLGPU3 backend");
        return false;
    }

    RC2D_log(RC2D_LOG_INFO, "cimgui initialized successfully with SDL3 and SDLGPU3 backends");
    return true;
}

void rc2d_cimgui_cleanup(void)
{
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    igDestroyContext(NULL);
    RC2D_log(RC2D_LOG_INFO, "cimgui cleaned up successfully");
}

void rc2d_cimgui_newFrame(void) 
{
    ImGuiIO* io = igGetIO_Nil();
    if (!io) 
    {
        RC2D_log(RC2D_LOG_ERROR, "ImGuiIO is null in rc2d_cimgui_newFrame");
        return;
    }

    // Configurer la taille d'affichage pour le viewport
    if (rc2d_engine_state.gpu_current_viewport) 
    {
        io->DisplaySize.x = rc2d_engine_state.gpu_current_viewport->w;
        io->DisplaySize.y = rc2d_engine_state.gpu_current_viewport->h;
        io->DisplayFramebufferScale.x = rc2d_engine_state.render_scale;
        io->DisplayFramebufferScale.y = rc2d_engine_state.render_scale;
    }

    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    igNewFrame();
}

void rc2d_cimgui_draw(void) 
{
    igRender();
    ImDrawData* draw_data = igGetDrawData();

    if (draw_data && rc2d_engine_state.gpu_current_command_buffer)    
    {
        // Préparer ImGui vertex/index buffers
        Imgui_ImplSDLGPU3_PrepareDrawData(draw_data, rc2d_engine_state.gpu_current_command_buffer);

        // Données pour le render pass ImGui
        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture = rc2d_engine_state.gpu_current_swapchain_texture;
        colorTargetInfo.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 1.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_LOAD;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.mip_level = 0;
        colorTargetInfo.layer_or_depth_plane = 0;
        colorTargetInfo.cycle = false;
        colorTargetInfo.resolve_texture = NULL;
        colorTargetInfo.resolve_mip_level = 0;
        colorTargetInfo.resolve_layer = 0;
        colorTargetInfo.cycle_resolve_texture = false;
        colorTargetInfo.padding1 = 0;
        colorTargetInfo.padding2 = 0;

        // Commencer le render pass pour ImGui
        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(rc2d_engine_state.gpu_current_command_buffer, &colorTargetInfo, 1, NULL);

        // Affiche les données de dessin ImGui
        ImGui_ImplSDLGPU3_RenderDrawData(draw_data, rc2d_engine_state.gpu_current_command_buffer, render_pass, NULL);

        // Terminer le render pass ImGui
        SDL_EndGPURenderPass(render_pass);
    }
    else 
    {
        RC2D_log(RC2D_LOG_ERROR, "Failed to get draw data or rc2d_engine_state.gpu_current_command_buffer is NULL");
    }
}

bool rc2d_cimgui_processEvent(SDL_Event* event) 
{
    return ImGui_ImplSDL3_ProcessEvent(event);
}

/**
 * \brief Mappe les conditions RC2D_ImGuiSetCondition vers ImGuiCond.
 *
 * Cette fonction interne convertit les conditions spécifiques à RC2D en leurs
 * équivalents ImGui pour une utilisation dans les appels ImGui.
 *
 * \param rc2d_cond Condition RC2D_ImGuiSetCondition à convertir.
 * \return ImGuiCond Condition ImGui correspondante.
 */
static ImGuiCond map_rc2d_to_imgui_set_condition(RC2D_ImGuiSetCondition rc2d_cond) 
{
    ImGuiCond imgui_cond = ImGuiCond_None;

    if (rc2d_cond & RC2D_IMGUI_SET_CONDITION_ALWAYS) 
    {
        imgui_cond |= ImGuiCond_Always;
    }
    if (rc2d_cond & RC2D_IMGUI_SET_CONDITION_ONCE) 
    {
        imgui_cond |= ImGuiCond_Once;
    }
    if (rc2d_cond & RC2D_IMGUI_SET_CONDITION_FIRST_USE) 
    {
        imgui_cond |= ImGuiCond_FirstUseEver;
    }
    if (rc2d_cond & RC2D_IMGUI_SET_CONDITION_APPEARING) 
    {
        imgui_cond |= ImGuiCond_Appearing;
    }

    return imgui_cond;
}

/**
 * \brief Mappe les flags RC2D_ImGuiSliderFlags vers ImGuiSliderFlags.
 *
 * Cette fonction interne convertit les flags spécifiques à RC2D en leurs
 * équivalents ImGui pour une utilisation dans les appels ImGui.
 *
 * \param rc2d_flags Flags RC2D_ImGuiSliderFlags à convertir.
 * \return ImGuiSliderFlags Flags ImGui correspondants.
 */
static ImGuiSliderFlags map_rc2d_to_imgui_slider_flags(RC2D_ImGuiSliderFlags rc2d_flags) 
{
    ImGuiSliderFlags imgui_flags = ImGuiSliderFlags_None;
    if (rc2d_flags & RC2D_IMGUI_SLIDER_FLAGS_LOGARITHMIC) {
        imgui_flags |= ImGuiSliderFlags_Logarithmic;
    }
    if (rc2d_flags & RC2D_IMGUI_SLIDER_FLAGS_NO_ROUND) {
        imgui_flags |= ImGuiSliderFlags_NoRoundToFormat;
    }
    if (rc2d_flags & RC2D_IMGUI_SLIDER_FLAGS_NO_INPUT) {
        imgui_flags |= ImGuiSliderFlags_NoInput;
    }
    if (rc2d_flags & RC2D_IMGUI_SLIDER_FLAGS_WRAP_AROUND) {
        imgui_flags |= ImGuiSliderFlags_WrapAround;
    }
    if (rc2d_flags & RC2D_IMGUI_SLIDER_FLAGS_CLAMP_ON_INPUT) {
        imgui_flags |= ImGuiSliderFlags_ClampOnInput;
    }
    if (rc2d_flags & RC2D_IMGUI_SLIDER_FLAGS_CLAMP_ZERO_RANGE) {
        imgui_flags |= ImGuiSliderFlags_ClampZeroRange;
    }
    return imgui_flags;
}

/**
 * \brief Mappe les flags RC2D_ImGuiWindowFlags vers ImGuiWindowFlags.
 *
 * Cette fonction interne convertit les flags spécifiques à RC2D en leurs
 * équivalents ImGui pour une utilisation dans les appels ImGui.
 *
 * \param rc2d_flags Flags RC2D_ImGuiWindowFlags à convertir.
 * \return ImGuiWindowFlags Flags ImGui correspondants.
 */
static ImGuiWindowFlags map_rc2d_to_imgui_window_flags(RC2D_ImGuiWindowFlags rc2d_flags) 
{
    ImGuiWindowFlags imgui_flags = ImGuiWindowFlags_None;

    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_TITLE) 
    {
        imgui_flags |= ImGuiWindowFlags_NoTitleBar;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_RESIZE) 
    {
        imgui_flags |= ImGuiWindowFlags_NoResize;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_MOVE) 
    {
        imgui_flags |= ImGuiWindowFlags_NoMove;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_SCROLLBAR) 
    {
        imgui_flags |= ImGuiWindowFlags_NoScrollbar;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_SCROLL_WITH_MOUSE) 
    {
        imgui_flags |= ImGuiWindowFlags_NoScrollWithMouse;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_COLLAPSE) 
    {
        imgui_flags |= ImGuiWindowFlags_NoCollapse;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_AUTO_SIZE) 
    {
        imgui_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_BACKGROUND) 
    {
        imgui_flags |= ImGuiWindowFlags_NoBackground;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_SAVED_SETTINGS) 
    {
        imgui_flags |= ImGuiWindowFlags_NoSavedSettings;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_MOUSE_INPUTS)
    {
        imgui_flags |= ImGuiWindowFlags_NoMouseInputs;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_MENU_BAR) 
    {
        imgui_flags |= ImGuiWindowFlags_MenuBar;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_HORIZONTAL_SCROLLBAR) 
    {
        imgui_flags |= ImGuiWindowFlags_HorizontalScrollbar;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_FOCUS_ON_APPEARING) 
    {
        imgui_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_BRING_TO_FRONT) 
    {
        imgui_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_ALWAYS_VERTICAL_SCROLLBAR) 
    {
        imgui_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_ALWAYS_HORIZONTAL_SCROLLBAR) 
    {
        imgui_flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_NAV_INPUTS) 
    {
        imgui_flags |= ImGuiWindowFlags_NoNavInputs;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_NAV_FOCUS) 
    {
        imgui_flags |= ImGuiWindowFlags_NoNavFocus;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_UNSAVED_DOCUMENT) 
    {
        imgui_flags |= ImGuiWindowFlags_UnsavedDocument;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_NO_DOCKING) 
    {
        imgui_flags |= ImGuiWindowFlags_NoDocking;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_DOCK_NODE_HOST) 
    {
        imgui_flags |= ImGuiWindowFlags_DockNodeHost;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_CHILD_WINDOW) 
    {
        imgui_flags |= ImGuiWindowFlags_ChildWindow;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_TOOLTIP) 
    {
        imgui_flags |= ImGuiWindowFlags_Tooltip;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_POPUP) 
    {
        imgui_flags |= ImGuiWindowFlags_Popup;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_MODAL) 
    {
        imgui_flags |= ImGuiWindowFlags_Modal;
    }
    if (rc2d_flags & RC2D_IMGUI_WINDOW_FLAGS_CHILD_MENU) 
    {
        imgui_flags |= ImGuiWindowFlags_ChildMenu;
    }

    return imgui_flags;
}

/**
 * \brief Mappe les flags RC2D_ImGuiInputTextFlags vers ImGuiInputTextFlags.
 *
 * Cette fonction interne convertit les flags spécifiques à RC2D en leurs
 * équivalents ImGui pour une utilisation dans les appels ImGui.
 *
 * \param rc2d_flags Flags RC2D_ImGuiInputTextFlags à convertir.
 * \return ImGuiInputTextFlags Flags ImGui correspondants.
 */
static ImGuiInputTextFlags map_rc2d_to_imgui_input_text_flags(RC2D_ImGuiInputTextFlags rc2d_flags) {
    ImGuiInputTextFlags imgui_flags = ImGuiInputTextFlags_None;
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_DECIMAL) {
        imgui_flags |= ImGuiInputTextFlags_CharsDecimal;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_HEX) {
        imgui_flags |= ImGuiInputTextFlags_CharsHexadecimal;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_UPPERCASE) {
        imgui_flags |= ImGuiInputTextFlags_CharsUppercase;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_CHARS_NO_BLANK) {
        imgui_flags |= ImGuiInputTextFlags_CharsNoBlank;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_AUTO_SELECT_ALL) {
        imgui_flags |= ImGuiInputTextFlags_AutoSelectAll;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_ENTER_RETURNS_TRUE) {
        imgui_flags |= ImGuiInputTextFlags_EnterReturnsTrue;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_READ_ONLY) {
        imgui_flags |= ImGuiInputTextFlags_ReadOnly;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_PASSWORD) {
        imgui_flags |= ImGuiInputTextFlags_Password;
    }
    if (rc2d_flags & RC2D_IMGUI_INPUT_TEXT_FLAGS_NO_UNDO_REDO) {
        imgui_flags |= ImGuiInputTextFlags_NoUndoRedo;
    }
    return imgui_flags;
}

/**
 * \brief Mappe les flags RC2D_ImGuiPopupFlags vers ImGuiPopupFlags.
 *
 * Cette fonction interne convertit les flags spécifiques à RC2D en leurs
 * équivalents ImGui pour une utilisation dans les appels ImGui.
 *
 * \param rc2d_flags Flags RC2D_ImGuiPopupFlags à convertir.
 * \return ImGuiPopupFlags Flags ImGui correspondants.
 */
static ImGuiPopupFlags map_rc2d_to_imgui_popup_flags(RC2D_ImGuiPopupFlags rc2d_flags) {
    ImGuiPopupFlags imgui_flags = ImGuiPopupFlags_None;
    if (rc2d_flags & RC2D_IMGUI_POPUP_FLAGS_MOUSE_BUTTON_LEFT) {
        imgui_flags |= ImGuiPopupFlags_MouseButtonLeft;
    }
    if (rc2d_flags & RC2D_IMGUI_POPUP_FLAGS_MOUSE_BUTTON_RIGHT) {
        imgui_flags |= ImGuiPopupFlags_MouseButtonRight;
    }
    if (rc2d_flags & RC2D_IMGUI_POPUP_FLAGS_MOUSE_BUTTON_MIDDLE) {
        imgui_flags |= ImGuiPopupFlags_MouseButtonMiddle;
    }
    if (rc2d_flags & RC2D_IMGUI_POPUP_FLAGS_NO_REOPEN) {
        imgui_flags |= ImGuiPopupFlags_NoReopen;
    }
    if (rc2d_flags & RC2D_IMGUI_POPUP_FLAGS_NO_OPEN_OVER_ITEMS) {
        imgui_flags |= ImGuiPopupFlags_NoOpenOverItems;
    }
    return imgui_flags;
}

void rc2d_cimgui_setNextWindowPos(float x, float y, RC2D_ImGuiSetCondition cond)
{
    // Récupère la position actuelle du viewport GPU + les valeurs x et y
    ImVec2 pos = { 
        x + rc2d_engine_state.gpu_current_viewport->x,
        y + rc2d_engine_state.gpu_current_viewport->y
    };

    // Mappe l'enumération RC2D_ImGuiSetCondition vers ImGuiCond
    ImGuiCond imgui_cond = map_rc2d_to_imgui_set_condition(cond);

    // Définit la position de la prochaine fenêtre ImGui
    igSetNextWindowPos(pos, imgui_cond, (ImVec2){ 0.0f, 0.0f }); // Pivot fixé à (0, 0)
}

bool rc2d_cimgui_begin(const char* name, bool* p_open, RC2D_ImGuiWindowFlags flags)
{
    // Mappe les flags RC2D_ImGuiWindowFlags vers ImGuiWindowFlags
    ImGuiWindowFlags imgui_flags = map_rc2d_to_imgui_window_flags(flags);

    // Commence une nouvelle fenêtre ImGui
    bool result = igBegin(name, p_open, imgui_flags);
    if (!result) 
    {
        RC2D_log(RC2D_LOG_WARN, "Failed to begin ImGui window '%s'", name);
    }

    // Retourne le résultat de l'appel à igBegin
    return result;
}

void rc2d_cimgui_end(void)
{
    igEnd();
}

bool rc2d_cimgui_button(const char* label, float width, float height)
{
    ImVec2 size = { width, height };
    return igButton(label, size);
}

void rc2d_cimgui_text(const char* text)
{
    igTextUnformatted(text, NULL);
}

bool rc2d_cimgui_checkbox(const char* label, bool* checked)
{
    return igCheckbox(label, checked);
}

bool rc2d_cimgui_sliderFloat(const char* label, float* value, float min_value, float max_value, const char* format, RC2D_ImGuiSliderFlags flags)
{
    ImGuiSliderFlags imgui_flags = map_rc2d_to_imgui_slider_flags(flags);
    return igSliderFloat(label, value, min_value, max_value, format ? format : "%.3f", imgui_flags);
}

bool rc2d_cimgui_input_text(const char* label, char* buffer, size_t buffer_size, RC2D_ImGuiInputTextFlags flags)
{
    ImGuiInputTextFlags imgui_flags = map_rc2d_to_imgui_input_text_flags(flags);
    return igInputText(label, buffer, buffer_size, imgui_flags, NULL, NULL);
}

bool rc2d_cimgui_combo(const char* label, int* current_item, const char* items[], int items_count, int popup_max_height_in_items)
{
    return igCombo_Str_arr(label, current_item, items, items_count, popup_max_height_in_items);
}

void rc2d_cimgui_open_popup(const char* id, RC2D_ImGuiPopupFlags flags)
{
    ImGuiPopupFlags imgui_flags = map_rc2d_to_imgui_popup_flags(flags);
    igOpenPopup_Str(id, imgui_flags);
}

bool rc2d_cimgui_begin_popup(const char* id, RC2D_ImGuiWindowFlags flags)
{
    ImGuiWindowFlags imgui_flags = map_rc2d_to_imgui_window_flags(flags);
    return igBeginPopup(id, imgui_flags);
}

bool rc2d_cimgui_list_box(const char* label, int* current_item, const char* items[], int items_count, int height_in_items)
{
    return igListBox_Str_arr(label, current_item, items, items_count, height_in_items);
}