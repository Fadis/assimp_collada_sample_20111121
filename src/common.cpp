/***************************************************************************
 *   Copyright (C) 2011 by Naomasa Matsubayashi   *
 *   fadis@quaternion.sakura.ne.jp   *
 *                                                                         *
 *   All rights reserved.                                                  *
 *                                                                         *
 * Redistribution and use in source and binary forms, with or without      *
 * modification, are permitted provided that the following conditions are  *
 * met:                                                                    *
 *                                                                         *
 *  1. Redistributions of source code must retain the above copyright      *
 *     notice, this list of conditions and the following disclaimer.       *
 *  2. Redistributions in binary form must reproduce the above copyright   *
 *     notice, this list of conditions and the following disclaimer in the *
 *     documentation and/or other materials provided with the distribution.*
 *                                                                         *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       *
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT      *
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   *
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        *
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   *
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   *
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    *
 ***************************************************************************/

#include <cmath>
#include <iostream>
#include <algorithm>
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <boost/preprocessor.hpp>

#include <GL/glew.h>
#include <Cg/cgGL.h>
#include <GL/glfw.h>

#include <IL/il.h>
#include <IL/ilu.h>

#include "common.hpp"

bool quit_flag;
float cam_position[ 3 ]; // x y z
float cam_direction[ 3 ]; // x y z
float cam_velocity[ 4 ]; // r θ z
float light_position;
float light_velocity;
float obj_position[ 2 ];
float obj_velocity[ 2 ];
int cam_moving_x;
int cam_moving_y;
int cam_moving_z;
int cam_moving_dir;
int lit_moving_x;
int obj_moving_x;
int obj_moving_y;
unsigned int window_width;
unsigned int window_height;

CGcontext context;

void checkCgError() {
  CGerror error = cgGetError();
  if ( error ) {
    std::cout << "CG error: ";
    std::cout << cgGetErrorString( error ) << std::endl;
    std::cout << cgGetLastListing( context ) << std::endl;
  }
}

Effect::Effect( const char *_filename ) {
  effect = cgCreateEffectFromFile( context, _filename, NULL );
  techniques = cgGetFirstTechnique(effect);
}
void Effect::setTexture( const char *_name, GLuint _texture ) {
  CGparameter texture = cgGetNamedEffectParameter( effect, _name );
  cgGLSetTextureParameter( texture, _texture );
}
void Effect::setParam( const char *_name, float _x, float _y, float _z ) {
  CGparameter param = cgGetNamedEffectParameter( effect, _name );
  cgGLSetParameter3f( param, _x, _y, _z );
}
void Effect::setParam( const char *_name, float _x, float _y ) {
  CGparameter param = cgGetNamedEffectParameter( effect, _name );
  cgGLSetParameter2f( param, _x, _y );
}
void Effect::setParam( const char *_name, float _x ) {
  CGparameter param = cgGetNamedEffectParameter( effect, _name );
  cgGLSetParameter1f( param, _x );
}
void Effect::setMatrix( const char *_name, const float *_matrix ) {
  CGparameter param = cgGetNamedEffectParameter( effect, _name );
  cgGLSetMatrixParameterfr( param, _matrix );
}
void Effect::load( const char *_name ) {
  pass = cgGetNamedPass( techniques, _name );
}
void Effect::flush() {
  cgSetPassState(pass);
}

void keycb( int key, int action ) {
  if( action == GLFW_PRESS ) {
    if( key == 'A' ) {
      cam_velocity[ 1 ] = -0.03;
      cam_moving_x++;
    }
    else if( key == 'D' ) {
      cam_velocity[ 1 ] = 0.03;
      cam_moving_x++;
    }
    else if( key == 'W' ) {
      cam_velocity[ 0 ] = 0.1 * 0.1;
      cam_moving_y++;
    }
    else if( key == 'S' ) {
      cam_velocity[ 0 ] = 0.1 * -0.1;
      cam_moving_y++;
    }
    else if( key == 'E' ) {
      cam_velocity[ 2 ] = 0.1 * 0.1;
      cam_moving_z++;
    }
    else if( key == 'C' ) {
      cam_velocity[ 2 ] = 0.1 * -0.1;
      cam_moving_z++;
    }
    else if( key == 'Q' ) {
      cam_velocity[ 3 ] = 0.1;
      cam_moving_dir++;
    }
    else if( key == 'Z' ) {
      cam_velocity[ 3 ] = -0.1;
      cam_moving_dir++;
    }
    else if( key == 'J' ) {
      light_velocity = -0.01;
      lit_moving_x++;
    }
    else if( key == 'K' ) {
      light_velocity = 0.01;
      lit_moving_x++;
    }
    else if( key == GLFW_KEY_LEFT ) {
      obj_velocity[ 0 ] = -0.1;
      obj_moving_x++;
    }
    else if( key == GLFW_KEY_RIGHT ) {
      obj_velocity[ 0 ] = 0.1;
      obj_moving_x++;
    }
    else if( key == GLFW_KEY_UP ) {
      obj_velocity[ 1 ] = -0.1;
      obj_moving_y++;
    }
    else if( key == GLFW_KEY_DOWN ) {
      obj_velocity[ 1 ] = 0.1;
      obj_moving_y++;
    }
    else if( key == GLFW_KEY_ESC ) {
      quit_flag = true;
    }
  }
  else if( action == GLFW_RELEASE ) {
    if( key == 'A' ) {
      cam_moving_x--;
      if( !cam_moving_x )
        cam_velocity[ 1 ] = 0.0;
    }
    else if( key == 'D' ) {
      cam_moving_x--;
      if( !cam_moving_x )
        cam_velocity[ 1 ] = 0.0;
    }
    else if( key == 'W' ) {
      cam_moving_y--;
      if( !cam_moving_y )
        cam_velocity[ 0 ] = 0.0;
    }
    else if( key == 'S' ) {
      cam_moving_y--;
      if( !cam_moving_y )
        cam_velocity[ 0 ] = 0.0;
    }
    else if( key == 'E' ) {
      cam_moving_z--;
      if( !cam_moving_z )
        cam_velocity[ 2 ] = 0.0;
    }
    else if( key == 'C' ) {
      cam_moving_z--;
      if( !cam_moving_z )
        cam_velocity[ 2 ] = 0.0;
    }
    else if( key == 'Q' ) {
      cam_moving_dir--;
      if( !cam_moving_dir )
        cam_velocity[ 3 ] = 0.0;
    }
    else if( key == 'Z' ) {
      cam_moving_dir--;
      if( !cam_moving_dir )
        cam_velocity[ 3 ] = 0.0;
    }
    else if( key == 'J' ) {
      lit_moving_x--;
      if( !lit_moving_x )
        light_velocity = 0.0;
    }
    else if( key == 'K' ) {
      lit_moving_x--;
      if( !lit_moving_x )
        light_velocity = 0.0;
    }
    else if( key == GLFW_KEY_LEFT ) {
      obj_moving_x--;
      if( !obj_moving_x )
        obj_velocity[ 0 ] = 0.0;
    }
    else if( key == GLFW_KEY_RIGHT ) {
      obj_moving_x--;
      if( !obj_moving_x )
        obj_velocity[ 0 ] = 0.0;
    }
    else if( key == GLFW_KEY_UP ) {
      obj_moving_y--;
      if( !obj_moving_y )
        obj_velocity[ 1 ] = 0.0;
    }
    else if( key == GLFW_KEY_DOWN ) {
      obj_moving_y--;
      if( !obj_moving_y )
        obj_velocity[ 1 ] = 0.0;
    }
  }
}

void updatePosition() {
  if( cam_moving_x || cam_moving_y || cam_moving_z || cam_moving_dir ) {
    float x = cosf( cam_velocity[ 1 ] ) * cam_direction[ 0 ] - sinf( cam_velocity[ 1 ] ) * cam_direction[ 2 ];
    float y = sinf( cam_velocity[ 1 ] ) * cam_direction[ 0 ] + cosf( cam_velocity[ 1 ] ) * cam_direction[ 2 ];
    cam_direction[ 0 ] = x;
    cam_direction[ 2 ] = y;
    cam_direction[ 1 ] += cam_velocity[ 3 ];
    cam_direction[ 1 ] *= 0.9;
    cam_position[ 0 ] += cam_direction[ 0 ] * cam_velocity[ 0 ];
    cam_position[ 2 ] += cam_direction[ 2 ] * cam_velocity[ 0 ];
    cam_position[ 1 ] += cam_velocity[ 2 ];
    
    if( fabsf( cam_velocity[ 0 ] ) < 0.1f )
      cam_velocity[ 0 ] *= 1.1;
    if( fabsf( cam_velocity[ 2 ] ) < 0.1f )
      cam_velocity[ 2 ] *= 1.1;
  }
  if( lit_moving_x ) {
    light_position += light_velocity;
    if( fabsf( light_velocity ) < 0.1f )
      light_velocity *= 1.1;
  }
  if( obj_moving_x || obj_moving_y ) {
    obj_position[ 0 ] += obj_velocity[ 0 ];
    if( fabsf( obj_velocity[ 0 ] ) < 1.0f )
      obj_velocity[ 0 ] *= 1.1;
    obj_position[ 1 ] += obj_velocity[ 1 ];
    if( fabsf( obj_velocity[ 1 ] ) < 1.0f )
      obj_velocity[ 1 ] *= 1.1;
  }
}

void resizeEvent( int _width, int _height ) {
  window_width = _width;
  window_height = _height;
}

double getAspect() {
  return static_cast<double>( window_width ) / static_cast<double>( window_height );
}

void init( unsigned int _width, unsigned int _height ) {
  window_width = _width;
  window_height = _height;
  glfwInit();
  glfwOpenWindowHint( GLFW_FSAA_SAMPLES, 8 );
  glfwOpenWindow( window_width, window_height, 8, 8, 8, 8, 16, 0, GLFW_WINDOW );
  glfwSetWindowSizeCallback( &resizeEvent );
  std::cout << glGetString( GL_VERSION ) << std::endl;
  glfwSwapInterval( 1 );
  
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    std::cout << "Error: " << glewGetErrorString(err) << std::endl;
    throw -1;
  }
  
//  ilInit();
//  iluInit();
  
  chdir( BOOST_PP_STRINGIZE( RESOURCE_DIRECTORY ) );
  
  cgSetErrorCallback(&checkCgError);
  context = cgCreateContext();
  cgGLRegisterStates( context );
  quit_flag = false;
  cam_position[ 0 ] = 0.0f;
  cam_position[ 1 ] = 3.5f;
  cam_position[ 2 ] = 7.0f;
  cam_velocity[ 0 ] = 0.0f;
  cam_velocity[ 1 ] = 0.0f;
  cam_velocity[ 2 ] = 0.0f;
  cam_velocity[ 3 ] = 0.0f;
  cam_direction[ 0 ] = 0.0f;
  cam_direction[ 1 ] = 0.0f;
  cam_direction[ 2 ] = -1.0f;
  light_position = 0.0;
  light_velocity = 0.0;
  obj_position[ 0 ] = 0.0f;
  obj_position[ 1 ] = 0.0f;
  obj_velocity[ 0 ] = 0.0f;
  obj_velocity[ 1 ] = 0.0f;
  cam_moving_x = 0;
  cam_moving_y = 0;
  cam_moving_z = 0;
  cam_moving_dir = 0;
  lit_moving_x = 0;
  obj_moving_x = 0;
  obj_moving_y = 0;
  glfwSetKeyCallback( &keycb );
}
