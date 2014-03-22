#include <stdio.h>
#include <jansson.h>

#include <cog.h>

//TILESET_w is tileset.image.width / tileset.tilewidth, same for h
void cog_tiled_load_background(double x, double y, double w, double h,
                               const char* tileset_path, int32_t tileset_w, int32_t tileset_h,
                               int32_t tile_layer_w, int32_t tile_layer_h, int32_t* ids, int32_t n_ids, cog_list* out_anims) {
    //Load in so the x, y is in the top-left and
    double elem_w = w / (double)tile_layer_w;
    double elem_h = h / (double)tile_layer_h;
    for(int i=0; i < n_ids; i++) {
        int32_t id = ids[i] - 1;
        if(id == -1) {
            continue;
        }
        cog_anim_id anim = cog_anim_add(tileset_path, tileset_h, tileset_w);
        cog_anim_set(anim, (cog_anim) {
            .dim = (cog_dim2) {.w=elem_w, .h=elem_h},
            .pos = (cog_pos2) {.x=x + (i%tile_layer_w)*elem_w*2 + elem_w, .y=y - ((i/tile_layer_h)*elem_h*2 + elem_h)},
            .paused = COG_TRUE
        });
        /*
        cog_debugf("id %d ", ids[i]-1);
        cog_debugf("elem_h %lf ", elem_h);
        cog_debugf("elem_w %lf ", elem_w);
        cog_debugf("x %lf ",x+((i % tile_layer_w) * elem_w));
        cog_debugf("y %lf ",y-((i / tile_layer_h) * elem_h));
        */
        cog_anim_set_frames(anim, id);
        cog_anim_set_frame(anim, id);
    }
}

typedef struct cog_tiled_image {
    char* source;
    int width;
    int height;
} cog_tiled_image;

typedef struct cog_tiled_tileset {
    int firstgid;
    char* name;
    int width;
    int height;
    cog_tiled_image image;
} cog_tiled_tileset;

typedef struct cog_tiled_property {
    char* name;
    char* value;
} cog_tiled_property;

typedef struct cog_tiled_tile {
    int gid;
} cog_tiled_tile;

typedef struct cog_tiled_data {
    cog_list tiles;
} cog_tiled_data;

typedef struct cog_tiled_object {
    char* name;
    int x;
    int y;
    int width;
    int height;
} cog_tiled_object;

typedef struct cog_tiled_objectgroup {
    char* name;
    int width;
    int height;
    cog_list objects;
} cog_tiled_objectgroup;

typedef struct cog_tiled_layer {
    cog_list properties;
    cog_tiled_data data;
    cog_list objectgroups;
} cog_tiled_layer;

typedef struct cog_tiled_map {
    int width;
    int height;
    int tile_width;
    int tile_height;
    cog_tiled_tileset tileset;
    cog_list layers;
} cog_tiled_map;


int32_t* read_in_tiled_map(const char* tiled_json_filename, int32_t* data_size) {
    json_error_t error;
    json_t* json = json_load_file(tiled_json_filename, 0, &error);
    if(!json) {
        cog_errorf("Could not load json file %s : %s", tiled_json_filename, error.text);
    } else {
        json_t* width_json = json_object_get(json, "width");
        double width = json_number_value(width_json);
        cog_infof("Parsed in width %lf", width);
        //Parsing in layers
        json_t* layers_json = json_object_get(json, "layers");
        json_t* layer0_json = json_array_get(layers_json, 0);
        json_t* layer0_data_json = json_object_get(layer0_json, "data");
        int32_t size = json_array_size(layer0_data_json);
        (*data_size) = size;
        int32_t* data_arr = (int32_t*)cog_malloc(size*sizeof(int32_t));
        for(int i = 0; i < size ; i++) {
            json_t* array_entry_json = json_array_get(layer0_data_json, i);
            int32_t val = json_integer_value(array_entry_json);
            data_arr[i] = val;
            //cog_debugf("val is %d", val);
        }
        json_decref(json);
        return data_arr;
    }
    return 0;
}

int main(int argc, char** argv) {
    cog_init();
    int32_t size;
    int32_t* ids = read_in_tiled_map("level.json", &size);
    //for(int i = 0; i < size ; i++) {
    //    cog_debugf("i is %d", ids[i]);
    //}
    cog_list out_anims;
    cog_list_init(&out_anims, sizeof(cog_anim_id));
    cog_tiled_load_background(-1.0, 1.0, 10.0, 10.0,
                              "media/tileset.png", 2, 2, 100, 100, ids, size, &out_anims);
    while(!cog_hasquit()) {
        cog_loopstep();
        cog_vec2 cam_vel;
        cog_graphics_cam_vel_get(&cam_vel);
        if(cog_input_key_pressed()) {
            uint32_t key = cog_input_key_code_pressed();
            double delta = 1.0;
            if(key == 'w') {
                cam_vel.y = delta;
            }
            if(key == 'a') {
                cam_vel.x = -delta;
            }
            if(key == 's') {
                cam_vel.y = -delta;
            }
            if(key == 'd') {
                cam_vel.x = delta;
            }
        }
        if(cog_input_key_depressed()) {
            uint32_t key = cog_input_key_code_depressed();
            if(key == 'w') {
                cam_vel.y = 0;
            }
            if(key == 'a') {
                cam_vel.x = 0;
            }
            if(key == 's') {
                cam_vel.y = 0;
            }
            if(key == 'd') {
                cam_vel.x = 0;
            }
        }
        cog_graphics_cam_vel_set(&cam_vel);
    }
}
