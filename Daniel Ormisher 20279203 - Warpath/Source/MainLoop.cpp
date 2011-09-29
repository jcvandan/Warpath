#include <windowsx.h>
#include "TankDefines.h"
#include "D3DManager.h"
#include "CTimer.h"
#include "GameManager.h"
using namespace malaz;

namespace malaz{

//D3D Manager
CD3DManager* d3dManager;

// Timer
CTimer Timer;

// Game manager
CGameManager* GameManager;

// Mouse coordinates
int MouseX;
int MouseY;

// Window message handler
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
		{
            PostQuitMessage( 0 );
            return 0;
		}

		case WM_KEYDOWN:
		{
			EKeyCode eKeyCode = static_cast<EKeyCode>(wParam);
			KeyDownEvent( eKeyCode );
			break;
		}

		case WM_LBUTTONDOWN :
		{
			EKeyCode eKeyCode = static_cast<EKeyCode>(wParam);
			KeyDownEvent( eKeyCode );
			break;
		}

		case WM_KEYUP:
		{
			EKeyCode eKeyCode = static_cast<EKeyCode>(wParam);
			KeyUpEvent( eKeyCode );
			break;
		}

		case WM_LBUTTONUP :
		{
			EKeyCode eKeyCode = static_cast<EKeyCode>(wParam);
			KeyUpEvent( Mouse_LButton );
			break;
		}

		case WM_MOUSEMOVE:
		{
			MouseX = GET_X_LPARAM(lParam);
			MouseY = GET_Y_LPARAM(lParam);
		}
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

}// namespace malaz

// Windows main function
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // Register the window class (adding our own icon to this window)
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL,
					  LoadCursor( NULL, IDC_ARROW ), NULL, NULL,
                      "Warpath", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
	HWND hWnd = CreateWindow( "Warpath", "Warpath",
                              WS_DLGFRAME ,//WS_OVERLAPPEDWINDOW/*WS_EX_TOPMOST | WS_POPUP*/,
							  300, 100, 1000, 800,
                              NULL, NULL, wc.hInstance, NULL );

	// Load the game manager
	CGameManager::Create();
	GameManager = CGameManager::GetInstance();

	// Load the d3d manager
	CD3DManager::Create( false );
	d3dManager = CD3DManager::GetInstance(); 

    // Initialize Direct3D
    if ( d3dManager->D3DSetup( hWnd ) )
    {
        ////////////////////////////////////////////////////////////
		// Main Game Setup and Game Loop

		if ( GameManager->Setup( d3dManager ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

			//Reset the timer
			Timer.Reset();

            // Enter the message loop
            MSG msg2;
            ZeroMemory( &msg2, sizeof(msg2) );

            while( msg2.message != WM_QUIT )
            {
				float updateTime = Timer.GetLapTime();

                if( PeekMessage( &msg2, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg2 );
                    DispatchMessage( &msg2 );
                }
                else
				{
					// Render and update the scene
					GameManager->Render( d3dManager, updateTime );
					if( !GameManager->Update( d3dManager, updateTime ) /*|| KeyHit( Key_Escape )*/ )
					{
						DestroyWindow( hWnd );
					}
				}
            }
        }
		// Shut down/ free up memory used by game
		GameManager->Shutdown();

		// Destroy the game manager
		CGameManager::Destroy();
		CD3DManager::Destroy();
    }

	UnregisterClass( "Warpath", wc.hInstance );
    return 0;
}