#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "viz.h"

ALLEGRO_DISPLAY *display;
ALLEGRO_EVENT_QUEUE *event_queue;
ALLEGRO_TIMER *timer;
const int FONTS = 32;
ALLEGRO_FONT *font[FONTS];

ALLEGRO_MOUSE_STATE mouse;

bool viz_init() {  
    if(!al_init()) {
        printf("failed to initialize allegro!\n");
        return false;
    }

    display = al_create_display(800, 800);
    if(!display) {
        printf("failed to create display!\n");
        return false;
    }

    al_install_keyboard();
    al_install_mouse();

    al_init_primitives_addon();
    al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon();// initialize the ttf (True Type Font) addon

    for(int i=0; i<32; i++) {
        font[i] = al_load_ttf_font("/usr/share/fonts/truetype/freefont/FreeMono.ttf", 5+i,0 );
    }

    if (!font){
        printf("Could not load font.\n");
        return false;
    }

    timer = al_create_timer(1.0 / 60);
    if(!timer) {
        printf("failed to create timer!\n");
        return false;
    }

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    al_start_timer(timer);
}

bool viz_update() {
    static bool redraw = false;
    ALLEGRO_EVENT ev;
    al_wait_for_event(event_queue, &ev);

    if(ev.type == ALLEGRO_EVENT_TIMER) {
        redraw = true;
    } else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        return false;
    } 

    if(redraw && al_event_queue_is_empty(event_queue)) {
        redraw = false;
    }
    return true;
}

float_point_t viz_mouse() {
    al_get_mouse_state(&mouse);

    float_point_t p;
    p.x = mouse.x;
    p.y = mouse.y;
    return p;
}


void viz_destroy() {
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
}

void viz_rect(float_point_t o, float w, float h, ALLEGRO_COLOR col, float thick) {
    if(thick <= 0)
        al_draw_filled_rectangle(o.x, o.y, o.x + w, o.y + h, col);
    else
        al_draw_rectangle(o.x, o.y, o.x + w, o.y + h, col, thick);
}

void viz_circle(float_point_t p, float r, ALLEGRO_COLOR col, float thick) {
    if(thick <= 0)
        al_draw_filled_circle(p.x, p.y, r, col);
    else
        al_draw_circle(p.x, p.y, r, col, thick);
}

void viz_line(float_point_t a, float_point_t b, ALLEGRO_COLOR col, float thick) {
    al_draw_line(a.x, a.y, b.x, b.y, col, thick);
}


void viz_arc(float cx, float cy, float r, float start_theta, float delta_theta, 
             ALLEGRO_COLOR col, float thick) {

    al_draw_arc(cx, cy, r, start_theta, delta_theta, col, thick);
}

void viz_triangle(float_point_t a, float_point_t b, float_point_t c, ALLEGRO_COLOR col, float thick) {

    if(thick <= 0)
        al_draw_filled_triangle(a.x, a.y, b.x, b.y, c.x, c.y, col);
    else    
        al_draw_triangle(a.x, a.y, b.x, b.y, c.x, c.y, col, thick);
}

void viz_text(float x, float y, int dim, ALLEGRO_COLOR col, const char *format, ...) {

    int font_id = dim-5;
    if(font_id <0) font_id=0;
    if(font_id >FONTS-1) font_id=FONTS-1;

    char buf[512];
    va_list va;
    va_start (va, format);
    vsprintf (buf, format, va);
    va_end (va);

    al_draw_text(font[font_id], col, x, y, 0, buf);
}

void viz_clear() {
    al_clear_to_color(al_map_rgb(0,0,0));
}

void viz_flip() {
    al_flip_display();
}

void draw_rotated_rectangle(float_point_t o, float w, float h, float angle, ALLEGRO_COLOR col) {

    float_point_t A, B, C, D;
    /*
        A -- B
        |    |
        D -- C
    */
    A.x = o.x - w/2; A.y = o.y - h/2;
    B.x = o.x + w/2; B.y = o.y - h/2;
    C.x = o.x + w/2; C.y = o.y + h/2;
    D.x = o.x - w/2; D.y = o.y + h/2;

    rotate_point(A, o, angle);
    rotate_point(B, o, angle);
    rotate_point(C, o, angle);
    rotate_point(D, o, angle);

    al_draw_line(A.x, A.y, B.x, B.y, col, 1);
    al_draw_line(B.x, B.y, C.x, C.y, col, 1);
    al_draw_line(C.x, C.y, D.x, D.y, col, 1);
    al_draw_line(D.x, D.y, A.x, A.y, col, 1);
}

void draw_drive_params(view_t &view, float throttle, float steer, float speed) {

    float_point_t origin;
    origin.x = view.x;
    origin.y = view.y + view.l + 10; 

    float t_height = 80;
    float t_width  = 10;

    //throttle
    ALLEGRO_COLOR t_col = al_map_rgba_f(0, 0, 0, 1);
    float t_value = (throttle /100) * (t_height/2);
    if(t_value < 0)
        t_col.r = 1.0f;
    else    
        t_col.g = 1.0f;

    al_draw_filled_rectangle(origin.x, origin.y + t_height/2, origin.x + t_width, origin.y + t_height/2 - t_value, t_col);
    al_draw_rectangle(origin.x, origin.y, origin.x + t_width, origin.y + t_height, VIEW_COLOR, 1);


    //steer
    origin.x += t_width + 10;
    float w_height = t_height/2;
    float w_width = t_width;

    float_point_t weel1o, weel2o;
    weel1o.x = origin.x + w_width/2 + 15;
    weel1o.y = origin.y + w_height/2;
    weel2o.x = weel1o.x + 80;
    weel2o.y = weel1o.y;
    
    float s_value = (steer/100) * M_PI/4;
    al_draw_line(weel1o.x, weel1o.y, weel2o.x, weel2o.y, VIEW_COLOR, 1);
    draw_rotated_rectangle(weel1o, w_width, w_height, s_value, VIEW_COLOR);
    draw_rotated_rectangle(weel2o, w_width, w_height, s_value, VIEW_COLOR);

    al_draw_textf(font[10], VIEW_COLOR, origin.x, origin.y + w_height +5, 0,  "throttle: %4.0f", throttle);
    al_draw_textf(font[10], VIEW_COLOR, origin.x, origin.y + w_height +25, 0, "steer:    %4.0f", steer);


    //speed
    static float smooth_speed =0;
    float lerp = 0.02;
    if(fabs(smooth_speed - speed) > lerp)
        smooth_speed < speed ? smooth_speed += lerp : smooth_speed -= lerp;

    origin.x = weel2o.x + w_height;
    float_point_t tacho, tach;
    tacho.x = origin.x + t_height/2;
    tacho.y = origin.y + t_height/2;
    tach.x = origin.x + t_height -5;
    tach.y = origin.y + t_height/2;

    float tach_start = M_PI/2 + M_PI/4, tach_ang = M_PI + M_PI/4;
    
    float v_value = tach_start + (smooth_speed/5 * tach_ang);
    rotate_point(tach, tacho, v_value);

    al_draw_line(tacho.x, tacho.y, tach.x, tach.y, VIEW_COLOR, 1);
    al_draw_circle(tacho.x, tacho.y, 2, VIEW_COLOR, 1);
    al_draw_arc(tacho.x, tacho.y, t_height/2, tach_start, tach_ang, VIEW_COLOR, 1);

    al_draw_textf(font[10], VIEW_COLOR, origin.x + t_height/2, origin.y + w_height +25, 0, "%2.1f m/s", speed);
}
