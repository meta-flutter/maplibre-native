
#ifndef MBGL_FFI_H
#define MBGL_FFI_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// #############################################################################
//  Types
// #############################################################################

struct ffi_Point {
    double x;
    double y;
};

struct ffi_LatLng {
    double lat;
    double lng;
};

struct ffi_CameraPosition {
    double todo;
};

struct ffi_Location {
    ffi_LatLng location;
    double altitude;
    double bearing;
    int speed;
    double horizontalAccuracy;
    double verticalAccuracy;
};

struct ffi_Heading {
    double magneticHeading;
    double trueHeading;
    double headingAccuracy;
    double x;
    double y;
    double z;
    double timestamp_ms;
};

struct ffi_EdgeInsets {
    double top;
    double left;
    double bottom;
    double right;
};

struct ffi_ScreenCoordinate {
    double x;
    double y;
};

struct ffi_CameraOptions {
    struct {
        ffi_LatLng value;
        bool valid;
    } center;

    struct {
        ffi_EdgeInsets value;
        bool valid;
    } padding;

    struct {
        ffi_ScreenCoordinate value;
        bool valid;
    } anchor;

    struct {
        double value;
        bool valid;
    } zoom;

    struct {
        double bearing;
        bool valid;
    } bearing;

    struct {
        double value;
        bool valid;
    } pitch;
};

typedef struct ffi_Context ffi_Context;

typedef enum {
    SYMBOL,
    LINE,
    CIRCLE,
    FILL,
    RASTER,
    HILL_SHADE,
} layer_type_t;

// #############################################################################
//  Function Pointers
// #############################################################################

typedef void (*ffi_info_window_tapped_cb)(const char* symbol_id);

typedef void (*ffi_feature_tapped_cb)(const char* id, ffi_Point* point, ffi_LatLng* latlng);

typedef void (*ffi_feature_dragged_cb)(const char* id,
                                       ffi_Point* point,
                                       ffi_LatLng* origin,
                                       ffi_LatLng* current,
                                       ffi_LatLng* delta,
                                       const char* event_type);

typedef void (*ffi_camera_move_started_cb)();

typedef void (*ffi_camera_move_cb)(ffi_CameraPosition* position);

typedef void (*ffi_camera_idle_cb)(ffi_CameraPosition* position);

typedef void (*ffi_map_style_loaded_cb)();

typedef void (*ffi_map_click_cb)(ffi_Point* point, ffi_LatLng* latlng);

typedef void (*ffi_map_long_click_cb)(ffi_Point* point, ffi_LatLng* latlng);

typedef void (*ffi_camera_tracking_change_cb)(int mode);

typedef void (*ffi_camera_tracking_dismissed_cb)();

typedef void (*ffi_map_idle_cb)();

typedef void (*ffi_user_location_update_cb)(struct ffi_UserLocation* location, struct ffi_Heading* heading);

typedef void (*ffi_query_rendered_features_cb)(void const* userdata, const unsigned char* json);

typedef void (*ffi_info_window_tapped_cb)(const char* symbol_id);

typedef void (*ffi_feature_tapped_cb)(const char* id, ffi_Point* point, ffi_LatLng* latlng);

typedef void (*ffi_feature_dragged_cb)(const char* id,
                                       ffi_Point* point,
                                       ffi_LatLng* origin,
                                       ffi_LatLng* current,
                                       ffi_LatLng* delta,
                                       const char* event_type);

// #############################################################################
//  Lifecycle Functions
// #############################################################################

uint32_t ffi_version();

ffi_Context* ffi_initialize(const char* access_token,
                            int width,
                            int height,
                            const void* native_window,
                            const char* assets_path,
                            const char* cache_path,
                            const char* misc,
                            ffi_camera_move_started_cb camera_move_started_cb,
                            ffi_camera_move_cb camera_move_platform_cb,
                            ffi_camera_idle_cb camera_idle_cb,
                            ffi_map_style_loaded_cb map_style_loaded_cb,
                            ffi_map_click_cb map_click_cb,
                            ffi_map_long_click_cb map_long_click_cb,
                            ffi_camera_tracking_change_cb camera_tracking_change_cb,
                            ffi_camera_tracking_dismissed_cb camera_tracking_dismissed_cb,
                            ffi_map_idle_cb map_idle_cb,
                            ffi_user_location_update_cb user_location_update_cb,
                            ffi_query_rendered_features_cb query_rendered_features_cb);

void ffi_draw_frame(ffi_Context* ctx, uint32_t framebufferId);

void ffi_run_task(ffi_Context* ctx);

void ffi_resize(ffi_Context* ctx, int width, int height);

void ffi_de_initialize(ffi_Context* ctx);

// #############################################################################
//  Runtime Functions
// #############################################################################

void ffi_set_network_status(bool status);

bool ffi_get_network_status();

ffi_CameraOptions ffi_update_map_options(ffi_Context* ctx, ffi_CameraOptions* options);

bool ffi_camera_animate(ffi_Context* ctx, const char* json, double duration);

bool ffi_camera_move(ffi_Context* ctx, const char* json);

void ffi_update_my_location_tracking_mode(ffi_Context* ctx, size_t mode);

void ffi_match_map_language_with_device_default(ffi_Context* ctx);

void ffi_update_content_insets(ffi_Context* ctx, ffi_EdgeInsets* insets, bool animated);

void ffi_set_map_language(ffi_Context* ctx, const char* language);

void ffi_set_telemetry_enabled(ffi_Context* ctx, bool enabled);

bool ffi_get_telemetry_enabled(ffi_Context* ctx);

void ffi_query_rendered_features(
    ffi_Context* ctx, ffi_Point& point, int layer_ids_count, const char** layer_ids, const char* filter_json);

void ffi_query_rendered_features_in_rectangle(ffi_Context* ctx,
                                              ffi_ScreenCoordinate& top_left,
                                              ffi_ScreenCoordinate& bottom_right,
                                              int layer_ids_count,
                                              const char** layer_ids,
                                              size_t layer_ids_size,
                                              const char* filter_json);

void ffi_query_source_features(ffi_Context* ctx,
                               const char* source_id,
                               const char* source_layer_id,
                               const char* filter_json,
                               const char** features,
                               size_t features_size);

void ffi_invalidate_ambient_cache(ffi_Context* ctx);

ffi_LatLng ffi_request_my_lat_lng(ffi_Context* ctx);

void ffi_get_visible_region(
    ffi_Context* ctx, ffi_LatLng** south_west, size_t south_west_size, ffi_LatLng** north_west, size_t north_west_size);

void ffi_add_image(ffi_Context* ctx, uint8_t* image, size_t size);

void ffi_add_image_source(
    ffi_Context* ctx, uint8_t* image, size_t size, ffi_LatLng* coordinates, size_t coordinates_size);

ffi_Point ffi_to_screen_location(ffi_Context* ctx, ffi_LatLng latlng);

void ffi_to_screen_location_batch(
    ffi_Context* ctx, ffi_LatLng* latlng, size_t latlng_size, ffi_Point** point, size_t point_size);

void ffi_remove_source(ffi_Context* ctx, const char* id);

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
                   const char* properties_json);

void ffi_remove_layer(ffi_Context* ctx, const char* layer_id);

void ffi_set_filter(ffi_Context* ctx, const char* json);

const char* ffi_get_filter(ffi_Context* ctx, const char* layer_id);

ffi_LatLng ffi_to_lat_lng(ffi_Context* ctx, ffi_Point* screen_location);

double ffi_get_meters_per_pixel_at_latitude(ffi_Context* ctx, double latitude);

void ffi_add_geojson_source(ffi_Context* ctx, const char* geojson);

void ffi_set_geojson_source(ffi_Context* ctx, const char* source_id, const char* geojson);

void ffi_set_camera_bounds(ffi_Context* ctx, double west, double north, double south, double east, int padding);

void ffi_add_source(ffi_Context* ctx, const char* source_id, const char* json);

void ffi_set_feature_for_geojson_source(ffi_Context* ctx, const char* source_id, const char* geojson_feature);

void ffi_set_layer_visibility(ffi_Context* ctx, const char* layer_id, bool visible);

void ffi_get_layer_ids(ffi_Context* ctx, const char** layer_ids, size_t layer_ids_size);

void ffi_get_source_ids(ffi_Context* ctx, const char** source_ids, size_t source_ids_size);

#ifdef __cplusplus
}
#endif

#endif // MBGL_FFI_H
