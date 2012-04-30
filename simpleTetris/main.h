#include <cmath>
#include "DxLib.h"

#define SAFE_DELETE( x ) if( x ){ delete x; }
#define SAFE_DELETE_ARRAY( x ) if( x ){ delete[] x; }

#define SAVE_FILE	"data/save_data.dat"

#define BLOCK_IMAGE "data/Blocks1.png"
#define BG_IMAGE	"data/bg.jpg"

#define LAND_SOUND		"data/land.wav"
#define MOVE_SOUND		"data/move.wav"
#define ROT_SOUND		"data/rot.wav"
#define DELETE_SOUND	"data/delete.wav"			
#define PAUSE_SOUND		"data/pause.wav"
#define GAMEOVER_SOUND1	"data/gameover1.wav"	// TODO	
#define GAMEOVER_SOUND2	"data/gameover2.wav"	// TODO
#define BGM				"data/bgm.wav"		// TODO

const float FPS = 60.f;

//�Z�[�u�f�[�^�̍\����
typedef struct{
	int highscore;
} save_data_t;
save_data_t save_data;
const char *filename = SAVE_FILE;

//�E�B���h�E�t���O
bool WindowFlag = true;

//���x��
int level = 1;

// �t�H���g	
int Pop;
int Gothic;
int Gothicb;
int Gyosyo;
int Gyosyob;

// �T�E���h
bool isPlay = true;
int LandSound;
int MoveSound;
int RotSound;
int DeleteSound;
int PauseSound;
int GameOverSound1, GameOverSound2;
int bgm;

// �X�e�[�W�n
const int BlockSize = 20;
const int X0 = 10, Y0 = 45;
const int width = 10, height = 20;	// �}�X�ڂ̐�
const int StageWidth = BlockSize * width;
const int StageHeight = BlockSize * height;
int BGImage;

// �u���b�N�n
const bool BLOCKS[ 7 ][ 4 ][ 4 ] ={
	{//1
		{false,true,false,false},
		{false,true,false,false},
		{false,true,false,false},
		{false,true,false,false}
	},
	{//2
		{false,false,false,false},
		{true,true,false,false},
		{false,true,true,false},
		{false,false,false,false}
	},
	{//3
		{false,false,false,false},
		{false,false,true,true},
		{false,true,true,false},
		{false,false,false,false}
	},
	{//4
		{false,false,false,false},
		{false,true,false,false},
		{true,true,true,false},
		{false,false,false,false}
	},
	{//5
		{false,false,false,false},
		{false,true,true,false},
		{false,true,true,false},
		{false,false,false,false}
	},
	{//6
		{false,true,true,false},
		{false,false,true,false},
		{false,false,true,false},
		{false,false,false,false}
	},
	{//7
		{false,true,true,false},
		{false,true,false,false},
		{false,true,false,false},
		{false,false,false,false}
	}
};

typedef struct _MAP{
	int x, y;
	bool exist;
	int type;
}MAP;

MAP CurBlocks[ 4 ][ 4 ], map[ height ][ width ], NextBlocks[ 4 ][ 4 ];
int BlockImages[ 9 ];

// �����X�s�[�h�n
int NormalSpeed = 20;
int HighSpeed = 5;
int FallSpeed;
// ���E�̈ړ��n
const int MoveSpeed = 7;
int movecnt = 0;

const int MaxDelete = 5;
int DeleteLine[ MaxDelete ];
int DeleteFlag = -1;

bool GameOverFlag = false;
// �Q�[���I�[�o�[�n�̊֐��Ŏg�����́B
int gocnt = 0;
int h = 1;

int score = 0, highscore = 0;


void Initialize();
void reset();
void resetWithChangingWindow();
void destroy();

void GetHighScore();
void SaveHighScore();
void ResetHighScore();

void NextBlock();

void UpdateGame();

void UpdateSpeed();

bool MoveBlocks();
void rotRight();
void rotLeft();
bool FallBlocks();

void UpdateMap();

void DeleteMap();
void CheckDeleteMap( int* cnt, const int WaitTime );
void ef_delete();
void DeleteLines();

void CheckGameOver();

void DrawGame( float fps );
void DrawBG();
void DrawMap();
void DrawCurrentBlock();
void DrawNextBlock();
void DrawParam( float fps );

void UpdateGameOver();
void DrawGameOver();

void DrawPause();