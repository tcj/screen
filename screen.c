/*
 *      screens.
 *      trying to get some colors on the screen
 *      of my own design.
 *
 *		TODO: exit-handler with table of errorvalues to 
 *				error-texts
 */

#define __USE_SYSBASE           /* now we require 68020+                */
                                /* & must define SysBase if we switch   */
                                /* to a third-party startup module      */

long __oslibversion = 37;       /* auto-initialize libs (SAS/C)         */
                                /* (Vol.1 ch. 10 p.164)                 */
                                /* versions: vol.1 ch.2 p8 (37 = 2.04)  */

#include <exec/types.h>

#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <workbench/startup.h>
#include <dos/stdio.h>

#include <proto/exec.h>
#include <proto/intuition.h> 
#include <proto/graphics.h>
#include <proto/dos.h>

#include <clib/alib_stdio_protos.h>
#include <stdlib.h>

/* 
   2. open screen
   3. set colors
   4. draw things
   5. wait for some period of time or event
   6. close screen
*/

/* my defines */

#define MY_DISPLAY_WIDTH 	( 320 )
#define MY_DISPLAY_HEIGHT	( 200 )
#define MY_DISPLAY_DEPTH	( 4 )

#define ERROR_CANNOT_OPEN_SCREEN	100

/* GLOBALS!  OH NO! */
struct Window *myWindow;
struct Screen *myScreen;

UWORD pens[] = { ~0 };					/* RKM:L p47, 55 */

BOOL openScreen( VOID )
{
	ULONG *errorCode = NULL;

	/* screen dimensions: viewport -> RKM:L p543 */
	
	myScreen = OpenScreenTags( NULL,
					SA_Pens,		(ULONG)pens,  
					SA_DisplayID,	LORES_KEY,
					SA_Width,		MY_DISPLAY_WIDTH, 
					SA_Height, 		MY_DISPLAY_HEIGHT,
					SA_Depth,		MY_DISPLAY_DEPTH,
					SA_Title,		(ULONG)"Our Screen",
					SA_ErrorCode,	errorCode,
/*					SA_FullPalette, TRUE,  */
					SA_ShowTitle,	FALSE,			/* RKM:L p75 */
					SA_SysFont,		0,				/* open with prefs fixed font */
				/*	SA_Colors,		... */
					TAG_DONE );						/* tags: RKM:L 47 */
	
	if( myScreen == NULL )
	{
		printf("cannot open screen: ");
		switch(*errorCode)
		{
			case OSERR_NOMONITOR:
				printf("monitortype not available\n");
				break;
			case OSERR_NOCHIPS:
				printf("need newer chipset\n");
				break;
			case OSERR_NOMEM:
				printf("low memory\n");
				break;
			case OSERR_NOCHIPMEM:
				printf("low chipmem\n");
				break;
			case OSERR_PUBNOTUNIQUE:
				printf("pubscreen-name already exists\n");
				break;
			case OSERR_UNKNOWNMODE:
				printf("unrecognized displaymode\n");
				break;
			default:
				printf("unknown reason!\n");
		}
		return FALSE;
	}
	else 
		return TRUE;
}

VOID closeScreen( VOID )
{
	if( myScreen )
		CloseScreen( myScreen );
}			

BOOL openWindow( VOID )
{
	/* open borderless backdrop window */
	myWindow = OpenWindowTags( NULL,			/* RKM:L p107 */
		WA_Left, 	0, 	WA_Top, 0,
/*		WA_Width, 320, WA_Height, 400, */		/* get screen dimensions? */
		WA_Width, 	MY_DISPLAY_WIDTH, 
		WA_Height, 	MY_DISPLAY_HEIGHT,
		WA_CustomScreen, myScreen,
		
/*		WA_Title, "myScreen Backdrop Window", */   /* omit title */
/*		WA_ScreenTitle, "This shows when window is active", */  
		WA_DragBar, 		FALSE, 	WA_DepthGadget, 	FALSE,
		WA_CloseGadget, 	FALSE,	WA_SizeGadget, 		FALSE,
		WA_NoCareRefresh, 	TRUE,	WA_SmartRefresh,	TRUE, 	
		WA_Borderless,		TRUE,	WA_Backdrop,	TRUE,	
/*		WA_AutoAdjust,	TRUE, */
		TAG_END );
	if( myWindow != NULL )
		return TRUE;
	else 
		return FALSE;
}

VOID closeWindow( VOID )
{
	/* might we need to flush intuition events here? */
	
	if( myWindow )
		CloseWindow( myWindow );
}

VOID drawInWindow( VOID )
{
	int boxesWide = 4;
	int boxesHigh = 2;
	int row, col = 0;
	
	for( row = 0; row < boxesWide; row++ )
	{
		
	}	
}

VOID colorsInWindow( VOID )
{
	int i;
	
	for( i = 0 ; i < MY_DISPLAY_DEPTH ; i++ )
	{
		Delay( 100 );
		SetRast( myWindow->RPort, i );
		WaitBlit( );
	}
}	

VOID drawCirclesInWindow( VOID )
{
	int x;
	int xC, yC;
	static int circRad = 8;
	struct RastPort *rp = myWindow->RPort;			/* RKM:L 581 */
	
	SetDrMd( rp, JAM1 );
	
	for( xC = 0; xC <= 4; xC++ )
	{
		SetAPen( rp, xC );
		SetBPen( rp, xC );
		SetOPen( rp, xC );
		AreaCircle( rp, (circRad * xC) + circRad, circRad * 2, circRad );
	}
}

#define MYI_LEFT	( 0 )
#define MYI_TOP		( 0 )
#define MYI_WIDTH	( 24 )
#define MYI_HEIGHT	( 10 )
#define MYI_DEPTH   ( 1 )

UWORD __chip myImageData[] =
{
	0xFFFF,	0xFF00,
	0xC000, 0x0300,
	0xC000, 0x0300,
	0xC000, 0x0300,
	0xC000, 0x0300,
	0xC000, 0x0300,
	0xC000, 0x0300,
	0xC000, 0x0300,
	0xC000, 0x0300,
	0xFFFF,	0xFF00,
};

VOID drawImageInWindow( VOID )
{
	struct Image myImage;
	
	myImage.LeftEdge = 0;
	myImage.TopEdge  = 0;
	myImage.Width	 = 24;
	myImage.Height	 = 10;
	myImage.Depth	 = 1;
	myImage.ImageData = myImageData;
	myImage.PlanePick = 0x1;
	myImage.PlaneOnOff = 0x0;
	myImage.NextImage = NULL;

	DrawImage( myWindow->RPort, &myImage, 10, 10 );

	myImage.PlanePick = 0x2;
	DrawImage( myWindow->RPort, &myImage, 100, 10 );
	
	myImage.PlanePick = 0x3;
	DrawImage( myWindow->RPort, &myImage, 10, 100 );
	
	myImage.PlanePick = 0x4;
	DrawImage( myWindow->RPort, &myImage, 100, 100);
}

VOID getDrawInfo( VOID )
{
	struct DrawInfo *dri = NULL;
	
	dri = GetScreenDrawInfo( myScreen );
	if( dri != NULL )
	{
		/* do what we need to do */
		printf("pens available: %ld\n", dri->dri_NumPens );
		printf("screen depth: %ld\n", dri->dri_Depth );
		
		FreeScreenDrawInfo( myScreen, dri );
		dri = NULL;
	}
}

int main( int argc, char *argv[] )           /* SAS/C-specific main()        */
{
    struct WBStartup *wbs;
   int i;
    
    if( argc == 0 )                        /* we started from Workbench */
    {
        wbs = (struct WBStartup *)argv;
        printf("command = %s\n", wbs->sm_ArgList[0].wa_Name);
        for( i = 1 ; i < wbs->sm_NumArgs ; i++ )
                printf("argument %d = %s\n", i, wbs->sm_ArgList[i].wa_Name );
    }
    else                                /* we started from CLI */
    {
        printf("command = %s\n", argv[0] );
        for( i = 0; argc > 0 ; i++, argc-- )
                printf("argument %d = %s\n", i, argv[i] );
    }
	if( openScreen( ) )
	{
		printf("bytes per raster line: %ld\n", myScreen->RastPort.BitMap->BytesPerRow );
		if( openWindow( ) )
		{
		/*  colorsInWindow( ); 		*/
		/*	drawCirclesInWindow( ); */
			getDrawInfo( );
			drawImageInWindow( );
			Delay( 200L );						/* 1/50th (60th?) of a second */
			closeWindow( );
		}					
		closeScreen( );
	}
	else
		exit( ERROR_CANNOT_OPEN_SCREEN );			/* SAS/C exit() */

    return( 0 );
}
