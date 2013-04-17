#include "Model.h"


Model::Model(void)
{
  g_enableTextures = true;

  mColorTint[0] = 0.0;
  mColorTint[1] = 0.0;
  mColorTint[2] = 0.0;

  mRotate = Vector3(0, 1, 0);
  mAngle = 0;

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

Model::Model( const Model* other )
{
  memcpy(this, other, sizeof(*other));
}


void Model::draw()
{
  const ModelOBJ::Mesh *pMesh = 0;
  const ModelOBJ::Material *pMaterial = 0;
  const ModelOBJ::Vertex *pVertices = 0;
  ModelTextures::const_iterator iter;

  //cout << getNumberOfMeshes();

  for (int i = 0; i < getNumberOfMeshes(); ++i)
  {
    pMesh = &getMesh(i);
    pMaterial = pMesh->pMaterial;
    pVertices = getVertexBuffer();

    GLfloat ambi[4];
    GLfloat diff[4];

    for (int i = 0; i < 3; i++)
    {
      ambi[i] = pMaterial->ambient[i] + mColorTint[i];
      diff[i] = pMaterial->diffuse[i] + mColorTint[i];

      if (ambi[i] > 1.0) ambi[i] = 1.0;
      if (diff[i] > 1.0) diff[i] = 1.0;
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pMaterial->specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pMaterial->shininess * 128.0f);

    if (g_enableTextures)
    {
      iter = mModelTextures.find(pMaterial->colorMapFilename);

      if (iter == mModelTextures.end())
      {
        glDisable(GL_TEXTURE_2D);
      }
      else
      {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, iter->second);
      }
    }
    else
    {
      glDisable(GL_TEXTURE_2D);
    }

    if (hasPositions())
    {
      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, getVertexSize(),
        getVertexBuffer()->position);
    }

    if (hasTextureCoords())
    {
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, getVertexSize(),
        getVertexBuffer()->texCoord);
    }

    if (hasNormals())
    {
      glEnableClientState(GL_NORMAL_ARRAY);
      glNormalPointer(GL_FLOAT, getVertexSize(),
        getVertexBuffer()->normal);
    }

    glDrawElements(GL_TRIANGLES, pMesh->triangleCount * 3, GL_UNSIGNED_INT,
      getIndexBuffer() + pMesh->startIndex);

    if (hasNormals())
      glDisableClientState(GL_NORMAL_ARRAY);

    if (hasTextureCoords())
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if (hasPositions())
      glDisableClientState(GL_VERTEX_ARRAY);
  }

}

void Model::loadModel(const char *pszFilename)
{
  // Import the OBJ file and normalize to unit length.


  if (!import(pszFilename))
  {
    throw std::runtime_error("Failed to load model.");
  }

  // Load any associated textures.
  // Note the path where the textures are assumed to be located.

  const ModelOBJ::Material *pMaterial = 0;
  GLuint textureId = 0;
  std::string::size_type offset = 0;
  std::string filename;

  for (int i = 0; i < getNumberOfMaterials(); ++i)
  {
    pMaterial = &getMaterial(i);

    // Look for and load any diffuse color map textures.

    if (pMaterial->colorMapFilename.empty())
      continue;

    // Try load the texture using the path in the .MTL file.
    textureId = loadTexture(pMaterial->colorMapFilename.c_str());

    if (!textureId)
    {
      offset = pMaterial->colorMapFilename.find_last_of('\\');

      if (offset != std::string::npos)
        filename = pMaterial->colorMapFilename.substr(++offset);
      else
        filename = pMaterial->colorMapFilename;

      // Try loading the texture from the same directory as the OBJ file.
      textureId = loadTexture((getPath() + filename).c_str());
    }

    if (textureId)
      mModelTextures[pMaterial->colorMapFilename] = textureId;

    // Look for and load any normal map textures.

    if (pMaterial->bumpMapFilename.empty())
      continue;

    // Try load the texture using the path in the .MTL file.
    textureId = loadTexture(pMaterial->bumpMapFilename.c_str());

    if (!textureId)
    {
      offset = pMaterial->bumpMapFilename.find_last_of('\\');

      if (offset != std::string::npos)
        filename = pMaterial->bumpMapFilename.substr(++offset);
      else
        filename = pMaterial->bumpMapFilename;

      // Try loading the texture from the same directory as the OBJ file.
      textureId = loadTexture((getPath() + filename).c_str());
    }

    if (textureId)
      mModelTextures[pMaterial->bumpMapFilename] = textureId;
  }
}

GLuint Model::loadTexture(const char *pszFilename)
{
  GLuint id = SOIL_load_OGL_texture(
    pszFilename,
    SOIL_LOAD_AUTO,
    SOIL_CREATE_NEW_ID,
    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
  );

  if( 0 != id )
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  }
  return id;
}

void Model::drawModel()
{
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0x2, 0x2);
  glStencilMask(0x2);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  float x, y, z;
  getCenter(x, y, z);

  glPushMatrix();
    // Move Object to coordinate origin
    glTranslatef(-x, -y, -z);
    glPushMatrix();
      glTranslated(mPos.x - mAnchor.x*getWidth(), 
        mPos.y - mAnchor.y*getHeight(),
        mPos.z - mAnchor.z*getLength());

      draw();
    glPopMatrix();
  glPopMatrix();

  glDisable(GL_STENCIL_TEST);
}

Vector3 Model::getCenterLocation()
{
  float centerX = mPos.x - mAnchor.x*getWidth();
  float centerY = mPos.y - mAnchor.y*getHeight();
  float centerZ = mPos.z - mAnchor.z*getLength();

  return Vector3(centerX, centerY, centerZ);
}

void Model::setColorTint( GLfloat red, GLfloat green, GLfloat blue)
{
  mColorTint[0] = red;
  mColorTint[1] = green;
  mColorTint[2] = blue;
}

Model::~Model(void)
{
}
