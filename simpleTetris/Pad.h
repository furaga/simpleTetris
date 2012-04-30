// ���͏����̂��߂̃N���X�B�V���O���g���ɂ��Ă���B

#ifndef PAD_H
#define PAD_H
#include <cmath>

enum Input{
	UP, DOWN, RIGHT, LEFT, A, B, ENTER, ESCAPE, INPUT_UNKNOWN 
};

class Pad{
public:
	char KeyInput[ 2 ][ 256 ];
	int MouseInput[ 2 ];
	bool isInput[ 2 ];
	int cur;

	static void create();
	static void destroy();
	static Pad* instance();

	void update();
	int isOn( Input );	// ������Ă��邩�ǂ���
	int isTriggered( Input ); // �V���������ꂽ���B
	int isOn( int );	// ������Ă��邩�ǂ���
	int isTriggered( int ); // �V���������ꂽ���B
	int isOn();	// ������Ă��邩�ǂ���
	int isTriggered(); // �V���������ꂽ���B
	void DrawParam();

private:
	Pad();	
	~Pad();
	static Pad* mInstance;

	int GetInputType( int id );
};

#endif
