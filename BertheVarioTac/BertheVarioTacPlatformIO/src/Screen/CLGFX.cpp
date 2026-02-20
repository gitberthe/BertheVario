////////////////////////////////////////////////////////////////////////////////
/// \file CLGFX.cpp
///
/// \brief Encapsulation de l'ecran bas niveau
///
/// \date creation     : 21/09/2024
/// \date modification : 24/01/2025
///

#include "../BertheVarioTac.h"

CLGFX g_tft;

/*Change to your screen resolution*/
static const uint32_t screenWidth  = 240;
static const uint32_t screenHeight = 320;
static lv_disp_draw_buf_t draw_buf;
const int size_buf = 1024 ;
static lv_color_t buf1[ size_buf ];
//static lv_color_t buf2[ screenWidth * 10 ];

////////////////////////////////////////////////////////////////////////////////
/// \brief Display flushing
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
//uint32_t w = ( area->x2 - area->x1 + 1 );
//uint32_t h = ( area->y2 - area->y1 + 1 );

/*g_tft.startWrite();
g_tft.setAddrWindow( area->x1, area->y1, w, h );
//g_tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
//g_tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
g_tft.fillRect(0, 0,240,320,TFT_WHITE);
g_tft.endWrite();*/

//if ( sync_berthe )
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
 lv_disp_flush_ready( disp );
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief lit le touch par et met a jour g_GlobalVar.m_Screen.
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
g_GlobalVar.m_Screen.m_MutexTft.PrendreMutex() ;
uint16_t touchX, touchY;

bool touched = g_tft.getTouch( &touchX, &touchY);
if( !touched )
    {
    data->state = LV_INDEV_STATE_REL;
    //g_GlobalVar.m_Screen.SetPressed(false) ;
    }
else
    {
    data->state = LV_INDEV_STATE_PR;

    /*Set the coordinates*/
    data->point.x = touchX;
    data->point.y = touchY;

    //Serial.print( "Data x " );
    //Serial.println( touchX );
    g_GlobalVar.m_Screen.SetX( touchX );

    //Serial.print( "Data y " );
    //Serial.println( touchY );
    g_GlobalVar.m_Screen.SetY( touchY ) ;

    g_GlobalVar.m_Screen.SetPressed() ;
    }
g_GlobalVar.m_Screen.m_MutexTft.RelacherMutex() ;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialise l'ecran et le touch pad
void CLGFX::InitScreen()
{
g_tft.begin();
g_tft.setRotation(7);
uint16_t calData[] = { 317,3787,3847,3813,331,232,3860,238 };

/*
// procedure de calibration à faire une fois
g_tft.calibrateTouch( calData , TFT_WHITE , TFT_BLACK ) ;
for ( int ic = 0 ; ic < 8 ; ic++ )
   {
   Serial.print( calData[ic] ) ;
   Serial.print( "," ) ;
   }
*/

g_tft.setTouchCalibrate(calData);

// touch_calibrate();//屏幕校准
lv_init();
lv_disp_draw_buf_init( &draw_buf, buf1, NULL , size_buf );

// Initialize the display
static lv_disp_drv_t disp_drv;
lv_disp_drv_init(&disp_drv);

// Change the following line to your display resolution
disp_drv.hor_res = 320;
disp_drv.ver_res = 240;
disp_drv.flush_cb = my_disp_flush;
disp_drv.draw_buf = &draw_buf;
lv_disp_drv_register(&disp_drv);

// Initialize the (dummy) input device driver
static lv_indev_drv_t indev_drv;
lv_indev_drv_init(&indev_drv);
indev_drv.type = LV_INDEV_TYPE_POINTER;
indev_drv.read_cb = my_touchpad_read;
lv_indev_drv_register(&indev_drv);

//lv_example_get_started_1();

// berthe
g_tft.fillRect(0, 0,240,320,TFT_BLACK);
g_tft.setTextColor(TFT_WHITE,TFT_BLACK) ;
 //g_tft.setEpdMode(epd_mode_t::epd_fastest);
g_tft.setEpdMode(epd_mode_t::epd_text);
//g_tft.setFont(&fonts::Font2);
g_tft.setColorDepth( 8 ) ;
}
/*
////////////////////////////////////////////////////////////////////////////////
/// \brief
static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        //Get the first child of the button which is the label and change its text
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
} */

/*
////////////////////////////////////////////////////////////////////////////////
/// \brief Create a button with a label and react on click event.
void lv_example_get_started_1(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());     //Add a button the current screen
    lv_obj_set_size(btn, 120, 50);                    //Set its size
    lv_obj_align(btn, LV_ALIGN_CENTER, 0,0);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           //Assign a callback to the button

    lv_obj_t * label = lv_label_create(btn);          //Add a label to the button
    lv_label_set_text(label, "Button");                     //Set the labels text
    lv_obj_center(label);
}*/
