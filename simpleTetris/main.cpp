#include "main.h"
#include "Pad.h"

Pad* pad;	// Padクラスのインスタンスはこれに入れて使う

int WINAPI WinMain( 
   HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR, int )
{
	ChangeWindowMode( WindowFlag );
	if ( DxLib_Init() == -1 ){
		return -1;
	}
	
	// FPS計測用の変数たち。
	int NowTime, PrevTime;
	float* Time = new float[ ( int )FPS ];
	int flame = 0;
	int IdealTime = 1000 / FPS;
	float fps = 60.f;
	PrevTime = GetNowCount(); // 現在時刻を入れる。
	
	Initialize();

	while ( ProcessMessage() != -1 ){
		ClearDrawScreen();

		// Padの生成または更新など
		if ( !Pad::instance() ){
			Pad::create();
			pad = Pad::instance();
		} else {
			pad->update();
			if ( pad->isTriggered( ESCAPE ) ){
				SaveHighScore();
				break;
			}
		}

		// ゲームの進行
		static bool goPause = false;
		if ( pad->isTriggered( KEY_INPUT_SPACE ) ){
			goPause = !goPause;
			PlaySoundMem( PauseSound, DX_PLAYTYPE_BACK );
		}
		if ( goPause ){
			DrawGame( fps );
			DrawPause();
		} else {
			UpdateGame();
			DrawGame( fps );
		}
		
		// FPSの計測・調整
		NowTime = GetNowCount();
		Time[ flame ] = ( NowTime - PrevTime ) * 0.001f;
		if ( flame == FPS - 1 ){
			fps = 0;
			for ( int i = 0; i < FPS; i++ ){
				fps += Time[ i ];
			}
			fps = FPS / fps;
		}
		if ( NowTime - PrevTime < IdealTime ){
			Sleep( ( int )( IdealTime - NowTime + PrevTime) );
		}
		flame = ( flame + 1 ) % ( int )FPS;
		PrevTime = NowTime;

		// 裏画面を表画面に表示
		ScreenFlip();
	}

	// メモリの解放
	SAFE_DELETE_ARRAY( Time );
	Pad::destroy();
	destroy();

	// DXライブラリの終了処理。
	DxLib_End();

	return 0;
}


// いろいろ初期化
void Initialize(){
	resetWithChangingWindow();
	reset();
}

void resetWithChangingWindow(){
	SetMainWindowText( "tetris by furaga" );
	SetDrawScreen( DX_SCREEN_BACK );
	//　フォント
	Pop = CreateFontToHandle( "HGP創英角ﾎﾟｯﾌﾟ体", 32, -1, DX_FONTTYPE_NORMAL );
	Gothic = CreateFontToHandle( "HGP創英角ｺﾞｼｯｸUB", 32, -1, DX_FONTTYPE_NORMAL );
	Gothicb = CreateFontToHandle( "HGP創英角ｺﾞｼｯｸUB", 112, -1, DX_FONTTYPE_NORMAL );
	Gyosyo = CreateFontToHandle( "HGP行書体", 32, -1, DX_FONTTYPE_NORMAL );
	Gyosyob = CreateFontToHandle( "HGP行書体", 96, -1, DX_FONTTYPE_NORMAL );
	// サウンド
	LandSound = LoadSoundMem( LAND_SOUND );
	MoveSound = LoadSoundMem( MOVE_SOUND );
//	MoveSound = LoadSoundMem( ROT_SOUND );
	RotSound = LoadSoundMem( ROT_SOUND );
	DeleteSound = LoadSoundMem( DELETE_SOUND );
	PauseSound = LoadSoundMem( PAUSE_SOUND );
	GameOverSound1 = LoadSoundMem( GAMEOVER_SOUND1 );
	GameOverSound2 = LoadSoundMem( GAMEOVER_SOUND2 );
	bgm = LoadSoundMem( BGM );
	ChangeVolumeSoundMem( 16, MoveSound );
	// 画像
	LoadDivGraph( BLOCK_IMAGE, 8, 1, 8, 20, 20, BlockImages );
	BlockImages[ 8 ] = 0;
	BGImage = LoadGraph( BG_IMAGE );
}

void reset(){
	GameOverFlag = false;
	gocnt = 0;
	h = 1;
	
	for ( int i = 0; i < MaxDelete; i++ ){
		DeleteLine[ i ] = -1;
	}
	DeleteFlag = -1;
	
	score = 0;
	GetHighScore();

	NormalSpeed = 20;
	HighSpeed = 5;
	FallSpeed = NormalSpeed;
	level = 1;


	movecnt = 0;
	for ( int i = 0; i < height; i++ ){
		for ( int j = 0; j < 10; j++ ){
			map[ i ][ j ].exist = false;
			map[ i ][ j ].type  = 8;
		}
	}
	for ( int i = 0; i < 4; i++ ){
		for ( int j = 0; j < 4; j++ ){
			CurBlocks[ i ][ j ].exist = false;
			CurBlocks[ i ][ j ].type = 8;
		}
	}
	NextBlocks[0][0].type = GetRand( 6 );
	NextBlock();
}


void destroy(){
}

void GetHighScore(){
	// DXライブラリはヘッダでstdio.hの方をインクルードしてるので、せっかくだしそっちを使う。
	FILE *fp;	
	if( ( fp = fopen( filename, "rb" ) ) == NULL ) {
		save_data.highscore = 0;
		return;
	} else {
		fread( &save_data, sizeof(save_data_t), 1, fp ) ;
		fclose( fp );//解放
		save_data.highscore;
	}
}

void SaveHighScore(){
	FILE *fp;
	if ( score > save_data.highscore ){
		if( ( fp = fopen( filename, "wb" ) ) == NULL ) {
			return;
		} else {
			save_data.highscore = score;
			fwrite( &save_data, sizeof(save_data_t), 1, fp ) ;
			fclose( fp );
		}
	}
}

void ResetHighScore(){
	FILE *fp;
	if( ( fp = fopen( filename, "wb" ) ) == NULL ) {
		return;
	} else {
		save_data.highscore = 0;
		fwrite( &save_data, sizeof(save_data_t), 1, fp ) ;
		fclose( fp );
	}
}

void NextBlock(){
	CurBlocks[0][0].type = NextBlocks[0][0].type;
	CurBlocks[0][0].x = 3;
	CurBlocks[0][0].y = -4;
	for ( int i = 0; i < 4; i++ ){
		for ( int j = 0; j < 4; j++ ){
			CurBlocks[ i ][ j ].exist = BLOCKS[ CurBlocks[0][0].type ][ i ][ j ];
		}
	}
	NextBlocks[0][0].type = GetRand( 6 );
	for ( int i = 0; i < 4; i++ ){
		for ( int j = 0; j < 4; j++ ){
			NextBlocks[ i ][ j ].exist = BLOCKS[ NextBlocks[0][0].type ][ i ][ j ];
		}
	}

}

void UpdateGame(){
	// ゲームオーバー時
	if ( GameOverFlag ){
		UpdateGameOver();
		static int WaitTime = 30;
		if ( pad->isTriggered() && WaitTime < 0 ){
			if ( h < height ){
				h = height;
			} else {
				WaitTime = 30;
				SaveHighScore();
				reset();
			}
		} else {
			WaitTime--;
		}
		return;
	}
	// F5でゲームをリセット
	if ( pad->isTriggered( KEY_INPUT_F5 ) ){
		SaveHighScore();
		reset();
	}
	// F10でハイスコアをリセット
	if ( pad->isTriggered( KEY_INPUT_F12 ) ){
		ResetHighScore();
		GetHighScore();
	}
	// F2で全画面
	if ( pad->isTriggered( KEY_INPUT_F2 ) ){
		WindowFlag = !WindowFlag;
		ChangeWindowMode( WindowFlag );
		resetWithChangingWindow();
	}
	//"Q"で音を消す。
	if ( pad->isTriggered( KEY_INPUT_Q ) ){
		isPlay = !isPlay;
		if ( isPlay ){
			ChangeVolumeSoundMem( 16, LandSound );
			ChangeVolumeSoundMem( 16, MoveSound );
			ChangeVolumeSoundMem( 16, RotSound );
			ChangeVolumeSoundMem( 16, DeleteSound );
			ChangeVolumeSoundMem( 16, PauseSound );
			ChangeVolumeSoundMem( 16, GameOverSound1 );
			ChangeVolumeSoundMem( 16, GameOverSound2 );
			ChangeVolumeSoundMem( 16, bgm );
		} else {
			ChangeVolumeSoundMem( 0, LandSound );
			ChangeVolumeSoundMem( 0, MoveSound );
			ChangeVolumeSoundMem( 0, RotSound );
			ChangeVolumeSoundMem( 0, DeleteSound );
			ChangeVolumeSoundMem( 0, PauseSound );
			ChangeVolumeSoundMem( 0, GameOverSound1 );
			ChangeVolumeSoundMem( 0, GameOverSound2 );
			ChangeVolumeSoundMem( 0, bgm );
		}

	}

	bool next = false;

	if ( DeleteFlag < 0 ){
		MoveBlocks();
		if ( FallBlocks() ){
			next = true;
		}
		if ( next ){
			CheckGameOver();
			UpdateMap();
			DeleteMap();
			NextBlock();
			if ( DeleteFlag < 0 ){
				PlaySoundMem( LandSound, DX_PLAYTYPE_BACK );
			}
		}
	} else {
		DeleteMap();
	}
	UpdateSpeed();
}

void UpdateSpeed(){
	if ( score < 300 ){
		NormalSpeed = 20;
		HighSpeed = 5;
		level = 1;
	} else if ( score < 1000 ){
		NormalSpeed = 15;
		HighSpeed = 5;
		level = 2;
	} else if ( score < 1500 ){
		NormalSpeed = 12;
		HighSpeed = 4;
		level = 3;
	} else if ( score < 2000 ){
		NormalSpeed = 8;
		HighSpeed = 3;
		level = 4;
	} else {
		NormalSpeed = 6;
		HighSpeed = 3;
		level = 5;
	}
}

bool MoveBlocks(){
	if ( pad->isOn( LEFT ) && !movecnt ){
		movecnt = MoveSpeed;
		CurBlocks[ 0 ][ 0 ].x--;
		for ( int j = 0; j < 4; j++ ){
			for ( int i = 0; i < 4; i++ ){
				int x = CurBlocks[ 0 ][ 0 ].x + i;
				int y = CurBlocks[ 0 ][ 0 ].y + j;
				if ( CurBlocks[ j ][ i ].exist && ( map[ y ][ x ].exist || x < 0 ) ){
					movecnt = 0;
					CurBlocks[ 0 ][ 0 ].x++;
					return true;
				}
			}
		}
		PlaySoundMem( MoveSound, DX_PLAYTYPE_BACK );
	} else if ( pad->isOn( RIGHT ) && !movecnt ){
		movecnt = MoveSpeed;
		CurBlocks[ 0 ][ 0 ].x++;
		for ( int j = 0; j < 4; j++ ){
			for ( int i = 0; i < 4; i++ ){
				int x = CurBlocks[ 0 ][ 0 ].x + i;
				int y = CurBlocks[ 0 ][ 0 ].y + j;
				if ( CurBlocks[ j ][ i ].exist && ( map[ y ][ x ].exist || x >= width ) ){
					movecnt = 0;
					CurBlocks[ 0 ][ 0 ].x--;
					return true;
				}
			}
		}
		PlaySoundMem( MoveSound, DX_PLAYTYPE_BACK );
	} else if ( pad->isOn( UP ) && !movecnt ){
		movecnt = MoveSpeed * 2;
		rotRight();
		for ( int j = 0; j < 4; j++ ){
			for ( int i = 0; i < 4; i++ ){
				int x = CurBlocks[ 0 ][ 0 ].x + i;
				int y = CurBlocks[ 0 ][ 0 ].y + j;
				if ( CurBlocks[ j ][ i ].exist && ( map[ y ][ x ].exist || x < 0 || width <= x || y >= height ) ){
					movecnt = 0;
					rotLeft();
					return true;
				}
			}
		}
		PlaySoundMem( RotSound, DX_PLAYTYPE_BACK );
	}

	if ( !pad->isOn( LEFT ) && !pad->isOn( RIGHT ) && !pad->isOn( UP ) ){
		movecnt = 0;
	} else {
		if ( movecnt ) movecnt--;
	}
	return false;
}

void rotRight(){
	bool tmp[ 4 ][ 4 ];
	for ( int j = 0; j < 4; j++ ){
		for ( int i = 0; i < 4; i++ ){
			tmp[ j ][ i ] = CurBlocks[ 3 - i ][ j ].exist;
		}
	}
	for ( int j = 0; j < 4; j++ ){
		for ( int i = 0; i < 4; i++ ){
			CurBlocks[ j ][ i ].exist = tmp[ j ][ i ];
		}
	}
}

void rotLeft(){
	bool tmp[ 4 ][ 4 ];
	for ( int j = 0; j < 4; j++ ){
		for ( int i = 0; i < 4; i++ ){
			tmp[ j ][ i ] = CurBlocks[ i ][ 3 - j ].exist;
		}
	}
	for ( int j = 0; j < 4; j++ ){
		for ( int i = 0; i < 4; i++ ){
			CurBlocks[ j ][ i ].exist = tmp[ j ][ i ];
		}
	}
}

bool FallBlocks(){
	static int cnt = 0;
	if ( cnt == 0 ){
		if ( pad->isOn( DOWN ) ){
			FallSpeed = HighSpeed;
		} else {
			FallSpeed = NormalSpeed;
		}
	}
	if ( cnt >= FallSpeed ){
		cnt = 0;
		CurBlocks[ 0 ][ 0 ].y++;
		for ( int j = 0; j < 4; j++ ){
			for ( int i = 0; i < 4; i++ ){
				int x = CurBlocks[ 0 ][ 0 ].x + i;
				int y = CurBlocks[ 0 ][ 0 ].y + j;
				if ( y < 0 ) continue;
				if ( CurBlocks[ j ][ i ].exist && ( map[ y ][ x ].exist || y >= height ) ){
					CurBlocks[ 0 ][ 0 ].y--;
					return true;
				}
			}
		}
	} else {
		cnt++;
	}

	return false;
}

void UpdateMap(){
	for ( int j = 0; j < 4; j++ ){
		for ( int i = 0; i < 4; i++ ){
			int x = CurBlocks[ 0 ][ 0 ].x + i;
			int y = CurBlocks[ 0 ][ 0 ].y + j;
			if ( CurBlocks[ j ][ i ].exist && 0 <= i && i < width && 0 <= j && j < height ){
				map[ y ][ x ].exist = true;
				map[ y ][ x ].type = CurBlocks[ 0 ][ 0 ].type;
			}
		}
	}
}

void DeleteMap(){
	int cnt = 0 ;
	const int WaitTime = 20;

	if ( DeleteFlag < 0 ){
		// 消せるかどうか調べる。
		CheckDeleteMap( &cnt, WaitTime );
	} else {
		if ( DeleteFlag > 0 ){
			if ( DeleteFlag == WaitTime ){
				PlaySoundMem( DeleteSound, DX_PLAYTYPE_BACK );
			}
			ef_delete();	// エフェクト
		} else if ( DeleteFlag == 0 ){
			DeleteLines();	// 行を消す
		} else {
			// 念のため
			DeleteFlag = -1;
			for ( int i = 0; i < MaxDelete; i++ ){
				DeleteLine[ i ] = -1;
			}
		}
		DeleteFlag--;
	}

	// 点数の更新
	switch( cnt ){
	case 1: score += 10 * level; break;
	case 2: score += 30 * level; break;
	case 3: score += 60 * level; break;
	case 4: score += 100 * level; break;
	default: break;
	}
}

void CheckDeleteMap( int* cnt, const int WaitTime ){
	for ( int j = height - 1; j >= 0; j-- ){
		DeleteLine[ *cnt ] = j;
		for ( int i = 0; i < width; i++ ){
			if ( !map[ j ][ i ].exist ){
				DeleteLine[ *cnt ] = -1;
				break;
			}
		}
		if ( DeleteLine[ *cnt ] >= 0 ){
			DeleteFlag = WaitTime;
			( *cnt )++;
		}
	}
}

void ef_delete(){
	for ( int n = MaxDelete - 1; n >= 0; n-- ){
		int j = DeleteLine[ n ];
		if ( j >= 0 ){
			for ( int x = 0; x < width; x++ ){
				map[ j ][ x ].type = 7;
			}
		}
	}
}

void DeleteLines(){
	for ( int n = MaxDelete - 1; n >= 0; n-- ){
		int j = DeleteLine[ n ];
		if ( j >= 0 ){
			for ( int y = j; y >= 1; y-- ){
				for ( int x = 0; x < width; x++ ){
					map[ y ][ x ].exist = map[ y - 1 ][ x ].exist;
					map[ y ][ x ].type = map[ y - 1 ][ x ].type;
				}
			}
			for ( int x = 0; x < width; x++ ){
				map[ 0 ][ x ].exist = false;
				map[ 0 ][ x ].type = 8;
			}
		}
	}
	for ( int i = 0; i < MaxDelete; i++ ){
		DeleteLine[ i ] = -1;
	}
}

void CheckGameOver(){
	for ( int j = 0; j < 4; j++ ){
		for ( int i = 0; i < 4; i++ ){
			if ( CurBlocks[ j ][ i ].exist && CurBlocks[ 0 ][ 0 ].y + j < 0 ){
				GameOverFlag = true;
			}
		}
	}
}

void DrawGame( float fps ){
	DrawBG();
	DrawMap();
	DrawCurrentBlock();
	DrawNextBlock();
	DrawParam( fps );
	if ( GameOverFlag ){
		DrawGameOver();
	}
}

void DrawBG(){
	DrawExtendGraph( 0,0,640,480,BGImage,FALSE );
	DrawBox( X0 + 5, Y0 + 5, X0 + StageWidth + 5, Y0 + StageHeight + 5, GetColor( 0,0,0 ), TRUE );
	DrawBox( X0, Y0, X0 + StageWidth, Y0 + StageHeight,GetColor( 0,0,64 ), TRUE );
	DrawString( 400, 60, "← / →　 ：移動", GetColor( 0, 0, 0 ) );
	DrawString( 400, 90, "↑　　　　：回転", GetColor( 0, 0, 0 ) );
	DrawString( 400, 120, "↓　　　　：高速落下", GetColor( 0, 0, 0 ) );
	DrawString( 400, 150, "スペースキー：一時停止", GetColor( 0, 0, 0 ) );
	DrawString( 400, 180, "F5キー　　：やり直し", GetColor( 0, 0, 0 ) );
	DrawStringToHandle( 400, 20, "制作者:フラガ", GetColor( 64, 64, 0 ), Gyosyo );
	DrawStringToHandle( 280, 350, "テトリス", GetColor( 64, 64, 0 ), Gyosyob );
}

void DrawMap(){
	for ( int j = 0; j < height; j++ ){
		for ( int i = 0; i < width; i++ ){
			if ( map[ j ][ i ].exist ){
				int x = X0 + i * BlockSize;
				int y = Y0 + j * BlockSize;
				DrawGraph( x, y, BlockImages[ map[ j ][ i ].type ], FALSE );
			}
		}
	}
}

void DrawCurrentBlock(){
	for ( int i = 0; i < 4; i++ ){
		for ( int j = 0; j < 4; j++ ){
			if ( CurBlocks[ i ][ j ].exist && 
				0 <= CurBlocks[ 0 ][ 0 ].x + j && CurBlocks[ 0 ][ 0 ].x + j < width && 
				0 <= CurBlocks[ 0 ][ 0 ].y + i && CurBlocks[ 0 ][ 0 ].y + i <= height )
			{
				int x = X0 + ( CurBlocks[ 0 ][ 0 ].x + j ) * BlockSize;
				int y = Y0 + ( CurBlocks[ 0 ][ 0 ].y + i ) * BlockSize;
				DrawGraph( x, y, BlockImages[ CurBlocks[ 0 ][ 0 ].type ], FALSE );
			}
		}
	}
}

void DrawNextBlock(){
	int d = 10;
	DrawStringToHandle( 2 * X0 + 12 * BlockSize, Y0, "NEXT", GetColor( 0,0,0 ), Pop );
	DrawBox( 2 * X0 + 12 * BlockSize + 5 - d, 2 * Y0 + 5 - d, 2 * X0 + 16 * BlockSize + 5 + d,
		2 * Y0 + 4 * BlockSize + 5 + d,	GetColor( 0,0,0 ), TRUE );
	DrawBox( 2 * X0 + 12 * BlockSize - d, 2 * Y0 - d, 2 * X0 + 16 * BlockSize + d, 
		2 * Y0 + 4 * BlockSize + d, GetColor( 0,0,64 ), TRUE );
	for ( int i = 0; i < 4; i++ ){
		for ( int j = 0; j < 4; j++ ){
			if ( NextBlocks[ i ][ j ].exist ){
				int x = 2 * X0 + ( 12 + j ) * BlockSize;
				int y = 2 * Y0 + i * BlockSize;
				DrawGraph( x, y, BlockImages[ NextBlocks[ 0 ][ 0 ].type ], FALSE );
			}
		}
	}
}

void DrawParam( float fps ){
	// 得点の表示
	DrawFormatStringToHandle( 280, 210, GetColor( 0,0,0 ), Pop, "HIGHSCORE" );
	DrawFormatStringToHandle( 472, 210, GetColor( 0,0,0 ), Gothic, "%d", save_data.highscore );
	DrawFormatStringToHandle( 280, 260, GetColor( 0,0,0 ), Pop, "SCORE" );
	DrawFormatStringToHandle( 472, 260, 
		score > save_data.highscore ? GetColor( 255,0,0 ) : GetColor( 0,0,0 ), Gothic, "%d", score );
	DrawFormatStringToHandle( 280, 310, GetColor( 0,0,0 ), Pop, "LEVEL" );
	DrawFormatStringToHandle( 472, 310, GetColor( 0,0,0 ), Gothic, "%d", level );
	// FPSの表示
	DrawFormatString( 550, 440, GetColor( 0,0,0 ), "FPS %3.1f", fps );
}

bool PlayFlag = false;

void UpdateGameOver(){
	if ( h <= 1 && gocnt <= 0 ){
		PlaySoundMem( GameOverSound1, DX_PLAYTYPE_BACK );
	}
	if ( h >= height ){
		PlayFlag = true;
	}
	if ( h < height && gocnt % 10 == 0 ){
		h++;
	}
	for ( int i = 0; i < h; i++ ){
		for ( int j = 0; j < width; j++ ){
			map[ height - 1 - i ][ j ].type = 0;
		}
	}
}

void DrawGameOver(){
	if ( h < height ){
		int y = Y0 + StageHeight - BlockSize * ( h - 1 + ( gocnt % 10 ) / 10.0 ); 
		DrawLine( X0, y, X0 + StageWidth, y, GetColor( 255,255,255 ) );
	} else {
		if ( PlayFlag ){
			PlayFlag = false;
			PlaySoundMem( GameOverSound2, DX_PLAYTYPE_BACK );
		}
		SetDrawBlendMode( DX_BLENDMODE_ALPHA, 0x80 );
		DrawBox( 0,140,640, 340, GetColor( 255,255,255 ), TRUE );
		SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
		DrawStringToHandle( 64, 170, "GameOver", GetColor( 128,0,0 ), Gothicb );
		DrawStringToHandle( 216, 295 + 3 * cosf( PHI_F * gocnt / FPS ),"Press Any Key", GetColor( 0,128,0 ), Pop );
		if ( score > save_data.highscore ){
			DrawStringToHandle( 400, 170, "HIGHSCORE!!", GetColor( 0,0,128 ), Pop );
		}
	}
	gocnt = ( gocnt >= 2 * FPS ? 0 : gocnt + 1 );
}

void DrawPause(){
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 0x80 );
	DrawBox( 0,0,640,480,GetColor(0,0,0),TRUE );
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );
	DrawStringToHandle( 176, 216, "Press Space Key", GetColor( 255,255,255 ), Pop );
}