
#include "../include/mbgl_ffi.h"

#include <mbgl/gfx/backend.hpp>
#include <mbgl/style/style.hpp>

#include <iostream>
#include <memory>
#include <vector>
#include <mutex>

#include "context.hpp"

// #############################################################################

#ifdef NDEBUG
#define TRACE(x)
#else
#define TRACE(x) (std::cerr << x << std::endl)
#endif

#define API_EXPORT __attribute__((visibility("default")))

// #############################################################################
//  Common
// #############################################################################

extern "C" struct ffi_Context {
    std::shared_ptr<mbgl::ffi::Context> context;
};

std::vector<struct ffi_Context*> gContextList;
std::mutex gContextListMutex;

namespace {
inline void checkContext(ffi_Context* ctx) {
    if (ctx == nullptr) {
        std::cerr << "Context is null" << std::endl;
        std::abort();
    }
    if (std::find(gContextList.begin(), gContextList.end(), ctx) == gContextList.end()) {
        std::cerr << "Invalid Context" << std::endl;
        std::abort();
    }
    if (ctx->context == nullptr) {
        std::cerr << "Context pointer is null" << std::endl;
        std::abort();
    }
}

inline mbgl::ffi::Context& getContext(ffi_Context* ctx) {
    checkContext(ctx);
    return *ctx->context;
}
} // namespace

// #############################################################################
//  Lifecycle API
// #############################################################################

// Interface Version
API_EXPORT
uint32_t ffi_version() {
    TRACE("ffi_version");
    return 0x00010000;
}

API_EXPORT
ffi_Context* ffi_initialize(const char* access_token,
                            int width,
                            int height,
                            const void* native_window,
                            const char* assets_path,
                            const char* cache_path,
                            const char* misc) {
    TRACE("ffi_initialize");
    auto* ctx = new ffi_Context;
    ctx->context = std::make_shared<mbgl::ffi::Context>(
        access_token, width, height, native_window, assets_path, cache_path, misc);
    gContextListMutex.lock();
    gContextList.emplace_back(ctx);
    gContextListMutex.unlock();
    return ctx;
}

API_EXPORT
void ffi_draw_frame(ffi_Context* ctx, uint32_t framebufferId) {
    getContext(ctx).draw_frame(framebufferId);
}

API_EXPORT
void ffi_run_task(ffi_Context* ctx) {
    getContext(ctx).run_task();
}

API_EXPORT
void ffi_resize(ffi_Context* ctx, int width, int height) {
    TRACE("ffi_resize");
    getContext(ctx).resize(width, height);
}

API_EXPORT
void ffi_de_initialize(ffi_Context* ctx) {
    TRACE("ffi_de_initialize");
    getContext(ctx).~Context();

    gContextListMutex.lock();
    gContextList.erase(std::remove(gContextList.begin(), gContextList.end(), ctx), gContextList.end());
    gContextListMutex.unlock();
    delete ctx;
}

// #############################################################################
//  Control API
// #############################################################################

API_EXPORT
void ffi_set_network_status(bool status) {
    mbgl::ffi::Context::NetworkStatusSet(status);
}

API_EXPORT
bool ffi_get_network_status() {
    return mbgl::ffi::Context::NetworkStatusGet();
}

API_EXPORT
bool ffi_get_network_status();

API_EXPORT
void ffi_init_callbacks(ffi_Context* ctx,
                        ffi_camera_move_started_cb camera_move_started_cb,
                        ffi_camera_move_cb camera_move_platform_cb,
                        ffi_camera_idle_cb camera_idle_cb,
                        ffi_map_style_loaded_cb map_style_loaded_cb,
                        ffi_camera_tracking_change_cb camera_tracking_change_cb,
                        ffi_camera_tracking_dismissed_cb camera_tracking_dismissed_cb,
                        ffi_map_idle_cb map_idle_cb,
                        ffi_user_location_update_cb user_location_update_cb,
                        ffi_query_rendered_features_cb query_rendered_features_cb) {
    TRACE("ffi_init_callbacks");
    return getContext(ctx).InitCallbacks(camera_move_started_cb,
                                         camera_move_platform_cb,
                                         camera_idle_cb,
                                         map_style_loaded_cb,
                                         camera_tracking_change_cb,
                                         camera_tracking_dismissed_cb,
                                         map_idle_cb,
                                         user_location_update_cb,
                                         query_rendered_features_cb);
}

API_EXPORT
ffi_CameraOptions ffi_update_map_options(ffi_Context* ctx, ffi_CameraOptions* options) {
    TRACE("ffi_update_map_options");
    return getContext(ctx).UpdateMapOptions(options);
}

API_EXPORT
bool ffi_camera_animate(ffi_Context* ctx, const char* json, double duration) {
    TRACE("ffi_camera_animate");
    return getContext(ctx).CameraAnimate(json, duration);
}

API_EXPORT
bool ffi_camera_move(ffi_Context* ctx, const char* json) {
    TRACE("ffi_camera_move");
    return getContext(ctx).CameraMove(json);
}

API_EXPORT
void ffi_update_my_location_tracking_mode(ffi_Context* ctx, size_t mode) {
    TRACE("ffi_update_my_location_tracking_mode");
    return getContext(ctx).UpdateMyLocationTrackingMode(mode);
}

API_EXPORT
void ffi_match_map_language_with_device_default(ffi_Context* ctx) {
    TRACE("ffi_match_map_language_with_device_default");
    return getContext(ctx).MatchMapLanguageWithDeviceDefault();
}

API_EXPORT
void ffi_update_content_insets(ffi_Context* ctx, ffi_EdgeInsets* insets, bool animated) {
    TRACE("ffi_update_content_insets");
    return getContext(ctx).UpdateContentInsets(insets, animated);
}

API_EXPORT
void ffi_set_map_language(ffi_Context* ctx, const char* language) {
    TRACE("ffi_set_map_language");
    getContext(ctx).SetMapLanguage(language);
}

API_EXPORT
void ffi_set_telemetry_enabled(ffi_Context* ctx, bool enabled) {
    TRACE("ffi_set_telemetry_enabled");
    getContext(ctx).SetTelemetryEnabled(enabled);
}

API_EXPORT
bool ffi_get_telemetry_enabled(ffi_Context* ctx) {
    TRACE("ffi_get_telemetry_enabled");
    return getContext(ctx).GetTelemetryEnabled();
}

API_EXPORT
void ffi_query_rendered_features(
    ffi_Context* ctx, ffi_Point& point, const char* layer_ids, size_t layer_ids_size, const char* filter_json) {
    TRACE("ffi_query_rendered_features");
    getContext(ctx).QueryRenderedFeatures(point, layer_ids, layer_ids_size, filter_json);
}

API_EXPORT
void ffi_query_rendered_features_in_rectangle(ffi_Context* ctx,
                                              ffi_ScreenCoordinate& top_left,
                                              ffi_ScreenCoordinate& bottom_right,
                                              const char* layer_ids,
                                              size_t layer_ids_size,
                                              const char* filter_json) {
    TRACE("ffi_query_rendered_features_in_rectangle");
    getContext(ctx).QueryRenderedFeatures(top_left, bottom_right, layer_ids, layer_ids_size, filter_json);
}

API_EXPORT
void ffi_query_source_features(ffi_Context* ctx,
                               const char* source_id,
                               const char* source_layer_id,
                               const char* filter_json,
                               const char** features,
                               size_t features_size) {
    TRACE("ffi_query_source_features");
    getContext(ctx).QuerySourceFeatures(source_id, source_layer_id, filter_json, features, features_size);
}

API_EXPORT
void ffi_invalidate_ambient_cache(ffi_Context* ctx) {
    TRACE("ffi_invalidate_ambient_cache");
    getContext(ctx).InvalidateAmbientCache();
}

API_EXPORT
ffi_LatLng ffi_request_my_lat_lng(ffi_Context* ctx) {
    TRACE("ffi_request_my_lat_lng");
    return getContext(ctx).RequestMyLatLng();
}

API_EXPORT
void ffi_get_visible_region(ffi_Context* ctx,
                            ffi_LatLng** south_west,
                            size_t south_west_size,
                            ffi_LatLng** north_west,
                            size_t north_west_size) {
    TRACE("ffi_get_visible_region");
    getContext(ctx).RequestMyLatLng(south_west, south_west_size, north_west, north_west_size);
}

API_EXPORT
void ffi_add_image(ffi_Context* ctx, uint8_t* image, size_t size) {
    TRACE("ffi_add_image");
    getContext(ctx).AddImage(image, size);
}

API_EXPORT
void ffi_add_image_source(
    ffi_Context* ctx, uint8_t* image, size_t size, ffi_LatLng* coordinates, size_t coordinates_size) {
    TRACE("ffi_add_image_source");
    getContext(ctx).AddImageSource(image, size, coordinates, coordinates_size);
}

API_EXPORT
ffi_Point ffi_to_screen_location(ffi_Context* ctx, ffi_LatLng latlng) {
    TRACE("ffi_to_screen_location");
    return getContext(ctx).ToScreenLocation(latlng);
}

API_EXPORT
void ffi_to_screen_location_batch(
    ffi_Context* ctx, ffi_LatLng* latlng, size_t latlng_size, ffi_Point** point, size_t point_size) {
    TRACE("ffi_to_screen_location_batch");
    getContext(ctx).ToScreenLocationBatch(latlng, latlng_size, point, point_size);
}

API_EXPORT
void ffi_remove_source(ffi_Context* ctx, const char* id) {
    TRACE("ffi_remove_source");
    getContext(ctx).RemoveSource(id);
}

API_EXPORT
void ffi_add_layer(ffi_Context* ctx,
                   layer_type_t type,
                   const char* source_id,
                   const char* layer_id,
                   const char* below_layer_id,
                   const char* source_layer,
                   double min_zoom,
                   double max_zoom,
                   const char* filter_json,
                   bool enable_interaction,
                   const char* properties_json) {
    TRACE("ffi_add_layer");
    getContext(ctx).AddLayer(type,
                             source_id,
                             layer_id,
                             below_layer_id,
                             source_layer,
                             min_zoom,
                             max_zoom,
                             filter_json,
                             enable_interaction,
                             properties_json);
}

API_EXPORT
void ffi_remove_layer(ffi_Context* ctx, const char* layer_id) {
    TRACE("ffi_remove_layer");
    getContext(ctx).RemoveLayer(layer_id);
}

API_EXPORT
void ffi_set_filter(ffi_Context* ctx, const char* json) {
    TRACE("ffi_set_filter");
    getContext(ctx).SetFilter(json);
}

API_EXPORT
const char* ffi_get_filter(ffi_Context* ctx, const char* layer_id) {
    TRACE("ffi_get_filter");
    return getContext(ctx).GetFilter(layer_id);
}

API_EXPORT
ffi_LatLng ffi_to_lat_lng(ffi_Context* ctx, ffi_Point* screen_location) {
    TRACE("ffi_to_lat_lng");
    return getContext(ctx).ToLatLong(screen_location);
}

API_EXPORT
double ffi_get_meters_per_pixel_at_latitude(ffi_Context* ctx, double latitude) {
    TRACE("ffi_get_meters_per_pixel_at_latitude");
    return getContext(ctx).GetMetersPerPixelAtLatitude(latitude);
}

API_EXPORT
void ffi_add_geojson_source(ffi_Context* ctx, const char* geojson) {
    TRACE("ffi_add_geojson_source");
    getContext(ctx).AddGeoJsonSource(geojson);
}

API_EXPORT
void ffi_set_geojson_source(ffi_Context* ctx, const char* source_id, const char* geojson) {
    TRACE("ffi_camera_animate");
    getContext(ctx).SetGeoJsonSource(source_id, geojson);
}

API_EXPORT
void ffi_set_camera_bounds(ffi_Context* ctx, double west, double north, double south, double east, int padding) {
    TRACE("ffi_set_camera_bounds");
    getContext(ctx).SetCameraBounds(west, north, south, east, padding);
}

API_EXPORT
void ffi_add_source(ffi_Context* ctx, const char* source_id, const char* json) {
    TRACE("ffi_add_source");
    getContext(ctx).AddSource(source_id, json);
}

API_EXPORT
void ffi_set_feature_for_geojson_source(ffi_Context* ctx, const char* source_id, const char* geojson_feature) {
    TRACE("ffi_set_feature_for_geojson_source");
    getContext(ctx).SetFeatureForGeoJsonSource(source_id, geojson_feature);
}

API_EXPORT
void ffi_set_layer_visibility(ffi_Context* ctx, const char* layer_id, bool visible) {
    TRACE("ffi_set_layer_visibility");
    getContext(ctx).SetLayerVisibility(layer_id, visible);
}

API_EXPORT
void ffi_get_layer_ids(ffi_Context* ctx, const char** layer_ids, size_t layer_ids_size) {
    TRACE("ffi_get_layer_ids");
    getContext(ctx).GetLayerIds(layer_ids, layer_ids_size);
}

API_EXPORT
void ffi_get_source_ids(ffi_Context* ctx, const char** source_ids, size_t source_ids_size) {
    TRACE("ffi_camera_animate");
    getContext(ctx).GetSourceIds(source_ids, source_ids_size);
}
