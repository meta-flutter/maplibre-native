
#include "context.hpp"

#include <string>
#include <unordered_map>
#include <iostream>

#include <mbgl/util/logging.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/storage/network_status.hpp>

namespace mbgl {
namespace ffi {

using ValueMap = std::unordered_map<std::string, Value>;
using A = mapbox::base::ValueArray;
using O = mapbox::base::ValueObject;

ValueMap modelValueObj(const std::string &uri,
                       std::vector<mapbox::base::Value> &&position,
                       std::vector<mapbox::base::Value> &&orientation) {
    return {{"uri", uri},
            {"position", std::forward<std::vector<mapbox::base::Value>>(position)},
            {"orientation", std::forward<std::vector<mapbox::base::Value>>(orientation)}};
}

mbgl::ffi::Context::Context(const char *access_token,
                            int width,
                            int height,
                            const void *nativeWindow,
                            const char *assets_path,
                            const char *cache_path,
                            const char *misc)
    : accessToken_(access_token),
      width_(width),
      height_(height),
      assetsPath_(assets_path),
      cachePath_(cache_path),
      misc_(misc),
      nativeWindow_(static_cast<const NATIVE_WINDOW *>(nativeWindow)),
      texture_(!nativeWindow) {
    std::cerr << "[ffi] Asset Path: " << assetsPath_ << std::endl;
    std::cerr << "[ffi] Cache Path: " << cachePath_ << std::endl;
    std::cerr << "[ffi] Misc: " << misc_ << std::endl;
    std::cerr << "[ffi] Texture: " << (texture_ ? "true" : "false") << std::endl;

    if (!texture_) {
        assert(nativeWindow_);
        assert(nativeWindow_->egl_display);
        assert(nativeWindow_->egl_window);
        auto q = const_cast<void *>(nativeWindow_->egl_display);
        auto egl_display = static_cast<EGLDisplay>(q);
        //        init_egl(nativeWindow_->egl_window, egl_display, egl_.eglSurface, egl_.eglContext);
    }

    auto &settings = mbgl::platform::Settings::getInstance();
    settings.set("key", "value");

    if (!texture_) {
        // set swap interval
    }

    // calculate aspect ratio

    mbgl::MapOptions mapOptions;
    if (!texture_) {
        // mapOptions.withContextMode = mbgl::ContextMode::Unique;
    } else {
        // mapOptions.withContextMode = mbgl::ContextMode::Shared;
    }
    mapOptions.withPixelRatio(aspectRatio_);
    mapOptions.withSize(mapSize_);

    mbgl::ResourceOptions resourceOptions;
    resourceOptions.withApiKey(accessToken_);
    resourceOptions.withAssetPath(assetsPath_);
    resourceOptions.withCachePath(cachePath_);

    mbgl::ClientOptions clientOptions;
    clientOptions.withName("FFI");
    clientOptions.withVersion("1.0.0");

    mbgl::TileServerOptions tileServerOptions;
    tileServerOptions.withBaseURL("http://tileserver.com");

}

mbgl::ffi::Context::~Context() {}

void Context::run_task() {}

void Context::draw_frame(uint32_t framebuffer_id) {
    (void)framebuffer_id;
}

void Context::resize(int width, int height) {}

// #############################################################################
//  Runtime Functions
// #############################################################################

bool Context::NetworkStatusGet() {
    return (NetworkStatus::Get() == NetworkStatus::Status::Online);
}

void Context::NetworkStatusSet(bool status) {
    NetworkStatus::Set(status ? NetworkStatus::Status::Online : NetworkStatus::Status::Offline);
}

void Context::InitCallbacks(ffi_camera_move_started_cb camera_move_started_cb,
                            ffi_camera_move_cb camera_move_cb,
                            ffi_camera_idle_cb camera_idle_cb,
                            ffi_map_style_loaded_cb map_style_loaded_cb,
                            ffi_camera_tracking_change_cb camera_tracking_change_cb,
                            ffi_camera_tracking_dismissed_cb camera_tracking_dismissed_cb,
                            ffi_map_idle_cb map_idle_cb,
                            ffi_user_location_update_cb user_location_update_cb,
                            ffi_query_rendered_features_cb query_rendered_features_cb) {
    cameraMoveStarted_cb_ = camera_move_started_cb;
    cameraMove_cb_ = camera_move_cb;
    cameraIdle_cb_ = camera_idle_cb;
    mapStyleLoaded_cb_ = map_style_loaded_cb;
    cameraTrackingChange_cb_ = camera_tracking_change_cb;
    cameraTrackingDismissed_cb_ = camera_tracking_dismissed_cb;
    mapIdle_cb_ = map_idle_cb;
    userLocationUpdate_cb_ = user_location_update_cb;
    queryRenderedFeatures_cb_ = query_rendered_features_cb;
}

ffi_CameraOptions Context::UpdateMapOptions(ffi_CameraOptions *options) {
    return ffi_CameraOptions();
}

bool Context::CameraAnimate(const char *json, double duration) {
    return false;
}

bool Context::CameraMove(const char *json) {
    return false;
}

void Context::UpdateMyLocationTrackingMode(size_t mode) {}

void Context::MatchMapLanguageWithDeviceDefault() {}

void Context::EdgeInsets(ffi_EdgeInsets *insets, bool animated) {}

void Context::UpdateContentInsets(ffi_EdgeInsets *insets, bool animated) {}

void Context::SetMapLanguage(const char *language) {}

void Context::SetTelemetryEnabled(bool enabled) {}

bool Context::GetTelemetryEnabled() {
    return false;
}

void Context::QueryRenderedFeatures(ffi_Point &point,
                                    const char *layer_ids,
                                    size_t layer_ids_size,
                                    const char *filter_json) {}

void Context::QueryRenderedFeatures(ffi_ScreenCoordinate &top_left,
                                    ffi_ScreenCoordinate &bottom_right,
                                    const char *layer_ids,
                                    size_t layer_ids_size,
                                    const char *filter_json) {}

void Context::QuerySourceFeatures(const char *source_id,
                                  const char *source_layer_id,
                                  const char *filter_json,
                                  const char **features,
                                  size_t features_size) {}

void Context::InvalidateAmbientCache() {}

ffi_LatLng Context::RequestMyLatLng() {
    return ffi_LatLng();
}

void Context::RequestMyLatLng(ffi_LatLng **south_west,
                              size_t south_west_size,
                              ffi_LatLng **north_west,
                              size_t north_west_size) {}

void Context::AddImage(uint8_t *image, size_t size) {}

void Context::AddImageSource(uint8_t *image, size_t size, ffi_LatLng *coordinates, size_t coordinates_size) {}

ffi_Point Context::ToScreenLocation(ffi_LatLng latlng) {
    return ffi_Point();
}

void Context::ToScreenLocationBatch(ffi_LatLng *latlng, size_t latlng_size, ffi_Point **point, size_t point_size) {}

void Context::RemoveSource(const char *id) {}

void Context::AddLayer(layer_type_t type,
                       const char *source_id,
                       const char *layer_id,
                       const char *below_layer_id,
                       const char *source_layer,
                       double min_zoom,
                       double max_zoom,
                       const char *filter_json,
                       bool enable_interaction,
                       const char *properties_json) {}

void Context::RemoveLayer(const char *layer_id) {}

void Context::SetFilter(const char *json) {}

const char *Context::GetFilter(const char *layer_id) {
    return nullptr;
}

ffi_LatLng Context::ToLatLong(ffi_Point *screen_location) {
    return ffi_LatLng();
}

double Context::GetMetersPerPixelAtLatitude(double latitude) {
    return 0;
}

void Context::AddGeoJsonSource(const char *geojson) {}

void Context::SetGeoJsonSource(const char *source_id, const char *geojson) {}

void Context::SetCameraBounds(double west, double north, double south, double east, int padding) {}

void Context::AddSource(const char *source_id, const char *json) {}

void Context::SetFeatureForGeoJsonSource(const char *source_id, const char *geojson_feature) {}

void Context::SetLayerVisibility(const char *layer_id, bool visible) {}

void Context::GetLayerIds(const char **layer_ids, size_t layer_ids_size) {}

void Context::GetSourceIds(const char **source_ids, size_t source_ids_size) {}

/* Observer Callbacks */
void Context::onCameraWillChange(MapObserver::CameraChangeMode) {}

void Context::onCameraIsChanging() {}

void Context::onCameraDidChange(MapObserver::CameraChangeMode) {}

void Context::onWillStartLoadingMap() {}

void Context::onDidFinishLoadingMap() {}

void Context::onDidFailLoadingMap(MapLoadError, const std::string &) {}

void Context::onWillStartRenderingFrame() {}

void Context::onDidFinishRenderingFrame(MapObserver::RenderFrameStatus) {}

void Context::onWillStartRenderingMap() {}

void Context::onDidFinishRenderingMap(MapObserver::RenderMode) {}

void Context::onDidFinishLoadingStyle() {
    MapObserver::onDidFinishLoadingStyle();
}

void Context::onSourceChanged(style::Source &) {}

void Context::onDidBecomeIdle() {}

void Context::onStyleImageMissing(const std::string &) {}

bool Context::onCanRemoveUnusedStyleImage(const std::string &) {
    return true;
}

void Context::onRegisterShaders(gfx::ShaderRegistry &) {}

bool Context::onRecord(EventSeverity severity, Event event, int64_t code, const std::string &msg) {
    (void)severity;
    (void)event;
    (void)code;
    (void)msg;
    return true;
}

} // namespace ffi
} // namespace mbgl
