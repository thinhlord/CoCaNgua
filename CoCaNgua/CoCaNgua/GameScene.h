#ifndef _CG_GAME_H
#define _CG_GAME_H

#include <string>

#include "Piece.h"
#include "Die.h"
#include "Board.h"
#include "Light.h"
#include "Graphic.h"
#include "Constants.h"

#define SHOW_GRID 1
#define SHOW_LIGHT_SOURCE 1

struct Field
{
  Vector3 position;
  Piece* piece;

  Field()
  {
    position = Vector3();
    piece = NULL;
  }

  Field(Vector3 pPos)
  {
    position = pPos;
    piece = NULL;
  }

  Field(Vector3 pPos, Piece* pPiece)
  {
    position = pPos;
    piece = pPiece;
  }
};

class GameScene
{
  void *font;

  Board*              mBoard;
  Piece*              mPieces[16]; // 0:3 RED, 4:7 BLUE, 8:11 GREEN, 12:16 YELLOW
  
  bool mEnablePiece[4];

  Vector3 mStartPos[16];
  Field mFields[40];
  Field mHome[16];
  int mConnerIndex[12];

  Vector3 mPredictPosition[4];
  MoveState mPredictMoveState[4];

  int mDieNumber;
  int mTries;
  bool mDieIsThrown;
  bool mustBeStart;
  bool mFullHome;

  Turn mWinner;

  void drawSence();
  void drawDie();
  Piece* getModelByName(int name);
  int getModelPositionIndex(Vector3 pPos, Field pArray[], int pSize);
  bool checkAllModelIdle();
  void nextTurn();

public:
  Die*                mDice;

  GLfloat lightPosition[4];
  float lightAngle, lightHeight;

  bool mDieIsDrawn;
  Turn mPlayerTurn;

  GameScene(void);
  ~GameScene(void);

  static GameScene& inst();

  void initBoard();
  void initDie();
  void initPieces();

  Piece** getPiecesArray();

  void resetBoard();
  void loop();
  void movePiece(int name);
  void rollDice(int number);

  void setDisablePiece(int index);

  bool processMouseBegan(int x, int y);
  void processMouseEnded(int x, int y);
  void processMousePassiveMotion(int x, int y);

  virtual void update();
};

inline Piece** GameScene::getPiecesArray()
{
  return mPieces;
}

#endif // !_GAME_H

