#pragma once

#include <cstdint>

#include <string>

#include <mbgl_ffi.h>

#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/actor/scheduler.hpp>
#include <mbgl/platform/settings.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/size.hpp>

#include <EGL/egl.h>

namespace mbgl {
namespace ffi {

class Context : public mbgl::MapObserver, mbgl::Log::Observer {
public:
    Context(const char* access_token,
            int width,
            int height,
            const void* nativeWindow,
            const char* assets_path,
            const char* cache_patch,
            const char* misc);

    void draw_frame(uint32_t framebuffer_id);

    void run_task();

    ~Context();

    void resize(int width, int height);

    // #############################################################################
    //  Runtime Functions
    // #############################################################################

    static bool NetworkStatusGet();
    static void NetworkStatusSet(bool status);

    void InitCallbacks(ffi_camera_move_started_cb camera_move_started_cb,
                       ffi_camera_move_cb camera_move_cb,
                       ffi_camera_idle_cb camera_idle_cb,
                       ffi_map_style_loaded_cb map_style_loaded_cb,
                       ffi_camera_tracking_change_cb camera_tracking_change_cb,
                       ffi_camera_tracking_dismissed_cb camera_tracking_dismissed_cb,
                       ffi_map_idle_cb map_idle_cb,
                       ffi_user_location_update_cb user_location_update_cb,
                       ffi_query_rendered_features_cb query_rendered_features_cb);

    ffi_CameraOptions UpdateMapOptions(ffi_CameraOptions* options);

    bool CameraAnimate(const char* json, double duration);

    bool CameraMove(const char* json);

    void UpdateMyLocationTrackingMode(size_t mode);

    void MatchMapLanguageWithDeviceDefault();

    void EdgeInsets(ffi_EdgeInsets* insets, bool animated);

    void UpdateContentInsets(ffi_EdgeInsets* insets, bool animated);

    void SetMapLanguage(const char* language);

    void SetTelemetryEnabled(bool enabled);

    bool GetTelemetryEnabled();

    void QueryRenderedFeatures(ffi_Point& point, const char* layer_ids, size_t layer_ids_size, const char* filter_json);

    void QueryRenderedFeatures(ffi_ScreenCoordinate& top_left,
                               ffi_ScreenCoordinate& bottom_right,
                               const char* layer_ids,
                               size_t layer_ids_size,
                               const char* filter_json);

    void QuerySourceFeatures(const char* source_id,
                             const char* source_layer_id,
                             const char* filter_json,
                             const char** features,
                             size_t features_size);

    void InvalidateAmbientCache();

    ffi_LatLng RequestMyLatLng();

    void RequestMyLatLng(ffi_LatLng** south_west,
                         size_t south_west_size,
                         ffi_LatLng** north_west,
                         size_t north_west_size);

    void AddImage(uint8_t* image, size_t size);

    void AddImageSource(uint8_t* image, size_t size, ffi_LatLng* coordinates, size_t coordinates_size);

    ffi_Point ToScreenLocation(ffi_LatLng latlng);

    void ToScreenLocationBatch(ffi_LatLng* latlng, size_t latlng_size, ffi_Point** point, size_t point_size);

    void RemoveSource(const char* id);

    void AddLayer(layer_type_t type,
                  const char* source_id,
                  const char* layer_id,
                  const char* below_layer_id,
                  const char* source_layer,
                  double min_zoom,
                  double max_zoom,
                  const char* filter_json,
                  bool enable_interaction,
                  const char* properties_json);

    void RemoveLayer(const char* layer_id);

    void SetFilter(const char* json);

    const char* GetFilter(const char* layer_id);

    ffi_LatLng ToLatLong(ffi_Point* screen_location);

    double GetMetersPerPixelAtLatitude(double latitude);

    void AddGeoJsonSource(const char* geojson);

    void SetGeoJsonSource(const char* source_id, const char* geojson);

    void SetCameraBounds(double west, double north, double south, double east, int padding);

    void AddSource(const char* source_id, const char* json);

    void SetFeatureForGeoJsonSource(const char* source_id, const char* geojson_feature);

    void SetLayerVisibility(const char* layer_id, bool visible);

    void GetLayerIds(const char** layer_ids, size_t layer_ids_size);

    void GetSourceIds(const char** source_ids, size_t source_ids_size);

private:
    std::string accessToken_;
    std::string assetsPath_;
    std::string cachePath_;
    std::string misc_;
    std::string roadName;
    int width_;
    int height_;
    bool texture_;

    struct NATIVE_WINDOW {
        struct wl_display* wl_display;
        struct wl_surface* wl_surface;
        EGLDisplay egl_display;
        struct wl_egl_window* egl_window;
        uint32_t width;
        uint32_t height;
    };

    struct {
        EGLSurface eglSurface;
        EGLContext eglContext;
    } egl_{};

    const NATIVE_WINDOW* nativeWindow_;

    const mapbox::base::WeakPtr<mbgl::util::RunLoop> loop_;
    std::unique_ptr<Scheduler> scheduler_ = nullptr;

    Size mapSize_{800, 600};
    std::unique_ptr<mbgl::Map> map_;
    float aspectRatio_ = 0.0f;

    double emphasisRadius_ = 0.0;
    double bearing_ = 0.0;

    ffi_camera_move_started_cb cameraMoveStarted_cb_;
    ffi_camera_move_cb cameraMove_cb_;
    ffi_camera_idle_cb cameraIdle_cb_;
    ffi_map_style_loaded_cb mapStyleLoaded_cb_;
    ffi_camera_tracking_change_cb cameraTrackingChange_cb_;
    ffi_camera_tracking_dismissed_cb cameraTrackingDismissed_cb_;
    ffi_map_idle_cb mapIdle_cb_;
    ffi_user_location_update_cb userLocationUpdate_cb_;
    ffi_query_rendered_features_cb queryRenderedFeatures_cb_;

    void onCameraWillChange(CameraChangeMode) override;
    void onCameraIsChanging() override;
    void onCameraDidChange(CameraChangeMode) override;
    void onWillStartLoadingMap() override;
    void onDidFinishLoadingMap() override;
    void onDidFailLoadingMap(MapLoadError, const std::string&) override;
    void onWillStartRenderingFrame() override;
    void onDidFinishRenderingFrame(RenderFrameStatus) override;
    void onWillStartRenderingMap() override;
    void onDidFinishRenderingMap(RenderMode) override;
    void onDidFinishLoadingStyle() override;
    void onSourceChanged(style::Source&) override;
    void onDidBecomeIdle() override;
    void onStyleImageMissing(const std::string&) override;
    bool onCanRemoveUnusedStyleImage(const std::string&) override;
    void onRegisterShaders(gfx::ShaderRegistry&) override;

    bool onRecord(EventSeverity severity, Event event, int64_t code, const std::string& msg) override;
};

} // namespace ffi
} // namespace mbgl
