#include "culling.hpp"
#include "object.hpp"
#include "camera.hpp"
#include "client.hpp"
#include "graphics.hpp"

#define DEG2RAD (1.0f / 360.0f * M_PI * 2.0f)

namespace culling {
	float tanCameraAngleTop;
	float tanCameraAngleBottom;
	float tanX;
	Eigen::Vector3f viewCornerBL, viewCornerBR, viewCornerTL, viewCornerTR, viewCenter;
	rc::Object *o1, *o2, *o3, *o4;

	rc::Object* newMarker( ) {
		rc::Object* temp = new rc::Object;
		temp->setMesh( rc::getRessource<rc::Mesh>( "media/cube2" ) );
		temp->setTexture( rc::getRessource<rc::Texture>( "media/spawn.png" ) );
		temp->scale( Eigen::Vector3f( 0.05f, 0.05f, 0.05f ) );
		temp->position( Eigen::Vector3f( 0.0f, 0.0f, 0.0f ) );
		temp->dontCull( true );
		return temp;
	}

	void setup( float fov, float aspect, float cameraAngle ) {
		tanCameraAngleTop = tan( (90.0f - cameraAngle + fov / 2.0f) * (float)DEG2RAD );
		tanCameraAngleBottom = tan((90.0f - cameraAngle - fov / 2.0f) * (float)DEG2RAD);
		tanX = tan(fov * aspect / 2.0f * (float)DEG2RAD);

		/*o1 = newMarker( );
		o2 = newMarker( );
		o3 = newMarker( );
		o4 = newMarker( );*/
	};

	void update( ) {
		static Eigen::Vector3f lastCamPos;

		if( camera::camPos != lastCamPos ) {
			float zBottom = tanCameraAngleBottom * camera::camPos.y( );
			float zTop = tanCameraAngleTop * camera::camPos.y( );
			float borderX1 = tanX * zTop;
			borderX1 = sqrt( borderX1 * borderX1 + camera::camPos.y( ) * camera::camPos.y( ) ) * tanX;
			float borderX2 = tanX * zBottom;
			borderX2 = sqrt( borderX2 * borderX2 + camera::camPos.y( ) * camera::camPos.y( ) ) * tanX;

			float minZ = -zTop + camera::camPos.z( );
			float maxZ = -zBottom + camera::camPos.z( );

			float borderXPart = 1.0f / (maxZ - minZ) * (borderX1 - borderX2);
			float borderX;

			minZ -= SPACING * 1.0f;
			maxZ += SPACING * 2.5f;

			viewCornerTL = Eigen::Vector3f( camera::camPos.x( ) - borderX1, 7.0f, minZ );
			viewCornerTR = Eigen::Vector3f( camera::camPos.x( ) + borderX1, 7.0f, minZ );
			viewCornerBL = Eigen::Vector3f( camera::camPos.x( ) - borderX2, 7.0f, maxZ );
			viewCornerBR = Eigen::Vector3f( camera::camPos.x( ) + borderX2, 7.0f, maxZ );
			viewCenter = 0.25f * (viewCornerTL + viewCornerTR + viewCornerBL + viewCornerBR);

			/*o1->position( viewCornerTL );
			o2->position( viewCornerTR );
			o3->position( viewCornerBL );
			o4->position( viewCornerBR );*/

			for( std::list<rc::Object*>::iterator i = rc::Object::objects.begin( ); i != rc::Object::objects.end( ); ++i ) {
				rc::Object* temp = *i;
				if( !temp->dontCull( ) ) {
					Eigen::Vector3f p = temp->position( );

					if( p.z( ) > maxZ || p.z( ) < minZ ) {
						temp->enabled( false );
					} else {
						borderX = (maxZ - p.z( )) * borderXPart + borderX2;
						if( p.x( ) > camera::camPos.x( ) + borderX || p.x( ) < camera::camPos.x( ) - borderX ) {
							temp->enabled( false );
						} else {
							temp->enabled( true );
						};
					};
				};
			};

			lastCamPos = camera::camPos;
		};
	};
}
