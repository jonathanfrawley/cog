local ffi = require("ffi")
local C = ffi.load('./libcogshared.so')
ffi.cdef[[

//cog_types.h

//types
typedef int cog_anim_id;
typedef int cog_snd_id;
typedef int cog_sprite_id;
typedef int cog_text_id;
typedef int cog_text_multiline_id;
typedef void* cog_dataptr;
typedef char* cog_string;

typedef struct cog_pos2 {
    double x;
    double y;
} cog_pos2;

typedef struct cog_vec2 {
    double x;
    double y;
} cog_vec2;

typedef struct cog_dim2 {
    double w;
    double h;
} cog_dim2;

typedef struct cog_color {
    double r;
    double g;
    double b;
    double a;
} cog_color;

//cog_list.h
typedef struct cog_list {
    struct cog_list* next;
    cog_dataptr data;
    uint32_t size;              //size of data member
} cog_list;

//List structure:
//  NULL -> a0 -> a1 -> a2 -> a3 ->|
//  / \                            |
//   |-----------------------------|
// The first element has COG_LIST_ENDSENTINAL as its data field.
// The last element is a pointer to the first element again, thereby acting as a sentinal to for loops.
//
//Memory:
// All data members are alloced and dealloced by the list data structure.
// Do not rely on these data values lying around for reference elsewhere.

cog_list* cog_list_alloc(uint32_t size);

void cog_list_init(cog_list* list, uint32_t size);

//----------------------------------------------------------------------
//  Adds data to this list. List will handle freeing data.
//----------------------------------------------------------------------
void cog_list_append(cog_list* list, cog_dataptr data);

//----------------------------------------------------------------------
//  Removes elem w/ data == data in this list.
//----------------------------------------------------------------------
void cog_list_remove(cog_list* list, cog_dataptr data);

//----------------------------------------------------------------------
//  Empties list and frees data.
//----------------------------------------------------------------------
void cog_list_removeall(cog_list* list);

//----------------------------------------------------------------------
//  Returns: Length of list.
//----------------------------------------------------------------------
uint32_t cog_list_len(cog_list* list);

//----------------------------------------------------------------------
//  Pops the front element of the list and returns it.
//----------------------------------------------------------------------
cog_dataptr cog_list_pop(cog_list* list);


//cog_main.h

bool cog_hasquit();
void cog_init();
void cog_loopstep();
void cog_mainloop();
void cog_quit();

//cog_anim.h
typedef struct {
    cog_anim_id id;
    uint32_t layer;
    uint32_t current_frame;
    uint32_t current_frame_millis;
    cog_list frames;
    uint32_t n_frames;
    //User attributes
    uint32_t transition_millis;
    bool looped;
    bool paused;
    cog_pos2 pos;
    cog_dim2 dim;
    double rot;
    cog_vec2 vel;
    double ang_vel;
    bool finished;
} cog_anim;

cog_anim_id cog_anim_add(const char* anim_img, int n_frames);
bool cog_anim_collides_anim(cog_anim_id id0, cog_anim_id id1);
bool cog_anim_collides_sprite(cog_anim_id id0, cog_sprite_id id1);
cog_anim* cog_anim_get(cog_anim_id);
void cog_anim_remove(cog_anim_id id);
void cog_anim_removeall(void);
void cog_anim_set(cog_anim_id id, cog_anim src);
void cog_anim_set_frames(cog_anim_id id, int frame0, ...);

/*-----------------------------------------------------------------------------
 *  Internal
 *-----------------------------------------------------------------------------*/
double cog_anim_dist_anim(cog_anim_id id0, cog_anim_id id1);
double cog_anim_dist_sprite(cog_anim_id id0, cog_sprite_id id1);
void cog_anim_draw_layer(uint32_t layer);
void cog_anim_init();
void cog_anim_update(uint32_t deltamillis);

void cog_snd_init(void);
cog_snd_id cog_snd_add(const char* fname);
void cog_snd_play(cog_snd_id id);
void cog_snd_play_sfx(cog_snd_id snd);
void cog_snd_play_music(cog_snd_id snd);
void cog_snd_stop(cog_snd_id id);
void cog_snd_stopall();
]]
cog = {}
cog.init = C.cog_init
cog.loopstep = C.cog_loopstep
cog.mainloop = C.cog_mainloop
cog.hasquit = C.cog_hasquit
cog.quit = C.cog_quit

cog.dim2 = {}
function cog.dim2.new(w, h) return ffi.new("cog_dim2", w, h) end
cog.vec2 = {}
function cog.vec2.new(x, y) return ffi.new("cog_vec2", x, y) end
cog.pos2 = {}
function cog.pos2.new(x, y) return ffi.new("cog_pos2", x, y) end

cog.anim = {}
cog.anim.add = C.cog_anim_add
cog.anim.get = C.cog_anim_get
function cog.anim.new() return ffi.new("cog_anim") end
cog.anim_set = C.cog_anim_set
cog.anim_set_frames = C.cog_anim_set_frames

cog.snd = {}
cog.snd.add = C.cog_snd_add
cog.snd.play = C.cog_snd_play

return cog

--return C
