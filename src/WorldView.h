#ifndef WORLDVIEW_H
#define WORLDVIEW_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <list>
#include "View.h"
#include "Cube.h"
#include "Polygon.h"
#include "Actor.h"
#include "Shader.h"
#include "Camera.h"

namespace cgl {
	class WorldView : public View {
		private:
			static Shader* worldViewShader;
			std::list<Actor*> actors;
			Camera camera;
		public:
			WorldView(float x, float y, float width, float height);
			void addActor(Actor* a);
			virtual void draw(const glm::mat4& parentModel, const Polygon& p);
	};
}

#endif