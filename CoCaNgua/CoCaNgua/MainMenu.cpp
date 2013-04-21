#include "MainMenu.h"


MainMenu::MainMenu(void)
{
  mBackground = new Sprite2D("img/background.png");
  mBackground->setAnchorPoint(Vector2(-0.5, -0.5));

  mStart = new Button("img/Button_Start.png", "img/Button_Start_Hover.png");
  mStart->setPosition(Vector2(478, 242));
}

MainMenu& MainMenu::inst()
{
  static MainMenu instance;
  return instance;
}

void MainMenu::drawScene()
{
  mBackground->drawImg();
  mStart->drawImg();
}

void MainMenu::loop()
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  {
    glLoadIdentity();
    glOrtho(0, Game::inst().mScreenWidth,
      0, Game::inst().mScreenHeight,
      -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    {
      glLoadIdentity();
      glDisable(GL_CULL_FACE);

      glClear(GL_DEPTH_BUFFER_BIT);

      drawScene();
    }
    glPopMatrix();

    // Making sure we can render 3d again
    glMatrixMode(GL_PROJECTION);
  }

  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

MainMenu::~MainMenu(void)
{
}
