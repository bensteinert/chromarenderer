/**
 * Original Author: C) 2003 by Holger Dammertz
 * Adaptions made : 2008 by Ben Steinert
 **/

#include <cstdlib>
#include <memory>

#include "Defines.h"
#include "Image.h"
#include "SDLRenderCanvas.h"


/* ASCII Font */
const unsigned char font9x16[] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,63,176,63,176,30,0,0,0,0,0,0,0,0,0,112,0,120,0,0,0,0,0,120,0,112,0,0,0,0,0,4,64,31,240,31,240,4,64,
		4,64,31,240,31,240,4,64,0,0,28,96,62,48,34,16,226,28,226,28,51,240,25,224,0,0,0,0,24,48,24,96,0,192,1,128,3,0,6,0,12,48,24,48,0,0,1,224,27,240,62,16,39,16,61,224,27,240,2,16,0,0,0,0,
		0,0,8,0,120,0,112,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,192,31,224,48,48,32,16,0,0,0,0,0,0,0,0,0,0,32,16,48,48,31,224,15,192,0,0,0,0,0,0,1,0,5,64,7,192,3,128,3,128,
		7,192,5,64,1,0,0,0,0,0,1,0,1,0,7,192,7,192,1,0,1,0,0,0,0,0,0,0,0,0,0,8,0,120,0,112,0,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,0,0,0,
		0,0,0,0,0,48,0,48,0,0,0,0,0,0,0,0,0,48,0,96,0,192,1,128,3,0,6,0,12,0,0,0,0,0,15,192,31,224,48,48,35,16,48,48,31,224,15,192,0,0,0,0,0,0,8,16,24,16,63,240,63,240,0,16,
		0,16,0,0,0,0,16,112,48,240,33,144,35,16,38,16,60,48,24,48,0,0,0,0,16,32,48,48,32,16,34,16,34,16,63,240,29,224,0,0,0,0,3,0,7,0,13,0,25,16,63,240,63,240,1,16,0,0,0,0,62,32,62,48,
		34,16,34,16,34,16,35,240,33,224,0,0,0,0,15,224,31,240,50,16,34,16,34,16,35,240,1,224,0,0,0,0,48,0,48,0,33,240,35,240,38,0,60,0,56,0,0,0,0,0,29,224,63,240,34,16,34,16,34,16,63,240,29,224,
		0,0,0,0,28,0,62,16,34,16,34,16,34,48,63,224,31,192,0,0,0,0,0,0,0,0,0,0,12,96,12,96,0,0,0,0,0,0,0,0,0,0,0,0,0,16,12,112,12,96,0,0,0,0,0,0,0,0,0,0,1,0,3,128,
		6,192,12,96,24,48,16,16,0,0,0,0,0,0,4,128,4,128,4,128,4,128,4,128,4,128,0,0,0,0,0,0,16,16,24,48,12,96,6,192,3,128,1,0,0,0,0,0,24,0,56,0,32,0,33,176,35,176,62,0,28,0,0,0,
		0,0,15,224,31,240,16,16,19,208,19,208,31,208,15,128,0,0,0,0,7,240,15,240,25,0,49,0,25,0,15,240,7,240,0,0,0,0,32,16,63,240,63,240,34,16,34,16,63,240,29,224,0,0,0,0,15,192,31,224,48,48,32,16,
		32,16,48,48,24,96,0,0,0,0,32,16,63,240,63,240,32,16,48,48,31,224,15,192,0,0,0,0,32,16,63,240,63,240,34,16,39,16,48,48,56,112,0,0,0,0,32,16,63,240,63,240,34,16,39,0,48,0,56,0,0,0,0,0,
		15,192,31,224,48,48,33,16,33,16,49,224,25,240,0,0,0,0,63,240,63,240,2,0,2,0,2,0,63,240,63,240,0,0,0,0,0,0,0,0,32,16,63,240,63,240,32,16,0,0,0,0,0,0,0,224,0,240,0,16,32,16,63,240,
		63,224,32,0,0,0,0,0,32,16,63,240,63,240,3,0,7,128,60,240,56,112,0,0,0,0,32,16,63,240,63,240,32,16,0,16,0,48,0,112,0,0,0,0,63,240,63,240,28,0,14,0,28,0,63,240,63,240,0,0,0,0,63,240,
		63,240,28,0,14,0,7,0,63,240,63,240,0,0,0,0,31,224,63,240,32,16,32,16,32,16,63,240,31,224,0,0,0,0,32,16,63,240,63,240,34,16,34,0,62,0,28,0,0,0,0,0,31,224,63,240,32,16,32,48,32,28,63,252,
		31,228,0,0,0,0,32,16,63,240,63,240,34,0,35,0,63,240,28,240,0,0,0,0,24,96,60,112,38,16,34,16,35,16,57,240,24,224,0,0,0,0,0,0,56,0,48,16,63,240,63,240,48,16,56,0,0,0,0,0,63,224,63,240,
		0,16,0,16,0,16,63,240,63,224,0,0,0,0,63,128,63,192,0,96,0,48,0,96,63,192,63,128,0,0,0,0,63,224,63,240,0,112,3,192,0,112,63,240,63,224,0,0,0,0,48,48,60,240,15,192,7,128,15,192,60,240,48,48,
		0,0,0,0,0,0,60,0,62,16,3,240,3,240,62,16,60,0,0,0,0,0,56,112,48,240,33,144,35,16,38,16,60,48,56,112,0,0,0,0,0,0,0,0,63,240,63,240,32,16,32,16,0,0,0,0,0,0,28,0,14,0,7,0,
		3,128,1,192,0,224,0,112,0,0,0,0,0,0,0,0,32,16,32,16,63,240,63,240,0,0,0,0,0,0,16,0,48,0,96,0,192,0,96,0,48,0,16,0,0,0,0,0,0,4,0,4,0,4,0,4,0,4,0,4,0,4,0,4,
		0,0,0,0,0,0,96,0,112,0,16,0,0,0,0,0,0,0,0,0,0,224,5,240,5,16,5,16,7,224,3,240,0,16,0,0,0,0,32,16,63,240,63,224,4,16,6,16,3,240,1,224,0,0,0,0,3,224,7,240,4,16,4,16,
		4,16,6,48,2,32,0,0,0,0,1,224,3,240,6,16,36,16,63,224,63,240,0,16,0,0,0,0,3,224,7,240,5,16,5,16,5,16,7,48,3,32,0,0,0,0,0,0,2,16,31,240,63,240,34,16,48,0,24,0,0,0,0,0,
		3,228,7,246,4,18,4,18,3,254,7,252,4,0,0,0,0,0,32,16,63,240,63,240,2,0,4,0,7,240,3,240,0,0,0,0,0,0,0,0,4,16,55,240,55,240,0,16,0,0,0,0,0,0,0,0,0,4,0,6,0,2,4,2,
		55,254,55,252,0,0,0,0,32,16,63,240,63,240,1,128,3,192,6,112,4,48,0,0,0,0,0,0,0,0,32,16,63,240,63,240,0,16,0,0,0,0,0,0,7,240,7,240,6,0,3,240,3,240,6,0,7,240,3,240,0,0,4,0,
		7,240,3,240,4,0,4,0,7,240,3,240,0,0,0,0,3,224,7,240,4,16,4,16,4,16,7,240,3,224,0,0,0,0,4,2,7,254,3,254,4,18,4,16,7,240,3,224,0,0,0,0,3,224,7,240,4,16,4,18,3,254,7,254,
		4,2,0,0,0,0,4,16,7,240,3,240,6,16,4,0,6,0,2,0,0,0,0,0,3,32,7,176,4,144,4,144,4,144,6,240,2,96,0,0,0,0,4,0,4,0,31,224,63,240,4,16,4,48,0,32,0,0,0,0,7,224,7,240,
		0,16,0,16,7,224,7,240,0,16,0,0,0,0,7,192,7,224,0,48,0,16,0,48,7,224,7,192,0,0,0,0,7,224,7,240,0,48,0,224,0,224,0,48,7,240,7,224,0,0,4,16,6,48,3,96,1,192,1,192,3,96,6,48,
		4,16,0,0,7,226,7,242,0,18,0,18,0,22,7,252,7,248,0,0,0,0,6,48,6,112,4,208,5,144,7,16,6,48,4,48,0,0,0,0,0,0,2,0,2,0,31,224,61,240,32,16,32,16,0,0,0,0,0,0,0,0,0,0,
		62,248,62,248,0,0,0,0,0,0,0,0,0,0,32,16,32,16,61,240,31,224,2,0,2,0,0,0,0,0,32,0,96,0,64,0,96,0,32,0,96,0,64,0,0,0,0,0,1,224,3,224,6,32,12,32,6,32,3,224,1,224,0,0};



SDLRenderCanvas::SDLRenderCanvas(int f_XRes, int f_YRes, bool f_FullScreen, bool f_DoubleBuffer) : AbstractRenderCanvas(f_XRes, f_YRes)
{
    m_pScreen = NULL;
    m_pVideoInfo = NULL;
    m_fpQuitFunction = NULL;

    m_DoubleBuffered = f_DoubleBuffer;
    m_FullScreen = f_FullScreen;

    m_FrameCountInterval = 5000;
    m_ElapsedTicks = 0;
}

SDLRenderCanvas::~SDLRenderCanvas(void)
{
}

/*void SDLRenderCanvas::HandleEvents(){

	bool consumed = false;

	while (SDL_PollEvent(&m_Event))
	{
		for (SDLEventConsumerVector::iterator i = m_EventConsumer.begin(); i != m_EventConsumer.end(); i++)
		{
			consumed = (*i)->Consume(m_Event);
			// At the moment do not handle consumed events any further
			if (consumed) return;
		}
	}
}*/

bool SDLRenderCanvas::Initialize()
{
    if (m_pScreen != NULL)
    {
        fprintf(stderr, "SDLRenderCanvas::Initialize(): There is already a screen initialized.\n");
        //return false;
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDLRenderCanvas::Initialize(): Video initialization failed: %s\n", SDL_GetError());
        return false;
    }

    SetVideoFlags(true);

    /* now we try to become a surface for rendering */
	m_pScreen = SDL_SetVideoMode(m_XResolution, m_YResolution, 32, m_VideoFlags);

    /* Verify that there is a surface */
    if (!m_pScreen)
    {
        fprintf(stderr, "SDLRenderCanvas::Initialize(): Video mode set failed: %s\n", SDL_GetError( ));
        return false;
    }
    fprintf(stderr, "Screen set to %d x %d x %d\n",  m_pScreen->w, m_pScreen->h, m_pScreen->format->BitsPerPixel);

    return true;
}

float alpha, beta, sin_a, cos_a, sin_b, cos_b;
int speed = 5, abs_x = 0, abs_y = 0;
bool cursor = true;

void SDLRenderCanvas::EnterRenderMode()
{
    if (SDL_MUSTLOCK(m_pScreen)) 
        while(SDL_LockSurface(m_pScreen) < 0);
}

void SDLRenderCanvas::LeaveRenderMode(const char * title)
{
    // some static variables for fps counting
    //static int oldTicks = 0;
    //static int t0;
    //static int frames;

    if (SDL_MUSTLOCK(m_pScreen)) 
        SDL_UnlockSurface(m_pScreen);
    if (m_DoubleBuffered) 
        SDL_Flip(m_pScreen);
    
    SDL_WM_SetCaption(title, NULL);

    /* gather frames per second */
//    frames++;
//    {
//        int t = SDL_GetTicks();
//        if (t - t0 >= m_FrameCountInterval) 
//        {
//            double seconds = (t - t0) / 1000.0;
//            double fps = frames / seconds;
//            sprintf(m_pWindowTitle, "%s - %.2f fps", title, fps);
//            // Call this only, when you have an SDL window openend!
//            SDL_WM_SetCaption(m_pWindowTitle, NULL);
//            t0 = t;
//            frames = 0;
//        }                                
//    }
//
//    // Calculate the new DeltaT
//    m_DeltaT = (float)(SDL_GetTicks()-m_ElapsedTicks)/1000.0f;
//    m_ElapsedTicks = SDL_GetTicks();
}


void SDLRenderCanvas::LeaveRenderMode(int f_X, int f_Y, int f_W, int f_H)
{
    #ifdef DEBUG
      if (m_DoubleBuffered) logError("SDLRenderCanvas::LeaveRenderMode(int f_X, int f_Y, int f_W, int f_H): called in double buffered mode!");
    #endif
    
    SDL_UpdateRect(m_pScreen, f_X, f_Y, f_W, f_H);    
}

inline void SDLRenderCanvas::DrawPixel(int f_X, int f_Y, float f_R, float f_G, float f_B)
{
    //static Uint32 color;
    //static unsigned char* pPosition;

    //color = SDL_MapRGB(m_pScreen->format, (Uint8)(f_R * 255), (Uint8)(f_G * 255), (Uint8)(f_B * 255));
    //color = SDL_MapRGB(m_pScreen->format, (Uint8)(f_R), (Uint8)(f_G), (Uint8)(f_B));
    *((Uint32 *)m_pScreen->pixels + f_Y * m_pScreen->pitch/4 + f_X) = SDL_MapRGB(m_pScreen->format, (Uint8)(f_R), (Uint8)(f_G), (Uint8)(f_B));
}

void SDLRenderCanvas::blitImage(const Image& img)
{
	for(int Y = 0 ; Y < img.height;Y++){
		int line = Y*img.width;
		for(int X = 0 ; X < img.width;X++){
			int pos = line + X;
			#ifdef CLAMP
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(fmin(img[pos][0],1.0f)*255), (Uint8)(fmin(img[pos][1],1.0f)*255), (Uint8)(fmin(img[pos][2],1.0f)*255));	
			#else
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(img[pos][0]*255), (Uint8)(img[pos][1]*255), (Uint8)(img[pos][2]*255));	
			#endif
		}
	}
}

void SDLRenderCanvas::blitImage_mirroredX(const Image& img)
{
	for(int Y = 0 ; Y < img.height;Y++){
		int line = Y*img.width;
		for(int X = 0 ; X < img.width;X++){
			int pos = line + (img.width-X);
			#ifdef CLAMP
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(fmin(img[pos][0],1.0f)*255), (Uint8)(fmin(img[pos][1],1.0f)*255), (Uint8)(fmin(img[pos][2],1.0f)*255));	
			#else
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(img[pos][0]*255), (Uint8)(img[pos][1]*255), (Uint8)(img[pos][2]*255));	
			#endif
		}
	}
}

void SDLRenderCanvas::blitImage_mirroredY(const Image& img)
{
	for(int Y = 0 ; Y < img.height;Y++){
		int line = (img.height-Y-1)*img.width;
		for(int X = 0 ; X < img.width;X++){
			int pos = line + X;
			#ifdef CLAMP
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(fmin(img[pos][0],1.0f)*255), (Uint8)(fmin(img[pos][1],1.0f)*255), (Uint8)(fmin(img[pos][2],1.0f)*255));	
			#else
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(img[pos][0]*255), (Uint8)(img[pos][1]*255), (Uint8)(img[pos][2]*255));	
			#endif
		}
	}
}

void SDLRenderCanvas::blitImage_mirroredXY(const Image& img)
{
	for(int Y = 0 ; Y < img.height;Y++){	
		int line = (img.height-Y-1)*img.width;
		for(int X = 0 ; X < img.width;X++){
			int pos = line+(img.width-X);
			#ifdef CLAMP
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(fmin(img[pos][0],1.0f)*255), (Uint8)(fmin(img[pos][1],1.0f)*255), (Uint8)(fmin(img[pos][2],1.0f)*255));	
			#else
			*((Uint32 *)m_pScreen->pixels + Y * m_pScreen->pitch/4 + X) = SDL_MapRGB(m_pScreen->format, (Uint8)(img[pos][0]*255), (Uint8)(img[pos][1]*255), (Uint8)(img[pos][2]*255));	
			#endif
		}
	}
}

void SDLRenderCanvas::renderString(const char* string, int posx, int posy) {
	
	size_t len = strlen(string);
	for (unsigned int pos = 0; pos < len; pos++) {
		int charPos = (string[pos] - 32)*9*2;
		for (int x = 0; x < 9*2; x++) {
			if (posx >= m_XResolution)
				return;
			unsigned char cLine = font9x16[charPos+x];
			for (int i = 0; i < 8; i++) {
				float col = 0.0f; //background
				if (cLine & (1<<(7-i)))
					col = 200.0f; // text color
				int y = posy - (15 - (i + (x&1)*8));
				if ((y < m_YResolution) && (y >= 0)) {
					*((Uint32 *)m_pScreen->pixels + y * m_XResolution + posx) = SDL_MapRGB(m_pScreen->format, (Uint8)(0), (Uint8)(col), (Uint8)(0));
				} else
					return;
			}
			if (x&1)
				posx++;
		}
	}
}




void SDLRenderCanvas::DrawPixelUchar(int f_X, int f_Y, unsigned char f_R, unsigned char  f_G, unsigned char  f_B)
{
    static Uint32 color;
    //static unsigned char* pPosition;

    // !!OPT: This is slow
    color = SDL_MapRGB(m_pScreen->format, f_R, f_G, f_B);

    *((Uint32 *)m_pScreen->pixels + f_Y * m_pScreen->pitch/4 + f_X) = color;
}

void SDLRenderCanvas::AddEventConsumer(SDLEventConsumer* f_pConsumer)
{
    m_EventConsumer.push_back(f_pConsumer);
}

/***************************************************************************
 *    Protected Methods                                                    *
 ***************************************************************************/


void SDLRenderCanvas::SetVideoFlags(bool f_PrintInfo)
{
    m_pVideoInfo = SDL_GetVideoInfo();
    if (!m_pVideoInfo)
    {
        fprintf(stderr, "SDLRenderCanvas::SetVideoFlags(bool f_PrintInfo): Could not get VideoInfo from SDL: %s\n", SDL_GetError());
        return;
    }

    m_VideoFlags = SDL_HWPALETTE;  // Store the palette in hardware
    
    if (m_DoubleBuffered) 
        m_VideoFlags |= SDL_DOUBLEBUF;    // Enable double buffering

    if (m_FullScreen) 
        m_VideoFlags |= SDL_FULLSCREEN;       // Enable fullscreen

    if (m_pVideoInfo->blit_hw) 
        m_VideoFlags |= SDL_HWACCEL; // This checks if hardware blits can be done 

    /* This checks to see if surfaces can be stored in memory */
    if (f_PrintInfo)
    {
        //logMessage("  Resolution: %dx%d", m_XResolution, m_YResolution);
        if (m_pVideoInfo->hw_available)
        {
            m_VideoFlags |= SDL_HWSURFACE;
            printf("  Surface is hardware.\n");
        }
        else
        {
            m_VideoFlags |= SDL_SWSURFACE;
            printf("  Surface is software.\n");
        }
        if (m_pVideoInfo->blit_hw)
        {
            printf("  Hardware accelerated blit enabled.\n");
        }
        else
        {
            printf("  Hardware accelerated blit disabled.\n");
        }
    }
}

