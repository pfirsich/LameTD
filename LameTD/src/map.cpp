#include "map.hpp"
#include "shadows.hpp"

namespace map {
	GridField* mapGrid;
	int width, height, occupied, clickable;
	rc::Object* ground;
	rc::Texture* colorMap;

	float randf( float max ) {
	#define PREC 10000
		return (rand( ) % PREC) / (float)PREC * max;
	}

	void placeWall( int x, int y, float vx, float vy ) {
		rc::Object* temp = new rc::Object( );
		temp->setMesh( rc::getRessource<rc::Mesh>( "media/wall" ) );
		temp->setTexture( rc::getRessource<rc::Texture>( "media/stein.png" ) );
		temp->setTexture( &shadows::shadowMap, rc::Texture::sampler1 );
		temp->setShader( rc::getRessource<rc::Shader>( "media/rocks.vert;media/rocks.frag" ) );
		temp->position( Eigen::Vector3f( (x - width * 0.5f) * SPACING + vx * 0.5f * SPACING, SPACING * 0.5f, (y - height * 0.5f) * SPACING + vy * 0.5f * SPACING ) );
		temp->scale( Eigen::Vector3f( SPACING / 100.0f, SPACING / 100.0f, SPACING / 200.0f ) );
		if( vy == 1.0f ) vy = 0.0f;
		temp->orientation(Eigen::Vector3f((float)M_PI / 2.0f, 0.0f, (-vx / 2.0f + vy) * (float)M_PI));
	};

	void setup( const char* fname ) {
		int channels;
		unsigned char* mapData = SOIL_load_image( fname, &width, &height, &channels, 3 );
		if( !mapData ) {
			puts( "Map could not be loaded." );
			return;
		}
		occupied = width * height;

		clickable = 0;
		unsigned char* p = mapData;
		for( int y = 0; y < height; ++y ) {
			for( int x = 0; x < width; ++x ) {
				if( p[0] == 255 ) {
					if( p[1] == 255 && p[2] == 255 ) ++clickable;
				} else if( p[0] == 200 ) {
					++clickable;
				};
				p += 3;
			};
		};

		unsigned char* highLinesTex = new unsigned char[width*height*4];
		p = mapData;
		unsigned char* p2 = highLinesTex;
		for( int y = 0; y < height; ++y ) {
			for( int x = 0; x < width; ++x ) {
				p2[0] = p2[1] = p2[2] = 200;
				if( p[0] == 200 ) {
					p2[3] = 255;
				} else {
					p2[3] = 0;
				};

				p += 3;
				p2 += 4;
			};
		};

		// Helfer: Julia, Niko, Alex, MrKohlenstoff, Smilodon

		/* Actually!:
		 * (floor(255 / delta) + 1)^3 = clickables
		 * floor( 255 / delta ) = cbrt( clickables ) - 1
		 * But this won't have a solution if cbrt( clickables ) - 1 is not a whole number.
		 * The correct solution would now be to calculate delta with clickables being the next power of 3
		 * (finding the next cbrt(clickables) with a whole numbered solution.
		 * because a higher clickables results in a higher cubic root of clickables the next higher power of 3
		 * would be the next whole cubic root of clickables. So:
		 * delta = 255.0f / (ceil( cbrt( clickables ) ) - 1.0f)
		 * Or:
		 * delta = 255.0f / (floor( cbrt( clickables ) ) + 1.0f - 1.0f)
		 * delta = 255.0f / floor( cbrt( clickables ) )
		 *
		 * This will correct results, if clickables is actually not x^3. But I don't care :) (I know it's easy to fix)
		 */

		float delta = 255.0f / floor( cbrt( (float)clickable ) );
		printf( "clickable: %d, delta: %f\n", clickable, delta );
		unsigned char* colorMapData = new unsigned char[width*height*3];
		p = mapData;
		p2 = colorMapData;
		bool colorize;
		float r = 0.0f, g = 0.0f, b = -delta; // So b will start with 0.0f
		for( int y = 0; y < height; ++y ) {
			for( int x = 0; x < width; ++x ) {
				colorize = false;
				if( p[0] == 255 ) {
					if( p[1] == 255 && p[2] == 255 ) colorize = true;
				} else if( p[0] == 200 ) {
					colorize = true;
				};

				if( colorize ) {
					b += delta;
					if( b > 255.0f ) {
						b = 0;
						g += delta;
						if( g > 255.0f ) {
							g = 0;
							r += delta;
						};
					};
					//printf( "%f, %f, %f\n", r, g, b );

					p2[0] = (unsigned char)r;
					p2[1] = (unsigned char)g;
					p2[2] = (unsigned char)b;
				} else {
					p2[0] = p2[1] = p2[2] = 0;
				};

				p += 3;
				p2 += 3;
			};
		};
		colorMap = new rc::Texture;
		colorMap->loadBuffer( colorMapData, width, height, 3 );
		SOIL_save_image( "color_map.bmp", SOIL_SAVE_TYPE_BMP, width, height, 3, colorMapData );

		rc::getRessource<rc::Shader>( "media/trees.vert;media/trees.frag" )->defaultUniforms.add( rc::uniform::FLOAT | rc::uniform::SCALAR, "time", &shaderTime );
		rc::Texture::setDefaultFilter( GL_LINEAR, GL_LINEAR, GL_REPEAT, 0.0f );

		rc::getRessource<rc::Shader>( "media/rocks.vert;media/rocks.frag" )->defaultUniforms.add( rc::uniform::SAMPLER, "shadowMap", &rc::Texture::sampler1 );
		rc::getRessource<rc::Shader>( "media/rocks.vert;media/rocks.frag" )->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "lightMVP", shadows::lightMVP );
		rc::getRessource<rc::Shader>( "media/rocks.vert;media/rocks.frag" )->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "objectMVP", rc::Object::MVP );

		rc::getRessource<rc::Shader>( "media/trees.vert;media/trees.frag" )->defaultUniforms.add( rc::uniform::SAMPLER, "shadowMap", &rc::Texture::sampler1 );
		rc::getRessource<rc::Shader>( "media/trees.vert;media/trees.frag" )->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "lightMVP", shadows::lightMVP );
		rc::getRessource<rc::Shader>( "media/trees.vert;media/trees.frag" )->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "objectMVP", rc::Object::MVP );

		rc::getRessource<rc::Texture>( "media/tree_leaf.png" )->setFilter( GL_LINEAR, GL_LINEAR, GL_CLAMP, 0.0f );
		rc::getRessource<rc::Texture>( "media/tree_trunk.png" )->setFilter( GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, 0.0f );
		rc::getRessource<rc::Texture>( "media/tree_trunk.png" )->generateMipMaps( );

		rc::Texture* highLinesMap = new rc::Texture;
		highLinesMap->loadBuffer( highLinesTex, width, height, 4 );
		delete[] highLinesTex;

		srand( 42 );
		int cwater = 0, cground = width * height;
		mapGrid = new GridField[width*height];
		GridField* pMap = mapGrid;
		unsigned char* pixel = mapData;
		unsigned char* pixel2 = colorMapData;
		for( int y = 0; y < height; ++y ) {
			for( int x = 0; x < width; ++x ) {
				if( pixel[0] == 255 ) { // red, yellow, white
					if( pixel[1] == 255 ) { // yellow, white
						if( pixel[2] == 255 ) { // white
							pMap->type = GridField::NORMAL;
							pMap->r = pixel2[0];
							pMap->g = pixel2[1];
							pMap->b = pixel2[2];
							pMap->occupied = false;
						} else { // yellow
							pMap->type = GridField::GOAL;
							rc::Object* temp = new rc::Object( );
							temp->setMesh( rc::getRessource<rc::Mesh>( "media/plain" ) );
							temp->setTexture( rc::getRessource<rc::Texture>( "media/goal.png" ) );
							temp->position( Eigen::Vector3f( x * SPACING - width * 0.5f * SPACING, 0.05f, y * SPACING - height * 0.5f * SPACING ) );
						};
					} else { // red
						pMap->type = GridField::SPAWN;
						rc::Object* temp = new rc::Object( );
						temp->setMesh( rc::getRessource<rc::Mesh>( "media/plain" ) );
						temp->setTexture( rc::getRessource<rc::Texture>( "media/spawn.png" ) );
						temp->position( Eigen::Vector3f( x * SPACING - width * 0.5f * SPACING, 0.05f, y * SPACING - height * 0.5f * SPACING ) );
					};
				} else if( pixel[0] == 100 ) { // dark grey
					pMap->type = GridField::ROCKS;

					float max = 0.0f;
					max += ((pMap + 1)->type == GridField::ROCKS) * 1.0f;
					max += ((pMap - 1)->type == GridField::ROCKS) * 1.0f;
					max += ((pMap + width)->type == GridField::ROCKS) * 1.0f;
					max += ((pMap - width)->type == GridField::ROCKS) * 1.0f;
					max += ((pMap + 1 + width)->type == GridField::ROCKS) * 0.7f;
					max += ((pMap + 1 - width)->type == GridField::ROCKS) * 0.7f;
					max += ((pMap - 1 + width)->type == GridField::ROCKS) * 0.7f;
					max += ((pMap - 1 - width)->type == GridField::ROCKS) * 0.7f;
					max = pow( max / 6.8f, 2.0f );

					float size = max * 4.8f;

					rc::Object* temp = new rc::Object( );
					temp->setMesh( rc::getRessource<rc::Mesh>( "media/Stein1" ) );
					temp->setTexture( rc::getRessource<rc::Texture>( "media/5.png" ) );
					temp->setTexture( &shadows::shadowMap, rc::Texture::sampler1 );
					temp->setShader( rc::getRessource<rc::Shader>( "media/rocks.vert;media/rocks.frag" ) );
					temp->position( Eigen::Vector3f( (x - width * 0.5f) * SPACING + randf( max ) * 2.0f - 1.0f, 0.0f, (y - height * 0.5f) * SPACING + randf( max ) * 2.0f - 1.0f ) );
					temp->scale( Eigen::Vector3f( size * 0.03f, size * 0.03f, size * 0.03f ) );
					temp->orientation( Eigen::Vector3f( randf( 360.0f ), randf( 360.0f ), randf( 360.0f ) ) );
				} else if( pixel[0] == 200 ) { // light grey
					pMap->type = GridField::RAISED;
					pMap->r = pixel2[0];
					pMap->g = pixel2[1];
					pMap->b = pixel2[2];
					pMap->occupied = false;

					if( pixel + 4 < mapData + width*height*3 && *(pixel + 4) != 200 ) {
						placeWall( x, y, 1.0f, 0.0f );
					};
					if( pixel - 4 > mapData && *(pixel - 4) != 200 ) {
						placeWall( x, y, -1.0f, 0.0f );
					};
					if( pixel + width*3 < mapData + width*height*3 && *(pixel + width*3) != 200 ) {
						placeWall( x, y, 0.0f, 1.0f );
					};
					if( pixel - width*3 > mapData && *(pixel - width*3) != 200 ) {
						placeWall( x, y, 0.0f, -1.0f );
					};
				} else { // green, blue, black
					if( pixel[1] == 255 ) { // green
						pMap->type = GridField::TREES;

						float size = (randf( 1.0f ) + 1.0f) * 0.03f;
						float angle = randf( 360.0f );
						rc::Object* temp = new rc::Object( );
						temp->setMesh( rc::getRessource<rc::Mesh>( "media/tree_trunk" ) );
						temp->setTexture( rc::getRessource<rc::Texture>( "media/tree_trunk.png" ) );
						temp->setTexture( &shadows::shadowMap, rc::Texture::sampler1 );
						temp->setShader( rc::getRessource<rc::Shader>( "media/trees.vert;media/trees.frag" ) );
						temp->position( Eigen::Vector3f( x * SPACING - width * 0.5f * SPACING, 0.0f, y * SPACING - height * 0.5f * SPACING ) );
						temp->scale( Eigen::Vector3f( size, size, size ) );
						temp->orientation( Eigen::Vector3f( -90.0f, 0.0f, angle ) );

						temp = new rc::Object( );
						temp->setMesh( rc::getRessource<rc::Mesh>( "media/tree_leaf" ) );
						temp->setTexture( rc::getRessource<rc::Texture>( "media/tree_leaf.png" ) );
						temp->setTexture( &shadows::shadowMap, rc::Texture::sampler1 );
						temp->setShader( rc::getRessource<rc::Shader>( "media/trees.vert;media/trees.frag" ) );
						temp->position( Eigen::Vector3f( x * SPACING - width * 0.5f * SPACING, 0.0f, y * SPACING - height * 0.5f * SPACING ) );
						temp->scale( Eigen::Vector3f( size, size, size ) );
						temp->orientation( Eigen::Vector3f( -90.0f, 0.0f, angle ) );
						temp->blending(rc::Object::TRANSPARENCY);
					} else if( pixel[2] == 255 ) { // blue
						pMap->type = GridField::WATER;
						cwater++;
						cground--;
					} else { // black
						pMap->type = GridField::EMPTY;
						occupied--;
						cground--;
					};
				};

				pixel += 3;
				pixel2 += 3;
				pMap++;
			}
		}
		delete[] colorMapData;

		cground *= 4;
		rc::Vertex* groundV = new rc::Vertex[cground];
		int vpGround = 0;
		unsigned short* groundI = new unsigned short[cground];
		for( int i = 0; i < cground; ++i ) groundI[i] = i;

		cwater *= 4;
		rc::Vertex* waterV = new rc::Vertex[cwater];
		int vpWater = 0;
		unsigned short* waterI = new unsigned short[cwater];
		for( int i = 0; i < cwater; ++i ) waterI[i] = i;

		pMap = mapGrid;
		rc::Vertex* v;
		int* vp;
		for( int y = 0; y < height; ++y ) {
			for( int x = 0; x < width; ++x ) {
				if( pMap->type != GridField::EMPTY ) {
					float vy = 0.0f;
					if( pMap->type == GridField::WATER ) {
						v = waterV;
						vp = &vpWater;
					} else {
						v = groundV;
						vp = &vpGround;
						if( pMap->type == GridField::RAISED ) vy = HEIGHT;
					};

					Eigen::Vector3f vt = Eigen::Vector3f( x * SPACING - width * 0.5f * SPACING, vy, y * SPACING - height * 0.5f * SPACING );
					v[*vp].pos = vt + Eigen::Vector3f( -SPACING * 0.5f, 0.0f,  SPACING * 0.5f );
					v[*vp].tex = Eigen::Vector2f( x / (float)width, (y+1) / (float)height );
					v[*vp].normal = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
					(*vp)++;

					v[*vp].pos = vt + Eigen::Vector3f(  SPACING * 0.5f, 0.0f,  SPACING * 0.5f );
					v[*vp].tex = Eigen::Vector2f( (x+1) / (float)width, (y+1) / (float)height );
					v[*vp].normal = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
					(*vp)++;

					v[*vp].pos = vt + Eigen::Vector3f(  SPACING * 0.5f, 0.0f, -SPACING * 0.5f );
					v[*vp].tex = Eigen::Vector2f( (x+1) / (float)width, y / (float)height );
					v[*vp].normal = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
					(*vp)++;

					v[*vp].pos = vt + Eigen::Vector3f( -SPACING * 0.5f, 0.0f, -SPACING * 0.5f );
					v[*vp].tex = Eigen::Vector2f( x / (float)width, y / (float)height );
					v[*vp].normal = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
					(*vp)++;
				};
				pMap++;
			};
		};

		rc::Mesh* groundMesh = new rc::Mesh;
		groundMesh->upload( cground, groundV, cground, groundI, rc::Mesh::QUADS );
		ground = new rc::Object;
		ground->setMesh( groundMesh );
		ground->setTexture( rc::getRessource<rc::Texture>( "media/grass.png" ) );
		ground->setTexture( &shadows::shadowMap, rc::Texture::sampler1 );
		ground->setShader( rc::getRessource<rc::Shader>( "media/ground.vert;media/ground.frag" ) );
		ground->getShader( )->defaultUniforms.add( rc::uniform::SAMPLER, "shadowMap", &rc::Texture::sampler1 );
		ground->getShader( )->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "lightMVP", shadows::lightMVP );
		ground->dontCull( true );

		rc::Mesh* waterMesh = new rc::Mesh;
		waterMesh->upload( cwater, waterV, cwater, waterI, rc::Mesh::QUADS );
		rc::Object* water = new rc::Object;
		water->setMesh( waterMesh );
		water->setTexture( rc::getRessource<rc::Texture>( "media/wasser.png" ) );
		water->setTexture( rc::getRessource<rc::Texture>( "media/wasser_nm.png" ), rc::Texture::sampler1 );
		water->setTexture( rc::getRessource<rc::Texture>( "media/stein.png" ), rc::Texture::sampler2 );
		water->setTexture( &shadows::shadowMap, rc::Texture::sampler3 );
		water->setShader( rc::getRessource<rc::Shader>( "media/water.vert;media/water.frag" ) );
		water->getShader( )->defaultUniforms.add( rc::uniform::SAMPLER, "normal", &rc::Texture::sampler1 );
		water->getShader( )->defaultUniforms.add( rc::uniform::SAMPLER, "base2", &rc::Texture::sampler2 );
		water->getShader( )->defaultUniforms.add( rc::uniform::FLOAT | rc::uniform::SCALAR, "time", &shaderTime );
		water->getShader( )->defaultUniforms.add( rc::uniform::FLOAT | rc::uniform::VEC3, "camPos", &camera::camPos );
		water->getShader( )->defaultUniforms.add( rc::uniform::SAMPLER, "shadowMap", &rc::Texture::sampler3 );
		water->getShader( )->defaultUniforms.add( rc::uniform::MAT4 | rc::uniform::FLOAT, "lightMVP", shadows::lightMVP );
		water->dontCull( true );

		printf( "clickables: %d/%d\n", clickable, occupied );

		unsigned short num = (width+1) * 2 + (height+1) * 2;

		unsigned short* ind = new unsigned short[num];
		for( int i = 0; i < num; ++i ) ind[i] = i;

		rc::Vertex* vert = new rc::Vertex[num];
		int i;
		for( i = 0; i < width + 1; ++i ) {
			int j = i * 2;
			vert[j].pos =   Eigen::Vector3f( i * SPACING - width * 0.5f * SPACING, 0.0f, -height * 0.5f * SPACING );
			vert[j].tex = 	Eigen::Vector2f( i / (float)width, 0.0f );
			vert[j+1].pos = Eigen::Vector3f( i * SPACING - width * 0.5f * SPACING, 0.0f,  height * 0.5f * SPACING );
			vert[j+1].tex = Eigen::Vector2f( i / (float)width, 1.0f );
		};
		int off = i - 1;
		for( i = 0; i < height + 1; ++i ) {
			int j = (i + off) * 2;
			vert[j].pos =   Eigen::Vector3f( -width * 0.5f * SPACING, 0.0f, i * SPACING - height * 0.5f * SPACING );
			vert[j].tex = 	Eigen::Vector2f( 0.0f, i / (float)height );
			vert[j+1].pos = Eigen::Vector3f(  width * 0.5f * SPACING, 0.0f, i * SPACING - height * 0.5f * SPACING );
			vert[j+1].tex = Eigen::Vector2f( 1.0f, i / (float)height );
		};
		rc::Mesh* lines = new rc::Mesh;
		lines->upload( num, vert, num, ind, rc::Mesh::LINES );

		rc::Object* lObject1 = new rc::Object;
		lObject1->setMesh( lines );
		lObject1->setTexture( rc::getRessource<rc::Texture>( "media/map.png" ) );
		lObject1->position( Eigen::Vector3f( -SPACING * 0.5f, 0.1f, -SPACING * 0.5f ) );
		lObject1->dontCull( true );

		rc::Object* lObject2 = new rc::Object;
		lObject2->setMesh( lines );
		lObject2->setTexture( highLinesMap );
		lObject2->position( Eigen::Vector3f( -SPACING * 0.5f, 0.1f + HEIGHT, -SPACING * 0.5f ) );
		lObject2->dontCull( true );
		lObject2->blending(rc::Object::TRANSPARENCY);
	}
}
