#include "Pad.h"
#include "DxLib.h"

#define SAFE_DELETE( x ) if( x ){ delete x; }
#define SAFE_DELETE_ARRAY( x ) if( x ){ delete[] x; }
#define next ( ( cur + 1 ) % 2 )
#define prev ( ( cur + 1 ) % 2 )

Pad* Pad::mInstance = 0;

void Pad::create(){
	if ( mInstance ){
		DrawString( 0, 0, "ìÒÇ¬à»è„PadÇ™çÏÇÁÇÍÇ‹ÇµÇΩÅB", GetColor( 0xff, 0xff, 0xff ) );
		ScreenFlip();
		WaitKey();
	}
	mInstance = new Pad();
}

void Pad::destroy(){
	SAFE_DELETE( mInstance );
}

Pad* Pad::instance(){
	return mInstance;
}

Pad::Pad():
cur( 0 )
{
	for ( int i = 0; i < 2; i++ ){
		for ( int j = 0; j < 256; j++ ){
			KeyInput[ i ][ j ] = 0;
		}
		isInput[ i ] = false;
		MouseInput[ i ] = 0;
	}
}

Pad::~Pad(){
}

void Pad::update(){
	cur = next;
	GetHitKeyStateAll( KeyInput[ cur ] );
	isInput[ cur ] = CheckHitKeyAll();
	MouseInput[ cur ] = GetMouseInput();
}

void Pad::DrawParam(){
}

int Pad::isOn( Input command ){
	bool result = false;
	switch( command ){
	case UP:	
		result = KeyInput[ cur ][ KEY_INPUT_UP ];
		break;
	case DOWN:	
		result = KeyInput[ cur ][ KEY_INPUT_DOWN ];
		break;
	case RIGHT:
		result = KeyInput[ cur ][ KEY_INPUT_RIGHT ];
		break;
	case LEFT:
		result = KeyInput[ cur ][ KEY_INPUT_LEFT ];
		break;
	case A:
		result = KeyInput[ cur ][ KEY_INPUT_A ];
		break;
	case B:
		result = KeyInput[ cur ][ KEY_INPUT_Z ];
		break;
	case ENTER:
		result = KeyInput[ cur ][ KEY_INPUT_RETURN ];
		break;
	case ESCAPE:
		result = KeyInput[ cur ][ KEY_INPUT_ESCAPE ];
		break;
	default :
		break;
	}
	return result;
}

int Pad::isTriggered( Input command ){
	bool result = false;
	switch( command ){
	case UP:	
		result = KeyInput[ cur ][ KEY_INPUT_UP ] && !KeyInput[ prev ][ KEY_INPUT_UP ];
		break;
	case DOWN:	
		result = KeyInput[ cur ][ KEY_INPUT_DOWN ] && !KeyInput[ prev ][ KEY_INPUT_DOWN ];
		break;
	case RIGHT:
		result = KeyInput[ cur ][ KEY_INPUT_RIGHT ] && !KeyInput[ prev ][ KEY_INPUT_RIGHT ];
		break;
	case LEFT:
		result = KeyInput[ cur ][ KEY_INPUT_LEFT ] && !KeyInput[ prev ][ KEY_INPUT_LEFT ];
		break;
	case A:
		result = KeyInput[ cur ][ KEY_INPUT_A ] && !KeyInput[ prev ][ KEY_INPUT_A ];
		break;
	case B:
		result = KeyInput[ cur ][ KEY_INPUT_Z ] && !KeyInput[ prev ][ KEY_INPUT_Z ];
		break;
	case ENTER:
		result =
			KeyInput[ cur ][ KEY_INPUT_RETURN ] && !KeyInput[ prev ][ KEY_INPUT_RETURN ]
			|| ( ( MouseInput[ cur ] == MOUSE_INPUT_LEFT ) && ( MouseInput[ prev ] != MOUSE_INPUT_LEFT ) );
		break;
	case ESCAPE:
		result = KeyInput[ cur ][ KEY_INPUT_ESCAPE ] && !KeyInput[ prev ][ KEY_INPUT_ESCAPE ];
		break;
	default :
		break;
	}
	return result;
}

int Pad::isOn( int command ){
	return KeyInput[ cur ][ command ];
}

int Pad::isTriggered( int command ){
	return KeyInput[ cur ][ command ] && !KeyInput[ prev ][ command ];
}

int Pad::isOn(){
	return isInput[ cur ];
}

int Pad::isTriggered(){
	return isInput[ cur ] && !isInput[ prev ];
}