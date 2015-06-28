/**
 * Original Author: C) 2003 by Sabrina Dammertz, Holger Dammertz
 * Adaptions made : 2008 by Ben Steinert
 **/
#ifndef SDLRENDERCANVAS_H
#define SDLRENDERCANVAS_H

#include <vector>
#include <SDL/SDL.h>

#include "AbstractRenderCanvas.h"
#include "Image.h"

class SDLEventConsumer {
public:
    virtual bool Consume(SDL_Event &m_rEvent) = 0;
    virtual ~SDLEventConsumer() { }
};

typedef std::vector<SDLEventConsumer *> SDLEventConsumerVector;

/**
 * This Implementation of AbstractRenderCanvas features a double buffered 
 * window that is created on initialization of the class. It uses the
 * SDL to create the windows.
 *
 * @TDO:
 *  - Make this class some kind of singelton because you can only open
 *    one SDL window at a time.
 */
class SDLRenderCanvas : public AbstractRenderCanvas {
    /** A pointer to our screen */
    SDL_Surface *m_pScreen;
    /** stores some information */
    const SDL_VideoInfo *m_pVideoInfo;
    /** The video flags that define the propperties of the screen */
    int m_VideoFlags;
    /** to store if we have a double bufferd screen or not */
    bool m_DoubleBuffered;

    bool m_FullScreen;

    /** This is used for event handling (at the moment only checking 
        for an exit action)    */
    SDL_Event m_Event;

    /** This is a pointer to the function that is called on a quit event */
    void (*m_fpQuitFunction)(int);

    SDLEventConsumerVector m_EventConsumer;

    /** the window title (at the moment only used by the fps display */
    char m_pWindowTitle[255];
    /** the time in milliseconds that has to elapse before a new fps value
        is evaluated */
    int m_FrameCountInterval;
    /** needed for deltaT calculation */
    int m_ElapsedTicks;

    //bool keysHeld[323];

public:
    SDLRenderCanvas(int f_XRes, int f_YRes, bool f_FullScreen, bool f_DoubleBuffer);

    ~SDLRenderCanvas(void);

    bool Initialize();

    /**
     * This method checks for quit events (ESC or the close button of the
     * window) and then calls the quit-function.
     */
    // void HandleEvents(bool &finished, Vector3 &origin, Vector3 *&Transform, Triangle *&triangles, int &numTriangles, int &numPatches, int RES_X2, int RES_Y2);
    void HandleEvents();

    /**
     * Make sure you call this method once before performing a DrawPixel call.
     * It locks the surface so you can draw to it. When the drawing has finished,
     * call a LeaveRenderMode();
     */
    void EnterRenderMode();

    /**
     * This method unlocks the screen surface if needed.
     * If double bufferig is enabled, it performas a buffer swap so the drawn 
     * image can be seen. If not, you have to make sure your on your own, that
     * the correct screen space is updated.
     */
    void LeaveRenderMode(const char *title);

    /**
     * This method unlocks the screen surface if needed and updates only the given
     * rectangular region.
     * (0, 0, 0, 0) Updates the whole screen
     * @remarks
     *   call this method only in non doublebufferd mode.
     */
    void LeaveRenderMode(int f_X, int f_Y, int f_W, int f_H);

    /**
     * Expects the color values to be between 0 and 1 and scales them to 
     * 0..255. At the moment this method uses a really slow color conversion
     * routine...
     */
    inline void DrawPixel(int f_X, int f_Y, float f_R, float f_G, float f_B);

    void blitImage(const Image &img);

    void blitImage_mirroredX(const Image &img);

    void blitImage_mirroredY(const Image &img);

    void blitImage_mirroredXY(const Image &img);

    void renderString(const char *string, int posx, int posy);

    void DrawPixelUchar(int f_X, int f_Y, unsigned char f_R, unsigned char f_G, unsigned char f_B);

    void AddEventConsumer(SDLEventConsumer *f_pConsumer);


protected:
    void SetVideoFlags(bool f_PrintInfo);
};

#endif
