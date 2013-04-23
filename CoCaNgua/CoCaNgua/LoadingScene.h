
#ifndef _LOADING_SCENE_H_
#define _LOADING_SCENE_H_

#define LOADING_DELAY_PER_LOOP 1

#include "Scene.h"

#include <iostream>

using namespace std;

class LoadingScene : Scene
{
public:
  LoadingScene(void);
  ~LoadingScene(void);

  static LoadingScene& inst();

  void loop();
};  
#endif // !_LOADING_SCENE_H_


