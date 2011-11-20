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

#ifndef FADIS_GL_DEMO_COMMON_HPP 
#define FADIS_GL_DEMO_COMMON_HPP

#include <cmath>
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

extern bool quit_flag;
extern float cam_position[ 3 ]; // x y z
extern float cam_direction[ 3 ]; // x y z
extern float light_position;
extern float obj_position[ 2 ];
extern unsigned int window_width;
extern unsigned int window_height;
extern CGcontext context;  

  class Effect {
  public:
    Effect( const char *_filename );
    void setTexture( const char *_name, GLuint _texture );
    void setParam( const char *_name, float _x, float _y, float _z );
    void setParam( const char *_name, float _x, float _y );
    void setParam( const char *_name, float _x );
    void setMatrix( const char *_name, const float *_matrix );
    void load( const char *_name );
    void flush();
  private:
    CGeffect effect;
    CGtechnique techniques;
    CGpass pass;
  };
  
void keycb( int key, int action );
  
void updatePosition();


double getAspect();
  
void init( unsigned int, unsigned int );

#endif
