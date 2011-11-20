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

#include <vector>
#include <boost/array.hpp>
#include <boost/foreach.hpp>

#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiMesh.h>
#include "common.hpp"

class Mesh {
public:
  Mesh( const aiMesh *mesh ) {
    std::vector< float > buffer;
    for( int current_face = 0; current_face != mesh->mNumFaces; ++current_face ) {
      const aiFace *face = mesh->mFaces + current_face;
      if( face->mNumIndices == 3 )
      for( int current_index = 0; current_index != 3; ++current_index ) {
        const aiVector3D *vertex = mesh->mVertices + face->mIndices[ current_index ];
        buffer.push_back( vertex->x ); buffer.push_back( vertex->y ); buffer.push_back( vertex->z );
        buffer.push_back( 1.0f );
        const aiVector3D *texcoord = mesh->mTextureCoords[ 0 ] + face->mIndices[ current_index ];
        buffer.push_back( texcoord->x ); buffer.push_back( texcoord->y ); buffer.push_back( texcoord->z );
        const aiVector3D *normal = mesh->mNormals + face->mIndices[ current_index ];
        buffer.push_back( normal->x ); buffer.push_back( normal->y ); buffer.push_back( normal->z );
      }
    }
    glGenBuffers( 1, &triangle_buffer_id );
    glBindBuffer( GL_ARRAY_BUFFER, triangle_buffer_id );
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof( float ), reinterpret_cast<void*>( buffer.data() ), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
    triangle_vertex_count = buffer.size() / 10;
    buffer.clear();
    for( int current_face = 0; current_face != mesh->mNumFaces; ++current_face ) {
      const aiFace *face = mesh->mFaces + current_face;
      if( face->mNumIndices == 4 )
      for( int current_index = 0; current_index != 4; ++current_index ) {
        const aiVector3D *vertex = mesh->mVertices + face->mIndices[ current_index ];
        buffer.push_back( vertex->x ); buffer.push_back( vertex->y ); buffer.push_back( vertex->z ); buffer.push_back( 1.0f );
        const aiVector3D *texcoord = mesh->mTextureCoords[ 0 ] + face->mIndices[ current_index ];
        buffer.push_back( texcoord->x ); buffer.push_back( texcoord->y ); buffer.push_back( texcoord->z );
        const aiVector3D *normal = mesh->mNormals + face->mIndices[ current_index ];
        buffer.push_back( normal->x ); buffer.push_back( normal->y ); buffer.push_back( normal->z );
      }
    }
    glGenBuffers( 1, &quad_buffer_id );
    glBindBuffer( GL_ARRAY_BUFFER, quad_buffer_id );
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof( float ), reinterpret_cast<void*>( buffer.data() ), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
    quad_vertex_count = buffer.size() / 10;
  }
  void operator()() const {
    glBindBuffer( GL_ARRAY_BUFFER, triangle_buffer_id );
    glEnableVertexAttribArray( 0 );  glEnableVertexAttribArray( 1 ); glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof( float ), reinterpret_cast<void*>( 0 ) );
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof( float ), reinterpret_cast<void*>( sizeof( float ) * 4 ) );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_TRUE, 10 * sizeof( float ), reinterpret_cast<void*>( sizeof( float ) * 7 ) );
    glDrawArrays( GL_TRIANGLES, 0, triangle_vertex_count );
    glBindBuffer( GL_ARRAY_BUFFER, quad_buffer_id );
    glEnableVertexAttribArray( 0 ); glEnableVertexAttribArray( 1 ); glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof( float ), reinterpret_cast<void*>( 0 ) );
    glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof( float ), reinterpret_cast<void*>( sizeof( float ) * 4 ) );
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_TRUE, 10 * sizeof( float ), reinterpret_cast<void*>( sizeof( float ) * 7 ) );
    glDrawArrays( GL_QUADS, 0, quad_vertex_count );
    glBindBuffer(GL_ARRAY_BUFFER, 0 );
  }
private:
  GLuint triangle_buffer_id;
  int triangle_vertex_count;
  GLuint quad_buffer_id;
  int quad_vertex_count;
};

class Node {
public:
  Node( aiNode *node ) {
    children.reserve( node->mNumChildren );
    for( int current_child = 0; current_child != node->mNumChildren; ++current_child )
      children.push_back( Node( node->mChildren[ current_child ] ) );
    for( int current_element = 0; current_element != 16; ++current_element )
      matrix[ current_element ] = node->mTransformation[ current_element % 4 ][ current_element / 4 ];
    mesh_indices.reserve( node->mNumMeshes );
    for( int current_element = 0; current_element != node->mNumMeshes; ++current_element )
      mesh_indices.push_back( node->mMeshes[ current_element ] );
  }
  void operator()( const std::vector< Mesh > &meshes ) const {
    glPushMatrix();
    glMultMatrixf( matrix.data() );
    BOOST_FOREACH( const Node &node, children )
      node( meshes );
    BOOST_FOREACH( int index, mesh_indices )
      meshes[ index ]();
    glPopMatrix();
  }
private:
  std::vector< Node > children;
  std::vector< int > mesh_indices;
  boost::array< float, 16 > matrix;
};

class Root {
public:
  Root( const aiScene *scene ) : node( scene->mRootNode ) {
    meshes.reserve( scene->mNumMeshes );
    for( int current_mesh = 0; current_mesh != scene->mNumMeshes; ++current_mesh )
      meshes.push_back( Mesh( scene->mMeshes[ current_mesh ] ) );
  }
  void operator()() const {
    node( meshes );
  }
private:
  std::vector< Mesh > meshes;
  Node node;
};

Root load( const char *_filename ) {
  Assimp::Importer importer;
  importer.ReadFile( _filename, 0 );
  const aiScene *scene = importer.GetScene();
  return Root( scene );
}

int main() {
  init( 800, 600 );
  Effect effect_tank( "effects/phong_in_world.cgfx" );
  effect_tank.load( "p0" );
  Root root = load( "models/can.dae" );
  glEnable(GL_DEPTH_TEST);
  glDrawBuffer( GL_BACK );
  glEnable(GL_CULL_FACE);
  glMatrixMode( GL_MODELVIEW );
  while( !quit_flag ) {
    const double begin_time = glfwGetTime();

    const GLfloat light0pos[] = { sinf( light_position ) * 7.0, 5.5, cosf( light_position ) * 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);

    glViewport( 0, 0, window_width, window_height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 30.0, getAspect(), 0.1, 50.0 );
    gluLookAt( cam_position[ 0 ], cam_position[ 1 ], cam_position[ 2 ],
              cam_position[ 0 ] + cam_direction[ 0 ], cam_position[ 1 ] + cam_direction[ 1 ], cam_position[ 2 ] + cam_direction[ 2 ],
              0, 10, 0 );
    glMatrixMode( GL_MODELVIEW );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    effect_tank.setParam( "camera_position", cam_position[ 0 ], cam_position[ 1 ], cam_position[ 2 ] );
    effect_tank.flush();
    root();
    glFlush();
    glfwSwapBuffers();
    glfwPollEvents();
    updatePosition();
    const double end_time = glfwGetTime();
    glfwSleep( 1.0 / 60.0 - ( end_time - begin_time ) );
  }
  glfwCloseWindow();
  glfwTerminate();
  
}
