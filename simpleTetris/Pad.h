// 入力処理のためのクラス。シングルトンにしている。

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
	int isOn( Input );	// 押されているかどうか
	int isTriggered( Input ); // 新しく押されたか。
	int isOn( int );	// 押されているかどうか
	int isTriggered( int ); // 新しく押されたか。
	int isOn();	// 押されているかどうか
	int isTriggered(); // 新しく押されたか。
	void DrawParam();

private:
	Pad();	
	~Pad();
	static Pad* mInstance;

	int GetInputType( int id );
};

#endif
